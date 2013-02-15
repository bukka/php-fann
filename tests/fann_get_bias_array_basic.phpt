--TEST--
Test function fann_get_bias_array() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );
$layers = fann_get_bias_array( $ann );
var_dump( count( $layers ) );
var_dump( $layers[1] );
?>
--EXPECTF--
int(3)
int(1)