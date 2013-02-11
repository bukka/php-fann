--TEST--
Test function fann_create_standard_array() by calling it with its expected arguments
--FILE--
<?php
   
$num_layers = 2;
$num_neurons1 = 3;
$num_neurons2 = 2;

var_dump( fann_create_standard_array( $num_layers, array( $num_neurons1, $num_neurons2 ) ) );

$num_layers++;
$num_neurons3 = 2;
var_dump( fann_create_standard_array( $num_layers, array( $num_neurons1, $num_neurons2, $num_neurons3 ) ) );


?>
--EXPECTF--
resource(%d) of type (FANN)
resource(%d) of type (FANN)
