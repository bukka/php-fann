<?php
// Use this code to split your data into smaller sets.
// Useful for splitting your training data into training and testing groups

// Load Data
$data_file = fann_read_train_from_file(dirname(__FILE__) . "/MyTrainingData.data");
$train_data = fann_read_train_from_file(dirname(__FILE__) . DIRECTORY_SEPARATOR . $data_file);

// Calculate how many examples are in the first group
$total_length = fann_length_train_data($train_data);
$a_length = floor($total_length / 10);

// Split the subsets
$training_data_a  = fann_subset_train_data($train_data, 0, $a_length);
$training_data_b  = fann_subset_train_data($train_data, $a_length, $total_length-$a_length);

// Save the training data to separate files
fann_save_train ($training_data_a, 'MyTrainingData_Subset_A.data'); // 1/10 of the training data
fann_save_train ($training_data_b, 'MyTrainingData_Subset_B.data'); // 9/10 of the training data
