--TEST--
Test function fann_set_learning_momentum() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_learning_momentum( $ann, 0.2 ) );
var_dump( round( fann_get_learning_momentum( $ann ), 2 ) == round( 0.2, 2 ) );

?>
--EXPECT--
bool(true)
bool(true)