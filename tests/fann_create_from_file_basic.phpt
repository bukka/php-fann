--TEST--
Test function fann_create_from_file() by calling it with its expected arguments
--FILE--
<?php

$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;

$ann = fann_create_standard( $num_layers, $num_input, $num_neurons_hidden, $num_output );

$filename = ( dirname(__FILE__) . "/fann_create_from_file_valid.tmp" );
var_dump( fann_save( $ann, $filename ) );
var_dump( fann_create_from_file( $filename ) );

//$filename = ( dirname(__FILE__) . "/fann_create_from_file_invalid.tmp" );
// temporary use a "small" name to avoid memory corruption in libfann
$filename = tempnam(sys_get_temp_dir(), 'fann');
file_put_contents( $filename, "invalid data" );
var_dump( fann_create_from_file( $filename ) );

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_create_from_file_valid.tmp" );
if ( file_exists( $filename ) )
        unlink( $filename );
$filename = ( dirname( __FILE__ ) . "/fann_create_from_file_invalid.tmp" );
if ( file_exists( $filename ) )
        unlink( $filename );
?>
--EXPECTF--
bool(true)
resource(%d) of type (FANN)

Warning: fann_create_from_file(): Invalid configuration file '%s' in %s%efann_create_from_file_basic.php on line %d
bool(false)