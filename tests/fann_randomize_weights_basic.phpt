--TEST--
Test function fann_randomize_weights() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 2, 3, 3 );

var_dump( fann_randomize_weights( $ann, -0.5, 0.5 ) );

?>
--EXPECTF--
bool(true)