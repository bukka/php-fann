<?php
$train_file = (dirname(__FILE__) . "/xor_float.net");
if (!is_file($train_file))
	die("The file xor_float.net has not been created! Please run simple_train.php to generate it" . PHP_EOL);

$ann = fann_create_from_file($train_file);
if ($ann) {
	$input = array(-1, 1);
	$calc_out = fann_run($ann, $input);
	printf("xor test (%f,%f) -> %f\n", $input[0], $input[1], $calc_out[0]);
	fann_destroy($ann);
} else {
	die("Invalid file format" . PHP_EOL);
}