--TEST--
Test function fann_set_activation_steepness_layer() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_activation_steepness_layer( $ann, 0.5, 1 ) );
var_dump( round( fann_get_activation_steepness( $ann, 1, 1 ), 2 ) == round( 0.5, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)