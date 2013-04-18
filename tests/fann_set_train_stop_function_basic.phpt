--TEST--
Test function fann_set_train_stop_function() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_train_stop_function( $ann, FANN_STOPFUNC_BIT ) );
var_dump( fann_get_train_stop_function( $ann ) == FANN_STOPFUNC_BIT );

?>
--EXPECT--
bool(true)
bool(true)