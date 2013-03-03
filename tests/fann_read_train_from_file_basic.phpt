--TEST--
Test function fann_read_train_from_file() by calling it with its expected arguments
--FILE--
<?php

$filename = ( dirname( __FILE__ ) . "/fann_read_train_from_file.tmp" );				 
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
var_dump( fann_read_train_from_file( $filename ) );

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_read_train_from_file.tmp" );
if ( file_exists( $filename ) )
	unlink( $filename );
?>
--EXPECTF--
resource(%d) of type (FANN Train Data)