<?php
$functions = get_extension_funcs('fann');
if ($functions === false)
	die("Fann extension is not loaded\n");

foreach ($functions as $fce) {
	$testfile = dirname(dirname(__FILE__)) . "/tests/${fce}_basic.phpt";
	if (!is_file($testfile))
		echo "Missing test: $fce\n";
}
