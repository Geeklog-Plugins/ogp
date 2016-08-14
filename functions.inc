<?php
//
// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/functions.inc                                         |
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

if (stripos($_SERVER['PHP_SELF'], 'functions.inc') !== FALSE) {
    die('This file can not be used on its own.');
}

/**
* Language file Include
*/
$langfile = $_CONF['path'] . 'plugins/ogp/language/' . $_CONF['language'] . '.php';
clearstatcache();

if (file_exists($langfile)) {
    include_once $langfile;
} else {
    include_once $_CONF['path'] . 'plugins/ogp/language/english.php';
}

/**
* Loads the plugin configuration
*/
global $_OGP_CONF;

require_once $_CONF['path'] . 'plugins/ogp/config.php';
OGP_loadConfig();

// +---------------------------------------------------------------------------+
// | Geeklog Plugin API Implementations                                        |
// +---------------------------------------------------------------------------+

/**
* Returns a URL to icon
*/
function plugin_geticon_ogp() {
	global $_CONF;
	
	return $_CONF['site_admin_url'] . '/plugins/ogp/images/ogp.png';
}

/**
* Returns code to be added to <head> section
*/
function plugin_getheadercode_ogp() {
	global $_CONF, $_PLUGINS, $_TABLES, $_OGP_CONF;
	
	$type  = '';
	$title = '';
	$image = '';
	$url   = COM_getCurrentURL();
	$desc  = '';
	$facebook_ids = '';
	
	if (!OGP_isUserIdSet()) {
		COM_errorLog(OGP_str('facebook_ids_not_set'));
	} else {
		$facebook_ids = $_OGP_CONF['facebook_ids'];
	}
	
	// <og:site_name> tag and <og:description> tag are optional, but are
	// recommended.
	$retval = LB
			. '<!-- Open Graph Protocol -->' . LB
			. '<meta property="og:site_name" content="' . $_CONF['site_name']
			. '"' . XHTML . '>' . LB;
	
	if ((strcasecmp($url, $_CONF['site_url'] . '/') === 0)
	 OR (strcasecmp($url, $_CONF['site_url']) === 0)) {
		// Site HOME
		$type  = 'website';
		
		// If you have an image file representing your site, then uncomment the
		// following line and set the value to $image variable.
// 		$image = 'http://yoursite/images/site.png';
		$desc  = ($_CONF['meta_tags'] > 0)
			   ? $_CONF['meta_description']
			   : $_CONF['site_slogan'];
	} else if (preg_match("@article\.php(?:\?story=|/)(.+)$@i", $url, $match)) {
		// Article
		$sql = "SELECT s.title, s.introtext, s.bodytext, s.meta_description "
			 . "FROM {$_TABLES['stories']} AS s "
			 . "  LEFT JOIN {$_TABLES['topics']} AS t "
			 . "    ON s.tid = t.tid "
			 . "WHERE (s.sid ='" . addslashes(COM_applyFilter($match[1])) . "') "
			 . "  AND (s.draft_flag = 0) AND (s.date < NOW()) "
			 . COM_getPermSQL('AND', 0, 2, 's')
			 . COM_getTopicSQL('AND', 0, 't');
		$result = DB_query($sql);
		
		if (!DB_error() AND (DB_numRows($result) == 1)) {
			$A = DB_fetchArray($result, FALSE);
			$A = array_map('stripslashes', $A);
			
			$title     = $A['title'];
			$introtext = PLG_replaceTags($A['introtext']);
			$bodytext  = PLG_replaceTags($A['bodytext']);
			$desc      = $A['meta_description'];
			
			$type = 'blog';
			$body = $introtext . ' ' . $bodytext;
			
			if (preg_match('@<img\s\s*.*?src="(.+?)".*?>@msi', $body, $match)) {
				$image = $match[1];
				$image = OGP_fixUri($image);
			}
			
			if (empty($desc)) {
				$desc = $introtext;
			}
		}
	} else if (preg_match("@index\.php\?topic=(.+)$@i", $url, $match)) {
		// Topic
		$sql = "SELECT topic, imageurl, meta_description "
			 . "FROM {$_TABLES['topics']} "
			 . "WHERE (tid ='" . addslashes(COM_applyFilter($match[1])) . "') "
			 . COM_getTopicSQL('AND', 0);
		$result = DB_query($sql);
		
		if (!DB_error() AND (DB_numRows($result) == 1)) {
			$A = DB_fetchArray($result, FALSE);
			$A = array_map('stripslashes', $A);
			
			$type  = 'article';
			$title = $A['topic'];
			$image = OGP_fixUri($A['imageurl']);
			$desc  = $A['meta_description'];
			
			if (empty($desc)) {
				$desc = $title;
			}
		}
	} else if (in_array('staticpages', $_PLUGINS)
	 AND preg_match("@staticpages/index\.php(?:\?page=|/)(.+)$@i", $url, $match)) {
		// Staticpages
		$output  = '';
		$args    = array(
					'sp_id' => $match[1],
					'mode'  => 'autotag',
		);
		$svc_msg = array();
		
		if (PLG_invokeService('staticpages', 'get', $args, $output, $svc_msg) == PLG_RET_OK) {
			$content = SP_render_content(
				stripslashes($output['sp_content']), $output['sp_php']
			);
			$title   = $output['sp_title'];
			$desc    = $output['meta_description'];
			$type    = 'article';
			
			if (preg_match('@<img\s\s*.*?src="(.+?)".*?>@msi', $content, $match)) {
				$image = $match[1];
				$image = OGP_fixUri($image);
			}
			
			if (empty($desc)) {
				$desc = $content;
			}
        }
	}
	
	if (!empty($type) AND !empty($title) AND !empty($image)) {
		$type    = OGP_escValue($type);
		$title   = OGP_escValue($title);
		$retval	.= '<meta property="og:title" content="' . $title . '"'
				.  XHTML . '>' . LB
				.  '<meta property="og:type" content="' . $type . '"'
				.  XHTML . '>' . LB
				.  '<meta property="og:url" content="' . $url . '"'
				.  XHTML . '>' . LB
				.  '<meta property="og:image" content="' . $image . '"'
				.  XHTML . '>' . LB;
	}
	
	if (!empty($facebook_ids)) {
		$facebook_ids = OGP_escValue($facebook_ids);
		$retval .= '<meta property="fb:admins" content="' . $facebook_ids . '"'
				.  XHTML . '>' . LB;
	}
	
	if (!empty($desc)) {
		$desc    = OGP_escValue($desc);
		$retval .= '<meta property="og:description" content="' . $desc . '"'
				.  XHTML . '>' . LB;
	}
	
	return $retval;
}

