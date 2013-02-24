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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_fann.h"

#ifdef PHP_FANN_FIXED
#include "fixedfann.h"
#elseif PHP_FANN_DOUBLE
#include "doublefann.h"
#else
#include "floatfann.h"
#endif

/* True global resources - no need for thread safety here */
static int le_fannbuf, le_fanntrainbuf;
#define le_fannbuf_name "FANN"
#define le_fanntrainbuf_name "FANN Train Data"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO_EX(arginfo_fann_create_standard, 0, 0, 3)
ZEND_ARG_INFO(0, num_layers)
ZEND_ARG_INFO(0, num_neurons1)
ZEND_ARG_INFO(0, num_neurons2)
ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_create_standard_array, 0)
ZEND_ARG_INFO(0, num_layers)
ZEND_ARG_INFO(0, layers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fann_create_sparse, 0, 0, 4)
ZEND_ARG_INFO(0, connection_rate)
ZEND_ARG_INFO(0, num_layers)
ZEND_ARG_INFO(0, num_neurons1)
ZEND_ARG_INFO(0, num_neurons2)
ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_create_sparse_array, 0)
ZEND_ARG_INFO(0, connection_rate)
ZEND_ARG_INFO(0, num_layers)
ZEND_ARG_INFO(0, layers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_fann_create_shortcut, 0, 0, 3)
ZEND_ARG_INFO(0, num_layers)
ZEND_ARG_INFO(0, num_neurons1)
ZEND_ARG_INFO(0, num_neurons2)
ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_create_shortcut_array, 0)
ZEND_ARG_INFO(0, num_layers)
ZEND_ARG_INFO(0, layers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_destroy, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_copy, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_run, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, input)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_randomize_weights, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, min_weight)
ZEND_ARG_INFO(0, max_weight)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_init_weights, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, train_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_num_input, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_num_output, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_total_neurons, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_total_connections, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_network_type, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_connection_rate, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_num_layers, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_layer_array, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_bias_array, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_connection_array, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_weight_array, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, connections)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_weight, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, from_neuron)
ZEND_ARG_INFO(0, to_neuron)
ZEND_ARG_INFO(0, weight)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_train, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, input)
ZEND_ARG_INFO(0, desired_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_test, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, input)
ZEND_ARG_INFO(0, desired_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_MSE, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_bit_fail, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_reset_MSE, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_train_on_file, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, filename)
ZEND_ARG_INFO(0, max_epochs)
ZEND_ARG_INFO(0, epochs_between_reports)
ZEND_ARG_INFO(0, desired_error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_read_train_from_file, 0)
ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_destroy_train, 0)
ZEND_ARG_INFO(0, train_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_training_algorithm, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_training_algorithm, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, training_algorithm)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_learning_rate, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_learning_rate, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, learning_rate)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_learning_momentum, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_learning_momentum, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, learning_momentum)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_train_error_function, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_train_error_function, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, error_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_train_stop_function, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_train_stop_function, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, stop_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_bit_fail_limit, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_bit_fail_limit, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, bit_fail_limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_callback, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_quickprop_decay, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_quickprop_decay, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, quickprop_decay)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_quickprop_mu, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_quickprop_mu, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, quickprop_mu)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_rprop_increase_factor, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_rprop_increase_factor, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, rprop_increase_factor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_rprop_decrease_factor, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_rprop_decrease_factor, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, rprop_decrease_factor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_rprop_delta_min, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_rprop_delta_min, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, rprop_delta_min)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_rprop_delta_max, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_rprop_delta_max, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, rprop_delta_max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_rprop_delta_zero, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_rprop_delta_zero, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0 , rprop_delta_zero)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_sarprop_weight_decay_shift, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_sarprop_weight_decay_shift, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, sarprop_weight_decay_shift)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_sarprop_step_error_threshold_factor, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_sarprop_step_error_threshold_factor, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, sarprop_step_error_threshold_factor)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_sarprop_step_error_shift, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_sarprop_step_error_shift, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, sarprop_step_error_shift)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_sarprop_temperature, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_sarprop_temperature, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, sarprop_temperature)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_function_hidden, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_function_output, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_steepness_hidden, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, steepness)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_steepness_output, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, steepness)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_create_from_file, 0)
ZEND_ARG_INFO(0, configuration_file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_save, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, configuration_file)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ fann_functions[] */
const zend_function_entry fann_functions[] = {
	PHP_FE(fann_create_standard,                          arginfo_fann_create_standard)
	PHP_FE(fann_create_standard_array,                    arginfo_fann_create_standard_array)
	PHP_FE(fann_create_sparse,                            arginfo_fann_create_sparse)
	PHP_FE(fann_create_sparse_array,                      arginfo_fann_create_sparse_array)
	PHP_FE(fann_create_shortcut,                          arginfo_fann_create_shortcut)
	PHP_FE(fann_create_shortcut_array,                    arginfo_fann_create_shortcut_array)
	PHP_FE(fann_destroy,                                  arginfo_fann_destroy)
	PHP_FE(fann_copy,                                     arginfo_fann_copy)
	PHP_FE(fann_run,                                      arginfo_fann_run)
	PHP_FE(fann_randomize_weights,                        arginfo_fann_randomize_weights)
	PHP_FE(fann_init_weights,                             arginfo_fann_init_weights)
	PHP_FE(fann_get_num_input,                            arginfo_fann_get_num_input)
	PHP_FE(fann_get_num_output,                           arginfo_fann_get_num_output)
	PHP_FE(fann_get_total_neurons,                        arginfo_fann_get_total_neurons)
	PHP_FE(fann_get_total_connections,                    arginfo_fann_get_total_connections)
	PHP_FE(fann_get_network_type,                         arginfo_fann_get_network_type)
	PHP_FE(fann_get_connection_rate,                      arginfo_fann_get_connection_rate)
	PHP_FE(fann_get_num_layers,                           arginfo_fann_get_num_layers)
	PHP_FE(fann_get_layer_array,                          arginfo_fann_get_layer_array)
	PHP_FE(fann_get_bias_array,                           arginfo_fann_get_bias_array)
	PHP_FE(fann_get_connection_array,                     arginfo_fann_get_connection_array)
	PHP_FE(fann_set_weight_array,                         arginfo_fann_set_weight_array)
	PHP_FE(fann_set_weight,                               arginfo_fann_set_weight)
	PHP_FE(fann_train,                                    arginfo_fann_train)
	PHP_FE(fann_test,                                     arginfo_fann_test)
	PHP_FE(fann_get_MSE,                                  arginfo_fann_get_MSE)
	PHP_FE(fann_get_bit_fail,                             arginfo_fann_get_bit_fail)
	PHP_FE(fann_reset_MSE,                                arginfo_fann_reset_MSE)
	PHP_FE(fann_train_on_file,                            arginfo_fann_train_on_file)
	PHP_FE(fann_read_train_from_file,                     arginfo_fann_read_train_from_file)
	PHP_FE(fann_destroy_train,                            arginfo_fann_destroy_train)
	PHP_FE(fann_get_learning_rate,                        arginfo_fann_get_learning_rate)
	PHP_FE(fann_set_learning_rate,                        arginfo_fann_set_learning_rate)
	PHP_FE(fann_get_learning_momentum,                    arginfo_fann_get_learning_momentum)
	PHP_FE(fann_set_learning_momentum,                    arginfo_fann_set_learning_momentum)
	PHP_FE(fann_get_train_error_function,                 arginfo_fann_get_train_error_function)
	PHP_FE(fann_set_train_error_function,                 arginfo_fann_set_train_error_function)
	PHP_FE(fann_get_train_stop_function,                  arginfo_fann_get_train_stop_function)
	PHP_FE(fann_set_train_stop_function,                  arginfo_fann_set_train_stop_function)
	PHP_FE(fann_get_bit_fail_limit,                       arginfo_fann_get_bit_fail_limit)
	PHP_FE(fann_set_bit_fail_limit,                       arginfo_fann_set_bit_fail_limit)
	PHP_FE(fann_set_callback,                             arginfo_fann_set_callback)
	PHP_FE(fann_get_quickprop_decay,                      arginfo_fann_get_quickprop_decay)
	PHP_FE(fann_set_quickprop_decay,                      arginfo_fann_set_quickprop_decay)
	PHP_FE(fann_get_quickprop_mu,                         arginfo_fann_get_quickprop_mu)
	PHP_FE(fann_set_quickprop_mu,                         arginfo_fann_set_quickprop_mu)
	PHP_FE(fann_get_rprop_increase_factor,                arginfo_fann_get_rprop_increase_factor)
	PHP_FE(fann_set_rprop_increase_factor,                arginfo_fann_set_rprop_increase_factor)
	PHP_FE(fann_get_rprop_decrease_factor,                arginfo_fann_get_rprop_decrease_factor)
	PHP_FE(fann_set_rprop_decrease_factor,                arginfo_fann_set_rprop_decrease_factor)
	PHP_FE(fann_get_rprop_delta_min,                      arginfo_fann_get_rprop_delta_min)
	PHP_FE(fann_set_rprop_delta_min,                      arginfo_fann_set_rprop_delta_min)
	PHP_FE(fann_get_rprop_delta_max,                      arginfo_fann_get_rprop_delta_max)
	PHP_FE(fann_set_rprop_delta_max,                      arginfo_fann_set_rprop_delta_max)
	PHP_FE(fann_get_rprop_delta_zero,                     arginfo_fann_get_rprop_delta_zero)
	PHP_FE(fann_set_rprop_delta_zero,                     arginfo_fann_set_rprop_delta_zero)
	PHP_FE(fann_get_sarprop_weight_decay_shift,           arginfo_fann_get_sarprop_weight_decay_shift)
	PHP_FE(fann_set_sarprop_weight_decay_shift,           arginfo_fann_set_sarprop_weight_decay_shift)
	PHP_FE(fann_get_sarprop_step_error_threshold_factor,  arginfo_fann_get_sarprop_step_error_threshold_factor)
	PHP_FE(fann_set_sarprop_step_error_threshold_factor,  arginfo_fann_set_sarprop_step_error_threshold_factor)
	PHP_FE(fann_get_sarprop_step_error_shift,             arginfo_fann_get_sarprop_step_error_shift)
	PHP_FE(fann_set_sarprop_step_error_shift,             arginfo_fann_set_sarprop_step_error_shift)
	PHP_FE(fann_get_sarprop_temperature,                  arginfo_fann_get_sarprop_temperature)
	PHP_FE(fann_set_sarprop_temperature,                  arginfo_fann_set_sarprop_temperature)
	PHP_FE(fann_get_training_algorithm,                   arginfo_fann_get_training_algorithm)
	PHP_FE(fann_set_training_algorithm,                   arginfo_fann_set_training_algorithm)
	PHP_FE(fann_set_activation_function_hidden,           arginfo_fann_set_activation_function_hidden)
	PHP_FE(fann_set_activation_function_output,           arginfo_fann_set_activation_function_output)
	PHP_FE(fann_set_activation_steepness_hidden,          arginfo_fann_set_activation_steepness_hidden)
	PHP_FE(fann_set_activation_steepness_output,          arginfo_fann_set_activation_steepness_output)
	PHP_FE(fann_create_from_file,                         arginfo_fann_create_from_file)
	PHP_FE(fann_save,                                     arginfo_fann_save)
	PHP_FE_END
};
/* }}} */

