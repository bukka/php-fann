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
  | Author: Jakub Zelenka <jakub.php@gmail.com>                          |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_filestat.h"
#include "ext/standard/php_string.h"
#include "php_fann.h"

/* used fann type - default float */
#ifdef PHP_FANN_DOUBLE
#include "doublefann.h"
#else
#include "floatfann.h"
#endif

/* True global resources - no need for thread safety here */
static int le_fannbuf, le_fanntrainbuf;
#define le_fannbuf_name "FANN"
#define le_fanntrainbuf_name "FANN Train Data"

/* fann user data structure */
typedef struct _php_fann_user_data {
	zval *callback;
	zval *z_ann;
	zval *z_train_data;
} php_fann_user_data;

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

#if PHP_FANN_VERSION >= 0x020200
ZEND_BEGIN_ARG_INFO(arginfo_fann_copy, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()
#endif

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

ZEND_BEGIN_ARG_INFO(arginfo_fann_train_on_data, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, max_epochs)
ZEND_ARG_INFO(0, epochs_between_reports)
ZEND_ARG_INFO(0, desired_error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_train_on_file, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, filename)
ZEND_ARG_INFO(0, max_epochs)
ZEND_ARG_INFO(0, epochs_between_reports)
ZEND_ARG_INFO(0, desired_error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_train_epoch, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_test_data, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_read_train_from_file, 0)
ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()

#if PHP_FANN_VERSION >= 0x020200
ZEND_BEGIN_ARG_INFO(arginfo_fann_create_train, 0)
ZEND_ARG_INFO(0, num_data)
ZEND_ARG_INFO(0, num_input)
ZEND_ARG_INFO(0, num_output)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_create_train_from_callback, 0)
ZEND_ARG_INFO(0, num_data)
ZEND_ARG_INFO(0, num_input)
ZEND_ARG_INFO(0, num_output)
ZEND_ARG_INFO(0, user_function)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_fann_destroy_train, 0)
ZEND_ARG_INFO(0, train_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_shuffle_train_data, 0)
ZEND_ARG_INFO(0, train_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_scale_train, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, train_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_descale_train, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, train_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_input_scaling_params, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, train_data)
ZEND_ARG_INFO(0, new_input_min)
ZEND_ARG_INFO(0, new_input_max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_output_scaling_params, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, train_data)
ZEND_ARG_INFO(0, new_output_min)
ZEND_ARG_INFO(0, new_output_max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_scaling_params, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, train_data)
ZEND_ARG_INFO(0, new_input_min)
ZEND_ARG_INFO(0, new_input_max)
ZEND_ARG_INFO(0, new_output_min)
ZEND_ARG_INFO(0, new_output_max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_clear_scaling_params, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_scale_input, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, input_vector)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_scale_output, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, output_vector)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_descale_input, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, input_vector)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_descale_output, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, output_vector)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_scale_input_train_data, 0)
ZEND_ARG_INFO(0, train_data)
ZEND_ARG_INFO(0, new_min)
ZEND_ARG_INFO(0, new_max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_scale_output_train_data, 0)
ZEND_ARG_INFO(0, train_data)
ZEND_ARG_INFO(0, new_min)
ZEND_ARG_INFO(0, new_max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_scale_train_data, 0)
ZEND_ARG_INFO(0, train_data)
ZEND_ARG_INFO(0, new_min)
ZEND_ARG_INFO(0, new_max)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_merge_train_data, 0)
ZEND_ARG_INFO(0, data1)
ZEND_ARG_INFO(0, data2)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_duplicate_train_data, 0)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_subset_train_data, 0)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, pos)
ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_length_train_data, 0)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_num_input_train_data, 0)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_num_output_train_data, 0)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_save_train, 0)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, file_name)
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

#if PHP_FANN_VERSION >= 0x020200
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
#endif

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_activation_function, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, layer)
ZEND_ARG_INFO(0, neuron)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_function, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_function)
ZEND_ARG_INFO(0, layer)
ZEND_ARG_INFO(0, neuron)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_function_layer, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_function)
ZEND_ARG_INFO(0, layer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_function_hidden, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_function_output, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_function)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_activation_steepness, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, layer)
ZEND_ARG_INFO(0, neuron)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_steepness, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_steepness)
ZEND_ARG_INFO(0, layer)
ZEND_ARG_INFO(0, neuron)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_steepness_layer, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_steepness)
ZEND_ARG_INFO(0, layer)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_steepness_hidden, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_steepness)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_activation_steepness_output, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, activation_steepness)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_cascadetrain_on_data, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, max_neurons)
ZEND_ARG_INFO(0, neurons_between_reports)
ZEND_ARG_INFO(0, desired_error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_cascadetrain_on_file, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, filename)
ZEND_ARG_INFO(0, max_neurons)
ZEND_ARG_INFO(0, neurons_between_reports)
ZEND_ARG_INFO(0, desired_error)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_output_change_fraction, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_output_change_fraction, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_output_change_fraction)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_output_stagnation_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_output_stagnation_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_output_stagnation_epochs)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_candidate_change_fraction, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_candidate_change_fraction, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_candidate_change_fraction)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_candidate_stagnation_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_candidate_stagnation_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_candidate_stagnation_epochs)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_weight_multiplier, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_weight_multiplier, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_weight_multiplier)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_candidate_limit, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_candidate_limit, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_candidate_limit)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_max_out_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_max_out_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_max_out_epochs)
ZEND_END_ARG_INFO()

#if PHP_FANN_VERSION >= 0x020200
ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_min_out_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_min_out_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_min_out_epochs)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_max_cand_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_max_cand_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_max_cand_epochs)
ZEND_END_ARG_INFO()

#if PHP_FANN_VERSION >= 0x020200
ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_min_cand_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_min_cand_epochs, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_min_cand_epochs)
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_num_candidates, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_activation_functions_count, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_activation_functions, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_activation_functions, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_activation_functions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_activation_steepnesses_count, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_activation_steepnesses, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_activation_steepnesses, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_activation_steepnesses_count)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_cascade_num_candidate_groups, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_cascade_num_candidate_groups, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, cascade_num_candidate_groups)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_create_from_file, 0)
ZEND_ARG_INFO(0, configuration_file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_save, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, configuration_file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_set_error_log, 0)
ZEND_ARG_INFO(0, errdat)
ZEND_ARG_INFO(0, log_file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_errno, 0)
ZEND_ARG_INFO(0, errdat)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_reset_errno, 0)
ZEND_ARG_INFO(0, errdat)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_reset_errstr, 0)
ZEND_ARG_INFO(0, errdat)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_get_errstr, 0)
ZEND_ARG_INFO(0, errdat)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_print_error, 0)
ZEND_ARG_INFO(0, errdat)
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
#if PHP_FANN_VERSION >= 0x020200
	PHP_FE(fann_copy,                                     arginfo_fann_copy)
#endif
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
	PHP_FE(fann_train_on_data,                            arginfo_fann_train_on_data)
	PHP_FE(fann_train_on_file,                            arginfo_fann_train_on_file)
	PHP_FE(fann_train_epoch,                              arginfo_fann_train_epoch)
	PHP_FE(fann_test_data,                                arginfo_fann_test_data)
	PHP_FE(fann_read_train_from_file,                     arginfo_fann_read_train_from_file)
#if PHP_FANN_VERSION >= 0x020200
	PHP_FE(fann_create_train,                             arginfo_fann_create_train)
	PHP_FE(fann_create_train_from_callback,               arginfo_fann_create_train_from_callback)
