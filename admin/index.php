<?php

// +---------------------------------------------------------------------------+
// | Open Graph Protocol Plugin for Geeklog - The Ultimate Weblog              |
// +---------------------------------------------------------------------------+
// | public_html/admin/plugins/ogp/index.php                                   |
// +---------------------------------------------------------------------------+
// | Copyright (C) 2011-2018 mystral-kk - mystralkk AT gmail DOT com           |
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

require_once '../../../lib-common.php';

// Only let admin users access this page
if (!SEC_hasRights('ogp.edit')) {
	// Someone is trying to illegally access this page
	COM_errorLog("Someone has tried to illegally access the ogp Admin page.  User id: {$_USER['uid']}, Username: {$_USER['username']}, IP: {$_SERVER['REMOTE_ADDR']}", 1);
	$content = COM_startBlock(OGP_esc($LANG_ACCESS['accessdenied']))
			 . OGP_esc($LANG_ACCESS['plugin_access_denied_msg'])
			 . COM_endBlock();
	
	if (is_callable('COM_createHTMLDocument')) {
		$display = COM_createHTMLDocument($content);
	} else {
		$display = COM_siteHeader()
				 . $content
				 . COM_siteFooter();
	}
	
	if (is_callable('COM_output')) {
		COM_output($display);
	} else {
		echo $display;
	}

	exit;
}

if (is_callable('COM_redirect')) {
	COM_redirect($_CONF['site_admin_url'] . '/configuration.php');
} else {
	echo COM_refresh($_CONF['site_admin_url'] . '/configuration.php');
}
