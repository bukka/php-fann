--TEST--
Test function fann_set_training_algorithm() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_training_algorithm( $ann, FANN_TRAIN_QUICKPROP ) );
var_dump( fann_get_training_algorithm( $ann ) == FANN_TRAIN_QUICKPROP );

?>
--EXPECT--
bool(true)
bool(true)