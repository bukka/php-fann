<?php

function train($data, $num_input, $num_output, $num_layers, $num_neurons_hidden, $desired_error, $max_epochs, $epochs_between_reports){
	
	$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);

	if ($ann) {
		fann_set_activation_function_hidden($ann, FANN_SIGMOID_SYMMETRIC);
		fann_set_activation_function_output($ann, FANN_SIGMOID_SYMMETRIC);

		$filename = dirname(__FILE__) . '/' . $data . '.data';
		if (fann_train_on_file($ann, $filename, $max_epochs, $epochs_between_reports, $desired_error)) {
			print($data . ' trained.<br>' . PHP_EOL);
        }
		
		if (fann_save($ann, dirname(__FILE__) . '/' . $data . '_float.net')) {
			print($data . '_float.net saved.<br>' . PHP_EOL);
        }
		
		fann_destroy($ann);
	}
}

train('and', 2, 1, 3, 3, 0.001, 500000, 1000);
train('nand', 2, 1, 3, 3, 0.001, 500000, 1000);
train('nor', 2, 1, 3, 3, 0.001, 500000, 1000);
train('not', 1, 1, 3, 3, 0.001, 500000, 1000);
train('or', 2, 1, 3, 3, 0.001, 500000, 1000);
train('xnor', 2, 1, 3, 3, 0.001, 500000, 1000);
train('xor', 2, 1, 3, 3, 0.001, 500000, 1000);


print("<a href='test_all.php'>Test All</a>");

?>