--TEST--
Test function fann_copy() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 2, 2, 1 );

var_dump( $ann_copy = fann_copy( $ann ) );
var_dump( $ann_copy === $ann );
?>
--EXPECTF--
resource(%d) of type (FANN)
bool(false)
