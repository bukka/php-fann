/*
There will be times when you need to merge separate data sets into a new single set. 

This code demonstrates how to merge the XOR dataset from two separate files into a single training file.


File (xor_part_1.data):
1 2 1
-1 -1
-1

File (xor_part_2.data):
3 2 1
-1 1
1
1 -1
1
1 1
-1

Resulting File (xor_complete.data):
4 2 1
-1 -1 
-1 
-1 1 
1 
1 -1 
1 
1 1 
-1 

*/
<?php
// Create partial data resources
$train_data_1 = fann_read_train_from_file("xor_part_1.data");
$train_data_2 = fann_read_train_from_file("xor_part_2.data");

// Merge into new data resource
$train_data = fann_merge_train_data ($train_data_1, $train_data_2);

// Remove the partial data resources from memory 
fann_destroy_train ( $train_data_1 );
fann_destroy_train ( $train_data_2 );

// Save the new complete Data file
fann_save_train ( $train_data , "xor_complete.data");

// Remove the complete data resources from memory 
fann_destroy_train ( $train_data );

echo 'All Done!' . PHP_EOL;
?>
