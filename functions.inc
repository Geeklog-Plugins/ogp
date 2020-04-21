<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/functions.inc                                         |
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
    die('This file can not be used on its own.');
}

// Language file Include
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

if (!is_callable('DB_escapeString')) {
    function DB_escapeString($string) {
        return addslashes($string);
    }
}

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

    if (isset($_CONF['disable_autolinks']) && ($_CONF['disable_autolinks'] == 1)) {
        // autolinks are disabled - return $content unchanged
        return $content;
    }

    $recursive = array();   // Checks for recursive call of [staticpage_content:]
    $start_pos = 0;
    $autolinkModules = PLG_collectTags();

    while (($start_pos < MBYTE_strlen($content))
     && preg_match('/\[(.+?)\s*:([^\]]*)\]/ms', $content, $match, 0, $start_pos)) {
        $tag      = $match[1];
        $module   = $autolinkModules[$tag];
        $tagstr   = $match[0];
        $length   = MBYTE_strlen($tagstr);
        $function = 'plugin_autotags_' . $module;

        if (array_key_exists($tag, $autolinkModules) && function_exists($function)) {
            $params = explode(' ', $match[2]);

            if (is_array($params) && (count($params) > 0)) {
                $param1 = $params[0];
            } else {
                $param1 = '';
            }

            $args = array(
                'module'    => $module,
                'tag'       => $tag,
                'tagstr'    => $tagstr,
                'startpos'  => $start_pos,
                'length'    => $length,
                'parm1'     => $param1,
                'parm2'     => $match[2],
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
                        $recursive[$sp_id] = true;
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
* Returns code to be added to <head> section
*/
function plugin_getheadercode_ogp() {
    global $_CONF, $_PLUGINS, $_TABLES, $_OGP_CONF;

    // Sets default values
    $site_name = $_CONF['site_name'];
    $type      = 'article';
    $title     = $_CONF['site_name'] . ' - ' . $_CONF['site_slogan'];
    $image     = $_OGP_CONF['fb_default_img_url'];
    $url       = COM_getCurrentURL();
    $desc      = ($_CONF['meta_tags'] > 0) ? $_CONF['meta_description'] : $_CONF['site_slogan'];

    // Tags specific to Facebook
    $fb_user_ids = '';
    $fb_app_id   = '';

    if (OGP_isFbUserIdSet()) {
        $fb_user_ids = trim($_OGP_CONF['fb_user_ids']);
    }

    if (OGP_isFbAppIdSet()) {
        $fb_app_id = trim($_OGP_CONF['fb_app_id']);
    }

    if (empty($fb_user_ids) && empty($fb_app_id)) {
        COM_errorLog(OGP_str('fb_ids_not_set'));
    }

    if ((strcasecmp($url, $_CONF['site_url'] . '/') === 0) ||
        (strcasecmp($url, $_CONF['site_url']) === 0)) {
        // Site HOME
        $type = 'website';
    } elseif (stripos($url, $_CONF['site_url'] . '/article.php') === 0) {
        // Article
        COM_setArgNames(array('story'));
        $sid = COM_getArgument('story');
        $sid = COM_applyFilter($sid);
        $sid = DB_escapeString($sid);

        if (version_compare(VERSION, '2.0.0') >= 0) {
            $sql = "SELECT s.title, s.introtext, s.bodytext, s.meta_description "
                 . "FROM {$_TABLES['stories']} AS s, {$_TABLES['topic_assignments']} AS ta "
                 . "WHERE (ta.type = 'article') AND (ta.id = s.sid) "
                 . "  AND (s.sid ='{$sid}') "
                 . "  AND (s.draft_flag = 0) AND (s.date < NOW()) "
                 . COM_getPermSQL('AND', 0, 2, 's')
                 . COM_getTopicSQL('AND', 0, 'ta')
                 . "GROUP BY title, introtext, bodytext, meta_description ";
        } else {
            $sql = "SELECT s.title, s.introtext, s.bodytext, s.meta_description "
                 . "FROM {$_TABLES['stories']} AS s "
                 . "  LEFT JOIN {$_TABLES['topics']} AS t "
                 . "    ON s.tid = t.tid "
                 . "WHERE (s.sid ='{$sid}') "
                 . "  AND (s.draft_flag = 0) AND (s.date < NOW()) "
                 . COM_getPermSQL('AND', 0, 2, 's')
                 . COM_getTopicSQL('AND', 0, 't')
                 . "GROUP BY title, introtext, bodytext, meta_description ";
        }

        $result = DB_query($sql);

        if (!DB_error() && (DB_numRows($result) == 1)) {
            $A = DB_fetchArray($result, false);
            $A = array_map('stripslashes', $A);

            $title     = $A['title'];
            $introtext = OGP_replaceTags($A['introtext']);
            $bodytext  = OGP_replaceTags($A['bodytext']);
            $desc      = $A['meta_description'];
            $body = $introtext . ' ' . $bodytext;

            if (preg_match('@<img\s\s*.*?src="(.+?)".*?>@si', $body, $match)) {
                $image = $match[1];
                $image = OGP_fixUri($image);
            }

            if (empty($desc)) {
                $desc = $introtext;
            }
        }
    } elseif (preg_match("@index\.php\?topic=(.+)$@i", $url, $match)) {
        // Topic
        $tid = $match[1];
        $tid = DB_escapeString(COM_applyFilter($tid));
        $sql = "SELECT topic, imageurl, meta_description "
             . "FROM {$_TABLES['topics']} "
             . "WHERE (tid ='{$tid}') "
             . COM_getTopicSQL('AND', 0);
        $result = DB_query($sql);

        if (!DB_error() && (DB_numRows($result) == 1)) {
            $A = DB_fetchArray($result, false);
            $A = array_map('stripslashes', $A);

            $title = $A['topic'];
            $image = OGP_fixUri($A['imageurl']);
            $desc  = $A['meta_description'];

            if (empty($desc)) {
                $desc = $title;
            }
        }
    } elseif (in_array('staticpages', $_PLUGINS) &&
               preg_match("@staticpages/index\.php(?:\?page=|/)(.+)$@i", $url, $match)) {
        // Staticpages
        $sp_id = $match[1];
        $sp_id = COM_applyFilter($sp_id);
        $output  = '';
        $args    = array(
                    'sp_id' => $sp_id,
                    'mode'  => 'autotag',
        );
        $svc_msg = array();

        if (PLG_invokeService('staticpages', 'get', $args, $output, $svc_msg) == PLG_RET_OK) {
            $content = $output['sp_content'];
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
    } elseif (in_array('calendar', $_PLUGINS) &&
               preg_match("@calendar/event\.php\?eid=(.+)$@i", $url, $match)) {
        // Calendar
        $eid = $match[1];
        $eid = DB_escapeString(COM_applyFilter($eid));
        $sql = "SELECT title, description, event_type, location "
             . "FROM {$_TABLES['events']} "
             . "WHERE (eid = '{$eid}') "
             . COM_getPermSQL('AND');
        $result = DB_query($sql);

        if (!DB_error() && (DB_numRows($result) == 1)) {
            $A = DB_fetchArray($result, false);
            $title = $A['title'];
            $desc  = $A['description'];

            if (preg_match('@<img\s\s*.*?src="(.+?)".*?>@msi', $desc, $m)) {
                $image = $m[1];
                $image = OGP_fixUri($image);
            }
        }
    } elseif (in_array('calendarjp', $_PLUGINS) &&
               preg_match("@calendarjp/event\.php\?eid=(.+)$@i", $url, $match)) {
        // Calendar
        $eid = $match[1];
        $eid = DB_escapeString(COM_applyFilter($eid));
        $sql = "SELECT title, description, event_type, location "
             . "FROM {$_TABLES['eventsjp']} "
             . "WHERE (eid = '{$eid}') "
             . COM_getPermSQL('AND');
        $result = DB_query($sql);

        if (!DB_error() && (DB_numRows($result) == 1)) {
            $A = DB_fetchArray($result, false);
            $title = $A['title'];
            $desc  = $A['description'];

            if (preg_match('@<img\s\s*.*?src="(.+?)".*?>@msi', $desc, $m)) {
                $image = $m[1];
                $image = OGP_fixUri($image);
            }
        }
    }

    // <og:site_name> tag and <og:description> tag are optional, but are recommended.
    $site_name = OGP_escValue($site_name);
    $type      = OGP_escValue($type);
    $locale    = OGP_escValue(OGP_getLocale());
    $title     = OGP_escValue($title);
    $desc      = OGP_escValue($desc);

    $pathImage = str_ireplace($_CONF['site_url'] . '/', $_CONF['path_html'], $image);
    $imageDimensions = getimagesize($pathImage);

    $retval = PHP_EOL
        . '<!-- Open Graph Protocol -->' . PHP_EOL
        . '<meta property="og:site_name" content="' . $site_name . '"' . XHTML . '>' . PHP_EOL
        . '<meta property="og:url" content="' . $url . '"' . XHTML . '>' . PHP_EOL
        . '<meta property="og:type" content="' . $type . '"' . XHTML . '>' . PHP_EOL
        . '<meta property="og:locale" content="' . $locale . '"' . XHTML . '>' . PHP_EOL
        . '<meta property="og:title" content="' . $title . '"' . XHTML . '>' . PHP_EOL
        . '<meta property="og:image" content="' . $image . '"' . XHTML . '>' . PHP_EOL
        . '<meta property="og:image:width" content="' . $imageDimensions[0] . '"' . XHTML . '>' . PHP_EOL
        . '<meta property="og:image:height" content="' . $imageDimensions[1] . '"' . XHTML . '>' . PHP_EOL
        . '<meta property="og:description" content="' . $desc . '"' . XHTML . '>' . PHP_EOL;

    if (!empty($fb_app_id)) {
        $fb_app_id = OGP_escValue($fb_app_id);
        $retval .= '<meta property="fb:app_id" content="' . $fb_app_id . '"' .  XHTML . '>' . PHP_EOL;
    }

    if (!empty($fb_user_ids)) {
        $fb_user_ids = OGP_escValue($fb_user_ids);
        $retval .= '<meta property="fb:admins" content="' . $fb_user_ids . '"' .  XHTML . '>' . PHP_EOL;
    }

    return $retval;
}

/**
* Returns code to be added to footer
*
* @since  GL-1.8.0
*/
function plugin_getfootercode_ogp() {
    global $_CONF, $_OGP_CONF;

    if (!OGP_isFbAppIdSet()) {
        return '';
    }

    $appId = trim($_OGP_CONF['fb_app_id']);
    $locale = OGP_getLocale();
    $srcURL = sprintf(
        'https://connect.facebook.net/%s/sdk.js#xfbml=1&version=v6.0&appId=%s',
        $locale,
        $appId
    );
    $retval = '<div id="fb-root"></div>' . PHP_EOL
        . '<script async defer crossorigin="anonymous" src="' . $srcURL . '"></script>';

    return $retval;
}

/**
* Checks to see if this plugin uses comments
*
* @return   boolean true or false
*/
function plugin_commentsupport_ogp() {
    return false;
}

/**
* Puts an option for the plugin in the command and control block on moderation.php
*
* @return   array   Array containing (plugin name, admin url, url of plugin icon)
*/
function plugin_cclabel_ogp() {
    global $_CONF;

    if (SEC_hasRights('ogp.edit')) {
        $pi_name = OGP_str('admin');

        if (!OGP_isFbUserIdSet() || !OGP_isFbAppIdSet()) {
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
        $pi_name = OGP_str('admin');

        if (!OGP_isFbUserIdSet() || !OGP_isFbAppIdSet()) {
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
* @return   boolean true (= success)
*/
function plugin_upgrade_ogp() {
    global $_CONF, $_TABLES, $_OGP_CONF;

    $pi_version = DB_getItem($_TABLES['plugins'], 'pi_version', "pi_name = 'ogp'");
    $done = false;

    while ($pi_version !== $_OGP_CONF['pi_version']) {
        switch ($pi_version) {
            case '1.0.0':
            case '1.0.1':
            case '1.0.2':
            case '1.0.4':
            case '1.1.0':
            case '1.1.1':
                require_once dirname(__FILE__) . '/install_defaults.php';

                $c = config::get_instance();
                $c->add('fs_kinds', null, 'fieldset', 0, 3, null, 0, true, 'ogp');
                $c->add(
                    'types', $_OGP_DEFAULT['types'], '%text', 0, 3, null, 140,
                    true, 'ogp'
                );
                $pi_version = '1.1.2';
                break;

            case '1.1.2':
            case '1.1.3':
            case '1.1.4':
            case '1.1.5':
            case '1.1.6':
            case '1.1.7':
            case '1.1.8':
            case '1.1.9';
            case '1.2.0':
                $defaultImageUrl = $_CONF['site_admin_url'] . '/plugins/ogp/images/default.png';
                $c = config::get_instance();
                $c->set('fb_default_img_url', $defaultImageUrl, 'ogp');

                // Does nothing
                $pi_version = '1.2.1';
                break;

            default:
                break;

        }
    }

    // Updates plugin info
    $sql = "UPDATE {$_TABLES['plugins']} "
         . "SET pi_version = '" . addslashes($_OGP_CONF['pi_version']) . "' "
         . "WHERE (pi_name = 'ogp') ";
    $result = DB_query($sql);
    COM_errorLog(ucfirst("ogp") . " plugin was successfully updated to version {$_OGP_CONF['pi_version']}.");

    return true;
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
        // give the name of the tables, without $_TABLES[]
        'tables' => array(),
        // give the full name of the group, as in the db
        'groups' => array('Open Graph Protocol Admin'),
        // give the full name of the feature, as in the db
        'features' => $_OGP_CONF['FEATURES'],
        // give the full name of the block, including 'phpblock_', etc
        'php_blocks' => array(),
        // give all vars with their name
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

        if (is_array($temp) && (count($temp) > 0)) {
            $_OGP_CONF = array_merge($_OGP_CONF, $temp);
            $_OGP_CONF['fb_app_id'] = trim(@$_OGP_CONF['fb_app_id']);
            $_OGP_CONF['fb_user_ids'] = trim(@$_OGP_CONF['fb_user_ids']);
        }
    }
}

/**
* Provide URL of a documentation file
*
* @param   string  $file   documentation file being requested, e.g. 'config'
* @return  mixed           URL or false when not available
*/
function plugin_getdocumentationurl_ogp($file) {
    global $_CONF;

    static $doc_url = null;

    if ($doc_url === null) {
        switch ($file) {
            case 'index':
            case 'config':
                $lang = COM_getLanguageName();
                $path = 'admin/plugins/ogp/docs/';

                if (!file_exists($_CONF['path_html'] . $path . $lang . '/ogp.html')) {
                    $lang = 'english';
                }

                $doc_url = $_CONF['site_url'] . '/' . $path . $lang . '/ogp.html';
                break;

            default:
                $doc_url = false;
                break;
        }
    }

    return $doc_url;
}

/**
* Provides text for a Configuration tooltip
*
* @param    string  $id     Id of config value
* @return   mixed           Text to use regular tooltip, null to use config 
*                           tooltip hack, or empty string when not available
*/
function plugin_getconfigtooltip_ogp($id) {
    // Use config tooltip hack where tooltip is read from the config documentation
    return null;
}

/**
* Returns a string escaped for HTML output
*/
function OGP_esc($str) {
    static $encoding = null;

    if ($encoding === null) {
        $encoding = COM_getCharset();
    }

    if (version_compare(PHP_VERSION, '5.2.3') >= 0) {
        return htmlspecialchars($str, ENT_QUOTES, $encoding, false);
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
function OGP_str($index, $noesc = false) {
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
    global $_OGP_CONF;

    return true;
}

function OGP_isFbAppIdSet() {
    global $_OGP_CONF;

    return true;
}

function OGP_fixUri($uri) {
    global $_CONF;

    if (!preg_match('@^https?://@i', $uri)) {
        $uri = $_CONF['site_url'] . $uri;
    }

    return $uri;
}

function OGP_getLocale() {
    global $_CONF, $_OGP_CONF;

    $lang = COM_getLanguageName();

    if (array_key_exists($lang, $_OGP_CONF['LOCALE_MAPPINGS'])) {
        return $_OGP_CONF['LOCALE_MAPPINGS'][$lang];
    } else {
        return 'en_US';
    }
}

function OGP_gerArgs($str) {
    global $_OGP_CONF;

    $retval = array();
    $parts = explode(' ', trim(strtolower($str)));

    if (is_array($parts) && (count($parts) > 0)) {
        foreach ($parts as $part) {
            $temp = explode(':', trim($part), 2);

            if (is_array($temp) && (count($temp) === 2)) {
                $key   = trim($temp[0]);
                $value = trim($temp[1]);

                switch ($key) {
                    case 'send':
                    case 'show_faces':
                        if ($value === 'true') {
                            $retval[$key] = true;
                        } elseif ($value === 'false') {
                            $retval[$key] = false;
                        }

                        break;

                    case 'width':
                    case 'num_posts':
                        if (preg_match('/^[0-9]+$/', $value)) {
                            $retval[$key] = (int) $value;
                        }

                        break;

                    case 'style_layout':
                        if (in_array($value, $_OGP_CONF['LAYOUTS'])) {
                            $retval[$key] = $value;
                        }

                        break;

                    case 'verb':
                        if (($value === 'like') || ($value === 'recommend')) {
                            $retval[$key] = $value;
                        }

                        break;

                    case 'color':
                        if (($value === 'light') || ($value === 'dark')) {
                            $retval[$key] = $value;
                        }

                        break;

                    case 'font':
                        if (in_array($value, $_OGP_CONF['FONTS'])) {
                            $retval[$key] = $value;
                        }

                        break;

                    default:
                        break;
                }
            }
        }
    }

    return $retval;
}

/**
* Checks if the content on the current URL is supported by OGP plugin
*
* @param   string  $url
* @return  boolean true = supported, false = otherwise
*/
function OGP_isSupportedItem($url) {
    global $_PLUGINS, $_TABLES, $_OGP_CONF;

    $pi_version = DB_getItem($_TABLES['plugins'], 'pi_version', "pi_name = 'ogp'");

    // Silently updates the plugin
    if ((version_compare($pi_version, '1.1.2') < 0) &&
        (version_compare($_OGP_CONF['pi_version'], '1.1.2') >= 0)) {
        // Silently upgrades the plugin to prevent configuration error
        plugin_upgrade_ogp();
    }

    // Temporary fix
    return true;

    if (!isset($_OGP_CONF['types']) || !is_array($_OGP_CONF['types'])) {
        $_OGP_CONF['types'] = array(
            'article', 'staticpages', 'calendar', 'calendarjp', 'links',
            'polls', 'filemgmt', 'downloads',
        );
    }

    $retval = (in_array('article', $_OGP_CONF['types']) && preg_match("@article\.php(?:\?story=|/)(.+)$@i", $url)) ||
        (in_array('staticpages', $_PLUGINS) && in_array('staticpages', $_OGP_CONF['types']) && preg_match("@staticpages/index\.php(?:\?page=|/)(.+)$@i", $url)) ||
        (in_array('calendar', $_PLUGINS) && in_array('calendar', $_OGP_CONF['types']) && preg_match("@calendar/event\.php\?eid=(.+)$@i", $url)) ||
        (in_array('calendarjp', $_PLUGINS) && in_array('calendarjp', $_OGP_CONF['types']) && preg_match("@calendarjp/event\.php\?eid=(.+)$@i", $url)) ||
        (in_array('links', $_PLUGINS) && in_array('links', $_OGP_CONF['types']) && preg_match("@links/index\.php(?:\?category=(.+))?$@i", $url)) ||
        (in_array('polls', $_PLUGINS) && in_array('polls', $_OGP_CONF['types']) && preg_match("@polls/index\.php(?:\?pid=(.+))?$@i", $url)) ||
        (in_array('filemgmt', $_PLUGINS) && in_array('filemgmt', $_OGP_CONF['types']) && preg_match("@filemgmt/index\.php(?:\?id=|/)\d+$@i", $url)) ||
        (in_array('downloads', $_PLUGINS) && in_array('downloads', $_OGP_CONF['types']) && preg_match("@downloads/index\.php(?:\?id=|/).+$@i", $url));

    return $retval;
}

/**
* Returns a code snippet for Facebook "Like" button
*
* @param   array  $params
* @return  string
*/
function OGP_like($params = array()) {
    global $_CONF, $_TABLES, $_OGP_CONF;

    $retval = '';
    $url    = COM_getCurrentURL();
    $locale = OGP_getLocale();

    if (!OGP_isSupportedItem($url)) {
        return $retval;
    }

    if (!OGP_isFbAppIdSet()) {
        COM_errorLog(OGP_str('fb_appid_not_set'));

        return $retval;
    }

    $send = isset($params['send']) ? $params['send'] : $_OGP_CONF['like_send'];
    $send = $send ? 'true' : 'false';
    $show_faces = isset($params['show_faces']) ? $params['show_faces'] : $_OGP_CONF['like_show_faces'];
    $show_faces = $show_faces ? 'true' : 'false';

    $style_layout = isset($params['style_layout']) ? $params['style_layout'] : $_OGP_CONF['like_style_layout'];

    $width = isset($params['width']) ? $params['width'] : (int) $_OGP_CONF['like_width'];

    if (($style_layout === 'standard') && ($width < 225)) {
        $width = 225;
    } elseif (($style_layout === 'button_count') && ($width < 90)) {
        $width = 90;
    } elseif (($style_layout === 'box_count') && ($width < 55)) {
        $width = 55;
    }

    $appId = trim($_OGP_CONF['fb_app_id']);
    $verb  = isset($params['verb']) ? $params['verb'] : $_OGP_CONF['like_verb'];
    $color = isset($params['color']) ? $params['color'] : $_OGP_CONF['like_color'];
    $font  = isset($params['font']) ? $params['font'] : $_OGP_CONF['like_font'];

    if (!empty($font)) {
        $font = ' data-font="' . $font . '"';
    }

    $retval = <<<EOD
<div class="fb-like" data-href="{$url}" data-send="{$send}" data-width="{$width}" data-show-faces="{$show_faces}" data-layout="{$style_layout}" data-action="{$verb}" data-colorscheme="{$color}"{$font}></div>
EOD;

    return $retval;
}

/**
* Returns a code snippet for Facebook "Comments" button
*
* @param   array  $params
* @return  string
*/
function OGP_comments($params = array()) {
    global $_CONF, $_TABLES, $_OGP_CONF;

    $retval = '';
    $url    = COM_getCurrentURL();
    $locale = OGP_getLocale();

    if (OGP_isSupportedItem($url)) {
        if (OGP_isFbAppIdSet()) {
            $num_posts = isset($params['num_posts']) ? $params['num_posts'] : (int) $_OGP_CONF['comments_num_posts'];

            if ($num_posts < 1) {
                $num_posts = 2;
            }

            $width = isset($params['width']) ? $params['width'] : (int) $_OGP_CONF['comments_width'];

            if ($width < 400) {
                $width = 400;
            }

            $color = isset($params['color']) ? $params['color'] : $_OGP_CONF['comments_color'];

            $retval = <<<EOD
<div class="fb-comments" data-href="{$url}" data-num-posts="{$num_posts}" data-width="{$width}" data-colorscheme="{$color}"></div>
EOD;
        } else {
            COM_errorLog(OGP_str('fb_appid_not_set'));
        }
    }

    return $retval;
}

/**
* Implements the [ogp:] autotag.
*
*/
function plugin_autotags_ogp($op, $content = '', $autotag = '') {
    global $_CONF, $_TABLES, $_OGP_CONF, $_GROUPS;

    if ($op === 'tagname') {
        return array('ogp', 'fb');
    } elseif ($op === 'parse') {
        $url     = COM_getCurrentURL();
        $what    = COM_applyFilter($autotag['parm1']);
        $params  = OGP_gerArgs($autotag['parm2']);
        $replace = '';

        switch ($what) {
            case 'like':
                $replace = OGP_like($params);
                break;

            case 'comments':
                $replace = OGP_comments($params);
                break;

            default:
                $replace = 'Unknown tag!';
                break;
        }

        $content = str_replace($autotag['tagstr'], $replace, $content);

        return $content;
    }
}

/**
* Exports template vars 'ogp_like' (Like button) and 'ogp_comments' (Comments) variables
*/
function plugin_templatesetvars_ogp($templatename, $template) {
    $like     = OGP_like();
    $comments = OGP_comments();
    $template->set_var('ogp_like', $like);
    $template->set_var('ogp_comments', $comments);
}
