--TEST--
Test function fann_get_errno() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 2, 2, 1 );
@fann_set_activation_function_layer( $ann, FANN_ELLIOT, 3 );
var_dump( fann_get_errno( $ann ) );

?>
--EXPECTF--
int(%d)