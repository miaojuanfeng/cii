#include "cii_hooks.h"

zend_class_entry *cii_hooks_ce;

/**
* Class constructor
*
* @return	void
*
* public function __construct()
*/
PHP_METHOD(cii_hooks, __construct)
{
	/*
	*	init cii_hooks::hooks
	*/
	zval *hooks;
	MAKE_STD_ZVAL(hooks);
	array_init(hooks);
	zend_update_property(cii_hooks_ce, getThis(), ZEND_STRL("hooks"), hooks TSRMLS_CC);
	zval_ptr_dtor(&hooks);
	/*
	*	init cii_hooks::_objects
	*/
	zval *_objects;
	MAKE_STD_ZVAL(_objects);
	array_init(_objects);
	zend_update_property(cii_hooks_ce, getThis(), ZEND_STRL("_objects"), _objects TSRMLS_CC);
	zval_ptr_dtor(&_objects);
	/*
	*	output log
	*/
	php_printf("Info: Hooks Class Initialized\n");
}

zend_function_entry cii_hooks_methods[] = {
	PHP_ME(cii_hooks, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_hooks)
{
	/**
	* Hooks Class
	*
	* Provides a mechanism to extend the base system without hacking.
	*/
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Hooks", cii_hooks_methods);
	cii_hooks_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * Determines whether hooks are enabled
	 *
	 * @var	bool
	 */
	zend_declare_property_bool(cii_hooks_ce, ZEND_STRL("enabled"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * List of all hooks set in config/hooks.php
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_hooks_ce, ZEND_STRL("hooks"), ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * Array with class objects to use hooks methods
	 *
	 * @var array
	 */
	zend_declare_property_null(cii_hooks_ce, ZEND_STRL("_objects"), ZEND_ACC_PROTECTED TSRMLS_CC);
	/**
	 * In progress flag
	 *
	 * Determines whether hook is in progress, used to prevent infinte loops
	 *
	 * @var	bool
	 */
	zend_declare_property_bool(cii_hooks_ce, ZEND_STRL("_in_progress"), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
}