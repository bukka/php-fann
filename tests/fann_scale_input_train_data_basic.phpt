--TEST--
Test function fann_scale_input_train_data() by calling it with its expected arguments
--FILE--
<?php

$filename = ( dirname( __FILE__ ) . "/fann_scale_input_train_data.tmp" );
$content = <<<EOF
4 2 1
-1 -1
-1
-1 1
1
1 -1
1
1 1
-1
EOF;

file_put_contents( $filename, $content );
$train_data = fann_read_train_from_file( $filename );

var_dump( fann_scale_input_train_data( $train_data, 0.1, 2.3 ) );

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_scale_input_train_data.tmp" );
if ( file_exists( $filename ) )
	unlink( $filename );
?>
--EXPECT--
bool(true)
