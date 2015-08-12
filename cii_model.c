#include "cii_model.h"

zend_class_entry *cii_model_ce;

ZEND_BEGIN_ARG_INFO_EX(cii_model___get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

PHP_METHOD(cii_model,__construct)
{
	php_printf("Info: Model Class Initialized\n");
}

PHP_METHOD(cii_model,__get)
{
	char *key;
	uint len;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s",&key,&len)==FAILURE){
		return;
	}
	zval *CII = GET_CII_CONTROLLER_INSTANCE();
	zval *property = zend_read_property(cii_controller_ce,CII,key,len,1 TSRMLS_CC);
	if(Z_TYPE_P(property)==IS_NULL){
		//RETURN_NULL();  return_value type already IS_NULL;
		return;
	}
	RETURN_ZVAL(property,1,0);
}

const zend_function_entry cii_model_methods[] = {
	PHP_ME(cii_model,__construct,NULL,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(cii_model,__get,cii_model___get_arginfo,ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

ZEND_MINIT_FUNCTION(cii_model){
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"cii_model",cii_model_methods);
	cii_model_ce = zend_register_internal_class(&ce TSRMLS_CC);
}
