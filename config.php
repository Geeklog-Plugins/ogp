<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/config.php                                            |
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

global $_OGP_CONF;

$_OGP_CONF = array();

// Plugin info

$_OGP_CONF['pi_version'] = '1.1.4';						// Plugin Version
$_OGP_CONF['gl_version'] = '1.6.0';						// GL Version plugin for
$_OGP_CONF['pi_url']     = 'http://mystral-kk.net/';	// Plugin Homepage
$_OGP_CONF['GROUPS']     = array(
		'Open Graph Protocol Admin' => 'Users in this group can administer the Open Graph Protocol plugin',
);
$_OGP_CONF['FEATURES']   = array(
		'ogp.edit' => 'Access to Open Graph Protocol editor',
);
$_OGP_CONF['MAPPINGS']   = array(
		'ogp.edit' => array('Open Graph Protocol Admin'),
);

$_OGP_CONF['LAYOUTS'] = array('standard', 'button_count', 'box_count');

$_OGP_CONF['FONTS'] = array(
	'', 'arial', 'lucida grande', 'segoe ui', 'tahoma', 'trebuchet ms',
	'verdana',
);

$_OGP_CONF['LOCALE_MAPPINGS'] = array(
//	Geeklog language name	=> Facebook locale
	'afrikaans'				=> 'af_ZA',
	'bosnian'				=> 'bs_BA',
	'bulgarian'				=> 'bg_BG',
	'catalan'				=> 'ca_ES',
	'chinese_simplified'	=> 'zh_CN',
	'chinese_traditional'	=> 'zh_TW',
	'croatian'				=> 'hr_HR',
	'czech'					=> 'cs_CZ',
	'danish'				=> 'da_DK',
	'dutch'					=> 'nl_NL',
	'english'				=> 'en_US',
	'estonian'				=> 'et_EE',
	'farsi'					=> 'fa_IR',
	'finnish'				=> 'fi_FI',
	'french_canada'			=> 'fr_CA',
	'french_france'			=> 'fr_FR',
	'german'				=> 'de_DE',
	'german_formal'			=> 'de_DE',
	'hebrew'				=> 'he_IL',
	'hellenic'				=> 'el_GR',
	'indonesian'			=> 'id_ID',
	'italian'				=> 'it_IT',
	'japanese'				=> 'ja_JP',
	'korean'				=> 'ko_KR',
	'norwegian'				=> 'nb_NO',	// OR 'nn_NO'?
	'polish'				=> 'pl_PL',
	'portuguese'			=> 'pt_PT',
	'portuguese_brazil'		=> 'pt_BR',
	'romanian'				=> 'ro_RO',
	'russian'				=> 'ru_RU',
	'serbian'				=> 'sr_RS',
	'slovak'				=> 'sk_SK',
	'slovenian'				=> 'sl_SI',
	'spanish'				=> 'es_ES',
	'spanish_argentina'		=> 'es_LA',
	'swedish'				=> 'sv_SE',
	'turkish'				=> 'tr_TR',
	'ukrainian'				=> 'uk_UA',
);
