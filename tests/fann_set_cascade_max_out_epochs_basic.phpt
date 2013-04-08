--TEST--
Test function fann_set_cascade_max_out_epochs() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_cascade_max_out_epochs( $ann, 140 ) );
var_dump( fann_get_cascade_max_out_epochs( $ann ) == 140 );

?>
--EXPECT--
bool(true)
bool(true)