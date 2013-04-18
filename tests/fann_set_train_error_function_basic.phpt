--TEST--
Test function fann_set_train_error_function() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_train_error_function( $ann, FANN_ERRORFUNC_LINEAR ) );
var_dump( fann_get_train_error_function( $ann ) == FANN_ERRORFUNC_LINEAR );

?>
--EXPECT--
bool(true)
bool(true)