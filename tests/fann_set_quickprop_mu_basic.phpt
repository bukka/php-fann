--TEST--
Test function fann_set_quickprop_mu() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_quickprop_mu( $ann, 1.7 ) );
var_dump( round( fann_get_quickprop_mu( $ann ), 2 ) == round( 1.7, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)