--TEST--
Test function fann_destroy_train() by calling it with its expected arguments
--FILE--
<?php

$filename = ( dirname( __FILE__ ) . "/fann_destroy_train.tmp" );
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
var_dump( fann_destroy_train( $train_data ) );

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_destroy_train.tmp" );
if ( file_exists( $filename ) )
	unlink( $filename );
?>
--EXPECT--
bool(true)
