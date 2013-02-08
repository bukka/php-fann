/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_FANN_H
#define PHP_FANN_H

extern zend_module_entry fann_module_entry;
#define phpext_fann_ptr &fann_module_entry

#ifdef PHP_WIN32
#	define PHP_FANN_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_FANN_API __attribute__ ((visibility("default")))
#else
#	define PHP_FANN_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(fann);
PHP_MSHUTDOWN_FUNCTION(fann);
PHP_RINIT_FUNCTION(fann);
PHP_RSHUTDOWN_FUNCTION(fann);
PHP_MINFO_FUNCTION(fann);

/* FANN Creation/Execution */
/* Creation, Destruction & Execution */
PHP_FUNCTION(fann_create_standard);
PHP_FUNCTION(fann_create_standard_array);
PHP_FUNCTION(fann_create_sparse);
PHP_FUNCTION(fann_create_sparse_array);
PHP_FUNCTION(fann_create_shortcut);
PHP_FUNCTION(fann_create_shortcut_array);
PHP_FUNCTION(fann_run);
PHP_FUNCTION(fann_destroy);
/* Parameters */
PHP_FUNCTION(fann_get_num_input);
PHP_FUNCTION(fann_get_num_output);
PHP_FUNCTION(fann_get_total_neurons);
PHP_FUNCTION(fann_get_total_connections);
PHP_FUNCTION(fann_get_network_type);
PHP_FUNCTION(fann_get_connection_rate);
PHP_FUNCTION(fann_get_num_layers);

/* FANN Training */
/* Training Data Training */
PHP_FUNCTION(fann_train_on_file);
/* Training Data Manipulation */
PHP_FUNCTION(fann_read_train_from_file);
PHP_FUNCTION(fann_destroy_train);
/* Parameters */
PHP_FUNCTION(fann_get_training_algorithm);
PHP_FUNCTION(fann_set_training_algorithm);
PHP_FUNCTION(fann_get_learning_rate);
PHP_FUNCTION(fann_set_learning_rate);
PHP_FUNCTION(fann_get_learning_momentum);
PHP_FUNCTION(fann_set_learning_momentum);
PHP_FUNCTION(fann_set_activation_function_hidden); 
PHP_FUNCTION(fann_set_activation_function_output);
PHP_FUNCTION(fann_set_activation_steepness_hidden);
PHP_FUNCTION(fann_set_activation_steepness_output);
PHP_FUNCTION(fann_get_train_error_function);
PHP_FUNCTION(fann_set_train_error_function);
PHP_FUNCTION(fann_get_train_stop_function);
PHP_FUNCTION(fann_set_train_stop_function);
PHP_FUNCTION(fann_get_bit_fail_limit);
PHP_FUNCTION(fann_set_bit_fail_limit);
PHP_FUNCTION(fann_set_callback);
PHP_FUNCTION(fann_get_quickprop_decay);
PHP_FUNCTION(fann_set_quickprop_decay);
PHP_FUNCTION(fann_get_quickprop_mu);
PHP_FUNCTION(fann_set_quickprop_mu);
PHP_FUNCTION(fann_get_rprop_increase_factor);
PHP_FUNCTION(fann_set_rprop_increase_factor);
PHP_FUNCTION(fann_get_rprop_decrease_factor);
PHP_FUNCTION(fann_set_rprop_decrease_factor);
PHP_FUNCTION(fann_get_rprop_delta_min);
PHP_FUNCTION(fann_set_rprop_delta_min);
PHP_FUNCTION(fann_get_rprop_delta_max);
PHP_FUNCTION(fann_set_rprop_delta_max);
PHP_FUNCTION(fann_get_rprop_delta_zero);
PHP_FUNCTION(fann_set_rprop_delta_zero);
PHP_FUNCTION(fann_get_sarprop_weight_decay_shift);
PHP_FUNCTION(fann_set_sarprop_weight_decay_shift);
PHP_FUNCTION(fann_get_sarprop_step_error_threshold_factor);
PHP_FUNCTION(fann_set_sarprop_step_error_threshold_factor);
PHP_FUNCTION(fann_get_sarprop_step_error_shift);
PHP_FUNCTION(fann_set_sarprop_step_error_shift);
PHP_FUNCTION(fann_get_sarprop_temperature);
PHP_FUNCTION(fann_set_sarprop_temperature);

/* FANN File Input/Output */
/* File Input and Output  */
PHP_FUNCTION(fann_create_from_file);
PHP_FUNCTION(fann_save);


#endif	/* PHP_FANN_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
