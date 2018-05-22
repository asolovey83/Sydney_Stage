<?php
/**
 * The base configuration for WordPress
 *
 * The wp-config.php creation script uses this file during the
 * installation. You don't have to use the web site, you can
 * copy this file to "wp-config.php" and fill in the values.
 *
 * This file contains the following configurations:
 *
 * * MySQL settings
 * * Secret keys
 * * Database table prefix
 * * ABSPATH
 *
 * @link https://codex.wordpress.org/Editing_wp-config.php
 *
 * @package WordPress
 */

// ** MySQL settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define('DB_NAME', 'marketin_devscrum');

/** MySQL database username */
define('DB_USER', 'marketin_scrum');

/** MySQL database password */
define('DB_PASSWORD', 'd41Ly$CruM');

/** MySQL hostname */
define('DB_HOST', 'localhost');

/** Database Charset to use in creating database tables. */
define('DB_CHARSET', 'utf8');

/** The Database Collate type. Don't change this if in doubt. */
define('DB_COLLATE', '');

/**#@+
 * Authentication Unique Keys and Salts.
 *
 * Change these to different unique phrases!
 * You can generate these using the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}
 * You can change these at any point in time to invalidate all existing cookies. This will force all users to have to log in again.
 *
 * @since 2.6.0
 */
define('AUTH_KEY',         '`mY2h5D@d%!f,}!.T3uoS!:!$x0=G0Za3hBsmd3wWc)XnWkVv#OG}+~H<a>Gql%?');
define('SECURE_AUTH_KEY',  'A*:?ULb#LtK=T;-mm:Udc_5@1Z,SJ{1`8RtFl7$G*s&-X@,p!MpJ@]v[<bcDEvfv');
define('LOGGED_IN_KEY',    'Pb}>r]qVmCKdDC)zH~$BUdB<gkCsr~Ou~f _96>I}jm1<1~51TKs1.BRyt.,LM>#');
define('NONCE_KEY',        'uwE&e=K[MN}+yzZ*j-&E)e%Q9KLzOgw#R[EnW/[` xlQpN;fNcJ]@S|NDZCoKRYA');
define('AUTH_SALT',        'MDaOof?tX$|Wr9eHJ7sBaFe7?,Y=5Z9*)+W4) %cZ@>@X17Shc@$A0j$7+r_7sAn');
define('SECURE_AUTH_SALT', 'PJ0XL*dv8^3nrk,w1jT9XM2Qn&`C%uSVmhp1Vu[i4+fU#YA];F1UB&r<] LI&?Nu');
define('LOGGED_IN_SALT',   'ZlFhYal_iM )%l}L%`V+<aPQ%-So%]`[ >DoL+Bp$|k|,W^ON#Ts2ZzQS?ddQ%*Q');
define('NONCE_SALT',       '7L(cj0uo6%m,@}h8:Qmr#H /J1e*j/rH=%[hNL&/EF-Ua37)j9!Zeo:s{K=[^V|q');

/**#@-*/

/**
 * WordPress Database Table prefix.
 *
 * You can have multiple installations in one database if you give each
 * a unique prefix. Only numbers, letters, and underscores please!
 */
$table_prefix  = 'wp_';

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 *
 * For information on other constants that can be used for debugging,
 * visit the Codex.
 *
 * @link https://codex.wordpress.org/Debugging_in_WordPress
 */
define('WP_DEBUG', false);

/* That's all, stop editing! Happy blogging. */

/** Absolute path to the WordPress directory. */
if ( !defined('ABSPATH') )
	define('ABSPATH', dirname(__FILE__) . '/');

/** Sets up WordPress vars and included files. */
require_once(ABSPATH . 'wp-settings.php');
