--TEST--
Test function fann_set_error_log() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 2, 2, 1 );
$input = array( 0.5, 0.5 );
$filename = ( dirname( __FILE__ ) . "/fann_set_error_log.tmp" );

var_dump( fann_set_error_log( $ann, $filename ) );
var_dump( file_exists( $filename ) );

/*
// extra test to check if the function logs anything
fann_set_activation_function_hidden( $ann, FANN_ELLIOT );
fann_run( $ann, $input );
var_dump( file_get_contents( $filename ) );
*/

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_set_error_log.tmp" );
if ( file_exists( $filename ) )
	unlink( $filename );
?>
--EXPECTF--
bool(true)
bool(true)
