--TEST--
Test function fann_reset_errno() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 2, 2, 1 );
@fann_set_activation_function_layer( $ann, FANN_ELLIOT, 3 );
$errno = fann_get_errno( $ann );
var_dump( $errno > 0 );
fann_reset_errno( $ann );
var_dump( fann_get_errno( $ann ) );

?>
--EXPECT--
bool(true)
int(0)