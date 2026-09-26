<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | geeklog/plugins/ogp/functions.inc                                         |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2011-2022 mystral-kk - mystralkk AT gmail DOT com           |
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

/**
 * Register page-level social metadata supplied by another Geeklog plugin.
 *
 * The first successful registration wins for the current request so that
 * multiple providers cannot overwrite each other accidentally.
 *
 * @param array $metadata
 * @return bool
 */
function OGP_registerSocialMetadata($metadata)
{
    if (!is_array($metadata)) {
        return false;
    }

    if (isset($GLOBALS['_OGP_PAGE_METADATA']) && is_array($GLOBALS['_OGP_PAGE_METADATA'])) {
        return false;
    }

    $allowed = array(
        'title', 'description', 'url', 'type', 'image',
        'image_width', 'image_height', 'image_alt',
        'video', 'video_secure_url', 'video_type',
        'twitter_card', 'plugin', 'item_id', 'subtype'
    );
    $clean = array();
    foreach ($allowed as $key) {
        if (array_key_exists($key, $metadata)) {
            $clean[$key] = $metadata[$key];
        }
    }

    if (empty($clean['title']) || empty($clean['url'])) {
        return false;
    }

    $GLOBALS['_OGP_PAGE_METADATA'] = $clean;
    return true;
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
        $tagstr   = $match[0];
        $length   = MBYTE_strlen($tagstr);

        if (array_key_exists($tag, $autolinkModules)) {
            $module = $autolinkModules[$tag];
            $function = 'plugin_autotags_' . $module;
        } else {
            $module = '';
            $function = '';
        }

        if (!empty($module) && function_exists($function)) {
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
    $twitterCard = 'summary';

    // Modern provider path: a plugin may register authoritative page metadata
    // before Geeklog builds the final document header.
    $registered = isset($GLOBALS['_OGP_PAGE_METADATA']) && is_array($GLOBALS['_OGP_PAGE_METADATA'])
        ? $GLOBALS['_OGP_PAGE_METADATA']
        : array();
    if (!empty($registered)) {
        if (!empty($registered['title'])) {
            $title = $registered['title'];
        }
        if (isset($registered['description'])) {
            $desc = $registered['description'];
        }
        if (!empty($registered['url'])) {
            $url = $registered['url'];
        }
        if (!empty($registered['type'])) {
            $type = $registered['type'];
        }
        if (!empty($registered['image'])) {
            $image = $registered['image'];
        }
        if (!empty($registered['twitter_card'])) {
            $twitterCard = $registered['twitter_card'];
        }
    }

    if (empty($registered)) {
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
            // Topic archive/listing
            $type = 'website';
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
    
    }

    // <og:site_name> tag and <og:description> tag are optional, but are recommended.
    $site_name = OGP_escValue($site_name);
    $type      = OGP_escValue($type);
    $locale    = OGP_escValue(OGP_getLocale());
    $title     = OGP_escValue($title);
    $desc      = OGP_escValue($desc);

    if (empty($registered)) {
        /*
         * Historical Geeklog page-detection path.
         *
         * OGP 2.0 keeps the legacy content detection, while modernizing the
         * emitted social metadata. Images that are too small for useful link
         * previews fall back to the configured default social image.
         */
        $pathImage = str_ireplace($_CONF['site_url'] . '/', $_CONF['path_html'], $image);
        $imageDimensions = array(0, 0);

        if (@is_readable($pathImage)) {
            $detectedDimensions = @getimagesize($pathImage);
            if (is_array($detectedDimensions)
                && isset($detectedDimensions[0], $detectedDimensions[1])
            ) {
                $imageDimensions = array(
                    (int) $detectedDimensions[0],
                    (int) $detectedDimensions[1]
                );
            }
        }

        // Tiny icons (for example 48x48 topic icons) make poor social cards.
        // Prefer the configured default social image below 200x200.
        if ($imageDimensions[0] > 0 && $imageDimensions[1] > 0
            && ($imageDimensions[0] < 200 || $imageDimensions[1] < 200)
        ) {
            $image = $_OGP_CONF['fb_default_img_url'];
            $pathImage = str_ireplace($_CONF['site_url'] . '/', $_CONF['path_html'], $image);
            $imageDimensions = array(0, 0);

            if (@is_readable($pathImage)) {
                $detectedDimensions = @getimagesize($pathImage);
                if (is_array($detectedDimensions)
                    && isset($detectedDimensions[0], $detectedDimensions[1])
                ) {
                    $imageDimensions = array(
                        (int) $detectedDimensions[0],
                        (int) $detectedDimensions[1]
                    );
                }
            }
        }

        $retval = PHP_EOL
            . '<!-- Open Graph Protocol -->' . PHP_EOL
            . '<meta property="og:site_name" content="' . $site_name . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:url" content="' . $url . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:type" content="' . $type . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:locale" content="' . $locale . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:title" content="' . $title . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:image" content="' . OGP_escValue($image) . '"' . XHTML . '>' . PHP_EOL;

        if ($imageDimensions[0] > 0 && $imageDimensions[1] > 0) {
            $retval .= '<meta property="og:image:width" content="' . $imageDimensions[0] . '"' . XHTML . '>' . PHP_EOL
                . '<meta property="og:image:height" content="' . $imageDimensions[1] . '"' . XHTML . '>' . PHP_EOL;
        }

        $retval .= '<meta property="og:description" content="' . $desc . '"' . XHTML . '>' . PHP_EOL
            . '<meta name="twitter:card" content="summary_large_image"' . XHTML . '>' . PHP_EOL
            . '<meta name="twitter:title" content="' . $title . '"' . XHTML . '>' . PHP_EOL
            . '<meta name="twitter:description" content="' . $desc . '"' . XHTML . '>' . PHP_EOL;

        if (!empty($image)) {
            $retval .= '<meta name="twitter:image" content="' . OGP_escValue($image) . '"' . XHTML . '>' . PHP_EOL;
        }
    } else {
        /*
         * Modern provider rendering path.
         *
         * This path is only entered after a plugin explicitly calls
         * OGP_registerSocialMetadata(). No provider means no behavior change.
         */
        $pathImage = str_ireplace($_CONF['site_url'] . '/', $_CONF['path_html'], $image);
        $imageDimensions = array(0, 0);

        if (!empty($registered['image_width']) && !empty($registered['image_height'])) {
            $imageDimensions = array(
                (int) $registered['image_width'],
                (int) $registered['image_height']
            );
        } elseif (@is_readable($pathImage)) {
            $detectedDimensions = @getimagesize($pathImage);
            if (is_array($detectedDimensions)
                && isset($detectedDimensions[0], $detectedDimensions[1])
            ) {
                $imageDimensions = array(
                    (int) $detectedDimensions[0],
                    (int) $detectedDimensions[1]
                );
            }
        }

        $retval = PHP_EOL
            . '<!-- Open Graph Protocol -->' . PHP_EOL
            . '<meta property="og:site_name" content="' . $site_name . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:url" content="' . $url . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:type" content="' . $type . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:locale" content="' . $locale . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:title" content="' . $title . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:image" content="' . $image . '"' . XHTML . '>' . PHP_EOL
            . '<meta property="og:description" content="' . $desc . '"' . XHTML . '>' . PHP_EOL;

        if ($imageDimensions[0] > 0 && $imageDimensions[1] > 0) {
            $retval .= '<meta property="og:image:width" content="' . $imageDimensions[0] . '"' . XHTML . '>' . PHP_EOL
                . '<meta property="og:image:height" content="' . $imageDimensions[1] . '"' . XHTML . '>' . PHP_EOL;
        }

        if (!empty($registered['image_alt']) && !empty($image)) {
            $imageAlt = OGP_escValue($registered['image_alt']);
            $retval .= '<meta property="og:image:alt" content="' . $imageAlt . '"' . XHTML . '>' . PHP_EOL;
        }
        if (!empty($registered['video'])) {
            $video = OGP_escValue($registered['video']);
            $retval .= '<meta property="og:video" content="' . $video . '"' . XHTML . '>' . PHP_EOL;
        }
        if (!empty($registered['video_secure_url'])) {
            $videoSecureUrl = OGP_escValue($registered['video_secure_url']);
            $retval .= '<meta property="og:video:secure_url" content="' . $videoSecureUrl . '"' . XHTML . '>' . PHP_EOL;
        }
        if (!empty($registered['video_type'])) {
            $videoType = OGP_escValue($registered['video_type']);
            $retval .= '<meta property="og:video:type" content="' . $videoType . '"' . XHTML . '>' . PHP_EOL;
        }

        $twitterCard = OGP_escValue($twitterCard);
        $retval .= '<meta name="twitter:card" content="' . $twitterCard . '"' . XHTML . '>' . PHP_EOL
            . '<meta name="twitter:title" content="' . $title . '"' . XHTML . '>' . PHP_EOL
            . '<meta name="twitter:description" content="' . $desc . '"' . XHTML . '>' . PHP_EOL;
        if (!empty($image)) {
            $retval .= '<meta name="twitter:image" content="' . OGP_escValue($image) . '"' . XHTML . '>' . PHP_EOL;
            if (!empty($registered['image_alt'])) {
                $retval .= '<meta name="twitter:image:alt" content="' . OGP_escValue($registered['image_alt']) . '"' . XHTML . '>' . PHP_EOL;
            }
        }
    }

    return $retval;
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
            case '1.2.1':
                $defaultImageUrl = $_CONF['site_url'] . '/ogp/social-default.jpg';
                $c = config::get_instance();
                $c->set('fb_default_img_url', $defaultImageUrl, 'ogp');

                // Does nothing
                $pi_version = '1.2.2';
                break;

            case '1.2.2':
                // Does nothing
                $pi_version = '1.2.3';
                break;

            case '1.2.3':
                // OGP 2.0.0 focuses exclusively on social metadata.
                // Remove configuration entries that only served the obsolete
                // Facebook Like/Comments widgets and SDK integration.
                $c = config::get_instance();
                if (method_exists($c, 'del')) {
                    $legacySettings = array(
                        'fb_user_ids', 'fb_app_id',
                        'like_send', 'like_show_faces', 'like_style_layout',
                        'like_width', 'like_verb', 'like_color', 'like_font',
                        'comments_num_posts', 'comments_width', 'comments_color',
                        'types', 'fs_like', 'fs_comments', 'fs_kinds'
                    );
                    foreach ($legacySettings as $legacySetting) {
                        $c->del($legacySetting, 'ogp');
                    }
                }
                $pi_version = '2.0.0';
                break;

            default:
                COM_errorLog(
                    'OGP upgrade stopped: unsupported installed version ' . $pi_version
                );
                return false;

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
        'features' => array_keys($_OGP_CONF['FEATURES']),
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
