#include "cii_input.h"

zend_class_entry *cii_input_ce;

PHP_METHOD(cii_input, __construct)
{
	/*
	*	output log
	*/
	cii_write_log(3, "Input Class Initialized");
}

zend_function_entry cii_input_methods[] = {
	PHP_ME(cii_input, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_input)
{
	/**
	 * Input Class
	 *
	 * Pre-processes global input data for security
	 */
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Input", cii_input_methods);
	cii_input_ce = zend_register_internal_class(&ce TSRMLS_CC);
}