#include "cii_router.h"

zend_class_entry *cii_router_ce;

zend_function_entry cii_router_methods[] = {
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_router)
{
	/**
	 * Router Class
	 *
	 * Parses URIs and determines routing
	 */
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Router", cii_router_methods);
	cii_router_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * Current class name
	 *
	 * @var	string
	 */
	zend_declare_property_stringl(cii_router_ce, ZEND_STRL("class"), "", 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * Current method name
	 *
	 * @var	string
	 */
	zend_declare_property_stringl(cii_router_ce, ZEND_STRL("method"), "index", 5, ZEND_ACC_PUBLIC TSRMLS_CC);
}