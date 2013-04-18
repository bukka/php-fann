--TEST--
Test function fann_set_quickprop_decay() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_quickprop_decay( $ann, -0.0002 ) );
var_dump( round( fann_get_quickprop_decay( $ann ), 2 ) == round( -0.0002, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)