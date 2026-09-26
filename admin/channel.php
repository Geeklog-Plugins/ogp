<?php

require_once '../../../lib-common.php';

$cache_expire = 60 * 60 * 24 * 365;
$locale = OGP_getLocale();

header("Pragma: public");
header("Cache-Control: max-age=" . $cache_expire);
header('Expires: ' . gmdate('D, d M Y H:i:s', time() + $cache_expire) . ' GMT');
echo '<script src="https://connect.facebook.net/' . $locale . '"></script>';
