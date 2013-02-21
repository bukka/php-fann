--TEST--
Test function fann_reset_MSE() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_reset_MSE( $ann ) );

?>
--EXPECTF--
bool(true)