<?php

function test($net, $test_data) {
	$train_file = (dirname(__FILE__) . '/' . $net . '_float.net');
	if (!is_file($train_file)) {
<<<<<<< HEAD:examples/logic_gates/test_all.php
		print('The file ' . $net . '_float.net has not been created! Please run train_all.php to generate it.<br>' . PHP_EOL);
	} else {
=======
		print('The file ' . $net . '_float.net has not been created! Please run <a href="train_all.php">train_all.php</a> to generate it.<br>' . PHP_EOL);
	} else{
>>>>>>> origin/master:examples/test_all.php
		$ann = fann_create_from_file($train_file);
		if ($ann) {
			$calc_out = fann_run($ann, $test_data);
			
			$num_inputs = count($test_data);
			$num_outputs = count($calc_out);
			
			$test_result = $net . ' test (';
			for($i = 0; $i < $num_inputs; $i++) {
				$test_result .= $test_data[$i];

				if ($i < $num_inputs - 1) {
					$test_result .= ', ';
				}
			}
			$test_result .= ') -> ';
			for($i = 0; $i < $num_outputs; $i++) {
				$test_result .= $calc_out[$i];

				if ($i < $num_outputs - 1) {
					$test_result .= ', ';
				}
			}
			print($test_result . '<br>' . PHP_EOL);
			
			fann_destroy($ann);
		} else {
			die("Invalid file format" . PHP_EOL);
		}
	}
}

test('and', array(1, 1));
test('nand', array(-1, -1));
test('nor', array(-1, -1));
test('not', array(-1));
test('or', array(1, -1));
test('xnor', array(-1, -1));
test('xor', array(-1, 1));

?>
