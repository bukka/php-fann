<?php

$fce_type = 'standard';
$fce_array = true;

$num_input = '2';
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;
$desired_error = 0.001;
$conn_rate = 0.2;
$max_epochs = 500000;
$epochs_between_reports = 1000;

$fce_name = "fann_create_" . $fce_type;

if ($fce_array) {
	$fce_name .= '_array';
	if ($fce_type == 'sparse')
		$ann = $fce_name($conn_rate, $num_layers, array($num_input, $num_neurons_hidden, $num_output));
	else
		$ann = $fce_name($num_layers, array($num_input, $num_neurons_hidden, $num_output));
}
else {
	if ($fce_type == 'sparse')
		$ann = $fce_name($conn_rate, $num_layers, $num_input, $num_neurons_hidden, $num_output);
	else
		$ann = $fce_name($num_layers, $num_input, $num_neurons_hidden, $num_output);
}

var_dump($ann);

if ($ann)
	fann_destroy($ann);
