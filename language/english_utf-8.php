<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/language/english_utf-8.php                            |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2011 mystral-kk - geeklog AT mystral-kk DOT net             |
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

if (strpos(strtolower($_SERVER['PHP_SELF']), strtolower(basename(__FILE__))) !== FALSE) {
    die('This file can not be used on its own!');
}

$LANG_OGP = array(
    'plugin'           	=> 'OGP',
	'admin'		       	=> 'OGP',
	'fb_ids_not_set'	=> 'OGP: Error!  You have to set either your Facebook User ID(s) or your Facebook App ID first at the configuration panel!',
);





// Localization of the Admin Configuration UI
$LANG_configsections['ogp'] = array(
    'label' => 'OGP',
    'title' => 'OGP Configuration'
);

$LANG_confignames['ogp'] = array(
	'fb_user_ids'	=> 'Your Facebook User ID(s)',
	'fb_app_id'		=> 'Your Facebook Application ID',
);

$LANG_configsubgroups['ogp'] = array(
    'sg_main' => 'Main Settings'
);

$LANG_fs['ogp'] = array(
    'fs_main'        => 'OGP Main Settings',
);

// Note: entries 0, 1, 9, and 12 are the same as in $LANG_configselects['Core']
$LANG_configselects['ogp'] = array(
    0 => array('True' => 1, 'False' => 0),
    1 => array('True' => TRUE, 'False' => FALSE),
);
