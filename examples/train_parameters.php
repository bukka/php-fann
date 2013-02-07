<?php
$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;

$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);
if (!is_resource($ann))
	die("FANN instance could not be created");

$fann_algorithms = array(
	FANN_TRAIN_INCREMENTAL,
	FANN_TRAIN_BATCH,
	FANN_TRAIN_RPROP,
	FANN_TRAIN_QUICKPROP,
	FANN_TRAIN_SARPROP
);

echo "Algorithm tests\n";
foreach ($fann_algorithms as $algorithm) {
	fann_set_training_algorithm($ann, $algorithm);
	var_dump($algorithm == fann_get_training_algorithm($ann));
}
		
fann_destroy($ann);