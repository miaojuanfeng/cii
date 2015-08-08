#include "cii_controller.h"

zend_class_entry *cii_controller_ce;

ZEND_BEGIN_ARG_INFO_EX(cii_controller_get_instance_arginfo, 0, 1, 0)
ZEND_END_ARG_INFO ()

ZEND_API zval* get_cii_controller_instance(TSRMLS_D){
	return zend_read_static_property(cii_controller_ce,ZEND_STRL("instance"),1 TSRMLS_CC);
}

PHP_METHOD(cii_controller,__construct){
	//init instance
	zend_update_static_property(cii_controller_ce,ZEND_STRL("instance"),getThis() TSRMLS_CC);
	//load object
	zval *load;
	MAKE_STD_ZVAL(load);
	extern zend_class_entry *cii_loader_ce;
	object_init_ex(load,cii_loader_ce);
	zend_update_property(cii_controller_ce,getThis(),ZEND_STRL("load"),load TSRMLS_CC);
	zval *load_method_construct;
	MAKE_STD_ZVAL(load_method_construct);
	ZVAL_STRING(load_method_construct,"__construct",1);
	call_user_function(NULL, &load, load_method_construct, *return_value_ptr, 0, NULL TSRMLS_CC);
	//
	php_printf("Info: Controller Class Initialized\n");
}

PHP_METHOD(cii_controller,get_instance){
	zval_ptr_dtor(return_value_ptr);
	(*return_value_ptr) = get_cii_controller_instance(TSRMLS_C);
	Z_SET_ISREF_P(*return_value_ptr);
	Z_ADDREF_P(*return_value_ptr);
}

const zend_function_entry cii_controller_methods[] = {
	PHP_ME(cii_controller,__construct,NULL,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(cii_controller,get_instance,cii_controller_get_instance_arginfo,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

ZEND_MINIT_FUNCTION(cii_controller){
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"cii_controller",cii_controller_methods);
	cii_controller_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(cii_controller_ce,ZEND_STRL("instance"),ZEND_ACC_PRIVATE | ZEND_ACC_STATIC TSRMLS_CC);
}
