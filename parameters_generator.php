#!/usr/bin/env php
<?php

$training_params = array(
	'training_algorithm' => array(
        'params' => array('training_algorithm' => ''),
        'comment' => 'the training algorithm as described by fann_train_enum',
        'test_param' => '',
	),
	'learning_rate' => array(
        'params' => array('learning_rate' => ''),
        'comment' => 'the learning rate',
        'test_param' => '',
	),
	'learning_momentum' => array(
        'params' => array('learning_momentum' => ''),
        'comment' => 'the learning momentum',
        'test_param' => '',
	),
	'activation_function' => array(
        'params' => array('activation_function' => ''),
        'comment' => 'the activation function for neuron number neuron in layer number layer, counting the input layer as layer 0',
        'test_param' => '',
	),
	'activation_steepness' => array(
        'params' => array('activation_steepness' => ''),
        'comment' => 'the activation steepness for neuron number neuron in layer number layer, counting the input layer as layer 0',
        'test_param' => '',
	),
	'train_error_function' => array(
        'params' => array('train_error_function' => ''),
        'comment' => 'the error function used during training',
        'test_param' => '',
	),
	'train_stop_function' => array(
        'params' => array('train_stop_function' => ''),
        'comment' => 'the the stop function used during training',
        'test_param' => '',
	),
	'bit_fail_limit' => array(
        'params' => array('bit_fail_limit' => ''),
        'comment' => 'the bit fail limit used during training',
        'test_param' => '',
	),
	'quickprop_decay' => array(
        'params' => array('quickprop_decay' => ''),
        'comment' => 'the decay is a small negative valued number which is the factor that the weights should become smaller in each iteration during quickprop training',
        'test_param' => '',
	),
	'quickprop_mu' => array(
        'params' => array('quickprop_mu' => ''),
        'comment' => 'the mu factor is used to increase and decrease the step-size during quickprop training',
        'test_param' => '',
	),
	'rprop_increase_factor' => array(
        'params' => array('rprop_increase_factor' => ''),
        'comment' => 'the increase factor is a value larger than 1, which is used to increase the step-size during RPROP training',
        'test_param' => '',
	),
	'rprop_decrease_factor' => array(
        'params' => array('rprop_decrease_factor' => ''),
        'comment' => 'the decrease factor is a value smaller than 1, which is used to decrease the step-size during RPROP training',
        'test_param' => '',
	),
	'rprop_delta_min' => array(
        'params' => array('rprop_delta_min' => ''),
        'comment' => 'the minimum step-size is a small positive number determining how small the minimum step-size may be',
        'test_param' => '',
	),
	'rprop_delta_max' => array(
        'params' => array('rprop_delta_max' => ''),
        'comment' => 'the maximum step-size is a positive number determining how large the maximum step-size may be',
        'test_param' => '',
	),
	'rprop_delta_zero' => array(
        'params' => array('rprop_delta_zero' => ''),
        'comment' => 'the initial step-size is a positive number determining the initial step size',
        'test_param' => '',
	),
	'sarprop_weight_decay_shift' => array(
        'params' => array('sarprop_weight_decay_shift' => ''),
        'comment' => 'the sarprop weight decay shift',
        'test_param' => '',
	),
	'sarprop_step_error_threshold_factor' => array(
        'params' => array('sarprop_step_error_threshold_factor' => ''),
        'comment' => 'the sarprop step error threshold factor',
        'test_param' => '',
	),
	'sarprop_step_error_shift' => array(
        'params' => array('sarprop_step_error_shift' => ''),
        'comment' => 'the get sarprop step error shift',
        'test_param' => '',
	),
	'sarprop_temperature' => array(
        'params' => array('sarprop_temperature' => ''),
        'comment' => 'the sarprop weight decay shift',
        'test_param' => '',
	),
);

