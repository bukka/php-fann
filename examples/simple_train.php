<?php

$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;
$desired_error = 0.001;
$max_epochs = 500000;
$epochs_between_reports = 1000;

$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);

fann_set_activation_function_hidden($ann, FANN_SIGMOID_SYMMETRIC);
fann_set_activation_function_output($ann, FANN_SIGMOID_SYMMETRIC);

var_dump($ann);

if (is_resource($ann)) {
	
	fann_destroy($ann);
}
