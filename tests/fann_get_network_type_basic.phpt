--TEST--
Test function fann_get_network_type() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_get_network_type( $ann ) );

?>
--EXPECTF--
int(%d)