/**
* Returns code to be added to footer
*
* @since  GL-1.8.0
*/
function plugin_getfootercode_ogp() {
	global $_CONF;
	
}

/**
* Checks to see if this plugin uses comments
*
* @return   boolean TRUE or FALSE
*/
function plugin_commentsupport_ogp() {
    return FALSE;
}

/**
* Puts an option for the plugin in the command and control block on moderation.php
*
* @return   array   Array containing (plugin name, admin url, url of plugin icon)
*/
function plugin_cclabel_ogp() {
	global $_CONF;
	
	if (SEC_hasRights('ogp.edit')) {
		$pi_name   = OGP_str('admin');
		
		if (!OGP_isUserIdSet()) {
			$pi_name .= '!';
		}
		
		$admin_url = $_CONF['site_admin_url'] . '/plugins/ogp/index.php';
		$icon_url  = plugin_geticon_ogp();
		
   		return array($pi_name, $admin_url, $icon_url);
	}
}

/**
* Returns the administrative option for this plugin
*
* @return   array   Array containing (plugin name, plugin admin url, # of items
* in plugin or '')
*/
function plugin_getadminoption_ogp() {
	global $_CONF, $_TABLES;
	
	if (SEC_hasRights('ogp.edit')) {
		$pi_name   = OGP_str('admin');
		
		if (!OGP_isUserIdSet()) {
			$pi_name .= '!';
		}
		
		$admin_url = $_CONF['site_admin_url'] . '/plugins/ogp/index.php';
		$num_items = '';
		
		return array($pi_name, $admin_url, $num_items);
	}
}

/**
* Returns the version for this plugin
*
* @return string VersionNo
*/
function plugin_chkVersion_ogp() {
	global $_OGP_CONF;
	
	return $_OGP_CONF['pi_version'];
}

/**
* Upgrade the plugin
*
* @return   boolean TRUE (= success)
*/
function plugin_upgrade_ogp() {
	global $_CONF, $_TABLES, $_OGP_CONF;
	
	$sql = "SELECT pi_version "
		 . "FROM {$_TABLES['plugins']} "
		 . "WHERE (pi_name = 'ogp') ";
	$result = DB_query($sql);
	$A = DB_fetchArray($result);
	$pi_version = $A['pi_version'];		// Version info stored in database
	
	// Do upgrading
	
// 	switch ($pi_version) {
// 		case '':
// 		break;
// 		
// 		case '':
// 		break;
// 		
// 	}
	
	// Updates plugin info
	
	$sql = "UPDATE {$_TABLES['plugins']} "
		 . "SET pi_version = '" . addslashes($_OGP_CONF['pi_version']) . "' "
		 . "WHERE (pi_name = 'ogp') ";
	$result = DB_query($sql);
	COM_errorLog(ucfirst("ogp") . " plugin was successfully updated to version {$_OGP_CONF['pi_version']}.");
	
	return TRUE;
}