#endif
	PHP_FE(fann_destroy_train,                            arginfo_fann_destroy_train)
	PHP_FE(fann_shuffle_train_data,                       arginfo_fann_shuffle_train_data)
	PHP_FE(fann_scale_train,                              arginfo_fann_scale_train)
	PHP_FE(fann_descale_train,                            arginfo_fann_descale_train)
	PHP_FE(fann_set_input_scaling_params,                 arginfo_fann_set_input_scaling_params)
	PHP_FE(fann_set_output_scaling_params,                arginfo_fann_set_output_scaling_params)
	PHP_FE(fann_set_scaling_params,                       arginfo_fann_set_scaling_params)
	PHP_FE(fann_clear_scaling_params,                     arginfo_fann_clear_scaling_params)
	PHP_FE(fann_scale_input,                              arginfo_fann_scale_input)
	PHP_FE(fann_scale_output,                             arginfo_fann_scale_output)
	PHP_FE(fann_descale_input,                            arginfo_fann_descale_input)
	PHP_FE(fann_descale_output,                           arginfo_fann_descale_output)
	PHP_FE(fann_scale_input_train_data,                   arginfo_fann_scale_input_train_data)
	PHP_FE(fann_scale_output_train_data,                  arginfo_fann_scale_output_train_data)
	PHP_FE(fann_scale_train_data,                         arginfo_fann_scale_train_data)
	PHP_FE(fann_merge_train_data,                         arginfo_fann_merge_train_data)
	PHP_FE(fann_duplicate_train_data,                     arginfo_fann_duplicate_train_data)
	PHP_FE(fann_subset_train_data,                        arginfo_fann_subset_train_data)
	PHP_FE(fann_length_train_data,                        arginfo_fann_length_train_data)
	PHP_FE(fann_num_input_train_data,                     arginfo_fann_num_input_train_data)
	PHP_FE(fann_num_output_train_data,                    arginfo_fann_num_output_train_data)
	PHP_FE(fann_save_train,                               arginfo_fann_save_train)
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
#if PHP_FANN_VERSION >= 0x020200
	PHP_FE(fann_get_sarprop_weight_decay_shift,           arginfo_fann_get_sarprop_weight_decay_shift)
	PHP_FE(fann_set_sarprop_weight_decay_shift,           arginfo_fann_set_sarprop_weight_decay_shift)
	PHP_FE(fann_get_sarprop_step_error_threshold_factor,  arginfo_fann_get_sarprop_step_error_threshold_factor)
	PHP_FE(fann_set_sarprop_step_error_threshold_factor,  arginfo_fann_set_sarprop_step_error_threshold_factor)
	PHP_FE(fann_get_sarprop_step_error_shift,             arginfo_fann_get_sarprop_step_error_shift)
	PHP_FE(fann_set_sarprop_step_error_shift,             arginfo_fann_set_sarprop_step_error_shift)
	PHP_FE(fann_get_sarprop_temperature,                  arginfo_fann_get_sarprop_temperature)
	PHP_FE(fann_set_sarprop_temperature,                  arginfo_fann_set_sarprop_temperature)
#endif
	PHP_FE(fann_get_training_algorithm,                   arginfo_fann_get_training_algorithm)
	PHP_FE(fann_set_training_algorithm,                   arginfo_fann_set_training_algorithm)
	PHP_FE(fann_get_activation_function,                  arginfo_fann_get_activation_function)
	PHP_FE(fann_set_activation_function,                  arginfo_fann_set_activation_function)
	PHP_FE(fann_set_activation_function_layer,            arginfo_fann_set_activation_function_layer)
	PHP_FE(fann_set_activation_function_hidden,           arginfo_fann_set_activation_function_hidden)
	PHP_FE(fann_set_activation_function_output,           arginfo_fann_set_activation_function_output)
	PHP_FE(fann_get_activation_steepness,                 arginfo_fann_get_activation_steepness)
	PHP_FE(fann_set_activation_steepness,                 arginfo_fann_set_activation_steepness)
	PHP_FE(fann_set_activation_steepness_layer,           arginfo_fann_set_activation_steepness_layer)
	PHP_FE(fann_set_activation_steepness_hidden,          arginfo_fann_set_activation_steepness_hidden)
	PHP_FE(fann_set_activation_steepness_output,          arginfo_fann_set_activation_steepness_output)
	PHP_FE(fann_cascadetrain_on_data,                     arginfo_fann_cascadetrain_on_data)
	PHP_FE(fann_cascadetrain_on_file,                     arginfo_fann_cascadetrain_on_file)
	PHP_FE(fann_get_cascade_output_change_fraction,       arginfo_fann_get_cascade_output_change_fraction)
	PHP_FE(fann_set_cascade_output_change_fraction,       arginfo_fann_set_cascade_output_change_fraction)
	PHP_FE(fann_get_cascade_output_stagnation_epochs,     arginfo_fann_get_cascade_output_stagnation_epochs)
	PHP_FE(fann_set_cascade_output_stagnation_epochs,     arginfo_fann_set_cascade_output_stagnation_epochs)
	PHP_FE(fann_get_cascade_candidate_change_fraction,    arginfo_fann_get_cascade_candidate_change_fraction)
	PHP_FE(fann_set_cascade_candidate_change_fraction,    arginfo_fann_set_cascade_candidate_change_fraction)
	PHP_FE(fann_get_cascade_candidate_stagnation_epochs,  arginfo_fann_get_cascade_candidate_stagnation_epochs)
	PHP_FE(fann_set_cascade_candidate_stagnation_epochs,  arginfo_fann_set_cascade_candidate_stagnation_epochs)
	PHP_FE(fann_get_cascade_weight_multiplier,            arginfo_fann_get_cascade_weight_multiplier)
	PHP_FE(fann_set_cascade_weight_multiplier,            arginfo_fann_set_cascade_weight_multiplier)
	PHP_FE(fann_get_cascade_candidate_limit,              arginfo_fann_get_cascade_candidate_limit)
	PHP_FE(fann_set_cascade_candidate_limit,              arginfo_fann_set_cascade_candidate_limit)
	PHP_FE(fann_get_cascade_max_out_epochs,               arginfo_fann_get_cascade_max_out_epochs)
	PHP_FE(fann_set_cascade_max_out_epochs,               arginfo_fann_set_cascade_max_out_epochs)
#if PHP_FANN_VERSION >= 0x020200
	PHP_FE(fann_get_cascade_min_out_epochs,               arginfo_fann_get_cascade_min_out_epochs)
	PHP_FE(fann_set_cascade_min_out_epochs,               arginfo_fann_set_cascade_min_out_epochs)
#endif
	PHP_FE(fann_get_cascade_max_cand_epochs,              arginfo_fann_get_cascade_max_cand_epochs)
	PHP_FE(fann_set_cascade_max_cand_epochs,              arginfo_fann_set_cascade_max_cand_epochs)
#if PHP_FANN_VERSION >= 0x020200
	PHP_FE(fann_get_cascade_min_cand_epochs,              arginfo_fann_get_cascade_min_cand_epochs)
	PHP_FE(fann_set_cascade_min_cand_epochs,              arginfo_fann_set_cascade_min_cand_epochs)
#endif
	PHP_FE(fann_get_cascade_num_candidates,               arginfo_fann_get_cascade_num_candidates)
	PHP_FE(fann_get_cascade_activation_functions_count,   arginfo_fann_get_cascade_activation_functions_count)
	PHP_FE(fann_get_cascade_activation_functions,         arginfo_fann_get_cascade_activation_functions)
	PHP_FE(fann_set_cascade_activation_functions,         arginfo_fann_set_cascade_activation_functions)
	PHP_FE(fann_get_cascade_activation_steepnesses_count, arginfo_fann_get_cascade_activation_steepnesses_count)
	PHP_FE(fann_get_cascade_activation_steepnesses,       arginfo_fann_get_cascade_activation_steepnesses)
	PHP_FE(fann_set_cascade_activation_steepnesses,       arginfo_fann_set_cascade_activation_steepnesses)
	PHP_FE(fann_get_cascade_num_candidate_groups,         arginfo_fann_get_cascade_num_candidate_groups)
	PHP_FE(fann_set_cascade_num_candidate_groups,         arginfo_fann_set_cascade_num_candidate_groups)
	PHP_FE(fann_create_from_file,                         arginfo_fann_create_from_file)
	PHP_FE(fann_save,                                     arginfo_fann_save)
	PHP_FE(fann_set_error_log,                            arginfo_fann_set_error_log)
	PHP_FE(fann_get_errno,                                arginfo_fann_get_errno)
	PHP_FE(fann_reset_errno,                              arginfo_fann_reset_errno)
	PHP_FE(fann_reset_errstr,                             arginfo_fann_reset_errstr)
	PHP_FE(fann_get_errstr,                               arginfo_fann_get_errstr)
	PHP_FE(fann_print_error,                              arginfo_fann_print_error)
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
#define PHP_FANN_RETURN_TRAIN_DATA()									\
	if (!train_data) { RETURN_FALSE; }									\
	ZEND_REGISTER_RESOURCE(return_value, train_data, le_fanntrainbuf)

