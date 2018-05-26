<?php
    if ( ! is_active_sidebar( 'postfooter' ))
        return;
?>
 
<div id="postfooter-widget">
    <div class="clearfix container">
    <?php
        $postfooter_sidebars = array( 'postfooter' );
        foreach ( $postfooter_sidebars as $key => $postfooter_sidebar ){
            if ( is_active_sidebar( $postfooter_sidebar ) ) {
                echo '<div class="postfooter-widget' . (  2 == $key ? ' last' : '' ) . '">';
                dynamic_sidebar( $postfooter_sidebar );
                echo '</div> <!-- end .postfooter-widget -->';
            }
        }
    ?>
    </div>
</div>
