#!/usr/bin/env php
<?php

$cascade_params = array(
	'cascade_output_change_fraction' => array(
		'params' => array( 'cascade_output_change_fraction' => 'float' ),
		'comment' => 'the cascade output change fraction'
		),
	'cascade_output_stagnation_epochs' => array(
		'params' => array( 'cascade_output_stagnation_epochs' => 'float' ),
		'comment' => 'the number of cascade output stagnation epochs'
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
		foreach ($params['params'] as $param_name => $param_type)
			fpg_printf($arg, $param_name);
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
	$return_type = isset($params['return']) ? $params['return'] : reset($params['params']);
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
	$getter = "
/* {{{ proto bool fann_get_$name(resource ann)
   Returns {$params['comment']} */
PHP_FUNCTION(fann_get_$name)
{
	PHP_FANN_GET_PARAM(fann_get_$name, $return_cmd);
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
		$setter = "
/* {{{ proto bool fann_set_$name($comment_args)
   Sets {$params['comment']}*/
PHP_FUNCTION(fann_set_$name)
{
    PHP_FANN_SET_PARAM($set_args);
}
/* }}} */";
		fpg_printf($setter);
	}
}

/* process parameters array */
function fpg_process_array($action, $params_list) {
	$fce_name = "fpg_$action";
	if (function_exists($fce_name)) {
		foreach ($params_list as $name => $params) {
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

