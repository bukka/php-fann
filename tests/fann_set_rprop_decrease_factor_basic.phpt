--TEST--
Test function fann_set_rprop_decrease_factor() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_rprop_decrease_factor( $ann, 0.6 ) );
var_dump( round( fann_get_rprop_decrease_factor( $ann ), 2 ) == round( 0.6, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)