/* macro for fetching ann resource */
#define PHP_FANN_FETCH_ANN()											\
	ZEND_FETCH_RESOURCE(ann, struct fann *, &z_ann, -1, le_fannbuf_name, le_fannbuf)

/* macro for fetching train data resource */
#define PHP_FANN_FETCH_TRAIN_DATA_EX(_train_data)											\
	ZEND_FETCH_RESOURCE(_train_data, struct fann_train_data *, &z_##_train_data, -1, le_fanntrainbuf_name, le_fanntrainbuf)

/* macro for fetching train data resource using train_data variable */
#define PHP_FANN_FETCH_TRAIN_DATA() PHP_FANN_FETCH_TRAIN_DATA_EX(train_data)

/* fetch error data */
#define PHP_FANN_FETCH_ERRDAT() \
	ZEND_FETCH_RESOURCE2(errdat, struct fann_error *, &z_errdat, -1, NULL, le_fannbuf, le_fanntrainbuf)

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
#define REGISTER_FANN_CONSTANT(constant) REGISTER_LONG_CONSTANT(#constant, constant, CONST_CS | CONST_PERSISTENT)

/* {{{ fann_destructor_fannbuf()
   fann resource destructor */
static void fann_destructor_fannbuf(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	php_fann_user_data *user_data;
	struct fann *ann = (struct fann *) rsrc->ptr;
	user_data = (php_fann_user_data *) fann_get_user_data(ann);
	if (user_data != (php_fann_user_data *) NULL) {
		zval_dtor(user_data->callback);
		FREE_ZVAL(user_data->callback);
		efree(user_data);
	}
	if (ann->error_log)
		fclose(ann->error_log);
	fann_destroy(ann);
}
/* }}} */

/* {{{ fann_destructor_fanntrainbuf()
   fann_train resource destructor */
static void fann_destructor_fanntrainbuf(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct fann_train_data *train_data = (struct fann_train_data *) rsrc->ptr;
	if (train_data->error_log)
		fclose(train_data->error_log);
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

	/* Fann version constant */
	REGISTER_STRING_CONSTANT("FANN_VERSION", PHP_FANN_VERSION_STRING, CONST_PERSISTENT | CONST_CS);
	
	/* Train constants */
	REGISTER_FANN_CONSTANT(FANN_TRAIN_INCREMENTAL);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_BATCH);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_RPROP);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_QUICKPROP);
#if PHP_FANN_VERSION >= 0x020200
	REGISTER_FANN_CONSTANT(FANN_TRAIN_SARPROP);
#endif
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
	/* FANN Error Codes */
	REGISTER_FANN_CONSTANT(FANN_E_NO_ERROR);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_OPEN_CONFIG_R);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_OPEN_CONFIG_W);
	REGISTER_FANN_CONSTANT(FANN_E_WRONG_CONFIG_VERSION);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_READ_CONFIG);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_READ_NEURON);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_READ_CONNECTIONS);
	REGISTER_FANN_CONSTANT(FANN_E_WRONG_NUM_CONNECTIONS);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_OPEN_TD_W);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_OPEN_TD_R);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_READ_TD);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_ALLOCATE_MEM);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_TRAIN_ACTIVATION);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_USE_ACTIVATION);
	REGISTER_FANN_CONSTANT(FANN_E_TRAIN_DATA_MISMATCH);
	REGISTER_FANN_CONSTANT(FANN_E_CANT_USE_TRAIN_ALG);
	REGISTER_FANN_CONSTANT(FANN_E_TRAIN_DATA_SUBSET);
	REGISTER_FANN_CONSTANT(FANN_E_INDEX_OUT_OF_BOUND);
	REGISTER_FANN_CONSTANT(FANN_E_SCALE_NOT_PRESENT);
#if PHP_FANN_VERSION >= 0x020200
	REGISTER_FANN_CONSTANT(FANN_E_INPUT_NO_MATCH);
	REGISTER_FANN_CONSTANT(FANN_E_OUTPUT_NO_MATCH);
#endif
	
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
	php_info_print_table_row(2, "FANN support", "enabled");
	php_info_print_table_row(2, "FANN library version", PHP_FANN_VERSION_STRING);
	php_info_print_table_end();
}
/* }}} */

#define PHP_FANN_PATH_OK(_retval) Z_TYPE(_retval) != IS_BOOL || !Z_BVAL(_retval)

/* php_fann_get_file_path() {{{ */
static char *php_fann_get_path_for_open(char *path, int path_len, int read TSRMLS_DC)
{
	zval retval;
	char *path_for_open;

	if (read) {
		php_stat(path, (php_stat_len) path_len, FS_IS_R, &retval TSRMLS_CC);
	}
	else {
		php_stat(path, (php_stat_len) path_len, FS_IS_W, &retval TSRMLS_CC);
		if (!PHP_FANN_PATH_OK(retval)) {
			char *dirname = estrndup(path, path_len);
			size_t dirname_len = php_dirname(dirname, (size_t) path_len);
			php_stat(dirname, (php_stat_len) dirname_len, FS_IS_W, &retval TSRMLS_CC);
			efree(dirname);
		}
	}
	if (PHP_FANN_PATH_OK(retval))  {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filename '%s' cannot be opened for %s",
		  path, read ? "reading" : "writing");
		path_for_open = NULL;
	}
	else
		php_stream_locate_url_wrapper(path, &path_for_open, 0 TSRMLS_CC);
	return path_for_open;
}
/* }}} */

/* php_fann_array_to_zval() {{{ */
static void php_fann_array_to_zval(const fann_type *from, zval *to, int len)
{
	int i;
	array_init_size(to, len);
	for (i = 0; i < len; i++)
		add_index_double(to, i, (double) from[i]);
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

/* {{{ php_funn_io_foreach()
   callback for converting input hash map to fann_type array */
static int php_funn_process_array_foreach(zval **element TSRMLS_DC, int num_args,
										  va_list args, zend_hash_key *hash_key)
{
	fann_type *input = va_arg(args, fann_type *);
	int *pos = va_arg(args, int *);
	
	convert_to_double_ex(element);
	input[(*pos)++] = (fann_type) Z_DVAL_PP(element);
	
	return ZEND_HASH_APPLY_KEEP;
}
/* }}} */

/* php_fann_convert_array() {{{ */
static int php_fann_process_array(struct fann *ann, zval *z_array, fann_type **array, int is_input TSRMLS_DC)
{
	int i = 0, n;
	n = zend_hash_num_elements(Z_ARRVAL_P(z_array));
	if ((is_input && php_fann_check_num_inputs(ann, n TSRMLS_CC) == FAILURE) ||
		(!is_input && php_fann_check_num_outputs(ann, n TSRMLS_CC))) {
		return 0;
	}
	*array = (fann_type *) emalloc(sizeof(fann_type) * n);
	zend_hash_apply_with_arguments(Z_ARRVAL_P(z_array) TSRMLS_CC,
								   (apply_func_args_t) php_funn_process_array_foreach, 2, *array, &i);
	return n;
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
	HashPosition pos;
	int i = 0;
	unsigned long tmpnum;
	double tmprate;

	if (conn_rate) {
		if (zend_parse_parameters(num_args TSRMLS_CC, "dla", &tmprate, &tmpnum, &array) == FAILURE) {
			return FAILURE;
		}
		*conn_rate = (float)tmprate;
		*num_layers = (uint)tmpnum;
	}
	else {
		if (zend_parse_parameters(num_args TSRMLS_CC, "la", &tmpnum, &array) == FAILURE) {
			return FAILURE;
		}
		*num_layers = (uint)tmpnum;
	}

	if (php_fann_check_num_layers(
			*num_layers, zend_hash_num_elements(Z_ARRVAL_P(array)) TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}

	*layers = (uint *) emalloc(*num_layers * sizeof(uint));
	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &ppdata, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)) {
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

/* php_fann_init_ann() {{{ */
static int php_fann_callback(struct fann *ann, struct fann_train_data *train,
							 unsigned int max_epochs,
							 unsigned int epochs_between_reports,
							 float desired_error, unsigned int epochs)
{
	php_fann_user_data *user_data = fann_get_user_data(ann);
	if (user_data != (php_fann_user_data *) NULL) {
		zend_fcall_info fci;
		zend_fcall_info_cache fci_cache;
		zval *retval, *z_max_epochs, *z_epochs_between_reports, *z_desired_error, *z_epochs, *z_train_data;
		zval **params[6];
		long rc;
		char *is_callable_error = NULL;
		TSRMLS_FETCH();
		
		if (zend_fcall_info_init(user_data->callback, 0, &fci, &fci_cache, NULL, &is_callable_error TSRMLS_CC)
			!= SUCCESS || is_callable_error) {
			if (is_callable_error) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "User callback is not a valie callback, %s",
								 is_callable_error);
				efree(is_callable_error);
			}
			else {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "User callback is not a valie callback");
			}
			return -1;
		}
		MAKE_STD_ZVAL(z_max_epochs);
		MAKE_STD_ZVAL(z_epochs_between_reports);
		MAKE_STD_ZVAL(z_desired_error);
		MAKE_STD_ZVAL(z_epochs);
		ZVAL_LONG(z_max_epochs, (long) max_epochs);
		ZVAL_LONG(z_epochs_between_reports, (long) epochs_between_reports);
		ZVAL_DOUBLE(z_desired_error, (double) desired_error);
		ZVAL_LONG(z_epochs, (long) epochs);
		fci.retval_ptr_ptr = &retval;
		fci.no_separation = 0;
		fci.param_count = 6;
		fci.params = params;
		params[0] = &user_data->z_ann;
		if (user_data->z_train_data) {
			params[1] = &user_data->z_train_data;
		}
		else {
			MAKE_STD_ZVAL(z_train_data);
			ZVAL_NULL(z_train_data);
			params[1] = &z_train_data;
		}
		params[2] = &z_max_epochs;
		params[3] = &z_epochs_between_reports;
		params[4] = &z_desired_error;
		params[5] = &z_epochs;
		if (zend_call_function(&fci, &fci_cache TSRMLS_CC) != SUCCESS || !retval) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the user callback");
			zval_ptr_dtor(&retval);
			return -1;
		}
		convert_to_boolean(retval);
		rc = Z_BVAL_P(retval);
		zval_ptr_dtor(&retval);
		if (!user_data->z_train_data)
			FREE_ZVAL(z_train_data);
		FREE_ZVAL(z_max_epochs);
		FREE_ZVAL(z_epochs_between_reports);
		FREE_ZVAL(z_desired_error);
		FREE_ZVAL(z_epochs);
		if (!rc)
			return -1;
	}
	return 0;
}
/* }}} */

