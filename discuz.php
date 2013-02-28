<?php
$br = (php_sapi_name() == "cli")? "":"<br>";

if(!extension_loaded('discuz')) {
	dl('discuz.' . PHP_SHLIB_SUFFIX);
}
$module = 'discuz';
$functions = get_extension_funcs($module);
echo "Functions available in the test extension:$br\n";
foreach($functions as $func) {
    echo $func."$br\n";
}
echo "$br\n";
$function = 'confirm_' . $module . '_compiled';
if (extension_loaded($module)) {
	$str = $function($module);
} else {
	$str = "Module $module is not compiled into PHP";
}
echo "$str\n";

date_default_timezone_set('Asia/Shanghai');

$str = 'abc';
$key = 'J1118bSbp47227s8X0DfT6nazfo3h61drdF91as5G4sfS8W1I623g5p0E2O9E3o6';
$expiry = 1000;

$php_encode = authcode($str, 'ENCODE', $key, $expiry);
$php_decode = authcode($php_encode, 'DECODE', $key);
echo "php encode:$php_encode\n";
echo "php decode:$php_decode\n";

$ext_encode = discuz_auth_encode($str, $key, $expiry);
$ext_decode = discuz_auth_decode($ext_encode, $key);
echo "ext encode:$ext_encode\n";
echo "ext decode:$ext_decode\n";

$php2ext = discuz_auth_decode($php_encode, $key);
$ext2php = authcode($ext_encode, 'DECODE', $key);
echo "php2ext decode:$php2ext\n";
echo "ext2php decode:$ext2php\n";

function authcode($string, $operation = 'DECODE', $key = '', $expiry = 0) {
	$ckey_length = 4;
	$key = md5($key);
	$keya = md5(substr($key, 0, 16));
	$keyb = md5(substr($key, 16, 16));
	$keyc = $ckey_length ? ($operation == 'DECODE' ? substr($string, 0, $ckey_length): substr(md5(microtime()), -$ckey_length)) : '';

	$cryptkey = $keya.md5($keya.$keyc);
	$key_length = strlen($cryptkey);

	$string = $operation == 'DECODE' ? base64_decode(substr($string, $ckey_length)) : sprintf('%010d', $expiry ? $expiry + time() : 0).substr(md5($string.$keyb), 0, 16).$string;
	$string_length = strlen($string);

	$result = '';
	$box = range(0, 255);

	$rndkey = array();
	for($i = 0; $i <= 255; $i++) {
		$rndkey[$i] = ord($cryptkey[$i % $key_length]);
	}

	for($j = $i = 0; $i < 256; $i++) {
		$j = ($j + $box[$i] + $rndkey[$i]) % 256;
		$tmp = $box[$i];
		$box[$i] = $box[$j];
		$box[$j] = $tmp;
	}

	for($a = $j = $i = 0; $i < $string_length; $i++) {
		$a = ($a + 1) % 256;
		$j = ($j + $box[$a]) % 256;
		$tmp = $box[$a];
		$box[$a] = $box[$j];
		$box[$j] = $tmp;
		$result .= chr(ord($string[$i]) ^ ($box[($box[$a] + $box[$j]) % 256]));
	}

	if($operation == 'DECODE') {
		if((substr($result, 0, 10) == 0 || substr($result, 0, 10) - time() > 0) && substr($result, 10, 16) == substr(md5(substr($result, 26).$keyb), 0, 16)) {
			return substr($result, 26);
		} else {
			return '';
		}
	} else {
		return $keyc.str_replace('=', '', base64_encode($result));
	}
}
?>
