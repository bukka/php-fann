--TEST--
Test function fann_set_cascade_activation_functions() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

$functions = array(FANN_SIGMOID, FANN_SIGMOID_SYMMETRIC);

var_dump( fann_set_cascade_activation_functions( $ann, $functions ) );

var_dump( fann_get_cascade_activation_functions( $ann ) == $functions );

?>
--EXPECTF--
bool(true)
bool(true)