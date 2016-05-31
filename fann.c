/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 Jakub Zelenka                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Jakub Zelenka <bukka@php.net>                                |
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
#include "phpc/phpc.h"
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
	phpc_val callback;
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

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
zend_function_entry fann_functions[] = {
	PHP_FE(fann_create_standard,                          arginfo_fann_create_standard)
	PHP_FE(fann_create_standard_array,                    arginfo_fann_create_standard_array)
	PHP_FE(fann_create_sparse,                            arginfo_fann_create_sparse)
	PHP_FE(fann_create_sparse_array,                      arginfo_fann_create_sparse_array)
	PHP_FE(fann_create_shortcut,                          arginfo_fann_create_shortcut)
	PHP_FE(fann_create_shortcut_array,                    arginfo_fann_create_shortcut_array)
	PHP_FE(fann_destroy,                                  arginfo_fann_destroy)
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
	PHP_FE(fann_get_cascade_min_out_epochs,               arginfo_fann_get_cascade_min_out_epochs)
	PHP_FE(fann_set_cascade_min_out_epochs,               arginfo_fann_set_cascade_min_out_epochs)
#endif
	PHP_FE(fann_get_cascade_max_cand_epochs,              arginfo_fann_get_cascade_max_cand_epochs)
	PHP_FE(fann_set_cascade_max_cand_epochs,              arginfo_fann_set_cascade_max_cand_epochs)
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
	PHPC_FE_END
};
/* }}} */

/* {{{ fann_module_entry */
zend_module_entry fann_module_entry = {
	STANDARD_MODULE_HEADER,
	"fann",
	fann_functions,
	PHP_MINIT(fann),
	PHP_MSHUTDOWN(fann),
	NULL,
	NULL,
	PHP_MINFO(fann),
	PHP_FANN_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_FANN
ZEND_GET_MODULE(fann)
#endif

/* macro for checking fann_error structs */
#define PHP_FANN_ERROR_CHECK_EX(_fann_struct, _error_msg) \
	if (!(_fann_struct)) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", _error_msg); \
		RETURN_FALSE; \
	} \
	if (fann_get_errno((struct fann_error *) (_fann_struct)) != 0) { \
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s", (_fann_struct)->errstr); \
		RETURN_FALSE; \
	}

/* macro for checking fann_error structs with default message */
#define PHP_FANN_ERROR_CHECK(_fann_struct) PHP_FANN_ERROR_CHECK_EX(_fann_struct, "Internal FANN error")

/* macro for checking ann errors */
#define PHP_FANN_ERROR_CHECK_ANN_EX(_error_msg) PHP_FANN_ERROR_CHECK_EX(ann, _error_msg)
#define PHP_FANN_ERROR_CHECK_ANN() PHP_FANN_ERROR_CHECK_ANN_EX("Neural network not created")

/* macro for checking ann errors */
#define PHP_FANN_ERROR_CHECK_TRAIN_DATA_EX(_error_msg) PHP_FANN_ERROR_CHECK_EX(train_data, _error_msg)
#define PHP_FANN_ERROR_CHECK_TRAIN_DATA() PHP_FANN_ERROR_CHECK_TRAIN_DATA_EX("Train data not created")

/* macro for returning ann resource */
#define PHP_FANN_RETURN_ANN() \
	if (!ann) { RETURN_FALSE; } \
	PHPC_RES_RETVAL(PHPC_RES_REGISTER(ann, le_fannbuf))

/* macro for returning train data resource */
#define PHP_FANN_RETURN_TRAIN_DATA() \
	if (!train_data) { RETURN_FALSE; } \
	PHPC_RES_RETVAL(PHPC_RES_REGISTER(train_data, le_fanntrainbuf))

/* macro for fetching ann resource */
#define PHP_FANN_FETCH_ANN() \
	ann = (struct fann *) PHPC_RES_FETCH(z_ann, le_fannbuf_name, le_fannbuf)

