#include "cache.h"
#include "pack.h"
#include "pack-revindex.h"
#include "progress.h"
#include "packfile.h"

struct idx_entry {
	off_t                offset;
	union idx_entry_object {
		const unsigned char *hash;
		struct object_id *oid;
	} oid;
	unsigned int nr;
};

static int compare_entries(const void *e1, const void *e2)
{
	const struct idx_entry *entry1 = e1;
	const struct idx_entry *entry2 = e2;
	if (entry1->offset < entry2->offset)
		return -1;
	if (entry1->offset > entry2->offset)
		return 1;
	return 0;
}

int check_pack_crc(struct packed_git *p, struct pack_window **w_curs,
		   off_t offset, off_t len, unsigned int nr)
{
	const uint32_t *index_crc;
	uint32_t data_crc = crc32(0, NULL, 0);

	do {
		unsigned long avail;
		void *data = use_pack(p, w_curs, offset, &avail);
		if (avail > len)
			avail = len;
		data_crc = crc32(data_crc, data, avail);
		offset += avail;
		len -= avail;
	} while (len);

	index_crc = p->index_data;
	index_crc += 2 + 256 + p->num_objects * (the_hash_algo->rawsz/4) + nr;

	return data_crc != ntohl(*index_crc);
}

static int verify_packfile(struct packed_git *p,
			   struct pack_window **w_curs,
			   verify_fn fn,
			   struct progress *progress, uint32_t base_count)

{
	off_t index_size = p->index_size;
	const unsigned char *index_base = p->index_data;
	git_hash_ctx ctx;
	unsigned char hash[GIT_MAX_RAWSZ], *pack_sig;
	off_t offset = 0, pack_sig_ofs = 0;
	uint32_t nr_objects, i;
	int err = 0;
	struct idx_entry *entries;

	if (!is_pack_valid(p))
		return error("packfile %s cannot be accessed", p->pack_name);

	the_hash_algo->init_fn(&ctx);
	do {
		unsigned long remaining;
		unsigned char *in = use_pack(p, w_curs, offset, &remaining);
		offset += remaining;
		if (!pack_sig_ofs)
			pack_sig_ofs = p->pack_size - the_hash_algo->rawsz;
		if (offset > pack_sig_ofs)
			remaining -= (unsigned int)(offset - pack_sig_ofs);
		the_hash_algo->update_fn(&ctx, in, remaining);
	} while (offset < pack_sig_ofs);
	the_hash_algo->final_fn(hash, &ctx);
	pack_sig = use_pack(p, w_curs, pack_sig_ofs, NULL);
	if (hashcmp(hash, pack_sig))
		err = error("%s pack checksum mismatch",
			    p->pack_name);
	if (hashcmp(index_base + index_size - the_hash_algo->hexsz, pack_sig))
		err = error("%s pack checksum does not match its index",
			    p->pack_name);
	unuse_pack(w_curs);

	/* Make sure everything reachable from idx is valid.  Since we
	 * have verified that nr_objects matches between idx and pack,
	 * we do not do scan-streaming check on the pack file.
	 */
	nr_objects = p->num_objects;
	ALLOC_ARRAY(entries, nr_objects + 1);
	entries[nr_objects].offset = pack_sig_ofs;
	/* first sort entries by pack offset, since unpacking them is more efficient that way */
	for (i = 0; i < nr_objects; i++) {
		entries[i].oid.hash = nth_packed_object_sha1(p, i);
		if (!entries[i].oid.hash)
			die("internal error pack-check nth-packed-object");
		entries[i].offset = nth_packed_object_offset(p, i);
		entries[i].nr = i;
	}
	QSORT(entries, nr_objects, compare_entries);

	for (i = 0; i < nr_objects; i++) {
		void *data;
		enum object_type type;
		unsigned long size;
		off_t curpos;
		int data_valid;

		if (p->index_version > 1) {
			off_t offset = entries[i].offset;
			off_t len = entries[i+1].offset - offset;
			unsigned int nr = entries[i].nr;
			if (check_pack_crc(p, w_curs, offset, len, nr))
				err = error("index CRC mismatch for object %s "
					    "from %s at offset %"PRIuMAX"",
					    oid_to_hex(entries[i].oid.oid),
					    p->pack_name, (uintmax_t)offset);
		}

		curpos = entries[i].offset;
		type = unpack_object_header(p, w_curs, &curpos, &size);
		unuse_pack(w_curs);

		if (type == OBJ_BLOB && big_file_threshold <= size) {
			/*
			 * Let check_sha1_signature() check it with
			 * the streaming interface; no point slurping
			 * the data in-core only to discard.
			 */
			data = NULL;
			data_valid = 0;
		} else {
			data = unpack_entry(p, entries[i].offset, &type, &size);
			data_valid = 1;
		}

		if (data_valid && !data)
			err = error("cannot unpack %s from %s at offset %"PRIuMAX"",
				    oid_to_hex(entries[i].oid.oid), p->pack_name,
				    (uintmax_t)entries[i].offset);
		else if (check_sha1_signature(entries[i].oid.hash, data, size, type_name(type)))
			err = error("packed %s from %s is corrupt",
				    oid_to_hex(entries[i].oid.oid), p->pack_name);
		else if (fn) {
			int eaten = 0;
			err |= fn(entries[i].oid.oid, type, size, data, &eaten);
			if (eaten)
				data = NULL;
		}
		if (((base_count + i) & 1023) == 0)
			display_progress(progress, base_count + i);
		free(data);

	}
	display_progress(progress, base_count + i);
	free(entries);

	return err;
}

int verify_pack_index(struct packed_git *p)
{
	off_t index_size;
	const unsigned char *index_base;
	git_hash_ctx ctx;
	unsigned char hash[GIT_MAX_RAWSZ];
	int err = 0;

	if (open_pack_index(p))
		return error("packfile %s index not opened", p->pack_name);
	index_size = p->index_size;
	index_base = p->index_data;

	/* Verify SHA1 sum of the index file */
	the_hash_algo->init_fn(&ctx);
	the_hash_algo->update_fn(&ctx, index_base, (unsigned int)(index_size - the_hash_algo->rawsz));
	the_hash_algo->final_fn(hash, &ctx);
	if (hashcmp(hash, index_base + index_size - the_hash_algo->rawsz))
		err = error("Packfile index for %s hash mismatch",
			    p->pack_name);
	return err;
}

int verify_pack(struct packed_git *p, verify_fn fn,
		struct progress *progress, uint32_t base_count)
{
	int err = 0;
	struct pack_window *w_curs = NULL;

	err |= verify_pack_index(p);
	if (!p->index_data)
		return -1;

	err |= verify_packfile(p, &w_curs, fn, progress, base_count);
	unuse_pack(&w_curs);

	return err;
}
