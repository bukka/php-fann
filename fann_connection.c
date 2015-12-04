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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_fann.h"

PHP_FANN_API zend_class_entry *php_fann_FANNConnection_class;

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

	PHP_FANN_CONN_PROP_UPDATE(long, return_value, "from_neuron", from_neuron);
	PHP_FANN_CONN_PROP_UPDATE(long, return_value, "to_neuron", to_neuron);
	PHP_FANN_CONN_PROP_UPDATE(double, return_value, "weight",  weight);
}
/* }}} */

/* {{{ php_fannconnection_get_property */
static void php_fannconnection_get_property(char *name, INTERNAL_FUNCTION_PARAMETERS)
{
	zval *res;
	PHPC_READ_PROPERTY_RV_DECLARE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "") == FAILURE) {
		return;
	}

	res = PHPC_READ_PROPERTY(php_fann_FANNConnection_class, getThis(), name, strlen(name), 1);
	RETVAL_ZVAL(res, 1, 0);
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
    PHP_FANN_CONN_PROP_UPDATE(double, getThis(), "weight", weight);
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

static zend_function_entry fannconnection_funcs[] = {
	PHP_ME(FANNConnection,  __construct,      arginfo_fannconnection___construct,  ZEND_ACC_CTOR | ZEND_ACC_PUBLIC)
	PHP_ME(FANNConnection,  getFromNeuron,    NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(FANNConnection,  getToNeuron,      NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(FANNConnection,  getWeight,        NULL,                                ZEND_ACC_PUBLIC)
	PHP_ME(FANNConnection,  setWeight,        arginfo_fannconnection_set_weight,   ZEND_ACC_PUBLIC)
	PHPC_FE_END
};


/* {{{ php_fannconnection_register_class */
void php_fannconnection_register_class(TSRMLS_D) 
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "FANNConnection", fannconnection_funcs);
	php_fann_FANNConnection_class = PHPC_CLASS_REGISTER(ce);
    PHP_FANN_CONN_PROP_DECLARE(long, "from_neuron");
    PHP_FANN_CONN_PROP_DECLARE(long, "to_neuron");
    PHP_FANN_CONN_PROP_DECLARE(double, "weight");
}
/* }}} */
