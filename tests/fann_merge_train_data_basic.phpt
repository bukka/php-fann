--TEST--
Test function fann_merge_train_data() by calling it with its expected arguments
--FILE--
<?php

$filename = ( dirname( __FILE__ ) . "/fann_merge_train_data_1.tmp" );
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
$train_data1 = fann_read_train_from_file( $filename );

$filename = ( dirname( __FILE__ ) . "/fann_merge_train_data_2.tmp" );
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
$train_data2 = fann_read_train_from_file( $filename );

$merge_train_data = fann_merge_train_data( $train_data1, $train_data2 );

var_dump( $merge_train_data );

?>
--CLEAN--
<?php
for ( $i = 1; $i <= 2; $i++ ) {
	$filename = ( dirname( __FILE__ ) . "/fann_merge_train_data_$i.tmp" );
	if ( file_exists( $filename ) )
		unlink( $filename );
}
?>
--EXPECTF--
resource(%d) of type (FANN Train Data)