/**
* Automatic uninstall function for plugins
*
* @return   array
*
* This code is automatically uninstalling the plugin.
* It passes an array to the core code function that removes
* tables, groups, features and php blocks from the tables.
* Additionally, this code can perform special actions that cannot be
* foreseen by the core code (interactions with other plugins for example)
*/
function plugin_autouninstall_ogp() {
	global $_OGP_CONF;
	
    $out = array (
        /* give the name of the tables, without $_TABLES[] */
        'tables' => array(),
        /* give the full name of the group, as in the db */
        'groups' => array('Open Graph Protocol Admin'),
        /* give the full name of the feature, as in the db */
        'features' => $_OGP_CONF['FEATURES'],
        /* give the full name of the block, including 'phpblock_', etc */
        'php_blocks' => array(),
        /* give all vars with their name */
        'vars'=> array()
    );
    
    return $out;
}

/**
* Loads config infor with config.class.php
*/
function OGP_loadConfig() {
	global $_CONF, $_OGP_CONF;
	
	require_once $_CONF['path_system'] . 'classes/config.class.php';
	$config = config::get_instance();
	
	if ($config->group_exists('ogp')) {
		$temp = $config->get_config('ogp');
		if (is_array($temp) AND (count($temp) > 0)) {
			$_OGP_CONF = array_merge($_OGP_CONF, $temp);
		}
	}
}

/**
* Returns a string escaped for HTML output
*/
function OGP_esc($str) {
	static $encoding = NULL;
	
	if ($encoding === NULL) {
		$encoding = COM_getCharset();
	}
	
	if (version_compare(PHP_VERSION, '5.2.3') >= 0) {
		return htmlspecialchars($str, ENT_QUOTES, $encoding, FALSE);
	} else {
		return str_replace(
			array('&amp;&amp;', '&amp;&lt;', '&amp;&gt;', '&amp;&quot;', '&amp;&#039;'),
			array('&amp;', '&lt:', '&gt;', '&quot;', '&#039;'),
			htmlspecialchars($str, ENT_QUOTES, $encoding)
		);
	}
}

/**
* Returns a lang var
*/
function OGP_str($index, $noesc = FALSE) {
	global $LANG_OGP;
	
	if (isset($LANG_OGP[$index])) {
		if ($noesc) {
			return $LANG_OGP[$index];
		} else {
			return OGP_esc($LANG_OGP[$index]);
		}
	} else {
		COM_errorLog('OGP_str: undefined index: "' . $index . '"');
		return '(undefined)';
	}
}

function OGP_escValue($str) {
	$str = OGP_esc(strip_tags($str));
	$str = str_replace(array("\n", "\r"), ' ', $str);
	$str = preg_replace('/\s\s+/ms', ' ', $str);
	
	return $str;
}

function OGP_lookupProfileId($user_name) {
	global $_CONF, $_OGP_CONF;

	require_once 'HTTP/Request.php';
	
	$retval = $user_name;
	
	$req = new HTTP_Request('https://graph.facebook.com/' . $user_name);
	$req->setMethod(HTTP_REQUEST_METHOD_GET);

	if (!PEAR::isError($req->sendRequest())) {
		$code = (int) $req->getResponseCode();
		
		if ($code === 200) {
			$result = $req->getResponseBody();
		
			if (is_callable('json_decode')) {
				$result = json_decode($result);
			
				if (!empty($result->id)) {
					$retval = $result->id;
				} else {
					COM_errorLog(__FUNCTION__ . ': Profile Id not found.');
				}
			} else {
				if (preg_match('/"id":\s*"([0-9]+)"/ims', $result, $match)) {
					$retval = $match[1];
				} else {
					COM_errorLog(__FUNCTION__ . ': Profile Id not found.');
				}
			}
		} else {
			COM_errorLog(__FUNCTION__ . ': HTTP_Request->sendRequest() failed.  HTTP Response Code = ' . $code);
		}
	} else {
		COM_errorLog(__FUNCTION__ . ': HTTP_Request->sendRequest() failed.');
	}
	
	return $retval;
}

function OGP_isUserIdSet() {
	global $_CONF, $_OGP_CONF;
	
	if ($_OGP_CONF['facebook_ids'] == '') {
		return FALSE;
	}
	
	if (!preg_match('/^\s*[0-9]+(\s*,\s*[0-9]+)*\s*$/', $_OGP_CONF['facebook_ids'])) {
		$ids = array();
		
		foreach (explode(',', $_OGP_CONF['facebook_ids']) as $user_name) {
			$user_name = trim($user_name);
			
			if (!preg_match('/^[0-9]+$/', $user_name)) {
				$user_name = OGP_lookupProfileId($user_name);
			}
			
			if (preg_match('/^[0-9]+$/', $user_name)) {
				$ids[] = $user_name;
			}
		}
		
		// Overwrites configuration
		require_once $_CONF['path_system'] . 'classes/config.class.php';
		$config = config::get_instance();
		
		if ($config->group_exists('ogp')) {
			$config->set('facebook_ids', implode(',', $ids), 'ogp');
		}
		
	}
	
	return TRUE;
}

function OGP_fixUri($uri) {
	global $_CONF;
	
	if (!preg_match('@^https?://@i', $uri)) {
		$uri = $_CONF['site_url'] . $uri;
	}
	
	return $uri;
}