/* {{{ fann_module_entry */
zend_module_entry fann_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"fann",
	fann_functions,
	PHP_MINIT(fann),
	PHP_MSHUTDOWN(fann),
	NULL,
	NULL,
	PHP_MINFO(fann),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1",
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FANN
ZEND_GET_MODULE(fann)
#endif

/* macro for chencking fann_error structs */
#define PHP_FANN_ERROR_CHECK(__fann_struct)								\
	if (fann_get_errno((struct fann_error *) __fann_struct) != 0) {		\
		php_error_docref(NULL TSRMLS_CC, E_WARNING, __fann_struct->errstr); \
		RETURN_FALSE;													\
	}

/* macro for checking ann errors */
#define PHP_FANN_ERROR_CHECK_ANN() PHP_FANN_ERROR_CHECK(ann)

/* macro for checking ann errors */
#define PHP_FANN_ERROR_CHECK_TRAIN_DATA() PHP_FANN_ERROR_CHECK(train_data)

/* macro for returning ann resource */
#define PHP_FANN_RETURN_ANN()								\
	if (!ann) { RETURN_FALSE; }								\
	ZEND_REGISTER_RESOURCE(return_value, ann, le_fannbuf)

/* macro for returning train data resource */
#define PHP_FANN_RETURN_TRAIN_DATA()								\
	if (!train_data) { RETURN_FALSE; }								\
ZEND_REGISTER_RESOURCE(return_value, train_data, le_fanntrainbuf)

/* macro for fetching ann resource */
#define PHP_FANN_FETCH_ANN()											\
	ZEND_FETCH_RESOURCE(ann, struct fann *, &z_ann, -1, le_fannbuf_name, le_fannbuf);

/* macro for fetching train data resource */
#define PHP_FANN_FETCH_TRAIN_DATA()											\
	ZEND_FETCH_RESOURCE(train_data, struct fann_train_data *, &z_train_data, -1, le_fanntrainbuf_name, le_fanntrainbuf);

/* macro for getting ann param identified by 0 args */
#define PHP_FANN_GET_PARAM0(__fce, __return)							\
	zval *z_ann; struct fann *ann;										\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) return; \
	PHP_FANN_FETCH_ANN();												\
	__return(__fce(ann))