/* macro for fetching train data resource */
#define PHP_FANN_FETCH_TRAIN_DATA_EX(_train_data) \
	_train_data = (struct fann_train_data *) \
		PHPC_RES_FETCH(z_##_train_data, le_fanntrainbuf_name, le_fanntrainbuf)

/* macro for fetching train data resource using train_data variable */
#define PHP_FANN_FETCH_TRAIN_DATA() PHP_FANN_FETCH_TRAIN_DATA_EX(train_data)

/* fetch error data */
#define PHP_FANN_FETCH_ERRDAT() \
	errdat = (struct fann_error *) PHPC_RES_FETCH2(z_errdat, NULL, le_fannbuf, le_fanntrainbuf)

/* macro for getting ann param identified by 0 args */
#define PHP_FANN_GET_PARAM0(_fce, _return)							\
	zval *z_ann; struct fann *ann;										\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) return; \
	PHP_FANN_FETCH_ANN();												\
	_return(_fce(ann))

/* macro for getting ann param identified by 2 args */
#define PHP_FANN_GET_PARAM2(_fce, _return, _zppval, _type1, _type2) \
	zval *z_ann; struct fann *ann;										\
	_type1 param1; _type2 param2;										\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r" #_zppval, &z_ann, &param1, &param2) == FAILURE) \
		return;															\
	PHP_FANN_FETCH_ANN();												\
	_return(_fce(ann, param1, param2))

/* macro for getting ann param (just alias for one param macro) */
#define PHP_FANN_GET_PARAM PHP_FANN_GET_PARAM0

/* macro for setting ann param not identified by arg */
#define PHP_FANN_SET_PARAM0(_fce)										\
	zval *z_ann; struct fann *ann;										\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) return; \
	PHP_FANN_FETCH_ANN();												\
	_fce(ann);															\
	PHP_FANN_ERROR_CHECK_ANN();											\
	RETURN_TRUE

/* macro for setting ann param identified by 1 arg */
#define PHP_FANN_SET_PARAM1(_fce, _zppval, _type)					\
	zval *z_ann; struct fann *ann; _type param;						\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r" #_zppval, &z_ann, &param) == FAILURE) return; \
	PHP_FANN_FETCH_ANN();												\
	_fce(ann, param);													\
	PHP_FANN_ERROR_CHECK_ANN();											\
	RETURN_TRUE


/* macro for setting ann param identified by 2 args */
#define PHP_FANN_SET_PARAM2(_fce, _zppval, _type1, _type2)			\
	zval *z_ann; struct fann *ann;										\
	_type1 param1; _type2 param2;										\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r" #_zppval, &z_ann, &param1, &param2) == FAILURE) \
		return;															\
	PHP_FANN_FETCH_ANN();												\
	_fce(ann, param1, param2);											\
	PHP_FANN_ERROR_CHECK_ANN();											\
	RETURN_TRUE

/* macro for setting ann param identified by 3 args */
#define PHP_FANN_SET_PARAM3(_fce, _zppval, _type1, _type2, _type3)	\
	zval *z_ann; struct fann *ann;										\
	_type1 param1; _type2 param2; _type3 param3;						\
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r" #_zppval, &z_ann, &param1, &param2, &param3) \
		== FAILURE) return;												\
	PHP_FANN_FETCH_ANN();												\
	_fce(ann, param1, param2, param3);									\
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
static void fann_destructor_fannbuf(phpc_res_entry_t *rsrc TSRMLS_DC)
{
	php_fann_user_data *user_data;
	struct fann *ann = (struct fann *) rsrc->ptr;
	user_data = (php_fann_user_data *) fann_get_user_data(ann);
	if (user_data != (php_fann_user_data *) NULL) {
		zval_dtor(PHPC_VAL_CAST_TO_PZVAL(user_data->callback));
		PHPC_VAL_FREE(user_data->callback);
		efree(user_data);
	}
	if (ann->error_log) {
		fclose(ann->error_log);
	}
	fann_destroy(ann);
}
/* }}} */

/* {{{ fann_destructor_fanntrainbuf()
   fann_train resource destructor */
