--TEST--
Test function fann_set_rprop_delta_zero() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_rprop_delta_zero( $ann, 0.2 ) );
var_dump( round( fann_get_rprop_delta_zero( $ann ), 2 ) == round( 0.2, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)