/* php_fann_update_user_data() {{{ */
static void php_fann_update_user_data(struct fann *ann, zval *z_ann, zval *z_train_data)
{
	php_fann_user_data *user_data = fann_get_user_data(ann);
	if (user_data != (php_fann_user_data *) NULL) {
		user_data->z_ann = z_ann;
		user_data->z_train_data = z_train_data;
	}
}
/* }}} */

/* php_fann_init_ann() {{{ */
static void php_fann_init_ann(struct fann *ann)
{
	/* set callback for reporting - don't print anything during fann_train_on_(data|file) */
	fann_set_callback(ann, (fann_callback_type) php_fann_callback);
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
	php_fann_init_ann(ann);
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
	php_fann_init_ann(ann);
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
	php_fann_init_ann(ann);
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
	php_fann_init_ann(ann);
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
	php_fann_init_ann(ann);
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
	php_fann_init_ann(ann);
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

#if PHP_FANN_VERSION >= 0x020200
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
#endif

/* {{{ proto array fann_run(resource ann, array input)
   Runs input through the neural network */
PHP_FUNCTION(fann_run)
{
	zval *z_ann, *z_input;
	struct fann *ann;
	fann_type *input, *output;
	int num_outputs, i;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &z_input) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	/* process input array */
	if (!php_fann_process_array(ann, z_input, &input, 1 TSRMLS_CC)) {
		RETURN_FALSE;
	}
		
	output = fann_run(ann, input);
	efree(input);
	num_outputs = fann_get_num_output(ann);
	PHP_FANN_ERROR_CHECK_ANN();

	array_init(return_value);
	for (i = 0; i < num_outputs; i++) {
		add_next_index_double(return_value, (double) output[i]);
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
	HashPosition pos;
	struct fann *ann;
	struct fann_connection *connections;
	uint num_connections, i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &array) == FAILURE) {
		return;
	}

	PHP_FANN_FETCH_ANN();
	num_connections = zend_hash_num_elements(Z_ARRVAL_P(array));
	connections = (struct fann_connection *) emalloc(num_connections * sizeof(struct fann_connection));
	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &current, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)) {
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
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "raa", &z_ann, &z_input, &z_output) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	
	/* process input array */
	if (!php_fann_process_array(ann, z_input, &input, 1 TSRMLS_CC)) {
		RETURN_FALSE;
	}
	/* process output array */
	if (!php_fann_process_array(ann, z_output, &desired_output, 0 TSRMLS_CC)) {
		efree(input);
		RETURN_FALSE;
	}
		
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
	int num_outputs, i;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "raa", &z_ann, &z_input, &z_output) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	/* process input array */
	if (!php_fann_process_array(ann, z_input, &input, 1 TSRMLS_CC)) {
		RETURN_FALSE;
	}
	/* process output array */
	if (!(num_outputs = php_fann_process_array(ann, z_output, &desired_output, 0 TSRMLS_CC))) {
		efree(input);
		RETURN_FALSE;
	}
		
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
   Trains on an entire dataset, for a period of time (data are read from file) */
PHP_FUNCTION(fann_train_on_file)
{
	zval *z_ann;
	char *filename;
	int filename_len;
	long max_epochs, epochs_between_reports;
	double desired_error;
	struct fann *ann;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rplld", &z_ann, &filename, &filename_len,
							  &max_epochs, &epochs_between_reports, &desired_error) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	filename = php_fann_get_path_for_open(filename, filename_len, 1 TSRMLS_CC);
	if (!filename) {
		RETURN_FALSE;
	}
	php_fann_update_user_data(ann, z_ann, (zval *) NULL);
	fann_train_on_file(ann, filename, max_epochs, epochs_between_reports, desired_error);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fann_train_on_data(resource ann, resource data, int max_epochs, int epochs_between_reports, float desired_error)
   Trains on an entire dataset, for a period of time */
PHP_FUNCTION(fann_train_on_data)
{

	zval *z_ann, *z_train_data;
	long max_epochs, epochs_between_reports;
	double desired_error;
	struct fann *ann;
	struct fann_train_data *train_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrlld", &z_ann, &z_train_data,
							  &max_epochs, &epochs_between_reports, &desired_error) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	php_fann_update_user_data(ann, z_ann, z_train_data);
	fann_train_on_data(ann, train_data, max_epochs, epochs_between_reports, desired_error);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto double fann_train_epoch(resource ann, resource train_data)
   Trains one epoch with a set of training data */
PHP_FUNCTION(fann_train_epoch)
{
	zval *z_ann, *z_train_data;
	struct fann *ann;
	struct fann_train_data *train_data;
	double mse;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &z_ann, &z_train_data) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	mse = (double) fann_train_epoch(ann, train_data);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_DOUBLE(mse);
}
/* }}} */

/* {{{ proto double fann_test_data(resource ann, resource train_data)
   Tests a set of training data and calculates the MSE for the training data */
PHP_FUNCTION(fann_test_data)
{
	zval *z_ann, *z_train_data;
	struct fann *ann;
	struct fann_train_data *train_data;
	double mse;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &z_ann, &z_train_data) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	mse = (double) fann_test_data(ann, train_data);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_DOUBLE(mse);
}
/* }}} */

/* {{{ proto resource fann_read_train_from_file(string filename)
   Reads a file that stores training data */
PHP_FUNCTION(fann_read_train_from_file)
{
	char *filename;
	int filename_len;
	struct fann_train_data *train_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &filename, &filename_len) == FAILURE) {
		return;
	}
	filename = php_fann_get_path_for_open(filename, filename_len, 1 TSRMLS_CC);
	if (!filename) {
		RETURN_FALSE;
	}
	train_data = fann_read_train_from_file(filename);
	if (!train_data) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid data train file format in '%s'", filename);
		RETURN_FALSE;
	}
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();
	PHP_FANN_RETURN_TRAIN_DATA();
}
/* }}} */

