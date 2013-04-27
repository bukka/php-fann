<?php
$functions = get_extension_funcs('fann');
if ($functions === false)
	die("Fann extension is not loaded\n");

$testdir = dirname(dirname(__FILE__)) . "/tests/";
$testnames = array();
foreach ($functions as $fce) {
	$testname = strtolower($fce)  . "_basic.phpt";
	$testfile = $testdir . $testname;
	if (!is_file($testfile))
		echo "Missing test: $fce\n";
	else
		$testnames[] = $testname;
}



if ($dh = opendir($testdir)) {
	while (($file = readdir($dh)) !== false) {
		if ($file != '.' && $file != '..' && !in_array($file, $testnames))
			echo "Additional test file: $file\n";
	}
	closedir($dh);
}
else {
	die("Cannot open the test dir\n");
}