static void fann_destructor_fanntrainbuf(phpc_res_entry_t *rsrc TSRMLS_DC)
{
	struct fann_train_data *train_data = (struct fann_train_data *) rsrc->ptr;
	if (train_data->error_log) {
		fclose(train_data->error_log);
	}
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
	REGISTER_STRING_CONSTANT("FANN_VERSION", PHP_FANN_LIBFANN_VERSION, CONST_PERSISTENT | CONST_CS);

	/* Train constants */
	REGISTER_FANN_CONSTANT(FANN_TRAIN_INCREMENTAL);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_BATCH);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_RPROP);
	REGISTER_FANN_CONSTANT(FANN_TRAIN_QUICKPROP);
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
	php_info_print_table_row(2, "FANN version", PHP_FANN_VERSION);
	php_info_print_table_row(2, "FANN library version", PHP_FANN_LIBFANN_VERSION);
	php_info_print_table_end();
}
/* }}} */

#define PHP_FANN_PATH_FORBIDDEN(_retval) !PHPC_ZVAL_IS_TRUE(_retval)

/* php_fann_get_file_path() {{{ */
static char *php_fann_get_path_for_open(char *path, phpc_str_size_t path_len, int read TSRMLS_DC)
{
	zval retval;
	char *path_for_open;

	if (read) {
		php_stat(path, (php_stat_len) path_len, FS_IS_R, &retval TSRMLS_CC);
	} else {
		php_stat(path, (php_stat_len) path_len, FS_IS_W, &retval TSRMLS_CC);
		if (PHP_FANN_PATH_FORBIDDEN(retval)) {
			char *dirname = estrndup(path, path_len);
			size_t dirname_len = php_dirname(dirname, (size_t) path_len);
			php_stat(dirname, (php_stat_len) dirname_len, FS_IS_W, &retval TSRMLS_CC);
			efree(dirname);
		}
	}
	if (PHP_FANN_PATH_FORBIDDEN(retval))  {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Filename '%s' cannot be opened for %s",
		  path, read ? "reading" : "writing");
		path_for_open = NULL;
	} else {
		PHPC_STREAM_LOCATE_URL_WRAPPER(path, &path_for_open, 0);
	}
	return path_for_open;
}
/* }}} */

