<?php

$data_file = fann_read_train_from_file(dirname(__FILE__) . "/MyTrainingData.data");

$train_data = fann_read_train_from_file(dirname(__FILE__) . DIRECTORY_SEPARATOR . $data_file);

$total_length = fann_length_train_data($train_data);
$a_length = floor($total_length / 10);

$training_data_a  = fann_subset_train_data($train_data, 0, $a_length);
$training_data_b  = fann_subset_train_data($train_data, $a_length, $total_length-$a_length);

fann_save_train ($training_data_a, 'MyTrainingData_Subset_A.data'); // 1/10 of the training data
fann_save_train ($training_data_b, 'MyTrainingData_Subset_B.data'); // 9/10 of the training data


