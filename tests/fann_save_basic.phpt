--TEST--
Test function fann_save() by calling it with its expected arguments
--FILE--
<?php

$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;

$ann = fann_create_standard( $num_layers, $num_input, $num_neurons_hidden, $num_output );

$filename = ( dirname(__FILE__) . "/fann_save.tmp" );
var_dump( fann_save( $ann, $filename ) );

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_save.tmp" );
if ( file_exists( $filename ) )
        unlink( $filename );
?>
--EXPECT--
bool(true)