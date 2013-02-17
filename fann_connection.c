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
#include "php_fann.h"

PHP_FANN_API zend_class_entry *php_fann_FANNConnection_class;

/* macros for dealing with FANNConnection property */
#define PHP_FANN_PROP(__name, __value) #__name, sizeof(#__name) - 1, __value
#define PHP_FANN_PROP_UPDATE(__name)									\
	php_fann_FANNConnection_class, getThis(), PHP_FANN_PROP(__name, __name) TSRMLS_CC
#define PHP_FANN_PROP_DECLARE(__name)									\
	php_fann_FANNConnection_class, PHP_FANN_PROP(__name, 0), ZEND_ACC_PUBLIC TSRMLS_CC

/* {{{ proto FANNConnection::__construct(int from_neuron, int to_neuron, double weight)
   Constructs a new FANNConnection instance */
PHP_METHOD(FANNConnection, __construct)
{
	long from_neuron, to_neuron;
	double weight;
	
	return_value = getThis();

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lld", &from_neuron, &to_neuron, &weight) == FAILURE) {
		return;
	}
	zend_update_property_long(PHP_FANN_PROP_UPDATE(from_neuron));
	zend_update_property_long(PHP_FANN_PROP_UPDATE(to_neuron));
	zend_update_property_double(PHP_FANN_PROP_UPDATE(weight));
}
/* }}} */

/* {{{ php_fannconnection_get_property */
static void php_fannconnection_get_property(char *name, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *res;
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	res = zend_read_property(php_fann_FANNConnection_class, getThis(), name, strlen(name), 1 TSRMLS_CC);
	*return_value = *res;
	zval_copy_ctor(return_value);
	INIT_PZVAL(return_value);
}
/* }}} */

/* {{{ proto int FANNConnection::getFromNeuron()
   Returns unique number used to identify source neuron */
ZEND_METHOD(FANNConnection, getFromNeuron)
{
	php_fannconnection_get_property("from_neuron", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto int FANNConnection::getToNeuron()
   Returns unique number used to identify destination neuron */
ZEND_METHOD(FANNConnection, getToNeuron)
{
	php_fannconnection_get_property("to_neuron", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto double FANNConnection::getWeight()
   Returns a numerical value of the weight */
ZEND_METHOD(FANNConnection, getWeight)
{
	php_fannconnection_get_property("weight", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}
/* }}} */

/* {{{ proto void FANNConnection::setWeight()
   Sets a numerical value of the weight */
ZEND_METHOD(FANNConnection, setWeight)
{
	double weight;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "d", &weight) == FAILURE) {
		return;
	}
	zend_update_property_double(PHP_FANN_PROP_UPDATE(weight));
}
/* }}} */


/* {{{ arginfo for FANNConnection class */
ZEND_BEGIN_ARG_INFO(arginfo_fannconnection___construct, 0)
	ZEND_ARG_INFO(0, from_neuron)
	ZEND_ARG_INFO(0, to_neuron)
	ZEND_ARG_INFO(0, weight)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_fannconnection_set_weight, 0)
	ZEND_ARG_INFO(0, weight)
ZEND_END_ARG_INFO()
/* }}} */

static const zend_function_entry fannconnection_funcs[] = {
	PHP_ME(FANNConnection,  __construct,      arginfo_fannconnection___construct,  ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME(FANNConnection,  getFromNeuron,    NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(FANNConnection,  getToNeuron,      NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(FANNConnection,  getWeight,        NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(FANNConnection,  setWeight,        arginfo_fannconnection_set_weight,   ZEND_ACC_PUBLIC)
	PHP_FE_END
};


/* {{{ php_fannconnection_register_class */
void php_fannconnection_register_class(TSRMLS_D) 
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "FANNConnection", fannconnection_funcs);
	php_fann_FANNConnection_class = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_long(PHP_FANN_PROP_DECLARE(from_neuron));
	zend_declare_property_long(PHP_FANN_PROP_DECLARE(to_neuron));
	zend_declare_property_double(PHP_FANN_PROP_DECLARE(weight));
}
/* }}} */
