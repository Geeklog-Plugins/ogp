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
    // Kept only for upgrade compatibility with pre-2.0 releases.
    'types' => array(
        'article', 'staticpages', 'calendar', 'calendarjp',
        'links', 'polls', 'filemgmt', 'downloads',
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
        $defaultImageUrl = $_CONF['site_url'] . '/ogp/social-default.jpg';
        $c->add('sg_main', null, 'subgroup', 0, 0, null, 0, true, $me);

        $c->add('fs_main', null, 'fieldset', 0, 0, null, 0, true, $me);
        $c->add('fb_default_img_url', $defaultImageUrl, 'text', 0, 0, null, 10, true, $me);
    }

    return true;
}