#if PHP_FANN_VERSION >= 0x020200
/* {{{ proto resource fann_create_train(int num_data, int num_input, int num_output)
   Creates an empty training data struct */
PHP_FUNCTION(fann_create_train)
{
	long num_data, num_input, num_output;
	struct fann_train_data *train_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &num_data, &num_input, &num_output) == FAILURE) {
		return;
	}
	train_data = fann_create_train(num_data, num_input, num_output);
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();
	PHP_FANN_RETURN_TRAIN_DATA();
}
/* }}} */

/* {{{ proto resource fann_create_train_from_callback(int num_data, int num_input, int num_output, callable user_function)
   Creates the training data struct from a user supplied function */
PHP_FUNCTION(fann_create_train_from_callback)
{
	zval *z_num_data, *z_num_input, *z_num_output, *retval;
	zval **z_input, **z_output, **params[3];
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	int i, j;
	struct fann_train_data *train_data;
	
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzzf", &z_num_data, &z_num_input, &z_num_output,
							  &fci, &fci_cache) == FAILURE) {
		return;
	}
	convert_to_long_ex(&z_num_data);
	convert_to_long_ex(&z_num_input);
	convert_to_long_ex(&z_num_output);
	
	train_data = fann_create_train(Z_LVAL_P(z_num_data), Z_LVAL_P(z_num_input), Z_LVAL_P(z_num_output));
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();
	
	// initialize callback function
	fci.retval_ptr_ptr = &retval;
	fci.no_separation = 0;
	fci.param_count = 3;
	fci.params = params;
	params[0] = &z_num_data;
	params[1] = &z_num_input;
	params[2] = &z_num_output;
	// call callback for each data
	for (i = 0; i < Z_LVAL_P(z_num_data); i++) {
		if (zend_call_function(&fci, &fci_cache TSRMLS_CC) != SUCCESS || !retval) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the user callback");
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (Z_TYPE_P(retval) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The user callback result should be an array");
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (zend_hash_index_find(Z_ARRVAL_P(retval), 0, (void **) &z_input) != SUCCESS &&
			zend_hash_find(Z_ARRVAL_P(retval), "input", sizeof("input"), (void **) &z_input) != SUCCESS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The return value must have 'input' or 0 key for input");
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (zend_hash_index_find(Z_ARRVAL_P(retval), 1, (void **) &z_output) != SUCCESS &&
			zend_hash_find(Z_ARRVAL_P(retval), "output", sizeof("output"), (void **) &z_output) != SUCCESS) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The return value must have 'output' or 1 key for output");
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (Z_TYPE_PP(z_input) != IS_ARRAY ||
			zend_hash_num_elements(Z_ARRVAL_PP(z_input)) != Z_LVAL_P(z_num_input)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Returned input must be an array with %ld elements",
							 Z_LVAL_P(z_num_input));
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (Z_TYPE_PP(z_output) != IS_ARRAY ||
			zend_hash_num_elements(Z_ARRVAL_PP(z_output)) != Z_LVAL_P(z_num_output)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Returned output must be an array with %ld elements",
							 Z_LVAL_P(z_num_output));
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		// convert input array
		j = 0;
		zend_hash_apply_with_arguments(Z_ARRVAL_PP(z_input) TSRMLS_CC,
									   (apply_func_args_t) php_funn_process_array_foreach, 2,
									   train_data->input[i], &j);
		// convert output array
		j = 0;
		zend_hash_apply_with_arguments(Z_ARRVAL_PP(z_output) TSRMLS_CC,
									   (apply_func_args_t) php_funn_process_array_foreach, 2,
									   train_data->output[i], &j);
		zval_ptr_dtor(&retval);
	}
	PHP_FANN_RETURN_TRAIN_DATA();
}
/* }}} */
#endif

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

/* {{{ proto bool fann_shuffle_train_data(resource train_data)
   Shuffles training data randomizing the order */
PHP_FUNCTION(fann_shuffle_train_data)
{
	zval *z_train_data;
	struct fann_train_data *train_data;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_train_data) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_TRAIN_DATA();
	fann_shuffle_train_data(train_data);
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fann_scale_train(resource ann, resource train_data)
   Scales input and output data based on previously calculated parameters */
PHP_FUNCTION(fann_scale_train)
{
	zval *z_ann, *z_train_data;
	struct fann *ann;
	struct fann_train_data *train_data;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &z_ann, &z_train_data) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	fann_scale_train(ann, train_data);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fann_descale_train(resource ann, resource train_data)
   Descales input and output data based on previously calculated parameters */
PHP_FUNCTION(fann_descale_train)
{
	zval *z_ann, *z_train_data;
	struct fann *ann;
	struct fann_train_data *train_data;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &z_ann, &z_train_data) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	fann_descale_train(ann, train_data);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fann_set_input_scaling_params(resource ann, resource train_data, int new_input_min, int new_input_max)
   Calculates input scaling parameters for future use based on training data */
PHP_FUNCTION(fann_set_input_scaling_params)
{
	zval *z_ann, *z_train_data;
	struct fann *ann;
	struct fann_train_data *train_data;
	double new_input_min, new_input_max;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrdd", &z_ann, &z_train_data,
							  &new_input_min, &new_input_max) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	if (fann_set_input_scaling_params(ann, train_data, (float) new_input_min, (float) new_input_max) == 0) {
		RETURN_TRUE;
	}
	else {
		PHP_FANN_ERROR_CHECK_TRAIN_DATA();
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool fann_set_output_scaling_params(resource ann, resource train_data, int new_output_min, int new_output_max)
   Calculates output scaling parameters for future use based on training data */
PHP_FUNCTION(fann_set_output_scaling_params)
{
	zval *z_ann, *z_train_data;
	struct fann *ann;
	struct fann_train_data *train_data;
	double new_output_min, new_output_max;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrdd", &z_ann, &z_train_data,
							  &new_output_min, &new_output_max) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	if (fann_set_output_scaling_params(ann, train_data, (float) new_output_min, (float) new_output_max) == 0) {
		RETURN_TRUE;
	}
	else {
		PHP_FANN_ERROR_CHECK_TRAIN_DATA();
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool fann_set_scaling_params(resource ann, resource train_data, int new_input_min, int new_input_max, int new_output_min, int new_output_max)
   Calculates input and output scaling parameters for future use based on training data */
PHP_FUNCTION(fann_set_scaling_params)
{
	zval *z_ann, *z_train_data;
	struct fann *ann;
	struct fann_train_data *train_data;
	double new_input_min, new_input_max, new_output_min, new_output_max;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrdddd", &z_ann, &z_train_data,
							  &new_input_min, &new_input_max, &new_output_min, &new_output_max) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	if (fann_set_scaling_params(ann, train_data, (float) new_input_min, (float) new_input_max,
								(float) new_output_min, (float) new_output_max) == 0) {
		RETURN_TRUE;
	}
	else {
		PHP_FANN_ERROR_CHECK_TRAIN_DATA();
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool fann_clear_scaling_params(resource ann)
   Clears scaling parameters */
PHP_FUNCTION(fann_clear_scaling_params)
{
    PHP_FANN_RESET_PARAM(fann_clear_scaling_params);
}
/* }}} */

/* {{{ proto array fann_scale_input(resource ann, array input_vector)
   Scales data in input vector before feed it to ann based on previously calculated parameters */
PHP_FUNCTION(fann_scale_input)
{
	struct fann *ann;
	zval *z_array, *z_ann;
	fann_type *array;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &z_array) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	if (!php_fann_process_array(ann, z_array, &array, 1 TSRMLS_CC)) {
		RETURN_FALSE;
	}
	fann_scale_input(ann, array);	
	php_fann_array_to_zval(array, return_value, fann_get_num_input(ann));
	efree(array);
	PHP_FANN_ERROR_CHECK_ANN();
}
/* }}} */

/* {{{ proto array fann_scale_output(resource ann, array output_vector)
   Scales data in output vector before feed it to ann based on previously calculated parameters */
PHP_FUNCTION(fann_scale_output)
{
	struct fann *ann;
	zval *z_array, *z_ann;
	fann_type *array;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &z_array) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	if (!php_fann_process_array(ann, z_array, &array, 0 TSRMLS_CC)) {
		RETURN_FALSE;
	}
	fann_scale_output(ann, array);
	php_fann_array_to_zval(array, return_value, fann_get_num_output(ann));
	efree(array);
	PHP_FANN_ERROR_CHECK_ANN();
}
/* }}} */

/* {{{ proto array fann_descale_input(resource ann, array input_vector)
   Scales data in input vector after feed it to ann based on previously calculated parameters */
PHP_FUNCTION(fann_descale_input)
{
	struct fann *ann;
	zval *z_array, *z_ann;
	fann_type *array;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &z_array) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	if (!php_fann_process_array(ann, z_array, &array, 1 TSRMLS_CC)) {
		RETURN_FALSE;
	}
	fann_descale_input(ann, array);
	php_fann_array_to_zval(array, return_value, fann_get_num_input(ann));
	efree(array);
	PHP_FANN_ERROR_CHECK_ANN();
}
/* }}} */

/* {{{ proto array fann_descale_output(resource ann, array output_vector)
   Scales data in output vector after feed it to ann based on previously calculated parameters */
PHP_FUNCTION(fann_descale_output)
{
	struct fann *ann;
	zval *z_array, *z_ann;
	fann_type *array;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &z_array) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	if (!php_fann_process_array(ann, z_array, &array, 0 TSRMLS_CC)) {
		RETURN_FALSE;
	}
	fann_descale_output(ann, array);
	php_fann_array_to_zval(array, return_value, fann_get_num_output(ann));
	efree(array);
	PHP_FANN_ERROR_CHECK_ANN();
}
/* }}} */

