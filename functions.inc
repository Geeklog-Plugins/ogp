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
* This function will allow plugins to support the use of custom autolinks
* in other site content. Plugins can now use this API when saving content
* and have the content checked for any autolinks before saving.
* The autolink would be like:  [story:20040101093000103 here]
*
* @param   string   $content   Content that should be parsed for autolinks
*/
function OGP_replaceTags($content) {
	global $_CONF, $_TABLES, $LANG32;

	if (isset($_CONF['disable_autolinks']) AND ($_CONF['disable_autolinks'] == 1)) {
		// autolinks are disabled - return $content unchanged
		return $content;
	}
	
	$recursive = array();	// Checks for recursive call of [staticpage_content:]
	$start_pos = 0;
	$autolinkModules = PLG_collectTags();
	
	while (($start_pos < MBYTE_strlen($content))
	 AND preg_match('/\[(.+?)\s*:([^\]]*)\]/ms', $content, $match, 0, $start_pos)) {
		$tag      = $match[1];
		$module   = $autolinkModules[$tag];
		$tagstr   = $match[0];
		$length   = MBYTE_strlen($tagstr);
		$function = 'plugin_autotags_' . $module;
		
		if (array_key_exists($tag, $autolinkModules)
		 AND function_exists($function)) {
			$params = explode(' ', $match[2]);
			
			if (is_array($params) AND (count($params) > 0)) {
				$param1 = $params[0];
			} else {
				$param1 = '';
			}
			
			$args = array(
				'module'	=> $module,
				'tag'		=> $tag,
				'tagstr'	=> $tagstr,
				'startpos'	=> $start_pos,
				'length'	=> $length,
				'parm1'		=> $param1,
				'parm2'		=> $match[2],
			);
			
			// Calling plugin_autotags_staticpages() with the same $sp_id more
			// than once returns '', so let's use our own code.
			if ($tag === 'staticpage_content') {
	            $sp_id = COM_applyFilter($param1);
				
	            if (!empty($sp_id)) {
					if (isset($recursive[$sp_id])) {
						COM_errorLog(__FUNCTION__ . ': Recursive call of [staticpage_content:' . $sp_id . '] tag detected.');
						$sp_content = '';
					} else {
						$recursive[$sp_id] = TRUE;
						$sp_content = SP_returnStaticpage($sp_id, 'autotag');
					}
					
                    $content = str_replace($tagstr, $sp_content, $content);
	            }
			} else {
				$content = $function('parse', $content, $args);
			}
		} else {
			$start_pos += $length;
		}
	}
	
	return $content;
}

/**
* Render the actual content of a static page (without any surrounding blocks)
*
* @param    string  $sp_content the content (HTML or PHP source)
* @param    int     $sp_php     flag: 1 = content is PHP source, 0 = is HTML
* @param    string  $query      optional search query string to highlight
* @return   string              rendered content (HTML)
*
*/
function OGP_SP_render_content($sp_content, $sp_php, $query = '') {
	global $_SP_CONF, $LANG_STATIC;

	$retval = '';

	if ($_SP_CONF['allow_php'] == 1) {
		// Check for type (ie html or php)
		if ($sp_php == 1) {
			$retval = eval($sp_content);
		} elseif ($sp_php == 2) {
			ob_start();
			eval($sp_content);
			$retval = ob_get_contents();
			ob_end_clean();
		} else {
			$retval = $sp_content;
		}
		$retval = OGP_replaceTags($retval);
		if (! empty($query)) {
			$retval = COM_highlightQuery($retval, $query);
		}
	} else {
		if ($sp_php != 0) {
			COM_errorLog("PHP in static pages is disabled. Can not display page.", 1);
			$retval .= $LANG_STATIC['deny_msg'];
		} else {
			$retval .= OGP_replaceTags($sp_content);
			if (! empty($query)) {
				$retval = COM_highlightQuery($retval, $query);
			}
		}
	}

	return $retval;
}

