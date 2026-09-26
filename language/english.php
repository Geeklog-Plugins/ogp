<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/language/english.php                                  |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2011-2020 mystral-kk - mystralkk AT gmail DOT com           |
// |                                                                           |
// | Constructed with the Universal Plugin                                     |
// +---------------------------------------------------------------------------|
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
// | along with this program; if not, write to the Free Software               |
// | Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA|
// |                                                                           |
// +---------------------------------------------------------------------------|

if (stripos($_SERVER['PHP_SELF'], basename(__FILE__)) !== false) {
    die('This file can not be used on its own!');
}

$LANG_OGP = array(
    'plugin' => 'OGP',
    'admin'  => 'OGP',
);

// Localization of the Admin Configuration UI
$LANG_configsections['ogp'] = array(
    'label' => 'OGP',
    'title' => 'OGP Configuration'
);

$LANG_confignames['ogp'] = array(
    'fb_default_img_url' => 'Default social image',
);

$LANG_configsubgroups['ogp'] = array(
    'sg_main' => 'Main Settings'
);

$LANG_fs['ogp'] = array(
    'fs_main' => 'Social metadata',
);

$LANG_configselects['ogp'] = array();
