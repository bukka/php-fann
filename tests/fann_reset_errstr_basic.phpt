--TEST--
Test function fann_reset_errstr() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 2, 2, 1 );
@fann_set_activation_function_layer( $ann, FANN_ELLIOT, 3 );
$err = fann_get_errstr( $ann );
var_dump( strlen($err) > 0 );
fann_reset_errstr( $ann );
var_dump( fann_get_errstr( $ann ) );

?>
--EXPECT--
bool(true)
string(0) ""