$cascade_params = array(
	'cascade_output_change_fraction' => array(
		'params' => array( 'cascade_output_change_fraction' => 'float' ),
		'comment' => 'the cascade output change fraction',
		'test_param' => 0.02,
	),
	'cascade_output_stagnation_epochs' => array(
		'params' => array( 'cascade_output_stagnation_epochs' => 'float' ),
		'comment' => 'the number of cascade output stagnation epochs',
		'test_param' => 10,
	),
	'cascade_candidate_change_fraction' => array(
		'params' => array( 'cascade_candidate_change_fraction' => 'float' ),
		'comment' => 'the cascade candidate change fraction',
		'test_param' => 0.02,
	),
	'cascade_candidate_stagnation_epochs' => array(
		'params' => array( 'cascade_candidate_stagnation_epochs' => 'int' ),
		'comment' => 'the number of cascade candidate stagnation epochs',
		'test_param' => 10,
	),
	'cascade_weight_multiplier' => array(
		'params' => array( 'cascade_weight_multiplier' => 'fann_type' ),
		'comment' => 'the weight multiplier',
		'test_param' => 0.5,
	),
	'cascade_candidate_limit' => array(
		'params' => array( 'cascade_candidate_limit' => 'fann_type' ),
		'comment' => 'the candidate limit',
		'test_param' => 950.0,
	),
	'cascade_max_out_epochs' => array(
		'params' => array( 'cascade_max_out_epochs' => 'int' ),
		'comment' => 'the maximum out epochs',
		'test_param' => 140,
	),
	'cascade_min_out_epochs' => array(
		'params' => array( 'cascade_min_out_epochs' => 'int' ),
		'comment' => 'the minimum out epochs',
		'test_param' => 60,
	),
	'cascade_max_cand_epochs' => array(
		'params' => array( 'cascade_max_cand_epochs' => 'int' ),
		'comment' => 'the max candidate epochs',
		'test_param' => 140,
	),
	'cascade_min_cand_epochs' => array(
		'params' => array( 'cascade_min_cand_epochs' => 'float' ),
		'comment' => 'the min candidate epochs',
		'test_param' => 60,
	),
	'cascade_num_candidates' => array(
		'params' => array( 'cascade_num_candidates' => 'int' ),
		'comment' => 'the number of candidates used during training',
		'only_getter' => true
	),
	'cascade_activation_functions_count' => array(
		'params' => array( 'cascade_activation_functions_count' => 'int' ),
		'comment' => 'the number of activation functions',
		'only_getter' => true
	),
    'cascade_activation_functions' => array(
		'params' => array( 'cascade_activation_functions' => 'array' ),
		'comment' => 'the array of cascade candidate activation functions',
		'empty_function' => true
	),
	'cascade_activation_steepnesses_count' => array(
		'params' => array( 'cascade_activation_steepnesses_count' => 'int' ),
		'comment' => 'the number of activation steepnesses',
		'only_getter' => true
	),
    'cascade_activation_steepnesses' => array(
		'params' => array( 'cascade_activation_steepnesses' => 'array' ),
		'comment' => 'the cascade activation steepnesses array is an array of the different activation functions used by the candidates',
		'empty_function' => true
	),
	'cascade_num_candidate_groups' => array(
		'params' => array( 'cascade_num_candidate_groups' => 'int' ),
		'comment' => 'the number of candidate groups',
		'test_param' => 3,
	),
	
);

$fpg_stream = STDOUT;

function fpg_printf($format = "") {
	global $fpg_stream;
	$args = func_get_args();
	vfprintf($fpg_stream, $format . PHP_EOL, array_splice($args, 1));
}

/* print function headers */
function fpg_headers($name, $params) {
	fpg_printf('PHP_FUNCTION(fann_get_%s);', $name);
	if ($params['has_setter']) {
		fpg_printf('PHP_FUNCTION(fann_set_%s);', $name);
	}
}

/* print function params definitions for reflection */
function fpg_reflections($name, $params) {
	$begin = "ZEND_BEGIN_ARG_INFO(arginfo_fann_%s_$name, 0)";
	$arg = "ZEND_ARG_INFO(0, %s)";
	$end = "ZEND_END_ARG_INFO()";
	// getter
	fpg_printf($begin, 'get');
	fpg_printf($arg, 'ann');
	fpg_printf($end);
	fpg_printf(); // EOL
	// setter
	if ($params['has_setter']) {
		fpg_printf($begin, 'set');
		fpg_printf($arg, 'ann');
		foreach ($params['params'] as $param_name => $param_type) {
			fpg_printf($arg, $param_name);
		}
		fpg_printf($end);
		fpg_printf();
	}
}

