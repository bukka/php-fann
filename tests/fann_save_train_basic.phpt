--TEST--
Test function fann_save_train() by calling it with its expected arguments
--FILE--
<?php

$filename_in = ( dirname( __FILE__ ) . "/fann_save_train_in.tmp" );
$filename_out = ( dirname( __FILE__ ) . "/fann_save_train_out.tmp" );
$content_in = <<<EOF
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

file_put_contents( $filename_in, $content_in );
$train_data = fann_read_train_from_file( $filename_in );
if ( fann_save_train( $train_data, $filename_out ) ) {
    echo file_get_contents( $filename_out );
}

?>
--CLEAN--
<?php
$filename_in = ( dirname( __FILE__ ) . "/fann_save_train_in.tmp" );
if ( file_exists( $filename_in ) )
	unlink( $filename_in );
$filename_out = ( dirname( __FILE__ ) . "/fann_save_train_out.tmp" );
if ( file_exists( $filename_out ) )
	unlink( $filename_out );
?>
--EXPECTF--
4 2 1
-1 -1%w
-1 %w
-1 1%w
1%w
1 -1%w 
1%w
1 1%w 
-1%w