/* macro for getting ann param identified by 2 args */
#define PHP_FANN_GET_PARAM2(__fce, __return, __zppval, __type1, __type2) \
	zval *z_ann; struct fann *ann;										\
	__type1 param1; __type2 param2;										\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r" #__zppval, &z_ann, &param1, &param2) == FAILURE) \
		return;															\
	PHP_FANN_FETCH_ANN();												\
	__return(__fce(ann, param1, param2))

/* macro for getting ann param (just alias for one param macro) */
#define PHP_FANN_GET_PARAM PHP_FANN_GET_PARAM0

/* macro for setting ann param not identified by arg */
#define PHP_FANN_SET_PARAM0(__fce)										\
	zval *z_ann; struct fann *ann;										\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) return; \
	PHP_FANN_FETCH_ANN();												\
	__fce(ann);															\
	PHP_FANN_ERROR_CHECK_ANN();											\
	RETURN_TRUE

/* macro for setting ann param identified by 1 arg */
#define PHP_FANN_SET_PARAM1(__fce, __zppval, __type)					\
	zval *z_ann; struct fann *ann; __type param;						\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r" #__zppval, &z_ann, &param) == FAILURE) return; \
	PHP_FANN_FETCH_ANN();												\
	__fce(ann, param);													\
	PHP_FANN_ERROR_CHECK_ANN();											\
	RETURN_TRUE


/* macro for setting ann param identified by 2 args */
#define PHP_FANN_SET_PARAM2(__fce, __zppval, __type1, __type2)			\
	zval *z_ann; struct fann *ann;										\
	__type1 param1; __type2 param2;										\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r" #__zppval, &z_ann, &param1, &param2) == FAILURE) \
		return;															\
	PHP_FANN_FETCH_ANN();												\
	__fce(ann, param1, param2);											\
	PHP_FANN_ERROR_CHECK_ANN();											\
	RETURN_TRUE

/* macro for setting ann param identified by 3 args */
#define PHP_FANN_SET_PARAM3(__fce, __zppval, __type1, __type2, __type3)	\
	zval *z_ann; struct fann *ann;										\
	__type1 param1; __type2 param2; __type3 param3;						\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r" #__zppval, &z_ann, &param1, &param2, &param3) \
		== FAILURE) return;												\
	PHP_FANN_FETCH_ANN();												\
	__fce(ann, param1, param2, param3);									\
	PHP_FANN_ERROR_CHECK_ANN();											\
	RETURN_TRUE

/* macro for setting ann param (just alian for one param macro) */
#define PHP_FANN_SET_PARAM PHP_FANN_SET_PARAM1
/* macro for resetting ann param */
#define PHP_FANN_RESET_PARAM PHP_FANN_SET_PARAM0

/* macro for registering FANN constants */
#define REGISTER_FANN_CONSTANT(__c) REGISTER_LONG_CONSTANT(#__c, __c, CONST_CS | CONST_PERSISTENT)

/* {{{ fann_destructor_fannbuf()
   fann resource destructor */
static void fann_destructor_fannbuf(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct fann *ann = (struct fann *) rsrc->ptr;
	fann_destroy(ann);
}
/* }}} */

/* {{{ fann_destructor_fanntrainbuf()
   fann_train resource destructor */
static void fann_destructor_fanntrainbuf(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct fann_train_data *train_data = (struct fann_train_data *) rsrc->ptr;
	fann_destroy_train(train_data);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(fann)
{
	/* register struct fann resource destructor */
	le_fannbuf = zend_register_list_destructors_ex(
		fann_destructor_fannbuf, NULL, le_fannbuf_name, module_number);
	/* register struct fann_train resource destructor */
	le_fanntrainbuf = zend_register_list_destructors_ex(
		fann_destructor_fanntrainbuf, NULL, le_fanntrainbuf_name, module_number);
	
	/* do not print fann errors */
	fann_set_error_log(NULL, NULL);
	
	/* Train constants */
	REGISTER_FANN_CONSTANT(FANN_TRAIN_INCREMENTAL);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_BATCH);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_RPROP);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_QUICKPROP);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_SARPROP);
	/* Activation functions constants */
	REGISTER_FANN_CONSTANT(FANN_LINEAR);
	REGISTER_FANN_CONSTANT(FANN_THRESHOLD);
	REGISTER_FANN_CONSTANT(FANN_THRESHOLD_SYMMETRIC);
	REGISTER_FANN_CONSTANT(FANN_SIGMOID);
	REGISTER_FANN_CONSTANT(FANN_SIGMOID_STEPWISE);
	REGISTER_FANN_CONSTANT(FANN_SIGMOID_SYMMETRIC);
	REGISTER_FANN_CONSTANT(FANN_SIGMOID_SYMMETRIC_STEPWISE);
	REGISTER_FANN_CONSTANT(FANN_GAUSSIAN);
	REGISTER_FANN_CONSTANT(FANN_GAUSSIAN_SYMMETRIC);
	REGISTER_FANN_CONSTANT(FANN_GAUSSIAN_STEPWISE);
	REGISTER_FANN_CONSTANT(FANN_ELLIOT);
	REGISTER_FANN_CONSTANT(FANN_ELLIOT_SYMMETRIC);
	REGISTER_FANN_CONSTANT(FANN_LINEAR_PIECE);
	REGISTER_FANN_CONSTANT(FANN_LINEAR_PIECE_SYMMETRIC);
	REGISTER_FANN_CONSTANT(FANN_SIN_SYMMETRIC);
	REGISTER_FANN_CONSTANT(FANN_COS_SYMMETRIC);
	REGISTER_FANN_CONSTANT(FANN_SIN);
	REGISTER_FANN_CONSTANT(FANN_COS);
	/* Error functions constants */
	REGISTER_FANN_CONSTANT(FANN_ERRORFUNC_LINEAR);
	REGISTER_FANN_CONSTANT(FANN_ERRORFUNC_TANH);
	/* Stop functions constants */
	REGISTER_FANN_CONSTANT(FANN_STOPFUNC_MSE);
	REGISTER_FANN_CONSTANT(FANN_STOPFUNC_BIT);
	/* Network type constants */
	REGISTER_FANN_CONSTANT(FANN_NETTYPE_LAYER);
	REGISTER_FANN_CONSTANT(FANN_NETTYPE_SHORTCUT);

	/* Init FANNConnection class */
	php_fannconnection_register_class(TSRMLS_C);
	
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(fann)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(fann)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "fann support", "enabled");
	php_info_print_table_end();
}
/* }}} */


/* {{{ php_funn_io_foreach()
   callback for converting input hash map to fann_type array */
