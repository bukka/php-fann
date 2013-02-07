<?php
$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;

$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);
if (!is_resource($ann))
	die("FANN instance could not be created");



fann_destroy($ann);