#define PHP_FANN_SCALE_TRAIN_DATA(_fce)									\
	zval *z_train_data;													\
	struct fann_train_data *train_data;									\
	double new_min, new_max;											\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &z_train_data, &new_min, &new_max) == FAILURE) { \
		return;															\
	}																	\
	PHP_FANN_FETCH_TRAIN_DATA();										\
	_fce(train_data, (fann_type) new_min, (fann_type) new_max);			\
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();									\
	RETURN_TRUE

/* {{{ proto bool fann_scale_input_train_data(resource train_data, double new_min, double new_max)
   Scales the inputs in the training data to the specified range */
PHP_FUNCTION(fann_scale_input_train_data)
{
	PHP_FANN_SCALE_TRAIN_DATA(fann_scale_input_train_data);
}
/* }}} */

/* {{{ proto bool fann_scale_output_train_data(resource train_data, double new_min, double new_max)
   Scales the outputs in the training data to the specified range */
PHP_FUNCTION(fann_scale_output_train_data)
{
	PHP_FANN_SCALE_TRAIN_DATA(fann_scale_output_train_data);
}
/* }}} */

/* {{{ proto bool fann_scale_train_data(resource train_data, double new_min, double new_max)
   Scales the inputs and outputs in the training data to the specified range */
PHP_FUNCTION(fann_scale_train_data)
{
	PHP_FANN_SCALE_TRAIN_DATA(fann_scale_train_data);
}
/* }}} */

/* {{{ proto resource fann_merge_train_data(resource data1, resource data2)
   Merges the data from data1 and data2 into new train data */
PHP_FUNCTION(fann_merge_train_data)
{
	zval *z_data1, *z_data2;
	struct fann_train_data *data1, *data2, *train_data;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rr", &z_data1, &z_data2) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_TRAIN_DATA_EX(data1);
	PHP_FANN_FETCH_TRAIN_DATA_EX(data2);
	train_data = fann_merge_train_data(data1, data2);
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();
	PHP_FANN_RETURN_TRAIN_DATA();
}
/* }}} */

/* {{{ proto resource fann_duplicate_train_data(resource train_data)
   Returns an exact copy of train data */
PHP_FUNCTION(fann_duplicate_train_data)
{
	zval *z_train_data;
	struct fann_train_data *train_data;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_train_data) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_TRAIN_DATA();
	train_data = fann_duplicate_train_data(train_data);
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();
	PHP_FANN_RETURN_TRAIN_DATA();
}
/* }}} */

/* {{{ proto resource fann_subset_train_data(resource train_data, int pos, int length)
   Returns an copy of a subset of the train data, starting at position pos and length elements forward */
PHP_FUNCTION(fann_subset_train_data)
{
	zval *z_train_data;
	struct fann_train_data *train_data;
	long pos, length;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rll", &z_train_data, &pos, &length) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_TRAIN_DATA();
	train_data = fann_subset_train_data(train_data, pos, length);
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();
	PHP_FANN_RETURN_TRAIN_DATA();
}
/* }}} */

/* {{{ proto int fann_length_train_data(resource train_data)
   Returns the number of training patterns in train data */
PHP_FUNCTION(fann_length_train_data)
{
	zval *z_train_data;
	struct fann_train_data *train_data;
		
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_train_data) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_TRAIN_DATA();
	RETURN_LONG((long) fann_length_train_data(train_data));
}
/* }}} */

/* {{{ proto int fann_num_input_train_data(resource train_data)
   Returns the number of inputs in each of the training patterns in train data */
PHP_FUNCTION(fann_num_input_train_data)
{
	zval *z_train_data;
	struct fann_train_data *train_data;
		
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_train_data) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_TRAIN_DATA();
	RETURN_LONG((long) fann_num_input_train_data(train_data));
}
/* }}} */

/* {{{ proto int fann_num_output_train_data(resource train_data)
   Returns the number of outputs in each of the training patterns in train data */
PHP_FUNCTION(fann_num_output_train_data)
{
	zval *z_train_data;
	struct fann_train_data *train_data;
		
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_train_data) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_TRAIN_DATA();
	RETURN_LONG((long) fann_num_output_train_data(train_data));
}
/* }}} */

/* {{{ proto bool fann_save_train(resource train_data, string filename)
   Saves the training structure to the file */
PHP_FUNCTION(fann_save_train)
{
	char *filename;
	int filename_len;
	zval *z_train_data;
	struct fann_train_data *train_data;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rp", &z_train_data, &filename, &filename_len)
		== FAILURE) {
		return;
	}
	PHP_FANN_FETCH_TRAIN_DATA();
	filename = php_fann_get_path_for_open(filename, filename_len, 0 TSRMLS_CC);
	if (!filename) {
		RETURN_FALSE;
	}
	if (fann_save_train(train_data, filename) == 0) {
		RETURN_TRUE;
	}
	else {
		PHP_FANN_ERROR_CHECK_TRAIN_DATA();
		RETURN_FALSE;
	}
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

/* {{{ proto double fann_get_learning_momentum(resource ann)
   Returns the learning momentum */
PHP_FUNCTION(fann_get_learning_momentum)
{
	PHP_FANN_GET_PARAM(fann_get_learning_momentum, RETURN_DOUBLE);
}

/* {{{ proto bool fann_set_learning_momentum(resource ann, double learning_momentum)
   Sets the learning momentum */
PHP_FUNCTION(fann_set_learning_momentum)
{
    PHP_FANN_SET_PARAM(fann_set_learning_momentum, d, double);
}
/* }}} */

/* {{{ proto bool fann_get_activation_function(resource ann, int layer, int neuron)
   Returns the activation function for neuron number neuron in layer number layer, counting the input layer as layer 0 */
PHP_FUNCTION(fann_get_activation_function)
{
	PHP_FANN_GET_PARAM2(fann_get_activation_function, RETURN_LONG, ll, long, long);
}
/* }}} */

/* {{{ proto bool fann_set_activation_function(resource ann, int activation_function, int layer, int neuron)
   Sets the activation function for neuron number neuron in layer number layer, counting the input layer as layer 0 */
PHP_FUNCTION(fann_set_activation_function)
{
	PHP_FANN_SET_PARAM3(fann_set_activation_function, lll, long, long, long);
}
/* }}} */

/* {{{ proto bool fann_set_activation_function_layer(resource ann, int activation_function, int layer)
   Sets the activation function for all the neurons in the layer number layer, counting the input layer as layer 0 */
