--TEST--
Test function fann_scale_input() by calling it with its expected arguments
--FILE--
<?php

$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;
$input = array(2.2, 40);

$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);


$filename = ( dirname( __FILE__ ) . "/fann_scale_input.tmp" );
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

$new_input_min = 0.01;
$new_input_max = 0.2;
$new_output_min = -1.0;
$new_output_max = 1.0;
fann_set_scaling_params( $ann, $train_data, $new_input_min, $new_input_max, $new_output_min, $new_output_max );

var_dump(fann_scale_input($ann, $input));


?>
--CLEAN--
<?php
$filename = ( dirname( __FILE__ ) . "/fann_scale_input.tmp" );
if ( file_exists( $filename ) )
	unlink( $filename );
?>
--EXPECTF--
array(2) {
  [0]=>
  float(%f)
  [1]=>
  float(%f)
}
