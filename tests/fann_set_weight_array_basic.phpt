--TEST--
Test function fann_set_weight_array() by calling it with its expected arguments
--FILE--
<?php

$connections = array(
	new FANNConnection( 0, 4, 0.5 ),
	new FANNConnection( 1, 4, 0.2 )
	);

$ann = fann_create_standard( 3, 3, 2, 1 );
var_dump( fann_set_weight_array( $ann, $connections ) );

$connections = fann_get_connection_array( $ann );
$conn = $connections[1];
echo "First conn: " . $conn->from_neuron . " -> " . $conn->to_neuron . " : ";
var_dump( round( $conn->weight, 1 ) );

?>
--EXPECTF--
bool(true)
First conn: 1 -> 4 : float(0.2)
