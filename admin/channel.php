<?php

require_once '../../../lib-common.php';

$cache_expire = 60 * 60 * 24 * 365;
$locale = OGP_getLocale();

header('Pragma: public');
header('Cache-Control: maxage=' . $expires);
header('Expires: ' . gmdate('D, d M Y H:i:s', time() + $cache_expire) . ' GMT');
echo '<script src="http://connect.facebook.net/' . $locale . '"></script>';
