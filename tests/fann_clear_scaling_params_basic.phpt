--TEST--
Test function fann_clear_scaling_params() by calling it with its expected arguments
--FILE--
<?php

$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;

$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);
var_dump( fann_clear_scaling_params( $ann ) );
?>
--EXPECTF--
bool(true)