/* php_fann_array_to_zval() {{{ */
static void php_fann_array_to_zval(const fann_type *from, zval *to, int len)
{
	int i;

	PHPC_ARRAY_INIT_SIZE(to, len);

	for (i = 0; i < len; i++) {
		PHPC_ARRAY_ADD_INDEX_DOUBLE(to, i, (double) from[i]);
	}
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

/* php_fann_convert_array() {{{ */
static int php_fann_convert_array(HashTable *source, fann_type *dest)
{
	phpc_val *element;
	int pos = 0;

	PHPC_HASH_FOREACH_VAL(source, element) {
		convert_to_double_ex(element);
		dest[pos++] = (fann_type) PHPC_DVAL_P(element);
	} PHPC_HASH_FOREACH_END();

	return pos;
}
/* }}} */

/* php_fann_process_array() {{{ */
static int php_fann_process_array(struct fann *ann, zval *z_array, fann_type **array, int is_input TSRMLS_DC)
{
	int n = PHPC_HASH_NUM_ELEMENTS(Z_ARRVAL_P(z_array));

	if ((is_input && php_fann_check_num_inputs(ann, n TSRMLS_CC) == FAILURE) ||
			(!is_input && php_fann_check_num_outputs(ann, n TSRMLS_CC))) {
		return 0;
	}
	*array = (fann_type *) emalloc(sizeof(fann_type) * n);

	return php_fann_convert_array(Z_ARRVAL_P(z_array), *array);
}
/* }}} */

/* php_fann_create() {{{ */
static int php_fann_create(int num_args, float *connection_rate,
						   unsigned *num_layers, unsigned **layers TSRMLS_DC)
{
	int i, pos;
	PHPC_ZPP_ARGS_DECLARE();

	PHPC_ZPP_ARGS_LOAD_EX(+, num_args, return FAILURE);

	pos = 0;
	if (connection_rate) {
		convert_to_double_ex(PHPC_ZPP_ARGS_GET_PVAL(pos));
		*connection_rate = PHPC_DVAL_P(PHPC_ZPP_ARGS_GET_PVAL(pos++));
	}

	convert_to_long_ex(PHPC_ZPP_ARGS_GET_PVAL(pos));
	*num_layers = PHPC_LVAL_P(PHPC_ZPP_ARGS_GET_PVAL(pos++));

	if (php_fann_check_num_layers(*num_layers, PHPC_ZPP_ARGS_COUNT - pos TSRMLS_CC) == FAILURE) {
		PHPC_ZPP_ARGS_FREE();
		return FAILURE;
	}

	*layers = (unsigned *) emalloc(*num_layers * sizeof(unsigned));
	i = 0;
	PHPC_ZPP_ARGS_LOOP_START_EX(pos) {
		convert_to_long_ex(PHPC_ZPP_ARGS_GET_CURRENT_PVAL());
		if (php_fann_check_num_neurons(
				PHPC_LVAL_P(PHPC_ZPP_ARGS_GET_CURRENT_PVAL()) TSRMLS_CC) == FAILURE) {
			PHPC_ZPP_ARGS_FREE();
			efree(*layers);
			return FAILURE;
		}
		(*layers)[i++] = PHPC_LVAL_P(PHPC_ZPP_ARGS_GET_CURRENT_PVAL());
	} PHPC_ZPP_ARGS_LOOP_END();

	PHPC_ZPP_ARGS_FREE();

	return SUCCESS;
}
/* }}} */

/* php_fann_create_array() {{{ */
static int php_fann_create_array(int num_args, float *conn_rate,
								 unsigned *num_layers, unsigned **layers TSRMLS_DC)
{
	zval *array;
	phpc_val *pdata;
	int i = 0;
	unsigned long tmpnum;
	double tmprate;

	if (conn_rate) {
		if (zend_parse_parameters(num_args TSRMLS_CC, "dla", &tmprate, &tmpnum, &array) == FAILURE) {
			return FAILURE;
		}
		*conn_rate = (float)tmprate;
		*num_layers = (unsigned)tmpnum;
	}
	else {
		if (zend_parse_parameters(num_args TSRMLS_CC, "la", &tmpnum, &array) == FAILURE) {
			return FAILURE;
		}
		*num_layers = (unsigned)tmpnum;
	}

	if (php_fann_check_num_layers(
			*num_layers, PHPC_HASH_NUM_ELEMENTS(Z_ARRVAL_P(array)) TSRMLS_CC) == FAILURE) {
		return FAILURE;
	}

	*layers = (unsigned *) emalloc(*num_layers * sizeof(unsigned));
	PHPC_HASH_FOREACH_VAL(Z_ARRVAL_P(array), pdata) {
		convert_to_long_ex(pdata);
		if (php_fann_check_num_neurons(PHPC_LVAL_P(pdata) TSRMLS_CC) == FAILURE) {
			efree(*layers);
			return FAILURE;
		}
		(*layers)[i++] = PHPC_LVAL_P(pdata);
	} PHPC_HASH_FOREACH_END();

	return SUCCESS;
}
/* }}} */

/* php_fann_init_ann() {{{ */
static int php_fann_callback(struct fann *ann, struct fann_train_data *train,
							 unsigned int max_epochs,
							 unsigned int epochs_between_reports,
							 float desired_error, unsigned int epochs)
{
	PHPC_FCALL_PARAMS_DECLARE(callback, 6);
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache;
	phpc_val retval;
	long rc;
	char *is_callable_error = NULL;
	php_fann_user_data *user_data = fann_get_user_data(ann);
	TSRMLS_FETCH();

	if (user_data == (php_fann_user_data *) NULL) {
		return 0;
	}

	if (PHPC_FCALL_INFO_INIT(PHPC_VAL_CAST_TO_PZVAL(user_data->callback), 0,
				&fci, &fci_cache, NULL, &is_callable_error) == FAILURE) {
		if (is_callable_error) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"User callback is not a valid callback, %s", is_callable_error);
			efree(is_callable_error);
		}
		else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "User callback is not a valid callback");
		}
		return -1;
	}

	PHPC_FCALL_PARAMS_INIT(callback);
	/* ann */
	PHPC_VAL_ASSIGN(PHPC_FCALL_PARAM_VAL(callback, 0), user_data->z_ann);
	/* train data */
	if (user_data->z_train_data) {
		PHPC_VAL_ASSIGN(PHPC_FCALL_PARAM_VAL(callback, 1), user_data->z_train_data);
	} else {
		PHPC_VAL_MAKE(PHPC_FCALL_PARAM_VAL(callback, 1));
		ZVAL_NULL(PHPC_FCALL_PARAM_PZVAL(callback, 1));
	}
	/* max epochs */
	PHPC_VAL_MAKE(PHPC_FCALL_PARAM_VAL(callback, 2));
	ZVAL_LONG(PHPC_FCALL_PARAM_PZVAL(callback, 2), (long) max_epochs);
	/* epochs between reports */
	PHPC_VAL_MAKE(PHPC_FCALL_PARAM_VAL(callback, 3));
	ZVAL_LONG(PHPC_FCALL_PARAM_PZVAL(callback, 3), (long) epochs_between_reports);
	/* desired error */
	PHPC_VAL_MAKE(PHPC_FCALL_PARAM_VAL(callback, 4));
	ZVAL_DOUBLE(PHPC_FCALL_PARAM_PZVAL(callback, 4), (double) desired_error);
	/* epochs */
	PHPC_VAL_MAKE(PHPC_FCALL_PARAM_VAL(callback, 5));
	ZVAL_LONG(PHPC_FCALL_PARAM_PZVAL(callback, 5), (long) epochs);

	/* set fci */
	PHPC_FCALL_RETVAL(fci, retval);
	fci.params = PHPC_FCALL_PARAMS_NAME(callback);
	fci.param_count = 6;
	fci.no_separation = 0;

	if (zend_call_function(&fci, &fci_cache TSRMLS_CC) != SUCCESS ||  PHPC_VAL_ISUNDEF(retval)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "An error occurred while invoking the user callback");
		zval_ptr_dtor(&retval);
		return -1;
	}
	convert_to_boolean(PHPC_VAL_CAST_TO_PZVAL(retval));
	rc = PHPC_VAL_IS_TRUE(retval);
	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&PHPC_FCALL_PARAM_VAL(callback, 0));
	if (!user_data->z_train_data)
		zval_ptr_dtor(&PHPC_FCALL_PARAM_VAL(callback, 1));
	zval_ptr_dtor(&PHPC_FCALL_PARAM_VAL(callback, 2));
	zval_ptr_dtor(&PHPC_FCALL_PARAM_VAL(callback, 3));
	zval_ptr_dtor(&PHPC_FCALL_PARAM_VAL(callback, 4));
	zval_ptr_dtor(&PHPC_FCALL_PARAM_VAL(callback, 5));
	if (!rc) {
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
	unsigned num_layers, *layers;
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
	unsigned num_layers, *layers;
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
	unsigned num_layers, *layers;
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
	unsigned num_layers, *layers;
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
	unsigned num_layers, *layers;
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
	unsigned num_layers, *layers;
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}

	RETURN_BOOL(PHPC_RES_CLOSE(z_ann) == SUCCESS);
}
/* }}} */

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
	PHPC_RES_RETVAL(PHPC_RES_REGISTER(ann_copy, le_fannbuf));
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

	PHPC_ARRAY_INIT_SIZE(return_value, num_outputs);
	for (i = 0; i < num_outputs; i++) {
		PHPC_ARRAY_ADD_NEXT_INDEX_DOUBLE(return_value, (double) output[i]);
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
	unsigned num_layers, *layers, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}

	PHP_FANN_FETCH_ANN();
	num_layers = fann_get_num_layers(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	layers = (unsigned *) emalloc(num_layers * sizeof(unsigned));
	fann_get_layer_array(ann, layers);
	PHP_FANN_ERROR_CHECK_ANN();
	PHPC_ARRAY_INIT_SIZE(return_value, num_layers);
	for (i = 0; i < num_layers; i++) {
		PHPC_ARRAY_ADD_INDEX_LONG(return_value, i, layers[i]);
	}
	efree(layers);
}

