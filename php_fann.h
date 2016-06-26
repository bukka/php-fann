/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 Jakub Zelenka                                |
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

#include "phpc/phpc.h"

/* fann version */
#define PHP_FANN_VERSION "1.1.1"
#ifdef HAVE_FANN_2_2
#define PHP_FANN_LIBFANN_VERSION_ID 0x020200
#define PHP_FANN_LIBFANN_VERSION "2.2"
#else
#define PHP_FANN_LIBFANN_VERSION_ID 0x020100
#define PHP_FANN_LIBFANN_VERSION "2.1"
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
PHP_FUNCTION(fann_destroy);
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
PHP_FUNCTION(fann_copy);
#endif
PHP_FUNCTION(fann_run);
PHP_FUNCTION(fann_randomize_weights);
PHP_FUNCTION(fann_init_weights);
/* Parameters */
PHP_FUNCTION(fann_get_num_input);
PHP_FUNCTION(fann_get_num_output);
PHP_FUNCTION(fann_get_total_neurons);
PHP_FUNCTION(fann_get_total_connections);
PHP_FUNCTION(fann_get_network_type);
PHP_FUNCTION(fann_get_connection_rate);
PHP_FUNCTION(fann_get_num_layers);
PHP_FUNCTION(fann_get_layer_array);
PHP_FUNCTION(fann_get_bias_array);
PHP_FUNCTION(fann_get_connection_array);
PHP_FUNCTION(fann_set_weight_array);
PHP_FUNCTION(fann_set_weight);

/* FANN Training */
/* Training */
PHP_FUNCTION(fann_train);
PHP_FUNCTION(fann_test);
PHP_FUNCTION(fann_get_MSE);
PHP_FUNCTION(fann_get_bit_fail);
PHP_FUNCTION(fann_reset_MSE);
/* Training Data Training */
PHP_FUNCTION(fann_train_on_data);
PHP_FUNCTION(fann_train_on_file);
PHP_FUNCTION(fann_train_epoch);
PHP_FUNCTION(fann_test_data);
/* Training Data Manipulation */
PHP_FUNCTION(fann_read_train_from_file);
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
PHP_FUNCTION(fann_create_train);
PHP_FUNCTION(fann_create_train_from_callback);
#endif
PHP_FUNCTION(fann_destroy_train);
PHP_FUNCTION(fann_shuffle_train_data);
PHP_FUNCTION(fann_scale_train);
PHP_FUNCTION(fann_descale_train);
PHP_FUNCTION(fann_set_input_scaling_params);
PHP_FUNCTION(fann_set_output_scaling_params);
PHP_FUNCTION(fann_set_scaling_params);
PHP_FUNCTION(fann_clear_scaling_params);
PHP_FUNCTION(fann_scale_input);
PHP_FUNCTION(fann_scale_output);
PHP_FUNCTION(fann_descale_input);
PHP_FUNCTION(fann_descale_output);
PHP_FUNCTION(fann_scale_input_train_data);
PHP_FUNCTION(fann_scale_output_train_data);
PHP_FUNCTION(fann_scale_train_data);
PHP_FUNCTION(fann_merge_train_data);
PHP_FUNCTION(fann_duplicate_train_data);
PHP_FUNCTION(fann_subset_train_data);
PHP_FUNCTION(fann_length_train_data);
PHP_FUNCTION(fann_num_input_train_data);
PHP_FUNCTION(fann_num_output_train_data);
PHP_FUNCTION(fann_save_train);
/* Parameters */
PHP_FUNCTION(fann_get_training_algorithm);
PHP_FUNCTION(fann_set_training_algorithm);
PHP_FUNCTION(fann_get_learning_rate);
PHP_FUNCTION(fann_set_learning_rate);
PHP_FUNCTION(fann_get_learning_momentum);
PHP_FUNCTION(fann_set_learning_momentum);
PHP_FUNCTION(fann_get_activation_function);
PHP_FUNCTION(fann_set_activation_function);
PHP_FUNCTION(fann_set_activation_function_layer);
PHP_FUNCTION(fann_set_activation_function_hidden);
PHP_FUNCTION(fann_set_activation_function_output);
PHP_FUNCTION(fann_get_activation_steepness);
PHP_FUNCTION(fann_set_activation_steepness);
PHP_FUNCTION(fann_set_activation_steepness_layer);
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
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
PHP_FUNCTION(fann_get_sarprop_weight_decay_shift);
PHP_FUNCTION(fann_set_sarprop_weight_decay_shift);
PHP_FUNCTION(fann_get_sarprop_step_error_threshold_factor);
PHP_FUNCTION(fann_set_sarprop_step_error_threshold_factor);
PHP_FUNCTION(fann_get_sarprop_step_error_shift);
PHP_FUNCTION(fann_set_sarprop_step_error_shift);
PHP_FUNCTION(fann_get_sarprop_temperature);
PHP_FUNCTION(fann_set_sarprop_temperature);
#endif

