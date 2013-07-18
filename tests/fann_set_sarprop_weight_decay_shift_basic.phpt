--TEST--
Test function fann_set_sarprop_weight_decay_shift() by calling it with its expected arguments
--SKIPIF--
<?php if ( version_compare( FANN_VERSION, "2.2" ) < 0 ) die("Skip: FANN version lower than 2.2"); ?>
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_sarprop_weight_decay_shift( $ann, -6.744 ) );
var_dump( round( fann_get_sarprop_weight_decay_shift( $ann ), 2 ) == round( -6.744, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)