/* {{{ proto array fann_get_bias_array(resource ann)
   Returns the number of bias in each layer in the network */
PHP_FUNCTION(fann_get_bias_array)
{
	zval *z_ann;
	struct fann *ann;
	unsigned num_layers, *layers, i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}

	PHP_FANN_FETCH_ANN();
	num_layers = fann_get_num_layers(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	layers = (unsigned *) emalloc(num_layers * sizeof(unsigned));
	fann_get_bias_array(ann, layers);
	PHP_FANN_ERROR_CHECK_ANN();
	PHPC_ARRAY_INIT_SIZE(return_value, num_layers);
	for (i = 0; i < num_layers; i++) {
		PHPC_ARRAY_ADD_INDEX_LONG(return_value, i, layers[i]);
	}
	efree(layers);
}

/* {{{ proto array fann_get_connection_array(resource ann)
   Returns connections in the network */
PHP_FUNCTION(fann_get_connection_array)
{
	zval *z_ann, *z_connection;
	phpc_val pv_connection;
	struct fann *ann;
	struct fann_connection *connections;
	unsigned num_connections, i;
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
	PHPC_ARRAY_INIT_SIZE(return_value, num_connections);
	for (i = 0; i < num_connections; i++) {
		from_neuron = (long) connections[i].from_neuron;
		to_neuron = (long) connections[i].to_neuron;
		weight = (double) connections[i].weight;
		PHPC_VAL_MAKE(pv_connection);
		PHPC_VAL_TO_PZVAL(pv_connection, z_connection);
		object_init_ex(z_connection, php_fann_FANNConnection_class);
		PHP_FANN_CONN_PROP_UPDATE(long, z_connection, "from_neuron", from_neuron);
		PHP_FANN_CONN_PROP_UPDATE(long, z_connection, "to_neuron", to_neuron);
		PHP_FANN_CONN_PROP_UPDATE(double, z_connection, "weight",  weight);
		PHPC_ARRAY_ADD_INDEX_ZVAL(return_value, i, z_connection);
	}
	efree(connections);
}

