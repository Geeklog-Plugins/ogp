<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/install_defaults.php                                  |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2011-2020 mystral-kk - mystralkk AT gmail DOT com           |
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

if (stripos($_SERVER['PHP_SELF'], basename(__FILE__)) !== false) {
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

$_OGP_DEFAULT = array(
    // Facebook "Like" button
    'like_send'          => true,
    'like_show_faces'    => true,
    'like_style_layout'  => 'standard',
    'like_width'         => 450,
    'like_verb'          => 'like',
    'like_color'         => 'light',
    'like_font'          => '',

    // Facebook "Comments"
    'comments_num_posts' => 10,
    'comments_width'     => 500,
    'comments_color'     => 'light',

    // Types of contents
    'types'              => array(
        'article', 'staticpages', 'calendar', 'calendarjp', 'links', 'polls', 'filemgmt', 'downloads',
    ),
);

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

    if (is_array($_OGP_CONF) && (count($_OGP_CONF) > 0)) {
        $_OGP_DEFAULT = array_merge($_OGP_DEFAULT, $_OGP_CONF);
    }

    $me = 'ogp';
    $c = config::get_instance();

    if (!$c->group_exists($me)) {
        $defaultImageUrl = $_CONF['site_admin_url'] . '/plugins/ogp/images/default.png';
        $c->add('sg_main', null, 'subgroup', 0, 0, null, 0, true, $me);

        $c->add('fs_main', null, 'fieldset', 0, 0, null, 0, true, $me);
        $c->add('fs_like', null, 'fieldset', 0, 1, null, 0, true, $me);
        $c->add('fs_comments', null, 'fieldset', 0, 2, null, 0, true, $me);
        $c->add('fb_user_ids', '', 'text', 0, 0, null, 10, true, $me);
        $c->add('fb_app_id', '', 'text', 0, 0, null, 20, true, $me);
        $c->add('fb_default_img_url', $defaultImageUrl, 'text', 0, 0, null, 30, true, $me);

        // Facebook "Like" button
        $c->add('like_send', $_OGP_DEFAULT['like_send'], 'select', 0, 1, 1, 40, true, $me);
        $c->add('like_show_faces', $_OGP_DEFAULT['like_show_faces'], 'select', 0, 1, 1, 50, true, $me);
        $c->add('like_style_layout', $_OGP_DEFAULT['like_style_layout'], 'select', 0, 1, 2, 60, true, $me);
        $c->add('like_width', $_OGP_DEFAULT['like_width'], 'text', 0, 1, null, 70, true, $me);
        $c->add('like_verb', $_OGP_DEFAULT['like_verb'], 'select', 0, 1, 3, 80, true, $me);
        $c->add('like_color', $_OGP_DEFAULT['like_color'], 'select', 0, 1, 4, 90, true, $me);
        $c->add('like_font', $_OGP_DEFAULT['like_font'], 'select', 0, 1, 5, 100, true, $me);

        // Facebook "Comments" form
        $c->add('comments_num_posts', $_OGP_DEFAULT['comments_num_posts'], 'text', 0, 2, null, 110, true, $me);
        $c->add('comments_width', $_OGP_DEFAULT['comments_width'], 'text', 0, 2, null, 120, true, $me);
        $c->add('comments_color', $_OGP_DEFAULT['comments_color'], 'select', 0, 2, 4, 130, true, $me);

        // Since v1.1.2
        $c->add('fs_kinds', null, 'fieldset', 0, 3, null, 0, true, $me);
        $c->add('types', $_OGP_DEFAULT['types'], '%text', 0, 3, null, 140, true, $me);
    }

    return true;
}
