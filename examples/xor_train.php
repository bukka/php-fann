<?php

$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;
$desired_error = 0;
$max_epochs = 1000;
$epochs_between_reports = 10;

printf("Creating network.\n");
$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);
if (!is_resource($ann))
	die("FANN instance could not be created");

$data = fann_read_train_from_file(dirname(__FILE__) . "/xor.data");
if (!is_resource($data))
	die("Train data could not be created");

/*
fann_set_activation_steepness_hidden($ann, 1);
fann_set_activation_steepness_output($ann, 1);

fann_set_activation_function_hidden($ann, FANN_SIGMOID_SYMMETRIC);
fann_set_activation_function_output($ann, FANN_SIGMOID_SYMMETRIC);

fann_set_train_stop_function($ann, FANN_STOPFUNC_BIT);
fann_set_bit_fail_limit($ann, 0.01);

fann_set_training_algorithm($ann, FANN_TRAIN_RPROP);

fann_init_weights($ann, $data);
	
printf("Training network.\n");
fann_train_on_data($ann, $data, $max_epochs, $epochs_between_reports, $desired_error);

printf("Testing network. %f\n", fann_test_data($ann, $data));

for($i = 0; $i < fann_length_train_data($data); $i++)
{
	// use callback
	$calc_out = fann_run($ann, data->input[i]);
	printf("XOR test (%f,%f) -> %f, should be %f, difference=%f\n",
		   data->input[i][0], data->input[i][1], calc_out[0], data->output[i][0],
		   fann_abs(calc_out[0] - data->output[i][0]));
}

printf("Saving network.\n");

fann_save($ann, "xor_float.net");

$decimal_point = fann_save_to_fixed($ann, "xor_fixed.net");
fann_save_train_to_fixed($data, "xor_fixed.data", $decimal_point);
*/

printf("Cleaning up.\n");
fann_destroy_train($data);
fann_destroy($ann);