/* FANN Cascade Training */
/* Cascade Training */
PHP_FUNCTION(fann_cascadetrain_on_data);
PHP_FUNCTION(fann_cascadetrain_on_file);
/* Parameters */
PHP_FUNCTION(fann_get_cascade_output_change_fraction);
PHP_FUNCTION(fann_set_cascade_output_change_fraction);
PHP_FUNCTION(fann_get_cascade_output_stagnation_epochs);
PHP_FUNCTION(fann_set_cascade_output_stagnation_epochs);
PHP_FUNCTION(fann_get_cascade_candidate_change_fraction);
PHP_FUNCTION(fann_set_cascade_candidate_change_fraction);
PHP_FUNCTION(fann_get_cascade_candidate_stagnation_epochs);
PHP_FUNCTION(fann_set_cascade_candidate_stagnation_epochs);
PHP_FUNCTION(fann_get_cascade_weight_multiplier);
PHP_FUNCTION(fann_set_cascade_weight_multiplier);
PHP_FUNCTION(fann_get_cascade_candidate_limit);
PHP_FUNCTION(fann_set_cascade_candidate_limit);
PHP_FUNCTION(fann_get_cascade_max_out_epochs);
PHP_FUNCTION(fann_set_cascade_max_out_epochs);
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
PHP_FUNCTION(fann_get_cascade_min_out_epochs);
PHP_FUNCTION(fann_set_cascade_min_out_epochs);
#endif
PHP_FUNCTION(fann_get_cascade_max_cand_epochs);
PHP_FUNCTION(fann_set_cascade_max_cand_epochs);
#if PHP_FANN_LIBFANN_VERSION_ID >= 0x020200
PHP_FUNCTION(fann_get_cascade_min_cand_epochs);
PHP_FUNCTION(fann_set_cascade_min_cand_epochs);
#endif
PHP_FUNCTION(fann_get_cascade_num_candidates);
PHP_FUNCTION(fann_get_cascade_activation_functions_count);
PHP_FUNCTION(fann_get_cascade_activation_functions);
PHP_FUNCTION(fann_set_cascade_activation_functions);
PHP_FUNCTION(fann_get_cascade_activation_steepnesses_count);
PHP_FUNCTION(fann_get_cascade_activation_steepnesses);
PHP_FUNCTION(fann_set_cascade_activation_steepnesses);
PHP_FUNCTION(fann_get_cascade_num_candidate_groups);
PHP_FUNCTION(fann_set_cascade_num_candidate_groups);

/* FANN File Input/Output */
/* File Input and Output  */
PHP_FUNCTION(fann_create_from_file);
PHP_FUNCTION(fann_save);

/* FANN Error Handling */
/* Error Handling */
PHP_FUNCTION(fann_set_error_log);
PHP_FUNCTION(fann_get_errno);
PHP_FUNCTION(fann_reset_errno);
PHP_FUNCTION(fann_reset_errstr);
PHP_FUNCTION(fann_get_errstr);
PHP_FUNCTION(fann_print_error);

/* FANNConnection class */
void php_fannconnection_register_class(TSRMLS_D);
PHP_FANN_API extern zend_class_entry *php_fann_FANNConnection_class;

/* macros for dealing with FANNConnection properties */
#define PHP_FANN_CONN_PROP_NAME(__name)  __name, sizeof(__name)-1
#define PHP_FANN_CONN_PROP_UPDATE(__type, __obj, __name, __value)		\
	zend_update_property_##__type(php_fann_FANNConnection_class, (__obj), \
								  PHP_FANN_CONN_PROP_NAME(__name), (__value) TSRMLS_CC)
#define PHP_FANN_CONN_PROP_DECLARE(__type, __name)					\
	zend_declare_property_##__type(php_fann_FANNConnection_class, PHP_FANN_CONN_PROP_NAME(__name), \
								   0, ZEND_ACC_PUBLIC TSRMLS_CC)
#define PHP_FANN_CONN_PROP_READ(__obj, __name) \
	PHPC_READ_PROPERTY(php_fann_FANNConnection_class, (__obj),	\
					   __name, sizeof(__name)-1, 0)

#endif	/* PHP_FANN_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
