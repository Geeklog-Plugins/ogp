<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/language/english.php                                  |
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
    'plugin'            => 'OGP',
	'admin'		        => 'OGP',
	'fb_ids_not_set'	=> 'OGP: Error!  You have to set either your Facebook User ID(s) or your Facebook App ID first at the configuration panel!',
	'fb_appid_not_set'	=> 'OGP: Error!  Facebook AppID is not set.',
);

// Localization of the Admin Configuration UI
$LANG_configsections['ogp'] = array(
    'label' => 'OGP',
    'title' => 'OGP Configuration'
);

$LANG_confignames['ogp'] = array(
	'fb_user_ids'			=> 'Your Facebook User ID(s)',
	'fb_app_id'				=> 'Your Facebook Application ID',
	'fb_default_img_url'	=> 'Your site\'s default image',
	'like_send'				=> 'Show "send" button',
	'like_show_faces'		=> 'Show faces',
	'like_style_layout'		=> 'Style layout',
	'like_width'			=> 'Width (pixels)',
	'like_verb'				=> 'Action',
	'like_color'			=> 'Color Scheme',
	'like_font'				=> 'Font',
	'comments_num_posts'	=> 'Number of posts',
	'comments_width'		=> 'Width (pixels)',
	'comments_color'		=> 'Color Scheme',
	
	// Since v1.1.2
	'types'					=> 'Types',
);

$LANG_configsubgroups['ogp'] = array(
    'sg_main' => 'Main Settings'
);

$LANG_fs['ogp'] = array(
    'fs_main'        		=> 'OGP Main Settings',
	'fs_like'				=> 'Facebook Like button',
	'fs_comments'			=> 'Facebook Comments',
	'fs_kinds'				=> 'Kinds of Contents',
);

// Note: entries 0, 1, 9, and 12 are the same as in $LANG_configselects['Core']
$LANG_configselects['ogp'] = array(
    0 => array('Yes' => 1, 'No' => 0),
    1 => array('Yes' => TRUE, 'No' => FALSE),
	2 => array(
			'Standard' => 'standard', 'Button' => 'button_count',
			'Box' => 'box_count'
		),
	3 => array('Like' => 'like', 'Recommend' => 'recommend'),
	4 => array('Light' => 'light', 'Dark' => 'dark'),
	5 => array('Default' => '', 'arial' => 'arial', 'lucida grande' => 'lucida grande',
			'segoe ui' => 'segoe ui', 'tahoma' => 'tahoma',
			'trebuchet ms' => 'trebuchet ms', 'verdana' => 'verdana',
		),
);
