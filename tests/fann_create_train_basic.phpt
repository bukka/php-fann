--TEST--
Test function fann_create_train() by calling it with its expected arguments
--SKIPIF--
<?php if ( version_compare( FANN_VERSION, "2.2" ) < 0 ) die("Skip: FANN version lower than 2.2"); ?>
--FILE--
<?php

$num_data = 3;
$num_input = 2;
$num_output = 1;
$train_data = fann_create_train( $num_data, $num_input, $num_output );
if ($train_data) {
	$filename = ( dirname( __FILE__ ) . "/fann_create_train.tmp" );
	fann_save_train( $train_data, $filename );
	echo file_get_contents( $filename );
}

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_create_train.tmp" );
if ( file_exists( $filename ) )
	unlink( $filename );
?>
--EXPECTF--
3 2 1
0 0%w
0%w
0 0%w
0%w
0 0%w
0%w
