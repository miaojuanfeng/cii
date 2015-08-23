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

	/*
	* init instance
	*/
	zend_update_static_property(cii_controller_ce, ZEND_STRL("instance"), getThis() TSRMLS_CC);
	/*
	* load is_loaded objects
	*/
	HashPosition pos;
	char *key;
	uint key_len;
	ulong idx;
	zval **value;
	zval **exist_object;
	uint i = 1;
	for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(CII_G(is_loaded)), &pos);
		    zend_hash_has_more_elements_ex(Z_ARRVAL_P(CII_G(is_loaded)), &pos) == SUCCESS;
		    zend_hash_move_forward_ex(Z_ARRVAL_P(CII_G(is_loaded)), &pos)){
			if(zend_hash_get_current_key_ex(Z_ARRVAL_P(CII_G(is_loaded)), &key, &key_len, &idx, 0, &pos) != HASH_KEY_IS_STRING){
				continue;
			}
			if(zend_hash_get_current_data_ex(Z_ARRVAL_P(CII_G(is_loaded)), (void**)&value, &pos) == FAILURE){
				continue;
			}
			if( zend_hash_find(Z_ARRVAL_P(CII_G(classes)), Z_STRVAL_PP(value), Z_STRLEN_PP(value)+1, (void**)&exist_object) == FAILURE ){
				continue;
			}
			switch(i){
				case 1:
					zend_update_property(cii_controller_ce, getThis(), "lang", 4, *exist_object TSRMLS_CC);
					break;
				case 2:
					zend_update_property(cii_controller_ce, getThis(), "uri", 3, *exist_object TSRMLS_CC);
					break;
				case 3:
					zend_update_property(cii_controller_ce, getThis(), "router", 6, *exist_object TSRMLS_CC);
					break;	
			}
			i++;
	}	    	
	/*
	* load cii_loader object
	*/
	zval *cii_load_obj;
	MAKE_STD_ZVAL(cii_load_obj);
	object_init_ex(cii_load_obj, cii_loader_ce);
	zend_update_property(cii_controller_ce, getThis(), ZEND_STRL("load"), cii_load_obj TSRMLS_CC);
	zval_ptr_dtor(&cii_load_obj);
	/*
	* call load construct
	*/
	if (zend_hash_exists(&cii_loader_ce->function_table, "__construct", 12)) {
		zval *cii_load_retval;
		CII_CALL_USER_METHOD_EX(&cii_load_obj, "__construct", &cii_load_retval, 0, NULL);
		zval_ptr_dtor(&cii_load_retval);
	}
	/*
	* output log
	*/
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
	INIT_CLASS_ENTRY(ce, "CII_Controller", cii_controller_methods);
	cii_controller_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * Reference to the CI singleton
	 *
	 * @var	object
	 */
	zend_declare_property_null(cii_controller_ce, ZEND_STRL("instance"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC TSRMLS_CC);
}