int php_funn_io_foreach(zval **element TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
{
	fann_type *input = va_arg(args, fann_type *);
	int *pos = va_arg(args, int *);
	
	convert_to_double(*element);
	input[*pos++] = (fann_type) Z_DVAL_PP(element);
	
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* php_fann_check_num_layers() {{{ */
static int php_fann_check_num_layers(int specified, int provided TSRMLS_DC)
{
	if (specified < 2) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of layers must be greater than 2");
		return FAILURE;
	}
	if (specified != provided) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid number of arguments");
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* php_fann_check_num_neurons() {{{ */
static int php_fann_check_num_neurons(int num_neurons TSRMLS_DC)
{
	if (num_neurons < 0) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of neurons cannot be negative");
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* php_fann_check_num_inputs() {{{ */
static int php_fann_check_num_inputs(struct fann *ann, int num_inputs TSRMLS_DC)
{
	if  (fann_get_num_input(ann) != num_inputs) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
						 "Number of inputs is different than number of neurons in the input layer");
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* php_fann_check_num_outputs() {{{ */
static int php_fann_check_num_outputs(struct fann *ann, int num_outputs TSRMLS_DC)
{
	if  (fann_get_num_output(ann) != num_outputs) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
						 "Number of outputs is different than number of neurons in the output layer");
		return FAILURE;
	}
	return SUCCESS;
}
/* }}} */

/* php_fann_create() {{{ */
static int php_fann_create(int num_args, float *connection_rate,
						   uint *num_layers, uint **layers TSRMLS_DC)
{
	zval ***args;
	int argc, i, pos;
	
	if (zend_parse_parameters(num_args TSRMLS_CC, "+", &args, &argc) == FAILURE) {
		return FAILURE;
	}

	pos = 0;
	if (connection_rate) {
		convert_to_double_ex(args[pos]);
		*connection_rate = Z_DVAL_PP(args[pos++]);
	}
	
	convert_to_long_ex(args[pos]);
	*num_layers = Z_LVAL_PP(args[pos++]);

	if (php_fann_check_num_layers(*num_layers, argc - pos TSRMLS_CC) == FAILURE) {
		efree(args);
		return FAILURE;
	}

	*layers = (uint *) emalloc(*num_layers * sizeof(uint));
	for (i = pos; i < argc; i++) {
		convert_to_long_ex(args[i]);
		if (php_fann_check_num_neurons(Z_LVAL_PP(args[i]) TSRMLS_CC) == FAILURE) {
			efree(args);
			efree(*layers);
			return FAILURE;
		}
		(*layers)[i - pos] = Z_LVAL_PP(args[i]);
	}
	efree(args);

	return SUCCESS;
}
/* }}} */

/* php_fann_create_array() {{{ */
static int php_fann_create_array(int num_args, float *conn_rate,
								 uint *num_layers, uint **layers TSRMLS_DC)
{
	zval *array, **ppdata;
	int i = 0;

	if (conn_rate) {
		if (zend_parse_parameters(num_args TSRMLS_CC, "dla", conn_rate, num_layers, &array) == FAILURE) {
			return FAILURE;
		}
	}
	else {
		if (zend_parse_parameters(num_args TSRMLS_CC, "la", num_layers, &array) == FAILURE) {
			return FAILURE;
		}
	}

	if (php_fann_check_num_layers(
			*num_layers, zend_hash_num_elements(Z_ARRVAL_P(array)) TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}

	*layers = (uint *) emalloc(*num_layers * sizeof(uint));
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(array));
		 zend_hash_get_current_data(Z_ARRVAL_P(array), (void **) &ppdata) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(array))) {
		convert_to_long_ex(ppdata);
		if (php_fann_check_num_neurons(Z_LVAL_PP(ppdata) TSRMLS_CC) == FAILURE) {
			efree(*layers);
			return FAILURE;
		}
		(*layers)[i++] = Z_LVAL_PP(ppdata);
	}

	return SUCCESS;
}
/* }}} */

/* {{{ proto resource fann_create_standard(int num_layers, int num_neurons1, int num_neurons2, [, int ... ])
   Creates a standard fully connected backpropagation neural network */
PHP_FUNCTION(fann_create_standard)
{
	uint num_layers, *layers; 
	struct fann *ann;
	
	if (php_fann_create(ZEND_NUM_ARGS(), NULL, &num_layers, &layers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ann = fann_create_standard_array(num_layers, layers);
	efree(layers);
	PHP_FANN_ERROR_CHECK_ANN();
	PHP_FANN_RETURN_ANN();
}
/* }}} */

/* {{{ proto resource fann_create_standard_array(int num_layers, array layers)
   Creates a standard fully connected backpropagation neural network (array for layers) */
PHP_FUNCTION(fann_create_standard_array)
{
	uint num_layers, *layers; 
	struct fann *ann;
	
	if (php_fann_create_array(ZEND_NUM_ARGS(), NULL, &num_layers, &layers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ann = fann_create_standard_array(num_layers, layers);
	efree(layers);
	PHP_FANN_ERROR_CHECK_ANN();
	PHP_FANN_RETURN_ANN();
}
/* }}} */

/* {{{ proto resource fann_create_sparse(float connection_rate, int num_layers, int num_neurons1, int num_neurons2, [, int ... ])
   Creates a standard backpropagation neural network, which is not fully connected */
PHP_FUNCTION(fann_create_sparse)
{
	uint num_layers, *layers;
	float connection_rate;
	struct fann *ann;
	
	if (php_fann_create(ZEND_NUM_ARGS(), &connection_rate, &num_layers, &layers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ann = fann_create_sparse_array(connection_rate, num_layers, layers);
	efree(layers);
	PHP_FANN_ERROR_CHECK_ANN();
	PHP_FANN_RETURN_ANN();
}
/* }}} */

/* {{{ proto resource fann_create_sparse_array(float connection_rate, int num_layers, array layers)
   Creates a standard backpropagation neural network, which is not fully connected
   (using array for layers)  */
PHP_FUNCTION(fann_create_sparse_array)
{
	uint num_layers, *layers;
	float connection_rate;
	struct fann *ann;
	
	if (php_fann_create_array(ZEND_NUM_ARGS(), &connection_rate, &num_layers, &layers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ann = fann_create_sparse_array(connection_rate, num_layers, layers);
	efree(layers);
	PHP_FANN_ERROR_CHECK_ANN();
	PHP_FANN_RETURN_ANN();
}
/* }}} */

/* {{{ proto resource fann_create_shortcut(int num_layers, int num_neurons1, int num_neurons2, [, int ... ])
   Creates a standard backpropagation neural network, which is not fully connected and
   which also has shortcut connections. */
PHP_FUNCTION(fann_create_shortcut)
{
	uint num_layers, *layers; 
	struct fann *ann;
	
	if (php_fann_create(ZEND_NUM_ARGS(), NULL, &num_layers, &layers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ann = fann_create_shortcut_array(num_layers, layers);
	efree(layers);
	PHP_FANN_ERROR_CHECK_ANN();
	PHP_FANN_RETURN_ANN();
}
/* }}} */

/* {{{ proto resource fann_create_shortcut_array(int num_layers, array layers)
   reates a standard backpropagation neural network, which is not fully connected and
   which also has shortcut connections (using array of layers) */
PHP_FUNCTION(fann_create_shortcut_array)
{
	uint num_layers, *layers; 
	struct fann *ann;
	
	if (php_fann_create_array(ZEND_NUM_ARGS(), NULL, &num_layers, &layers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ann = fann_create_shortcut_array(num_layers, layers);
	efree(layers);
	PHP_FANN_ERROR_CHECK_ANN();
	PHP_FANN_RETURN_ANN();
}
/* }}} */

/* {{{ proto bool fann_destroy(resource ann)
   Destroys neural network */
PHP_FUNCTION(fann_destroy)
{
	zval *z_ann;
	struct fann *ann;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}

	PHP_FANN_FETCH_ANN();
	RETURN_BOOL(zend_list_delete(Z_LVAL_P(z_ann)) == SUCCESS);
}
/* }}} */

/* {{{ proto resource fann_copy(resource ann)
   Creates a copy of the neural network */
PHP_FUNCTION(fann_copy)
{
	zval *z_ann;
	struct fann *ann, *ann_copy;
		
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();

	ann_copy = fann_copy(ann);
	if (!ann_copy) {
		RETURN_FALSE;
	}
	ZEND_REGISTER_RESOURCE(return_value, ann_copy, le_fannbuf)
}
/* }}} */

/* {{{ proto array fann_run(resource ann, array input)
   Runs input through the neural network */
PHP_FUNCTION(fann_run)
{
	zval *z_ann, *array;
	struct fann *ann;
	fann_type *input, *calc_out;
	int i = 0, num_out, num_inputs;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &array) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	num_inputs = zend_hash_num_elements(Z_ARRVAL_P(array));
	if (php_fann_check_num_inputs(ann, num_inputs TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	input = (float *) emalloc(sizeof(float) * num_inputs);
	zend_hash_apply_with_arguments(Z_ARRVAL_P(array) TSRMLS_CC,
								   (apply_func_args_t) php_funn_io_foreach, 2, input, &i);
	
	calc_out = fann_run(ann, input);
	efree(input);
	num_out = fann_get_num_output(ann);
	PHP_FANN_ERROR_CHECK_ANN();

	array_init(return_value);
	for (i = 0; i < num_out; i++) {
		add_next_index_double(return_value, (double) calc_out[i]);
	}
}
/* }}} */


/* {{{ proto bool fann_randomize_weights(resource ann, double min_weight, double max_weight)
   Gives each connection a random weight between min_weight and max_weight */
PHP_FUNCTION(fann_randomize_weights)
{
	zval *z_ann;
	struct fann *ann;
	double min_weight, max_weight;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &z_ann, &min_weight, &max_weight) == FAILURE) {
		return;
	}
	
	PHP_FANN_FETCH_ANN();
	fann_randomize_weights(ann, min_weight, max_weight);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto bool fann_init_weights(resource ann, resource train_data)
   Initializes weights using Widrow + Nguyen’s algorithm */
PHP_FUNCTION(fann_init_weights)
{
	zval *z_ann, *z_train_data;
	struct fann *ann;
	struct fann_train_data *train_data;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &z_ann, &z_train_data) == FAILURE) {
		return;
	}
	
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	fann_init_weights(ann, train_data);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto long fann_get_num_input(resource ann)
   Returns the number of input neurons */
PHP_FUNCTION(fann_get_num_input)
{
	PHP_FANN_GET_PARAM(fann_get_num_input, RETURN_LONG);
}

/* {{{ proto long fann_get_num_output(resource ann)
   Returns the number of output neurons */
PHP_FUNCTION(fann_get_num_output)
{
	PHP_FANN_GET_PARAM(fann_get_num_output, RETURN_LONG);
}

/* {{{ proto long fann_get_total_neurons(resource ann)
   Returns the number of output neurons the total number of neurons in the entire network
   (this number does also include the bias neurons, so a 2-4-2 network has 2+4+2 +2(bias) = 10 neurons) */
PHP_FUNCTION(fann_get_total_neurons)
{
	PHP_FANN_GET_PARAM(fann_get_total_neurons, RETURN_LONG);
}

/* {{{ proto long fann_get_total_connections(resource ann)
   Returns the total number of connections in the entire network */
PHP_FUNCTION(fann_get_total_connections)
{
	PHP_FANN_GET_PARAM(fann_get_total_connections, RETURN_LONG);
}

/* {{{ proto long fann_get_network_type(resource ann)
   Returns the type of neural network it was created as */
PHP_FUNCTION(fann_get_network_type)
{
	PHP_FANN_GET_PARAM(fann_get_network_type, RETURN_LONG);
}

/* {{{ proto double fann_get_connection_rate(resource ann)
   Returns the connection rate used when the network was created  */
PHP_FUNCTION(fann_get_connection_rate)
{
	PHP_FANN_GET_PARAM(fann_get_connection_rate, RETURN_DOUBLE);
}

/* {{{ proto long fann_get_num_layers(resource ann)
   Returns the number of layers in the network */
PHP_FUNCTION(fann_get_num_layers)
{
	PHP_FANN_GET_PARAM(fann_get_num_layers, RETURN_LONG);
}

/* {{{ proto array fann_get_layer_array(resource ann)
   Returns the number of neurons in each layer in the network (bias is not included) */
PHP_FUNCTION(fann_get_layer_array)
{
	zval *z_ann;
	struct fann *ann;
	uint num_layers, *layers, i;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}
	
	PHP_FANN_FETCH_ANN();
	num_layers = fann_get_num_layers(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	layers = (uint *) emalloc(num_layers * sizeof(uint)); 
	fann_get_layer_array(ann, layers);
	PHP_FANN_ERROR_CHECK_ANN();
	array_init(return_value);
	for (i = 0; i < num_layers; i++) {
		add_index_long(return_value, i, layers[i]);
	}
	efree(layers);
}

/* {{{ proto array fann_get_bias_array(resource ann)
   Returns the number of bias in each layer in the network */
PHP_FUNCTION(fann_get_bias_array)
{
	zval *z_ann;
	struct fann *ann;
	uint num_layers, *layers, i;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}
	
	PHP_FANN_FETCH_ANN();
	num_layers = fann_get_num_layers(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	layers = (uint *) emalloc(num_layers * sizeof(uint)); 
	fann_get_bias_array(ann, layers);
	PHP_FANN_ERROR_CHECK_ANN();
	array_init(return_value);
	for (i = 0; i < num_layers; i++) {
		add_index_long(return_value, i, layers[i]);
	}
	efree(layers);
}

/* {{{ proto array fann_get_connection_array(resource ann)
   Returns connections in the network */
PHP_FUNCTION(fann_get_connection_array)
{
	zval *z_ann, *z_connection;
	struct fann *ann;
	struct fann_connection *connections;
	uint num_connections, i;
	long from_neuron, to_neuron;
	double weight;
		
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}
	
	PHP_FANN_FETCH_ANN();
	num_connections = fann_get_total_connections(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	connections = (struct fann_connection *) emalloc(num_connections * sizeof(struct fann_connection)); 
	fann_get_connection_array(ann, connections);
	PHP_FANN_ERROR_CHECK_ANN();
	array_init(return_value);
	for (i = 0; i < num_connections; i++) {
		from_neuron = (long) connections[i].from_neuron;
		to_neuron = (long) connections[i].to_neuron;
		weight = (double) connections[i].weight;
		MAKE_STD_ZVAL(z_connection);
		object_init_ex(z_connection, php_fann_FANNConnection_class);
		PHP_FANN_CONN_PROP_UPDATE(long, z_connection, "from_neuron", from_neuron);
		PHP_FANN_CONN_PROP_UPDATE(long, z_connection, "to_neuron", to_neuron);
		PHP_FANN_CONN_PROP_UPDATE(double, z_connection, "weight",  weight);
		add_index_zval(return_value, i, z_connection);
	}
	efree(connections);
}

/* {{{ proto bool fann_set_weight_array(resource ann, array connections)
   Sets connections in the network */
PHP_FUNCTION(fann_set_weight_array)
{
	zval *z_ann, *array, **current;
	struct fann *ann;
	struct fann_connection *connections;
	uint num_connections, i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &array) == FAILURE) {
		return;
	}

	PHP_FANN_FETCH_ANN();
	num_connections = zend_hash_num_elements(Z_ARRVAL_P(array));
	connections = (struct fann_connection *) emalloc(num_connections * sizeof(struct fann_connection));
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(array));
		 zend_hash_get_current_data(Z_ARRVAL_P(array), (void **) &current) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(array))) {
		if (Z_TYPE_PP(current) == IS_OBJECT && instanceof_function(
				Z_OBJCE_PP(current), php_fann_FANNConnection_class TSRMLS_CC)) {
			connections[i].from_neuron = Z_LVAL_P(PHP_FANN_CONN_PROP_READ(*current, "from_neuron"));
			connections[i].to_neuron = Z_LVAL_P(PHP_FANN_CONN_PROP_READ(*current, "to_neuron"));
			connections[i].weight = Z_DVAL_P(PHP_FANN_CONN_PROP_READ(*current, "weight"));
			++i;
		}
		else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Weights array can contain only object of FANNConnection");
			efree(connections);
			RETURN_FALSE;
		}
	}
	fann_set_weight_array(ann, connections, i);
	efree(connections);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}

/* {{{ proto bool fann_set_weight(resource ann, int from_neuron, int to_neuron, double weight)
   Sets a connection in the network */
PHP_FUNCTION(fann_set_weight)
{
	zval *z_ann;
	struct fann *ann;
	long from_neuron, to_neuron;
	double weight;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlld", &z_ann, &from_neuron, &to_neuron, &weight)
		== FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	fann_set_weight(ann, from_neuron, to_neuron, weight);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}

/* {{{ proto bool fann_train(resource ann, array input, array desired_output)
   Trains one iteration with a set of inputs, and a set of desired outputs */
PHP_FUNCTION(fann_train)
{
	zval *z_ann, *z_input, *z_output;
	struct fann *ann;
	fann_type *input, *desired_output;
	int i = 0, num_outputs, num_inputs;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "raa", &z_ann, &z_input, &z_output) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	// check params
	num_inputs = zend_hash_num_elements(Z_ARRVAL_P(z_input));
	if (php_fann_check_num_inputs(ann, num_inputs TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	num_outputs = zend_hash_num_elements(Z_ARRVAL_P(z_output));
	if (php_fann_check_num_outputs(ann, num_outputs TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	input = (fann_type *) emalloc(sizeof(fann_type) * num_inputs);
	zend_hash_apply_with_arguments(Z_ARRVAL_P(z_input) TSRMLS_CC,
								   (apply_func_args_t) php_funn_io_foreach, 2, input, &i);
	i = 0;
	desired_output = (fann_type *) emalloc(sizeof(fann_type) * num_outputs);
	zend_hash_apply_with_arguments(Z_ARRVAL_P(z_output) TSRMLS_CC,
								   (apply_func_args_t) php_funn_io_foreach, 2, desired_output, &i);
	
	fann_test(ann, input, desired_output);
	efree(input);
	efree(desired_output);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto array fann_test(resource ann, array input, array desired_output)
   Tests with a set of inputs, and a set of desired outputs -
   this operation updates the mean square error, but does not change the network in any way. */
PHP_FUNCTION(fann_test)
{
	zval *z_ann, *z_input, *z_output;
	struct fann *ann;
	fann_type *input, *desired_output, *output;
	int i = 0, num_outputs, num_inputs;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "raa", &z_ann, &z_input, &z_output) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	// check params
	num_inputs = zend_hash_num_elements(Z_ARRVAL_P(z_input));
	if (php_fann_check_num_inputs(ann, num_inputs TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	num_outputs = zend_hash_num_elements(Z_ARRVAL_P(z_output));
	if (php_fann_check_num_outputs(ann, num_outputs TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	input = (fann_type *) emalloc(sizeof(fann_type) * num_inputs);
	zend_hash_apply_with_arguments(Z_ARRVAL_P(z_input) TSRMLS_CC,
								   (apply_func_args_t) php_funn_io_foreach, 2, input, &i);
	i = 0;
	desired_output = (fann_type *) emalloc(sizeof(fann_type) * num_outputs);
	zend_hash_apply_with_arguments(Z_ARRVAL_P(z_output) TSRMLS_CC,
								   (apply_func_args_t) php_funn_io_foreach, 2, desired_output, &i);
	
	output = fann_test(ann, input, desired_output);
	efree(input);
	efree(desired_output);
	PHP_FANN_ERROR_CHECK_ANN();

	array_init(return_value);
	for (i = 0; i < num_outputs; i++) {
		add_next_index_double(return_value, (double) output[i]);
	}
}
/* }}} */

/* {{{ proto double fann_get_MSE(resource ann)
   Reads the mean square error from the network */
PHP_FUNCTION(fann_get_MSE)
{
	PHP_FANN_GET_PARAM(fann_get_MSE, RETURN_DOUBLE);
}

/* {{{ proto int fann_get_bit_fail(resource ann)
   Returns he number of fail bits; means the number of output neurons which differ more than the bit fail limit */
PHP_FUNCTION(fann_get_bit_fail)
{
	PHP_FANN_GET_PARAM(fann_get_bit_fail, RETURN_LONG);
}

/* {{{ proto bool fann_reset_MSE(resource ann)
   Resets the mean square error from the network */
PHP_FUNCTION(fann_reset_MSE)
{
	PHP_FANN_RESET_PARAM(fann_reset_MSE);
}

/* {{{ proto bool fann_train_on_file(resource ann, string filename, int max_epochs, int epochs_between_reports, float desired_error)
   Set the activation function for all of the hidden layers */
PHP_FUNCTION(fann_train_on_file)
{
	zval *z_ann;
	char *filename;
	int filename_len;
	long max_epochs, epochs_between_reports;
	double desired_error;
	struct fann *ann;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rslld", &z_ann, &filename, &filename_len,
							  &max_epochs, &epochs_between_reports, &desired_error) == FAILURE) {
		return;
	}

	PHP_FANN_FETCH_ANN();
	fann_train_on_file(ann, filename, max_epochs, epochs_between_reports, desired_error);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto resource fann_read_train_from_file(string filename)
   Reads a file that stores training data */
PHP_FUNCTION(fann_read_train_from_file)
{
	char *filename;
	int filename_len;
	struct fann_train_data *train_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
		return;
	}

	train_data = fann_read_train_from_file(filename);
	if (!train_data) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Train data file reading failed");
		RETURN_FALSE;
	}
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();
	PHP_FANN_RETURN_TRAIN_DATA();
}
/* }}} */


/* {{{ proto bool fann_destroy_train(resource train_data)
   Destructs the training data */
PHP_FUNCTION(fann_destroy_train)
{
	zval *z_train_data;
	struct fann_train_data *train_data;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_train_data) == FAILURE) {
		return;
	}

	PHP_FANN_FETCH_TRAIN_DATA();
	RETURN_BOOL(zend_list_delete(Z_LVAL_P(z_train_data)) == SUCCESS);
}
/* }}} */

/* {{{ proto int fann_get_training_algorithm(resource ann)
   Returns the training algorithm that is used by fann_train_on_data and associated functions */
PHP_FUNCTION(fann_get_training_algorithm)
{
	PHP_FANN_GET_PARAM(fann_get_training_algorithm, RETURN_LONG);
}

/* {{{ proto bool fann_set_training_algorithm(resource ann, int training_algorithm)
   Sets the training algorithm that is used by fann_train_on_data and associated functions */
PHP_FUNCTION(fann_set_training_algorithm)
{
    PHP_FANN_SET_PARAM(fann_set_training_algorithm, l, long);
}
/* }}} */

/* {{{ proto double fann_get_learning_rate(resource ann)
   Returns the learning rate */
PHP_FUNCTION(fann_get_learning_rate)
{
	PHP_FANN_GET_PARAM(fann_get_learning_rate, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_learning_rate(resource ann, double learning_rate)
   Sets the learning rate */
PHP_FUNCTION(fann_set_learning_rate)
{
    PHP_FANN_SET_PARAM(fann_set_learning_rate, d, double);
}
/* }}} */

/* {{{ proto int fann_get_learning_momentum(resource ann)
   Returns the learning momentum */
PHP_FUNCTION(fann_get_learning_momentum)
{
	PHP_FANN_GET_PARAM(fann_get_learning_momentum, RETURN_LONG);
}

/* {{{ proto bool fann_set_learning_momentum(resource ann, double learning_momentum)
   Sets the learning momentum */
PHP_FUNCTION(fann_set_learning_momentum)
{
    PHP_FANN_SET_PARAM(fann_set_learning_momentum, d, double);
}
/* }}} */

/* {{{ proto bool fann_set_activation_function_hidden(resource ann, int activation_function)
   Sets the activation function for all of the hidden layers */
PHP_FUNCTION(fann_set_activation_function_hidden)
{
	PHP_FANN_SET_PARAM(fann_set_activation_function_hidden, l, long);
	
}
/* }}} */

/* {{{ proto bool fann_set_activation_function_output(resource ann, int activation_function)
   Sets the activation function for the output layer */
PHP_FUNCTION(fann_set_activation_function_output)
{
	PHP_FANN_SET_PARAM(fann_set_activation_function_output, l, long);
}
/* }}} */

/* {{{ proto bool fann_set_activation_steepness_hidden(resource ann, double steepness)
   Sets the steepness of the activation steepness in all of the hidden layers */
PHP_FUNCTION(fann_set_activation_steepness_hidden)
{
	PHP_FANN_SET_PARAM(fann_set_activation_steepness_hidden, d, double);
}
/* }}} */


/* {{{ proto bool fann_set_activation_steepness_output(resource ann, double steepness)
   Sets the steepness of the activation steepness in the output layer */
PHP_FUNCTION(fann_set_activation_steepness_output)
{
	PHP_FANN_SET_PARAM(fann_set_activation_steepness_output, d, double);
}
/* }}} */

/* {{{ proto int fann_get_train_error_function(resource ann)
   Returns the error function used during training */
PHP_FUNCTION(fann_get_train_error_function)
{
	PHP_FANN_GET_PARAM(fann_get_train_error_function, RETURN_LONG);
}

/* {{{ proto bool fann_set_train_error_function(resource ann, int train_error_function)
   Sets the error function used during training */
PHP_FUNCTION(fann_set_train_error_function)
{
    PHP_FANN_SET_PARAM(fann_set_train_error_function, l, long);
}
/* }}} */

/* {{{ proto int fann_get_train_stop_function(resource ann)
   Returns the stop function used during training */
PHP_FUNCTION(fann_get_train_stop_function)
{
	PHP_FANN_GET_PARAM(fann_get_train_stop_function, RETURN_LONG);
}

/* {{{ proto bool fann_set_train_stop_function(resource ann, int train_stop_function)
   Sets the stop function used during training */
PHP_FUNCTION(fann_set_train_stop_function)
{
    PHP_FANN_SET_PARAM(fann_set_train_stop_function, l, long);
}
/* }}} */

/* {{{ proto bool fann_get_bit_fail_limit(resource ann)
   Returns the bit fail limit used during training */
PHP_FUNCTION(fann_get_bit_fail_limit)
{
	PHP_FANN_GET_PARAM(fann_get_bit_fail_limit, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_bit_fail_limit(resource ann, double bit_fail_limit)
   Sets the bit fail limit used during training */
PHP_FUNCTION(fann_set_bit_fail_limit)
{
    PHP_FANN_SET_PARAM(fann_set_bit_fail_limit, d, double);
}
/* }}} */

/* {{{ proto bool fann_set_callback(resource ann, int callback)
   Sets the callback function for use during training */
PHP_FUNCTION(fann_set_callback)
{
    /* TODO: callback fce: fann_callback_type */
}
/* }}} */

/* {{{ proto double fann_get_quickprop_decay(resource ann)
   Returns the quickprop decay factor. */
PHP_FUNCTION(fann_get_quickprop_decay)
{
	PHP_FANN_GET_PARAM(fann_get_quickprop_decay, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_quickprop_decay(resource ann, double quickprop_decay)
   Sets the quickprop decay factor */
PHP_FUNCTION(fann_set_quickprop_decay)
{
    PHP_FANN_SET_PARAM(fann_set_quickprop_decay, d, double);
}
/* }}} */

/* {{{ proto double fann_get_quickprop_mu(resource ann)
   Returns the quickprop mu factor */
PHP_FUNCTION(fann_get_quickprop_mu)
{
	PHP_FANN_GET_PARAM(fann_get_quickprop_mu, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_quickprop_mu(resource ann, double quickprop_mu)
   Sets the quickprop mu factor */
PHP_FUNCTION(fann_set_quickprop_mu)
{
    PHP_FANN_SET_PARAM(fann_set_quickprop_mu, d, double);
}
/* }}} */

/* {{{ proto double fann_get_rprop_increase_factor(resource ann)
   Returns the increase factor is a value larger than 1,
   which is used to increase the step-size during RPROP training */
PHP_FUNCTION(fann_get_rprop_increase_factor)
{
	PHP_FANN_GET_PARAM(fann_get_rprop_increase_factor, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_rprop_increase_factor(resource ann, double rprop_increase_factor)
   Sets the increase factor used during RPROP training */
PHP_FUNCTION(fann_set_rprop_increase_factor)
{
    PHP_FANN_SET_PARAM(fann_set_rprop_increase_factor, d, double);
}
/* }}} */

/* {{{ proto double fann_get_rprop_decrease_factor(resource ann)
   Returns The decrease factor is a value smaller than 1,
   which is used to decrease the step-size during RPROP training */
PHP_FUNCTION(fann_get_rprop_decrease_factor)
{
	PHP_FANN_GET_PARAM(fann_get_rprop_decrease_factor, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_rprop_decrease_factor(resource ann, double rprop_decrease_factor)
   Sets the decrease factor is a value smaller than 1,
   which is used to decrease the step-size during RPROP training */
PHP_FUNCTION(fann_set_rprop_decrease_factor)
{
    PHP_FANN_SET_PARAM(fann_set_rprop_decrease_factor, d, double);
}
/* }}} */

/* {{{ proto double fann_get_rprop_delta_min(resource ann)
   Returns the minimum step-size is a small positive number determining how small the minimum step-size may be */
PHP_FUNCTION(fann_get_rprop_delta_min)
{
	PHP_FANN_GET_PARAM(fann_get_rprop_delta_min, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_rprop_delta_min(resource ann, double rprop_delta_min)
   Sets the minimum step-size is a small positive number determining how small the minimum step-size may be */
PHP_FUNCTION(fann_set_rprop_delta_min)
{
    PHP_FANN_SET_PARAM(fann_set_rprop_delta_min, d, double);
}
/* }}} */

/* {{{ proto bool fann_get_rprop_delta_max(resource ann)
   Returns the maximum step-size is a positive number determining how large the maximum step-size may be */
PHP_FUNCTION(fann_get_rprop_delta_max)
{
	PHP_FANN_GET_PARAM(fann_get_rprop_delta_max, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_rprop_delta_max(resource ann, double rprop_delta_max)
   Sets the maximum step-size is a positive number determining how large the maximum step-size may be */
PHP_FUNCTION(fann_set_rprop_delta_max)
{
    PHP_FANN_SET_PARAM(fann_set_rprop_delta_max, d, double);
}
/* }}} */

/* {{{ proto double fann_get_rprop_delta_zero(resource ann)
   Returns the initial step-size is a positive number determining the initial step size */
PHP_FUNCTION(fann_get_rprop_delta_zero)
{
	PHP_FANN_GET_PARAM(fann_get_rprop_delta_zero, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_rprop_delta_zero(resource ann, double rprop_delta_zero)
   Sets the initial step-size is a positive number determining the initial step size */
PHP_FUNCTION(fann_set_rprop_delta_zero)
{
    PHP_FANN_SET_PARAM(fann_set_rprop_delta_zero, d, double);
}
/* }}} */

/* {{{ proto double fann_get_sarprop_weight_decay_shift(resource ann)
   Returns the sarprop weight decay shift */
PHP_FUNCTION(fann_get_sarprop_weight_decay_shift)
{
	PHP_FANN_GET_PARAM(fann_get_sarprop_weight_decay_shift, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_sarprop_weight_decay_shift(resource ann, double sarprop_weight_decay_shift)
   Sets the sarprop weight decay shift */
PHP_FUNCTION(fann_set_sarprop_weight_decay_shift)
{
    PHP_FANN_SET_PARAM(fann_set_sarprop_weight_decay_shift, d, double);
}
/* }}} */

/* {{{ proto double fann_get_sarprop_step_error_threshold_factor(resource ann)
   Returns the sarprop step error threshold factor */
PHP_FUNCTION(fann_get_sarprop_step_error_threshold_factor)
{
	PHP_FANN_GET_PARAM(fann_get_sarprop_step_error_threshold_factor, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_sarprop_step_error_threshold_factor(resource ann, double sarprop_step_error_threshold_factor)
   Sets the sarprop step error threshold factor */
PHP_FUNCTION(fann_set_sarprop_step_error_threshold_factor)
{
    PHP_FANN_SET_PARAM(fann_set_sarprop_step_error_threshold_factor, d, double);
}
/* }}} */

/* {{{ proto double fann_get_sarprop_step_error_shift(resource ann)
   Returns the sarprop step error shift */
PHP_FUNCTION(fann_get_sarprop_step_error_shift)
{
	PHP_FANN_GET_PARAM(fann_get_sarprop_step_error_shift, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_sarprop_step_error_shift(resource ann, double sarprop_step_error_shift)
   Sets the sarprop step error shift */
PHP_FUNCTION(fann_set_sarprop_step_error_shift)
{
    PHP_FANN_SET_PARAM(fann_set_sarprop_step_error_shift, d, double);
}
/* }}} */

/* {{{ proto double fann_get_sarprop_temperature(resource ann)
   Returns the sarprop_temperature */
PHP_FUNCTION(fann_get_sarprop_temperature)
{
	PHP_FANN_GET_PARAM(fann_get_sarprop_temperature, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_(resource ann, double sarprop_temperature)
   Sets the sarprop_temperature */
PHP_FUNCTION(fann_set_sarprop_temperature)
{
    PHP_FANN_SET_PARAM(fann_set_sarprop_temperature, d, double);
}
/* }}} */

/* {{{ proto resource fann_create_from_file(string configuration_file)
   Initializes neural network from configuration file */
PHP_FUNCTION(fann_create_from_file)
{
	char *cf_name = NULL;
	int cf_name_len;
	struct fann *ann;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &cf_name, &cf_name_len) == FAILURE) {
		return;
	}

	ann = fann_create_from_file(cf_name);
	PHP_FANN_ERROR_CHECK_ANN();
	PHP_FANN_RETURN_ANN();
}
/* }}} */

/* {{{ proto bool fann_save(resource ann, string configuration_file)
   Save the entire network to a configuration file */
PHP_FUNCTION(fann_save)
{
	zval *z_ann;
	char *cf_name = NULL;
	int cf_name_len;
	struct fann *ann;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ann, &cf_name, &cf_name_len) == FAILURE) {
		return;
	}

	PHP_FANN_FETCH_ANN();
	if (fann_save(ann, cf_name) == 0) {
		RETURN_TRUE;
	} else {
		PHP_FANN_ERROR_CHECK_ANN();
		RETURN_FALSE;
	}
}
/* }}} */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
