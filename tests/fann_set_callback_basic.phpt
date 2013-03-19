--TEST--
Test function fann_set_callback() by calling it with its expected arguments
--FILE--
<?php

function ann_callback( $ann, $train_data, $max_epochs, $epochs_between_reports, $desired_error, $epochs ) {
	var_dump($ann);
	var_dump($train_data);
	var_dump($max_epochs);
	var_dump($epochs_between_reports);
	var_dump($desired_error);
	var_dump($epochs);
}

$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;
$desired_error = 0.001;
$max_epochs = 50000;
$epochs_between_reports = 1000;

$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);

fann_set_callback($ann, 'ann_callback');

$filename = ( dirname( __FILE__ ) . "/fann_set_callback.tmp" );				 
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
var_dump( fann_train_on_data( $ann, $train_data, $max_epochs, $epochs_between_reports, $desired_error ) );

?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_set_callback.tmp" );
if ( file_exists( $filename ) )
	unlink( $filename );
?>
--EXPECTF--
resource(%d) of type (FANN)
resource(%d) of type (FANN Train Data)
int(%d)
int(%d)
float(%f)
int(%d)
bool(true)