/* print function definitions for the module spec */
function fpg_definitions($name, $params) {
	$format = "\tPHP_FE(fann_%-41s arginfo_fann_%s_$name)";
	fpg_printf($format, "get_$name,", "get");
	if ($params['has_setter'])
		fpg_printf($format, "set_$name,", "set");
}

/* print functions body */
function fpg_functions($name, $params) {
	$return_type = $params['return'];
	$return_cmd = 'RETURN_';
	switch ($return_type) {
	  case 'float':
	  case 'double':
	  case 'fann_type':
		  $return_cmd .= 'DOUBLE';
		  break;
	  case 'int':
		  $return_cmd .= 'LONG';
		  break;
	}
	$body = $params['empty_function'] ? "": "PHP_FANN_GET_PARAM(fann_get_$name, $return_cmd);";
	$getter = "
/* {{{ proto bool fann_get_$name(resource ann)
   Returns {$params['comment']} */
PHP_FUNCTION(fann_get_$name)
{
	$body
}
/* }}} */";
	fpg_printf($getter);
	if ($params['has_setter']) {
		$set_args = "fann_set_$name";
		$comment_args = "resource ann";
		foreach ($params['params'] as $param_name => $param_type) {
			$comment_args .= sprintf(", %s %s", $param_type == 'fann_type' ? 'double' : $param_type, $param_name);
			$set_args .= sprintf(", %s", $param_type == 'int' ? 'l, long' : 'd, double');
		}
		$body = $params['empty_function'] ? "": "PHP_FANN_SET_PARAM($set_args);";
		$setter = "
/* {{{ proto bool fann_set_$name($comment_args)
   Sets {$params['comment']}*/
PHP_FUNCTION(fann_set_$name)
{
    $body
}
/* }}} */";
		fpg_printf($setter);
	}
}

/* generate testes */
function fpg_tests($name, $params) {
	$file_path = (dirname(__FILE__) . "/tests/fann_%s_{$name}_basic.phpt");
	// getter
	$getter_test_file = sprintf($file_path, 'get');
	$getter_test_content = '--TEST--
Test function fann_get_' . $name . '() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_get_' . $name . '( $ann ) );

?>
--EXPECTF--
' . ($params['return'] == 'int' ? 'int(%d)' : ($params['return'] == 'array' ? 'array(%d) {%A}' : 'float(%f)'));
	if ($params['empty_test']) {
		$getter_test_content = (substr($getter_test_content, 0, strpos($getter_test_content, '<?php') + 6) .
								substr($getter_test_content, strpos($getter_test_content, '?>') - 1));
	}
	file_put_contents($getter_test_file, $getter_test_content);
	// setter
	if ($params['has_setter'] && isset($params['test_param'])) {
		$setter_test_file = sprintf($file_path, 'set');
		if (in_array($params['return'], array('float', 'double', 'fann_type')))
			$setter_cmp_cond = 'round( fann_get_'  .$name . '( $ann ), 2 ) == round( ' . $params['test_param'] . ', 2 )';
		else
			$setter_cmp_cond = 'fann_get_'  .$name . '( $ann ) == ' . $params['test_param'];
		$setter_test_content = '--TEST--
Test function fann_set_' . $name . '() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_' . $name . '( $ann, ' . $params['test_param'] . ' ) );
var_dump( ' . $setter_cmp_cond . ' );

?>
--EXPECT--
bool(true)
bool(true)';
		file_put_contents($setter_test_file, $setter_test_content);
	}
}

/* process parameters array */
function fpg_process_array($action, $params_list) {
	$fce_name = "fpg_$action";
	if (function_exists($fce_name)) {
		foreach ($params_list as $name => $params) {
            if (!isset($params['return']))
                $params['return'] = reset($params['params']);
			$params['empty_function'] = isset($params['empty_function']) && $params['empty_function'];
			$params['empty_test'] = isset($params['empty_test']) && $params['empty_test'];
			$params['has_setter'] = !isset($params['only_getter']) || !$params['only_getter'];
			$fce_name($name, $params);
		}
	}
	else {
		fprintf(STDERR, "Argument $action is not a valid action\n");
	}
}

if ($argc < 2)
	die("You need to provide arguments\n");

for ($i = 1; $i < $argc; $i++) {
	fpg_process_array($argv[$i], $cascade_params);
}

