--TEST--
Test function fann_create_from_file() by calling it with its expected arguments
--FILE--
<?php

$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;

$ann = fann_create_standard( $num_layers, $num_input, $num_neurons_hidden, $num_output );

$filename = ( dirname(__FILE__) . "/fann_create_from_file.tmp" );
var_dump( fann_save( $ann, $filename ) );
var_dump( fann_create_from_file( $filename ) );

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_create_from_file.tmp" );
if ( file_exists( $filename ) )
        unlink( $filename );
?>
--EXPECTF--
bool(true)
resource(%d) of type (FANN)