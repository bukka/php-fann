--TEST--
Test function fann_run() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 2, 2, 1 );
$input = array( 0.5, 0.5 );

var_dump( fann_run( $ann, $input ) );

?>
--EXPECTF--
array(1) {%a[0]=>%afloat(%f)%a}