/* {{{ proto bool fann_set_weight_array(resource ann, array connections)
   Sets connections in the network */
PHP_FUNCTION(fann_set_weight_array)
{
	zval *z_ann, *array, *z_connection;
	phpc_val *pv_connection;
	struct fann *ann;
	struct fann_connection *connections;
	unsigned num_connections, i = 0;
	PHPC_READ_PROPERTY_RV_DECLARE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &array) == FAILURE) {
		return;
	}

	PHP_FANN_FETCH_ANN();
	num_connections = PHPC_HASH_NUM_ELEMENTS(Z_ARRVAL_P(array));
	connections = (struct fann_connection *) emalloc(num_connections * sizeof(struct fann_connection));

	PHPC_HASH_FOREACH_VAL(Z_ARRVAL_P(array), pv_connection) {
		if (PHPC_TYPE_P(pv_connection) == IS_OBJECT && instanceof_function(
				PHPC_OBJCE_P(pv_connection), php_fann_FANNConnection_class TSRMLS_CC)) {
			PHPC_PVAL_TO_PZVAL(pv_connection, z_connection);
			connections[i].from_neuron = Z_LVAL_P(PHP_FANN_CONN_PROP_READ(z_connection, "from_neuron"));
			connections[i].to_neuron = Z_LVAL_P(PHP_FANN_CONN_PROP_READ(z_connection, "to_neuron"));
			connections[i].weight = Z_DVAL_P(PHP_FANN_CONN_PROP_READ(z_connection, "weight"));
			++i;
		}
		else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Weights array can contain only object of FANNConnection");
			efree(connections);
			RETURN_FALSE;
		}
	} PHPC_HASH_FOREACH_END();

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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlld", &z_ann, &from_neuron, &to_neuron, &weight) == FAILURE) {
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

	fann_train(ann, input, desired_output);
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

	PHPC_ARRAY_INIT_SIZE(return_value, num_outputs);
	for (i = 0; i < num_outputs; i++) {
		PHPC_ARRAY_ADD_NEXT_INDEX_DOUBLE(return_value, (double) output[i]);
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
	phpc_str_size_t filename_len;
	long max_epochs, epochs_between_reports;
	double desired_error;
	struct fann *ann;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rslld", &z_ann, &filename, &filename_len,
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
	phpc_str_size_t filename_len;
	struct fann_train_data *train_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &filename, &filename_len) == FAILURE) {
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

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
	PHPC_FCALL_PARAMS_DECLARE(callback, 3);
	phpc_val retval, *pv_input, *pv_output;
	zend_fcall_info fci;
	zend_fcall_info_cache fci_cache = empty_fcall_info_cache;
	phpc_long_t num_data, num_input, num_output;
	int i;
	struct fann_train_data *train_data;


	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllf",
			&num_data, &num_input, &num_output, &fci, &fci_cache) == FAILURE) {
		return;
	}

	train_data = fann_create_train(num_data, num_input, num_output);
	PHP_FANN_ERROR_CHECK_TRAIN_DATA();

	/* initialize callback function */
	PHPC_FCALL_PARAMS_INIT(callback);
	/* number of data */
	PHPC_VAL_MAKE(PHPC_FCALL_PARAM_VAL(callback, 0));
	ZVAL_LONG(PHPC_FCALL_PARAM_PZVAL(callback, 0), num_data);
	/* nummber of inputs */
	PHPC_VAL_MAKE(PHPC_FCALL_PARAM_VAL(callback, 1));
	ZVAL_LONG(PHPC_FCALL_PARAM_PZVAL(callback, 1), num_input);
	/* number of outputs */
	PHPC_VAL_MAKE(PHPC_FCALL_PARAM_VAL(callback, 2));
	ZVAL_LONG(PHPC_FCALL_PARAM_PZVAL(callback, 2), num_output);

	/* set fci */
	PHPC_FCALL_RETVAL(fci, retval);
	fci.params = PHPC_FCALL_PARAMS_NAME(callback);
	fci.param_count = 3;
	fci.no_separation = 0;

	/* call callback for each data */
	for (i = 0; i < num_data; i++) {
		if (zend_call_function(&fci, &fci_cache TSRMLS_CC) != SUCCESS || PHPC_VAL_ISUNDEF(retval)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"An error occurred while invoking the user callback");
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (PHPC_TYPE(retval) != IS_ARRAY) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"The user callback result should be an array");
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (!PHPC_HASH_INDEX_FIND_IN_COND(PHPC_ARRVAL(retval), 0, pv_input) &&
				!PHPC_HASH_CSTR_FIND_IN_COND(PHPC_ARRVAL(retval), "input", pv_input)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"The return value must have 'input' or 0 key for input");
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (!PHPC_HASH_INDEX_FIND_IN_COND(PHPC_ARRVAL(retval), 1, pv_output) &&
				!PHPC_HASH_CSTR_FIND_IN_COND(PHPC_ARRVAL(retval), "output", pv_output)) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"The return value must have 'output' or 1 key for output");
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (PHPC_TYPE_P(pv_input) != IS_ARRAY ||
				PHPC_HASH_NUM_ELEMENTS(PHPC_ARRVAL_P(pv_input)) != num_input) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"Returned input must be an array with %ld elements", num_input);
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		if (PHPC_TYPE_P(pv_output) != IS_ARRAY ||
				PHPC_HASH_NUM_ELEMENTS(PHPC_ARRVAL_P(pv_output)) != num_output) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,
					"Returned output must be an array with %ld elements", num_output);
			zval_ptr_dtor(&retval);
			RETURN_FALSE;
		}
		/* convert input array */
		php_fann_convert_array(PHPC_ARRVAL_P(pv_input), train_data->input[i]);
		/* convert output array */
		php_fann_convert_array(PHPC_ARRVAL_P(pv_output), train_data->output[i]);

		zval_ptr_dtor(&retval);
	}

	zval_ptr_dtor(&PHPC_FCALL_PARAM_VAL(callback, 0));
	zval_ptr_dtor(&PHPC_FCALL_PARAM_VAL(callback, 1));
	zval_ptr_dtor(&PHPC_FCALL_PARAM_VAL(callback, 2));

	PHP_FANN_RETURN_TRAIN_DATA();
}
/* }}} */
#endif