PHP_FUNCTION(fann_set_activation_function_layer)
{
	PHP_FANN_SET_PARAM2(fann_set_activation_function_layer, ll, long, long);
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

/* {{{ proto double fann_get_activation_steepness(resource ann, int layer, int neuron)
   Returns the activation steepness for neuron number neuron in layer number layer, counting the input layer as layer 0 */
PHP_FUNCTION(fann_get_activation_steepness)
{
	PHP_FANN_GET_PARAM2(fann_get_activation_steepness, RETURN_DOUBLE, ll, long, long);
}
/* }}} */

/* {{{ proto bool fann_set_activation_steepness(resource ann, double steepness, int layer, int neuron)
   Sets the activation steepness for neuron number neuron in layer number layer, counting the input layer as layer 0 */
PHP_FUNCTION(fann_set_activation_steepness)
{
	PHP_FANN_SET_PARAM3(fann_set_activation_steepness, dll, double, long, long);
}
/* }}} */

/* {{{ proto bool fann_set_activation_steepness_layer(resource ann, double steepness, int layer)
   Sets the activation steepness for all the neurons in the layer number layer, counting the input layer as layer 0 */
PHP_FUNCTION(fann_set_activation_steepness_layer)
{
	PHP_FANN_SET_PARAM2(fann_set_activation_steepness_layer, dl, double, long);
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
	zval *z_ann, *callback;
	struct fann *ann;
	php_fann_user_data *user_data;
		
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rz", &z_ann, &callback) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	user_data = (php_fann_user_data *) fann_get_user_data(ann);
	if (user_data == (php_fann_user_data *) NULL) {
		user_data = (php_fann_user_data *) emalloc(sizeof (php_fann_user_data));
		MAKE_STD_ZVAL(user_data->callback);
	}
	ZVAL_ZVAL(user_data->callback, callback, 1, 0);
	fann_set_user_data(ann, user_data);
	RETURN_TRUE;
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

#if PHP_FANN_VERSION >= 0x020200
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
#endif

/* {{{ proto bool fann_cascadetrain_on_data(resource ann, resource data, int max_neurons, int neurons_between_reports, float desired_error)
   Trains on an entire dataset for a period of time using the Cascade2 training algorithm */
PHP_FUNCTION(fann_cascadetrain_on_data)
{
	zval *z_ann, *z_train_data;
	long max_neurons, neurons_between_reports;
	double desired_error;
	struct fann *ann;
	struct fann_train_data *train_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rrlld", &z_ann, &z_train_data,
							  &max_neurons, &neurons_between_reports, &desired_error) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	PHP_FANN_FETCH_TRAIN_DATA();
	php_fann_update_user_data(ann, z_ann, z_train_data);
	fann_cascadetrain_on_data(ann, train_data, max_neurons, neurons_between_reports, desired_error);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fann_cascadetrain_on_file(resource ann, string filename, int max_neurons, int neurons_between_reports, float desired_error)
   Trains on an entire dataset in file for a period of time using the Cascade2 training algorithm */
PHP_FUNCTION(fann_cascadetrain_on_file)
{
	zval *z_ann;
	char *filename;
	int filename_len;
	long max_neurons, neurons_between_reports;
	double desired_error;
	struct fann *ann;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rplld", &z_ann, &filename, &filename_len,
							  &max_neurons, &neurons_between_reports, &desired_error) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	filename = php_fann_get_path_for_open(filename, filename_len, 1 TSRMLS_CC);
	if (!filename) {
		RETURN_FALSE;
	}
	php_fann_update_user_data(ann, z_ann, (zval *) NULL);
	fann_cascadetrain_on_file(ann, filename, max_neurons, neurons_between_reports, desired_error);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fann_get_cascade_output_change_fraction(resource ann)
   Returns the cascade output change fraction */
PHP_FUNCTION(fann_get_cascade_output_change_fraction)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_output_change_fraction, RETURN_DOUBLE);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_output_change_fraction(resource ann, float cascade_output_change_fraction)
   Sets the cascade output change fraction */
PHP_FUNCTION(fann_set_cascade_output_change_fraction)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_output_change_fraction, d, double);
}
/* }}} */

/* {{{ proto bool fann_get_cascade_output_stagnation_epochs(resource ann)
   Returns the number of cascade output stagnation epochs */
PHP_FUNCTION(fann_get_cascade_output_stagnation_epochs)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_output_stagnation_epochs, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_output_stagnation_epochs(resource ann, int cascade_output_stagnation_epochs)
   Sets the number of cascade output stagnation epochs */
PHP_FUNCTION(fann_set_cascade_output_stagnation_epochs)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_output_stagnation_epochs, l, long);
}
/* }}} */

/* {{{ proto bool fann_get_cascade_candidate_change_fraction(resource ann)
   Returns the cascade candidate change fraction */
PHP_FUNCTION(fann_get_cascade_candidate_change_fraction)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_candidate_change_fraction, RETURN_DOUBLE);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_candidate_change_fraction(resource ann, float cascade_candidate_change_fraction)
   Sets the cascade candidate change fraction */
PHP_FUNCTION(fann_set_cascade_candidate_change_fraction)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_candidate_change_fraction, d, double);
}
/* }}} */

/* {{{ proto bool fann_get_cascade_candidate_stagnation_epochs(resource ann)
   Returns the number of cascade candidate stagnation epochs */
PHP_FUNCTION(fann_get_cascade_candidate_stagnation_epochs)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_candidate_stagnation_epochs, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_candidate_stagnation_epochs(resource ann, int cascade_candidate_stagnation_epochs)
   Sets the number of cascade candidate stagnation epochs */
PHP_FUNCTION(fann_set_cascade_candidate_stagnation_epochs)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_candidate_stagnation_epochs, l, long);
}
/* }}} */

/* {{{ proto bool fann_get_cascade_weight_multiplier(resource ann)
   Returns the weight multiplier */
PHP_FUNCTION(fann_get_cascade_weight_multiplier)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_weight_multiplier, RETURN_DOUBLE);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_weight_multiplier(resource ann, double cascade_weight_multiplier)
   Sets the weight multiplier */
PHP_FUNCTION(fann_set_cascade_weight_multiplier)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_weight_multiplier, d, double);
}
/* }}} */

/* {{{ proto bool fann_get_cascade_candidate_limit(resource ann)
   Returns the candidate limit */
PHP_FUNCTION(fann_get_cascade_candidate_limit)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_candidate_limit, RETURN_DOUBLE);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_candidate_limit(resource ann, double cascade_candidate_limit)
   Sets the candidate limit*/
PHP_FUNCTION(fann_set_cascade_candidate_limit)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_candidate_limit, d, double);
}
/* }}} */

/* {{{ proto bool fann_get_cascade_max_out_epochs(resource ann)
   Returns the maximum out epochs */
PHP_FUNCTION(fann_get_cascade_max_out_epochs)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_max_out_epochs, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_max_out_epochs(resource ann, int cascade_max_out_epochs)
   Sets the maximum out epochs */
PHP_FUNCTION(fann_set_cascade_max_out_epochs)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_max_out_epochs, l, long);
}
/* }}} */

#if PHP_FANN_VERSION >= 0x020200
/* {{{ proto bool fann_get_cascade_min_out_epochs(resource ann)
   Returns the minimum out epochs */
PHP_FUNCTION(fann_get_cascade_min_out_epochs)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_min_out_epochs, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_min_out_epochs(resource ann, int cascade_min_out_epochs)
   Sets the minimum out epochs */
PHP_FUNCTION(fann_set_cascade_min_out_epochs)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_min_out_epochs, l, long);
}
/* }}} */
#endif

/* {{{ proto bool fann_get_cascade_max_cand_epochs(resource ann)
   Returns the max candidate epochs */
PHP_FUNCTION(fann_get_cascade_max_cand_epochs)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_max_cand_epochs, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_max_cand_epochs(resource ann, int cascade_max_cand_epochs)
   Sets the max candidate epochs */
PHP_FUNCTION(fann_set_cascade_max_cand_epochs)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_max_cand_epochs, l, long);
}
/* }}} */

#if PHP_FANN_VERSION >= 0x020200
/* {{{ proto bool fann_get_cascade_min_cand_epochs(resource ann)
   Returns the min candidate epochs */
