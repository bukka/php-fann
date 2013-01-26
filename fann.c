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

/* If you declare any globals in php_fann.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(fann)
*/

/* True global resources - no need for thread safety here */
static int le_fannbuf;
#define le_fannbuf_name "FANN"

/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_fann_create_standard, 0)
ZEND_ARG_INFO(0, num_layers)
ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_create_standard_array, 0)
ZEND_ARG_INFO(0, num_layers)
ZEND_ARG_INFO(0, layers)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_create_from_file, 0)
ZEND_ARG_INFO(0, configuration_file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_run, 0)
ZEND_ARG_INFO(0, ann)
ZEND_ARG_INFO(0, info)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fann_destroy, 0)
ZEND_ARG_INFO(0, ann)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ fann_functions[] */
const zend_function_entry fann_functions[] = {
	PHP_FE(fann_create_standard,          arginfo_fann_create_standard)
	PHP_FE(fann_create_standard_array,    arginfo_fann_create_standard_array)
	PHP_FE(fann_create_from_file,         arginfo_fann_create_from_file)
	PHP_FE(fann_run,                      arginfo_fann_run)
	PHP_FE(fann_destroy,                  arginfo_fann_destroy)
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

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("fann.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_fann_globals, fann_globals)
    STD_PHP_INI_ENTRY("fann.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_fann_globals, fann_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_fann_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_fann_init_globals(zend_fann_globals *fann_globals)
{
	fann_globals->global_value = 0;
	fann_globals->global_string = NULL;
}
*/
/* }}} */

static void fann_destructor_fannbuf(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct fann *ann = (struct fann *) rsrc->ptr;
	fann_destroy(ann);
}

#define PHP_FANN_ERROR_CHECK(ann) \
if (fann_get_errno((struct fann_error *) ann) != 0) { \
	php_error_docref(NULL TSRMLS_CC, E_WARNING, fann_get_errstr((struct fann_error *) ann)); \
	RETURN_FALSE; \
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(fann)
{
	le_fannbuf = zend_register_list_destructors_ex(fann_destructor_fannbuf, NULL, le_fannbuf_name, module_number);
	
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(fann)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
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

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* php_fann_create() {{{ */
static int php_fann_create(int num_args, float *connection_rate,
						   unsigned int *num_layers, unsigned int **layers TSRMLS_DC)
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
	if (argc - pos != *num_layers) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid number of arguments");
		efree(args);
		return FAILURE;
	}

	*layers = (unsigned int *) emalloc(*num_layers * sizeof(unsigned int));
	for (i = pos; i < argc; i++) {
		convert_to_long_ex(args[i]);
		if (Z_LVAL_PP(args[i]) < 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of neurons cannot be negative");
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
								 unsigned int *num_layers, unsigned int **layers TSRMLS_DC)
{
	zval *array, **ppdata;
	int i = 0;

	if (conn_rate) {
		if (zend_parse_parameters(num_args TSRMLS_CC, "fla", conn_rate, num_layers, &array) == FAILURE) {
			return FAILURE;
		}
	}
	else {
		if (zend_parse_parameters(num_args TSRMLS_CC, "la", num_layers, &array) == FAILURE) {
			return FAILURE;
		}
	}

	if (zend_hash_num_elements(Z_ARRVAL_P(array)) != *num_layers) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid number of arguments");
		return FAILURE;
	}

	*layers = (unsigned int *) emalloc(*num_layers * sizeof(unsigned int));
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(array));
		 zend_hash_get_current_data(Z_ARRVAL_P(array), (void **) &ppdata) == SUCCESS;
		 zend_hash_move_forward(Z_ARRVAL_P(array))) {
		convert_to_long_ex(ppdata);
		if (Z_LVAL_PP(ppdata) <= 0) {
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Number of neurons must be greater than zero");
			efree(*layers);
			return FAILURE;
		}
		(*layers)[i++] = Z_LVAL_PP(ppdata);
	}

	return SUCCESS;
}
/* }}} */

/* macro for returning ann resource */
#define PHP_FANN_RETURN_ANN() \
	if (!ann) { RETURN_FALSE; } \
	ZEND_REGISTER_RESOURCE(return_value, ann, le_fannbuf)

/* {{{ proto resource fann_create_standard(int num_layers [, int ... ])
   Initializes neural network from configuration file */
PHP_FUNCTION(fann_create_standard)
{
	unsigned int num_layers, *layers; 
	struct fann *ann;
	
	if (php_fann_create(ZEND_NUM_ARGS(), NULL, &num_layers, &layers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ann = fann_create_standard_array(num_layers, layers);
	efree(layers);
	PHP_FANN_RETURN_ANN();
}
/* }}} */

/* {{{ proto resource fann_create_standard_array(int num_layers, array layers)
   Initializes neural network from configuration file using array as 2nd argument */
PHP_FUNCTION(fann_create_standard_array)
{
	unsigned int num_layers, *layers; 
	struct fann *ann;
	
	if (php_fann_create_array(ZEND_NUM_ARGS(), NULL, &num_layers, &layers TSRMLS_CC) == FAILURE) {
		RETURN_FALSE;
	}
	
	ann = fann_create_standard_array(num_layers, layers);
	efree(layers);
	PHP_FANN_RETURN_ANN();
}
/* }}} */


/* {{{ proto resource fann_create_from_file(string configuration_file)
   Initializes neural network from configuration file */
PHP_FUNCTION(fann_create_from_file)
{
	char *cf_name = NULL;
	int cf_name_len;
	FILE *cf_file;
	struct fann *ann;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &cf_name, &cf_name_len) == FAILURE) {
		return;
	}

	cf_file = fopen(cf_name, "r");
	if (!cf_file) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "File cannot be opened for reading");
		RETURN_FALSE;
	}
	fclose(cf_file);

	ann = fann_create_from_file(cf_name);
	PHP_FANN_RETURN_ANN();
}
/* }}} */

/* {{{ funn_input_foreach
   callback for converting input hash map to fann_type array */
int funn_input_foreach(zval **element TSRMLS_DC, int num_args, va_list args, zend_hash_key *hash_key)
{
	fann_type *input = va_arg(args, fann_type *);
	int *pos = va_arg(args, int *);
	
	convert_to_double(*element);
	input[*pos++] = (fann_type) Z_DVAL_PP(element);
	
	return ZEND_HASH_APPLY_KEEP;
}


/* {{{ proto resource fann_run(string configuration_file)
   Runs input through the neural network */
PHP_FUNCTION(fann_run)
{
	zval *z_ann, *array;
	struct fann *ann;
	fann_type *input, *calc_out;
	int i = 0, num_out = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ra", &z_ann, &array) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ann, struct fann *, &z_ann, -1, le_fannbuf_name, le_fannbuf);
	
	input = (float *) emalloc(sizeof(float) * zend_hash_num_elements(Z_ARRVAL_P(array)));
	zend_hash_apply_with_arguments(Z_ARRVAL_P(array) TSRMLS_CC, (apply_func_args_t) funn_input_foreach, 2, input, &i);
	
	calc_out = fann_run(ann, input);
	num_out = fann_get_num_output(ann);

	array_init(return_value);
	for (i = 0; i < num_out; i++) {
		add_next_index_double(return_value, (double) calc_out[i]);
	}

	efree(input);

	PHP_FANN_ERROR_CHECK(ann);
}
/* }}} */


/* {{{ proto resource fann_create_from_file(string configuration_file)
   Destroys neural network */
PHP_FUNCTION(fann_destroy)
{
	zval *z_ann;
	struct fann *ann;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &z_ann) == FAILURE) {
		return;
	}
	
	ZEND_FETCH_RESOURCE(ann, struct fann *, &z_ann, -1, le_fannbuf_name, le_fannbuf);

	RETURN_BOOL(zend_list_delete(Z_LVAL_P(z_ann)) == SUCCESS);
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
