#!/usr/bin/env php
<?php

$cascade_params = array(
	'cascade_output_change_fraction' => array(
		'params' => array( 'cascade_output_change_fraction' => 'float' ),
		'comment' => 'the cascade output change fraction',
	),
	'cascade_output_stagnation_epochs' => array(
		'params' => array( 'cascade_output_stagnation_epochs' => 'float' ),
		'comment' => 'the number of cascade output stagnation epochs',
	),
	'cascade_candidate_change_fraction' => array(
		'params' => array( 'cascade_candidate_change_fraction' => 'float' ),
		'comment' => 'the cascade candidate change fraction',
	),
	'cascade_candidate_stagnation_epochs' => array(
		'params' => array( 'cascade_candidate_stagnation_epochs' => 'int' ),
		'comment' => 'the number of cascade candidate stagnation epochs',
	),
	'cascade_weight_multiplier' => array(
		'params' => array( 'cascade_weight_multiplier' => 'fann_type' ),
		'comment' => 'the weight multiplier',
	),
	'cascade_candidate_limit' => array(
		'params' => array( 'cascade_candidate_limit' => 'fann_type' ),
		'comment' => 'the candidate limit',
	),
	'cascade_max_out_epochs' => array(
		'params' => array( 'cascade_max_out_epochs' => 'int' ),
		'comment' => 'the maximum out epochs',
	),
	'cascade_min_out_epochs' => array(
		'params' => array( 'cascade_min_out_epochs' => 'int' ),
		'comment' => 'the minimum out epochs',
	),
	'cascade_max_cand_epochs' => array(
		'params' => array( 'cascade_max_cand_epochs' => 'int' ),
		'comment' => 'the max candidate epochs',
	),
	'cascade_min_cand_epochs' => array(
		'params' => array( 'cascade_min_cand_epochs' => 'float' ),
		'comment' => 'the min candidate epochs',
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
		$setter_test_content = '--TEST--
Test function fann_set_' . $name . '() by calling it with its expected arguments
--FILE--
<?php

$ann = fann_create_standard( 3, 3, 2, 1 );

var_dump( fann_set_' . $name . '( $ann, ' . $params['test_param'] . ' ) );
var_dump( fann_get_'  .$name . '( $ann ) == ' . $params['test_param'] . ' );

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

