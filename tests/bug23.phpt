--TEST--
Test bug #23: Segmenation fault after fann_destroy
--FILE--
<?php
$num_input = 2;
$num_output = 1;
$num_layers = 3;
$num_neurons_hidden = 3;
$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);
// destroy is the key - there is no segfault without it
fann_destroy($ann);
$ann = fann_create_standard($num_layers, $num_input, $num_neurons_hidden, $num_output);
var_dump($ann);
// now any accesss will segfault
var_dump(fann_get_errno($ann));

?>
--EXPECTF--
resource(%d) of type (FANN)
int(0)