/**
* Returns code to be added to <head> section
*/
function plugin_getheadercode_ogp() {
	global $_CONF, $_PLUGINS, $_TABLES, $_OGP_CONF;
	
	// Sets default values
	$site_name = $_CONF['site_name'];
	$type      = 'article';
	$title     = $_CONF['site_name'] . ' - ' . $_CONF['site_slogan'];
	$image     = $_CONF['site_admin_url'] . '/plugins/ogp/images/anon.png';
	$url       = COM_getCurrentURL();
	$desc      = ($_CONF['meta_tags'] > 0)
			   ? $_CONF['meta_description']
			   : $_CONF['site_slogan'];
	
	// Tags specific to Facebook
	$fb_user_ids = '';
	$fb_app_id   = '';
	
	if (OGP_isFbUserIdSet()) {
		$fb_user_ids = trim($_OGP_CONF['fb_user_ids']);
	}
	
	if (OGP_isFbAppIdSet()) {
		$fb_app_id = trim($_OGP_CONF['fb_app_id']);
	}
	
	if (empty($fb_user_ids) AND empty($fb_app_id)) {
		COM_errorLog(OGP_str('fb_ids_not_set'));
	}
	
	if ((strcasecmp($url, $_CONF['site_url'] . '/') === 0)
	 OR (strcasecmp($url, $_CONF['site_url']) === 0)) {
		// Site HOME
		$type = 'website';
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
			$introtext = OGP_replaceTags($A['introtext']);
			$bodytext  = OGP_replaceTags($A['bodytext']);
			$desc      = $A['meta_description'];
			
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
			$content = OGP_SP_render_content(
				stripslashes($output['sp_content']), $output['sp_php']
			);
			$title   = $output['sp_title'];
			$desc    = $output['meta_description'];
			
			if (preg_match('@<img\s\s*.*?src="(.+?)".*?>@msi', $content, $match)) {
				$image = $match[1];
				$image = OGP_fixUri($image);
			}
			
			if (empty($desc)) {
				$desc = $content;
			}
		}
	}
	
	// <og:site_name> tag and <og:description> tag are optional, but are
	// recommended.
	$site_name = OGP_escValue($site_name);
	$type      = OGP_escValue($type);
	$title     = OGP_escValue($title);
	$desc      = OGP_escValue($desc);
	
	$retval = LB
			. '<!-- Open Graph Protocol -->' . LB
			. '<meta property="og:site_name" content="' . $site_name . '"'
			. XHTML . '>' . LB
			. '<meta property="og:url" content="' . $url . '"' 
			. XHTML . '>' . LB
			. '<meta property="og:type" content="' . $type . '"'
			. XHTML . '>' . LB
			. '<meta property="og:title" content="' . $title . '"'
			. XHTML . '>' . LB
			. '<meta property="og:image" content="' . $image . '"'
			. XHTML . '>' . LB
			. '<meta property="og:description" content="' . $desc . '"'
			. XHTML . '>' . LB;
	
	if (!empty($fb_app_id)) {
		$fb_app_id = OGP_escValue($fb_app_id);
		$retval .= '<meta property="fb:app_id" content="' . $fb_app_id . '"'
				.  XHTML . '>' . LB;
	}
	
	if (!empty($fb_user_ids)) {
		$fb_user_ids = OGP_escValue($fb_user_ids);
		$retval .= '<meta property="fb:admins" content="' . $fb_user_ids . '"'
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
		
		if (!OGP_isFbUserIdSet() AND !OGP_isFbAppIdSet()) {
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
		
		if (!OGP_isFbUserIdSet() AND !OGP_isFbAppIdSet()) {
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

function OGP_isFbUserIdSet() {
	global $_CONF, $_OGP_CONF;
	
	if (preg_match('/^\s*[0-9]+(\s*,\s*[0-9]+)*\s*$/', $_OGP_CONF['fb_user_ids'])) {
		return TRUE;
	} else {
		return FALSE;
	}
}

function OGP_isFbAppIdSet() {
	global $_CONF, $_OGP_CONF;
	
	if (preg_match('/^\s*[0-9]+\s*$/', $_OGP_CONF['fb_app_id'])) {
		return TRUE;
	} else {
		return FALSE;
	}
}

function OGP_fixUri($uri) {
	global $_CONF;
	
	if (!preg_match('@^https?://@i', $uri)) {
		$uri = $_CONF['site_url'] . $uri;
	}
	
	return $uri;
}
