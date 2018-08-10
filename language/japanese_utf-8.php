<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/language/japanese_utf-8.php                           |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2011-2018 mystral-kk - mystralkk AT gmail DOT com           |
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

$LANG_OGP = array (
    'plugin'           	=> 'OGP',
	'admin'		       	=> 'OGP',
	'fb_ids_not_set'	=> 'OGP: エラー!　最初に「コンフィギュレーション」でFacebookユーザーIDかFacebookアプリケーションIDのいずれかを指定してください。',
	'fb_appid_not_set'	=> 'OGP: エラー!　FacebookアプリケーションIDがセットされていません。',
);

// Localization of the Admin Configuration UI
$LANG_configsections['ogp'] = array(
    'label' => $LANG_OGP['plugin'],
    'title' => $LANG_OGP['plugin'] . 'の設定',
);

$LANG_confignames['ogp'] = array(
	'fb_user_ids'			=> 'FacebookユーザーID',
	'fb_app_id'				=> 'FacebookアプリケーションID',
	'fb_default_img_url'	=> 'サイトのデフォルト画像',
	'like_send'				=> '「送る」ボタンを表示する',
	'like_show_faces'		=> '顔を表示する',
	'like_style_layout'		=> 'レイアウト',
	'like_width'			=> '幅（ピクセル）',
	'like_verb'				=> '動作',
	'like_color'			=> '配色',
	'like_font'				=> 'フォント',
	'comments_num_posts'	=> 'コメントの表示数',
	'comments_width'		=> '幅（ピクセル）',
	'comments_color'		=> '配色',
	
	// Since v1.1.2
	'types'					=> '種類',
);

$LANG_configsubgroups['ogp'] = array(
    'sg_main' 				=> '主要設定'
);

$LANG_fs['ogp'] = array(
    'fs_main'       		=> $LANG_OGP['plugin'] . 'の主要設定',
	'fs_like'				=> 'Facebookいいね!ボタンの設定',
	'fs_comments'			=> 'Facebookコメントの設定',
	'fs_kinds'				=> 'コンテンツの種類',
);

// Note: entries 0, 1, 9, and 12 are the same as in $LANG_configselects['Core']
$LANG_configselects['ogp'] = array(
    0 => array('はい' => 1, 'いいえ' => 0),
    1 => array('はい' => true, 'いいえ' => false),
	2 => array(
			'標準' => 'standard', 'ボタン' => 'button_count',
			'ボックス' => 'box_count'
		),
	3 => array('いいね!' => 'like', 'おすすめ' => 'recommend'),
	4 => array('明るい色' => 'light', '暗い色' => 'dark'),
	5 => array('標準' => '', 'arial' => 'arial', 'lucida grande' => 'lucida grande',
			'segoe ui' => 'segoe ui', 'tahoma' => 'tahoma',
			'trebuchet ms' => 'trebuchet ms', 'verdana' => 'verdana',
		),
);
