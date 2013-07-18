--TEST--
Test function fann_create_train_from_callback() by calling it with its expected arguments
--SKIPIF--
<?php if ( version_compare( FANN_VERSION, "2.2" ) < 0 ) die("Skip: FANN version lower than 2.2"); ?>
--FILE--
<?php

function create_train_callback( $num_data, $num_input, $num_output ) {
	return array(
		"input" => array_fill(0, $num_input, 1),
		"output" => array_fill(0, $num_output, 1),
	);
}

$num_data = 3;
$num_input = 2;
$num_output = 1;
$train_data = fann_create_train_from_callback( $num_data, $num_input, $num_output, "create_train_callback" );
if ($train_data) {
	$filename = ( dirname( __FILE__ ) . "/fann_create_train_from_callback.tmp" );
	fann_save_train( $train_data, $filename );
	echo file_get_contents( $filename );
}

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_create_train_from_callback.tmp" );
if ( file_exists( $filename ) )
	unlink( $filename );
?>
--EXPECTF--
3 2 1
1 1%w
1%w
1 1%w
1%w
1 1%w
1%w
