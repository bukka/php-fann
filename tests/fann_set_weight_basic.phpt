--TEST--
Test function fann_set_weight() by calling it with its expected arguments
--FILE--
<?php

$from_neuron = 0;
$to_neuron = 4;
$weight = 0.5;

$ann = fann_create_standard( 3, 3, 2, 1 );
var_dump( fann_set_weight( $ann, $from_neuron, $to_neuron, $weight ) );

$connections = fann_get_connection_array( $ann );
$conn = $connections[0];
echo "First conn: " . $conn->from_neuron . " -> " . $conn->to_neuron . " : ";
var_dump( round( $conn->weight, 1 ) );

?>
--EXPECTF--
bool(true)
First conn: 0 -> 4 : float(0.5)