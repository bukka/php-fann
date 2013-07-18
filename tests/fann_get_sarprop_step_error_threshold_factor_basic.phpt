--TEST--
Test function fann_get_sarprop_step_error_threshold_factor() by calling it with its expected arguments
--SKIPIF--
<?php if ( version_compare( FANN_VERSION, "2.2" ) < 0 ) die("Skip: FANN version lower than 2.2"); ?>
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_get_sarprop_step_error_threshold_factor( $ann ) );

?>
--EXPECTF--
float(%f)