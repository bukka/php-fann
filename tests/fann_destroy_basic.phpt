--TEST--
Test function fann_destroy() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 2, 3, 3 );
var_dump( fann_destroy( $ann ) );

?>
--EXPECTF--
bool(true)