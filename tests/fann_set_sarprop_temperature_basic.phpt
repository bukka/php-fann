--TEST--
Test function fann_set_sarprop_temperature() by calling it with its expected arguments
--SKIPIF--
<?php if ( version_compare( FANN_VERSION, "2.2" ) < 0 ) die("Skip: FANN version lower than 2.2"); ?>
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_sarprop_temperature( $ann, 0.02 ) );
var_dump( round( fann_get_sarprop_temperature( $ann ), 2 ) == round( 0.02, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)