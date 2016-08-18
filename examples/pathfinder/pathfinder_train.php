<?php
$num_input = 25;
$num_output = 25;
$num_layers = 3;
$num_neurons_hidden = 70;
$desired_error = 0.001;
$max_epochs = 5000000;
$epochs_between_reports = 1000;

$pathfinder_ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);


if ($pathfinder_ann) {
	fann_set_activation_function_hidden($pathfinder_ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output($pathfinder_ann, FANN_SIGMOID_SYMMETRIC);
	

	$filename = dirname(__FILE__) . "/pathfinder.data";
	if (fann_train_on_file($pathfinder_ann, $filename, $max_epochs, $epochs_between_reports, $desired_error))
		fann_save($pathfinder_ann, dirname(__FILE__) . "/pathfinder_float.net");
		
	fann_destroy($pathfinder_ann);
	echo "<h1 class='green'>Training Complete!</h1><a href='pathfinder_test.php'>Test Pathfinder</a>";
}
?>