PHP_FUNCTION(fann_get_cascade_min_cand_epochs)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_min_cand_epochs, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_min_cand_epochs(resource ann, int cascade_min_cand_epochs)
   Sets the min candidate epochs */
PHP_FUNCTION(fann_set_cascade_min_cand_epochs)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_min_cand_epochs, l, long);
}
/* }}} */
#endif

/* {{{ proto bool fann_get_cascade_num_candidates(resource ann)
   Returns the number of candidates used during training */
PHP_FUNCTION(fann_get_cascade_num_candidates)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_num_candidates, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_get_cascade_activation_functions_count(resource ann)
   Returns the number of activation functions */
PHP_FUNCTION(fann_get_cascade_activation_functions_count)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_activation_functions_count, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_get_cascade_activation_functions(resource ann)
   Returns the array of cascade candidate activation functions */
PHP_FUNCTION(fann_get_cascade_activation_functions)
{
	zval *z_ann;
	struct fann *ann;
	uint num_functions, i;
	enum fann_activationfunc_enum *functions;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	num_functions = fann_get_cascade_activation_functions_count(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	functions = fann_get_cascade_activation_functions(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	array_init_size(return_value, num_functions);
	for (i = 0; i < num_functions; i++) {
		add_index_long(return_value, i, (long) functions[i]);
	}
}
/* }}} */

/* {{{ proto bool fann_set_cascade_activation_functions(resource ann, array cascade_activation_functions)
   Sets the array of cascade candidate activation functions */
PHP_FUNCTION(fann_set_cascade_activation_functions)
{
	zval *z_ann, *array, **current;
	HashPosition pos;
	struct fann *ann;
	enum fann_activationfunc_enum *functions;
	uint num_functions, i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &array) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	num_functions = zend_hash_num_elements(Z_ARRVAL_P(array));
	functions = (enum fann_activationfunc_enum *) emalloc(num_functions * sizeof(enum fann_activationfunc_enum));
	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &current, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)) {
		convert_to_long_ex(current);
		functions[i++] = (enum fann_activationfunc_enum) Z_LVAL_PP(current);
	}
	fann_set_cascade_activation_functions(ann, functions, i);
	efree(functions);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fann_get_cascade_activation_steepnesses_count(resource ann)
   Returns the number of activation steepnesses */
PHP_FUNCTION(fann_get_cascade_activation_steepnesses_count)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_activation_steepnesses_count, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_get_cascade_activation_steepnesses(resource ann)
   Returns the cascade activation steepnesses array is an array of the different activation functions used by the candidates */
PHP_FUNCTION(fann_get_cascade_activation_steepnesses)
{
	zval *z_ann;
	struct fann *ann;
	uint num_steepnesses, i;
	fann_type *steepnesses;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	num_steepnesses = fann_get_cascade_activation_steepnesses_count(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	steepnesses = fann_get_cascade_activation_steepnesses(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	array_init_size(return_value, num_steepnesses);
	for (i = 0; i < num_steepnesses; i++) {
		add_index_double(return_value, i, (fann_type) steepnesses[i]);
	}
}
/* }}} */

/* {{{ proto bool fann_set_cascade_activation_steepnesses(resource ann, array cascade_activation_steepnesses)
   Sets the cascade activation steepnesses array is an array of the different activation functions used by the candidates */
PHP_FUNCTION(fann_set_cascade_activation_steepnesses)
{
	zval *z_ann, *array, **current;
	HashPosition pos;
	struct fann *ann;
	fann_type *steepnesses;
	uint num_steepnesses, i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &array) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	num_steepnesses = zend_hash_num_elements(Z_ARRVAL_P(array));
	steepnesses = (fann_type *) emalloc(num_steepnesses * sizeof(fann_type));
	for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
		 zend_hash_get_current_data_ex(Z_ARRVAL_P(array), (void **) &current, &pos) == SUCCESS;
		 zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos)) {
		convert_to_double_ex(current);
		steepnesses[i++] = (fann_type) Z_DVAL_PP(current);
	}
	fann_set_cascade_activation_steepnesses(ann, steepnesses, i);
	efree(steepnesses);
	PHP_FANN_ERROR_CHECK_ANN();
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool fann_get_cascade_num_candidate_groups(resource ann)
   Returns the number of candidate groups */
PHP_FUNCTION(fann_get_cascade_num_candidate_groups)
{
	PHP_FANN_GET_PARAM(fann_get_cascade_num_candidate_groups, RETURN_LONG);
}
/* }}} */

/* {{{ proto bool fann_set_cascade_num_candidate_groups(resource ann, int cascade_num_candidate_groups)
   Sets the number of candidate groups */
PHP_FUNCTION(fann_set_cascade_num_candidate_groups)
{
    PHP_FANN_SET_PARAM(fann_set_cascade_num_candidate_groups, l, long);
}
/* }}} */

/* {{{ proto resource fann_create_from_file(string configuration_file)
   Initializes neural network from configuration file */
PHP_FUNCTION(fann_create_from_file)
{
	char *cf_name = NULL;
	int cf_name_len;
	struct fann *ann;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "p", &cf_name, &cf_name_len) == FAILURE) {
		return;
	}
	cf_name = php_fann_get_path_for_open(cf_name, cf_name_len, 1 TSRMLS_CC);
	if (!cf_name) {
		RETURN_FALSE;
	}
	ann = fann_create_from_file(cf_name);
	PHP_FANN_ERROR_CHECK_ANN();
	php_fann_init_ann(ann);
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
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rp", &z_ann, &cf_name, &cf_name_len) == FAILURE) {
		return;
	}
	cf_name = php_fann_get_path_for_open(cf_name, cf_name_len, 0 TSRMLS_CC);
	if (!cf_name) {
		RETURN_FALSE;
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

/* {{{ proto bool fann_set_error_log(resource errdat, string log_file)
   Changes where errors are logged to */
PHP_FUNCTION(fann_set_error_log)
{
	zval *z_errdat;
	char *log_name = NULL;
	FILE *log_file;
	int log_name_len;
	struct fann_error *errdat;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rp", &z_errdat, &log_name, &log_name_len) == FAILURE) {
		return;
	}
	log_name = php_fann_get_path_for_open(log_name, log_name_len, 0 TSRMLS_CC);
	if (!log_name) {
		RETURN_FALSE;
	}
	PHP_FANN_FETCH_ERRDAT();
	if (errdat->error_log)
		fclose(errdat->error_log);
	log_file = fopen(log_name, "w");
	fann_set_error_log(errdat, log_file);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int fann_get_errno(resource errdat)
   Returns the last error number */
PHP_FUNCTION(fann_get_errno)
{
	zval *z_errdat;
	struct fann_error *errdat;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_errdat) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ERRDAT();
	RETURN_LONG(fann_get_errno(errdat));
}
/* }}} */

/* {{{ proto void fann_reset_errno(resource errdat)
   Resets the last error number */
PHP_FUNCTION(fann_reset_errno)
{
	zval *z_errdat;
	struct fann_error *errdat;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_errdat) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ERRDAT();
	fann_reset_errno(errdat);
}
/* }}} */

/* {{{ proto void fann_reset_errstr(resource errdat)
   Resets the last error string */
PHP_FUNCTION(fann_reset_errstr)
{
	zval *z_errdat;
	struct fann_error *errdat;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_errdat) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ERRDAT();

	/* Use this in place of fann_reset_errstr that sets NULL which generates segfault later */
	errdat->errstr[0] = 0;
}
/* }}} */

/* {{{ proto string fann_get_errstr(resource errdat)
   Returns the last error string */
PHP_FUNCTION(fann_get_errstr)
{
	zval *z_errdat;
	struct fann_error *errdat;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_errdat) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ERRDAT();
	if (errdat->errstr) {
		RETURN_STRING(errdat->errstr, 1);
	} else {
		RETURN_EMPTY_STRING();
	}
}
/* }}} */

/* {{{ proto void fann_print_error(resource errdat)
   Prints the last error string */
PHP_FUNCTION(fann_print_error)
{
	zval *z_errdat;
	struct fann_error *errdat;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_errdat) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ERRDAT();
	if (errdat->errstr) {
		php_printf("%s", errdat->errstr);
	} else {
		php_printf("No error");
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
