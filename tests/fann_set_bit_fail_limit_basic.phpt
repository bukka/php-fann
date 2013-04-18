--TEST--
Test function fann_set_bit_fail_limit() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_bit_fail_limit( $ann, 0.5 ) );
var_dump( round( fann_get_bit_fail_limit( $ann ), 2 ) == round( 0.5, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)