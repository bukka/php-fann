--TEST--
Test function fann_get_connection_array() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );
$connections = fann_get_connection_array( $ann );
foreach ( $connections as $conn )
{
	echo "Instance of FANNConnection: ";
	var_dump( $conn instanceof FANNConnection );
}
$conn = $connections[0];
echo "First conn: " . $conn->from_neuron . " -> " . $conn->to_neuron . " : ";
var_dump( $conn->weight );

?>
--EXPECTF--
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
Instance of FANNConnection: bool(true)
First conn: 0 -> 4 : float(%f)
