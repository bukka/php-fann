--TEST--
Test function fann_set_cascade_output_stagnation_epochs() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_cascade_output_stagnation_epochs( $ann, 10 ) );
var_dump( fann_get_cascade_output_stagnation_epochs( $ann ) == 10 );

?>
--EXPECT--
bool(true)
bool(true)