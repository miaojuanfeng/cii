#include "cii_router.h"

zend_class_entry *cii_router_ce;

PHP_METHOD(cii_router, __construct)
{
	/*
	*	add cii_uri object into cii_router
	*/
	zval *uri = load_class("URI", 0, NULL);
	zend_update_property(cii_router_ce, getThis(), ZEND_STRL("uri"), uri TSRMLS_CC);
	/*
	*	update cii_uri::rsegments
	*/
	zval *segments = zend_read_property(cii_uri_ce, uri, ZEND_STRL("segments"), 1 TSRMLS_CC);
	zval *rsegments = zend_read_property(cii_uri_ce, uri, ZEND_STRL("rsegments"), 1 TSRMLS_CC);
	/*
	*	copy cii_uri::segments to cii_uri::rsegments
	*/
	HashPosition pos;
	char *key;
	uint key_len;
	ulong idx;
	zval **value;
	for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(segments), &pos);
	    zend_hash_has_more_elements_ex(Z_ARRVAL_P(segments), &pos) == SUCCESS;
	    zend_hash_move_forward_ex(Z_ARRVAL_P(segments), &pos)){

		zend_hash_get_current_key_ex(Z_ARRVAL_P(segments), &key, &key_len, &idx, 0, &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(segments), (void**)&value, &pos);

		Z_ADDREF_PP(value);
		zend_hash_index_update(Z_ARRVAL_P(rsegments), idx, value, sizeof(zval *), NULL);
	}
	/*
	*	check whether cii_uri::rsegments[1] and cii_uri::rsegments[2] is empty
	*	if empty set default value to them
	*/
	zval **class, **method;
	zval *rsegments_class, *rsegments_method;
	if( zend_hash_index_find(Z_ARRVAL_P(rsegments), 1, (void**)&class) == FAILURE ||
		Z_TYPE_PP(class) == IS_STRING && Z_STRLEN_PP(class) == 0 ){
		MAKE_STD_ZVAL(rsegments_class);
		ZVAL_STRING(rsegments_class, "welcome", 1);
		zend_hash_index_update(Z_ARRVAL_P(rsegments), 1, &rsegments_class, sizeof(zval *), NULL);
	}else{
		rsegments_class = *class;
	}
	if( zend_hash_index_find(Z_ARRVAL_P(rsegments), 2, (void**)&method) == FAILURE ||
		Z_TYPE_PP(method) == IS_STRING && Z_STRLEN_PP(method) == 0 ){
		MAKE_STD_ZVAL(rsegments_method);
		ZVAL_STRING(rsegments_method, "index", 1);
		zend_hash_index_update(Z_ARRVAL_P(rsegments), 2, &rsegments_method, sizeof(zval *), NULL);
	}else{
		rsegments_method = *method;
	}
	/*
	*	update cii_router::class and update cii_router::method
	*/
	zend_update_property(cii_router_ce, getThis(), ZEND_STRL("class"), rsegments_class TSRMLS_CC);
	zend_update_property(cii_router_ce, getThis(), ZEND_STRL("method"), rsegments_method TSRMLS_CC);
	/*
	*	output log
	*/
	php_printf("Info: Router Class Initialized\n");
}

zend_function_entry cii_router_methods[] = {
	PHP_ME(cii_router, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
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
	zend_declare_property_null(cii_router_ce, ZEND_STRL("class"), ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * Current method name
	 *
	 * @var	string
	 */
	zend_declare_property_null(cii_router_ce, ZEND_STRL("method"), ZEND_ACC_PUBLIC TSRMLS_CC);
}