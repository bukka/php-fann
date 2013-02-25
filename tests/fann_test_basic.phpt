--TEST--
Test function fann_test() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );
$input = array( 0.5, 1.5, 0.5 );
$desired_output = array( 1 );

var_dump( fann_test( $ann, $input, $desired_output ) );

?>
--EXPECTF--
array(1) {%a[0]=>%afloat(%f)%a}