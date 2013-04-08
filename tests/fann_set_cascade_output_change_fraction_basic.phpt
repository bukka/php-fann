--TEST--
Test function fann_set_cascade_output_change_fraction() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_cascade_output_change_fraction( $ann, 0.02 ) );
var_dump( round( fann_get_cascade_output_change_fraction( $ann ), 2 ) == round( 0.02, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)