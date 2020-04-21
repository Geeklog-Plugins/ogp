<?php

require_once '../lib-common.php';

if (is_callable('COM_redirect')) {
    COM_redirect($_CONF['site_url');
} else {
    echo COM_refresh($_CONF['site_url']);
}
