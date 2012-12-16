<?php
$ann = fann_create_from_file(dirname(__FILE__) . "/xor_float.net");
$input = array(-1, 1);
$calc_out = fann_run($ann, $input);
printf("xor test (%f,%f) -> %f\n", $input[0], $input[1], $calc_out[0]);
fann_destroy($ann);