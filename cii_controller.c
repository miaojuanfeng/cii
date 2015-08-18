#include "cii_controller.h"

zend_class_entry *cii_controller_ce;
extern zend_class_entry *cii_loader_ce;

ZEND_BEGIN_ARG_INFO_EX(cii_controller_get_instance_arginfo, 0, 1, 0)
ZEND_END_ARG_INFO ()

#define GET_CII_CONTROLLER_INSTANCE() \
	zend_read_static_property(cii_controller_ce, ZEND_STRL("instance"), 1 TSRMLS_CC)

#define GET_CII_CONTROLLER_INSTANCE_BY_REF() \
	zval_ptr_dtor(return_value_ptr); \
	(*return_value_ptr) = GET_CII_CONTROLLER_INSTANCE(); \
	Z_ADDREF_P(*return_value_ptr)
/**
* Class constructor
*
* @return	void
*
* public function __construct()
*/
PHP_METHOD(cii_controller, __construct){
	//init instance
	zend_update_static_property(cii_controller_ce, ZEND_STRL("instance"), getThis() TSRMLS_CC);
	//load object
	zval *load;
	MAKE_STD_ZVAL(load);
	object_init_ex(load, cii_loader_ce);
	zend_update_property(cii_controller_ce, getThis(), ZEND_STRL("load"), load TSRMLS_CC);
	zval_ptr_dtor(&load);
	//call load construct
	zval *retval;
	CII_CALL_USER_FUNCTION_EX(NULL, &load, "__construct", &retval, 0, NULL);
	zval_ptr_dtor(&retval);
	//output log
	php_printf("Info: Controller Class Initialized\n");
}
/**
* Get the CI singleton
*
* @static
* @return	object
*
* public static function &get_instance()
*/
PHP_METHOD(cii_controller, get_instance){
	GET_CII_CONTROLLER_INSTANCE_BY_REF();
}

const zend_function_entry cii_controller_methods[] = {
	PHP_ME(cii_controller, __construct,NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_controller, get_instance, cii_controller_get_instance_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_FE_END
};

ZEND_MINIT_FUNCTION(cii_controller){
	/**
	 * Application Controller Class
	 *
	 * This class object is the super class that every library in
	 * CodeIgniter will be assigned to.
	 */
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "cii_controller", cii_controller_methods);
	cii_controller_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * Reference to the CI singleton
	 *
	 * @var	object
	 */
	zend_declare_property_null(cii_controller_ce, ZEND_STRL("instance"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC TSRMLS_CC);
}
