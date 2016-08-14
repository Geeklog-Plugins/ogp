<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/install_defaults.php                                  |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2011 mystral-kk - geeklog AT mystral-kk DOT net             |
// |                                                                           |
// | Constructed with the Universal Plugin                                     |
// +---------------------------------------------------------------------------+
// |                                                                           |
// | This program is free software; you can redistribute it and/or             |
// | modify it under the terms of the GNU General Public License               |
// | as published by the Free Software Foundation; either version 2            |
// | of the License, or (at your option) any later version.                    |
// |                                                                           |
// | This program is distributed in the hope that it will be useful,           |
// | but WITHOUT ANY WARRANTY; without even the implied warranty of            |
// | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             |
// | GNU General Public License for more details.                              |
// |                                                                           |
// | You should have received a copy of the GNU General Public License         |
// | along with this program; if not, write to the Free Software Foundation,   |
// | Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.           |
// |                                                                           |
// +---------------------------------------------------------------------------+

if (stripos($_SERVER['PHP_SELF'], basename(__FILE__)) !== FALSE) {
    die('This file can not be used on its own!');
}

/**
* Open Graph Protocol default settings
*
* Initial Installation Defaults used when loading the online configuration
* records.  These settings are only used during the initial installation
* and not referenced any more once the plugin is installed
*
*/
global $_OGP_DEFAULT;

$_OGP_DEFAULT = array();

// Facebook Like button
$_OGP_DEFAULT['like_send'] = TRUE;

$_OGP_DEFAULT['like_show_faces'] = TRUE;

$_OGP_DEFAULT['like_style_layout'] = 'standard';

$_OGP_DEFAULT['like_width'] = 450;

$_OGP_DEFAULT['like_verb'] = 'like';

$_OGP_DEFAULT['like_color'] = 'light';

$_OGP_DEFAULT['like_font'] = '';

// Facebook Comments
$_OGP_DEFAULT['comments_num_posts'] = 10;

$_OGP_DEFAULT['comments_width'] = 500;

$_OGP_DEFAULT['comments_color'] = 'light';

/**
* Initializes Open Graph Protocol plugin configuration
*
* Creates the database entries for the configuation if they don't already
* exist.  Initial values will be taken from $_OGP_DEFAULT
* if available (e.g. from an old config.php), uses $_OGP_DEFAULT
* otherwise.
*
* @return   boolean     TRUE: success; FALSE: an error occurred
*/
function plugin_initconfig_ogp() {
    global $_CONF, $_OGP_CONF, $_OGP_DEFAULT;
	
    if (is_array($_OGP_CONF) AND (count($_OGP_CONF) > 0)) {
        $_OGP_DEFAULT = array_merge($_OGP_DEFAULT, $_OGP_CONF);
    }
	
	$me = 'ogp';
    $c = config::get_instance();
	
    if (!$c->group_exists($me)) {
		$default_img_url = $_CONF['site_admin_url']
						 . '/plugins/ogp/images/facebook_128.png';
        $c->add('sg_main', NULL, 'subgroup', 0, 0, NULL, 0, TRUE, $me);
		
        $c->add('fs_main', NULL, 'fieldset', 0, 0, NULL, 0, TRUE, $me);
        $c->add('fs_like', NULL, 'fieldset', 0, 1, NULL, 0, TRUE, $me);
        $c->add('fs_comments', NULL, 'fieldset', 0, 2, NULL, 0, TRUE, $me);
        $c->add('fb_user_ids', '', 'text', 0, 0, NULL, 10, TRUE, $me);
        $c->add('fb_app_id', '', 'text', 0, 0, NULL, 20, TRUE, $me);
        $c->add('fb_default_img_url', $default_img_url, 'text', 0, 0, NULL, 30, TRUE, $me);
		
		// Facebook Like
        $c->add('like_send', $_OGP_DEFAULT['like_send'], 'select', 0, 1, 1, 40, TRUE, $me);
        $c->add('like_show_faces', $_OGP_DEFAULT['like_show_faces'], 'select', 0, 1, 1, 50, TRUE, $me);
        $c->add('like_style_layout', $_OGP_DEFAULT['like_style_layout'], 'select', 0, 1, 2, 60, TRUE, $me);
        $c->add('like_width', $_OGP_DEFAULT['like_width'], 'text', 0, 1, NULL, 70, TRUE, $me);
        $c->add('like_verb', $_OGP_DEFAULT['like_verb'], 'select', 0, 1, 3, 80, TRUE, $me);
        $c->add('like_color', $_OGP_DEFAULT['like_color'], 'select', 0, 1, 4, 90, TRUE, $me);
        $c->add('like_font', $_OGP_DEFAULT['like_font'], 'select', 0, 1, 5, 100, TRUE, $me);
		// Facebook comments
        $c->add('comments_num_posts', $_OGP_DEFAULT['comments_num_posts'], 'text', 0, 2, NULL, 110, TRUE, $me);
        $c->add('comments_width', $_OGP_DEFAULT['comments_width'], 'text', 0, 2, NULL, 120, TRUE, $me);
        $c->add('comments_color', $_OGP_DEFAULT['comments_color'], 'select', 0, 2, 4, 130, TRUE, $me);
    }
	
    return TRUE;
}