/* {{{ proto bool fann_destroy_train(resource train_data)
   Destructs the training data */
PHP_FUNCTION(fann_destroy_train)
{
	zval *z_train_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_train_data) == FAILURE) {
		return;
	}

	RETURN_BOOL(PHPC_RES_CLOSE(z_train_data) == SUCCESS);
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
	phpc_str_size_t filename_len;
	zval *z_train_data;
	struct fann_train_data *train_data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_train_data, &filename, &filename_len)
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
		PHPC_VAL_MAKE(user_data->callback);
	}
	PHPC_VAL_COPY(user_data->callback, callback);
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

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
	phpc_str_size_t filename_len;
	long max_neurons, neurons_between_reports;
	double desired_error;
	struct fann *ann;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rslld", &z_ann, &filename, &filename_len,
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

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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

#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
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
	unsigned num_functions, i;
	enum fann_activationfunc_enum *functions;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	num_functions = fann_get_cascade_activation_functions_count(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	functions = fann_get_cascade_activation_functions(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	PHPC_ARRAY_INIT_SIZE(return_value, num_functions);
	for (i = 0; i < num_functions; i++) {
		PHPC_ARRAY_ADD_INDEX_LONG(return_value, i, (long) functions[i]);
	}
}
/* }}} */

/* {{{ proto bool fann_set_cascade_activation_functions(resource ann, array cascade_activation_functions)
   Sets the array of cascade candidate activation functions */
PHP_FUNCTION(fann_set_cascade_activation_functions)
{
	zval *z_ann, *array;
	phpc_val *element;
	struct fann *ann;
	enum fann_activationfunc_enum *functions;
	unsigned num_functions, i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &array) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	num_functions = PHPC_HASH_NUM_ELEMENTS(Z_ARRVAL_P(array));
	functions = (enum fann_activationfunc_enum *) emalloc(num_functions * sizeof(enum fann_activationfunc_enum));
	PHPC_HASH_FOREACH_VAL(Z_ARRVAL_P(array), element) {
		convert_to_long_ex(element);
		functions[i++] = (enum fann_activationfunc_enum) PHPC_LVAL_P(element);
	} PHPC_HASH_FOREACH_END();
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
	unsigned num_steepnesses, i;
	fann_type *steepnesses;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	num_steepnesses = fann_get_cascade_activation_steepnesses_count(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	steepnesses = fann_get_cascade_activation_steepnesses(ann);
	PHP_FANN_ERROR_CHECK_ANN();
	PHPC_ARRAY_INIT_SIZE(return_value, num_steepnesses);
	for (i = 0; i < num_steepnesses; i++) {
		PHPC_ARRAY_ADD_INDEX_DOUBLE(return_value, i, (double) steepnesses[i]);
	}
}
/* }}} */

/* {{{ proto bool fann_set_cascade_activation_steepnesses(resource ann, array cascade_activation_steepnesses)
   Sets the cascade activation steepnesses array is an array of the different activation functions used by the candidates */
PHP_FUNCTION(fann_set_cascade_activation_steepnesses)
{
	zval *z_ann, *array;
	phpc_val *element;
	struct fann *ann;
	fann_type *steepnesses;
	unsigned num_steepnesses, i = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &array) == FAILURE) {
		return;
	}
	PHP_FANN_FETCH_ANN();
	num_steepnesses = zend_hash_num_elements(Z_ARRVAL_P(array));
	steepnesses = (fann_type *) emalloc(num_steepnesses * sizeof(fann_type));
	PHPC_HASH_FOREACH_VAL(Z_ARRVAL_P(array), element) {
		convert_to_double_ex(element);
		steepnesses[i++] = (fann_type) PHPC_DVAL_P(element);
	} PHPC_HASH_FOREACH_END();
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
	phpc_str_size_t cf_name_len;
	struct fann *ann;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &cf_name, &cf_name_len) == FAILURE) {
		return;
	}
	cf_name = php_fann_get_path_for_open(cf_name, cf_name_len, 1 TSRMLS_CC);
	if (!cf_name) {
		RETURN_FALSE;
	}
	ann = fann_create_from_file(cf_name);
	if (!ann) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid configuration file '%s'", cf_name);
		RETURN_FALSE;
	}
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
	phpc_str_size_t cf_name_len;
	struct fann *ann;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_ann, &cf_name, &cf_name_len) == FAILURE) {
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
	phpc_str_size_t log_name_len;
	struct fann_error *errdat;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &z_errdat, &log_name, &log_name_len) == FAILURE) {
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
		PHPC_CSTR_RETURN(errdat->errstr);
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
