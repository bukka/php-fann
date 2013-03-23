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

function fpg_printf($format) {
	global $fpg_stream;
	$args = func_get_args();
	vfprintf($fpg_stream, $format, array_splice($args, 1));
}

/* function parameters generate data headers */
function fpgd_headers($name, $params) {
	fpg_printf('PHP_FUNCTION(fann_get_%s);' . PHP_EOL, $name);
	if ($params['has_setter']) {
		fpg_printf('PHP_FUNCTION(fann_set_%s);' . PHP_EOL, $name);
	}
}


function fpg_process_array($action, $params_list) {
	$fce_name = "fpgd_$action";
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

