--TEST--
Test function fann_test_data() by calling it with its expected arguments
--FILE--
<?php
$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;
$desired_error = 0.001;
$max_epochs = 50000;
$epochs_between_reports = 1000;

$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);

$filename = ( dirname( __FILE__ ) . "/fann_test_data.tmp" );				 
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
var_dump( fann_test_data( $ann, $train_data ) );

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_test_data.tmp" );
if ( file_exists( $filename ) )
	unlink( $filename );
?>
--EXPECTF--
float(%f)
