#include "cii_config.h"

zend_class_entry *cii_config_ce;
/**
* Class constructor
*
* Sets the $config data from the primary config.php file as a class variable.
*
* @return	void
*
* public function __construct()
*/
PHP_METHOD(cii_config, __construct)
{
	//init cii_config::config
	zval *retval;
	CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "cii_get_config", &retval, 0, NULL);
	/*
	* we will use zend_update_property() or add_property_zval_ex() function to update class property.
	* these function will separate zval when zval's is_ref__gc is true.
	* so i do a hack here:
	* --  Z_UNSET_ISREF_P();
	* --  zend_update_property();
	* --  Z_SET_ISREF_P();
	* it's a good way to lead zend_update_property() not separate zval.
	*/
	Z_UNSET_ISREF_P(retval);
	zend_update_property(cii_config_ce, getThis(), ZEND_STRL("config"), retval TSRMLS_CC);
	Z_SET_ISREF_P(retval);
	zval_ptr_dtor(&retval);
	//
	//preg_match('/^((\[[0-9a-f:]+\])|(\d{1,3}(\.\d{1,3}){3})|[a-z0-9\-\.]+)(:\d+)?$/i', $_SERVER['HTTP_HOST']);
	zval **server = &PG(http_globals)[TRACK_VARS_SERVER];
	zval **http_host, **script_name, **script_filename;
    if( zend_hash_find(Z_ARRVAL_PP(server), "HTTP_HOST", sizeof("HTTP_HOST"), (void**)&http_host) == FAILURE ||
    	zend_hash_find(Z_ARRVAL_PP(server), "SCRIPT_NAME", sizeof("SCRIPT_NAME"), (void**)&script_name) == FAILURE ||
    	zend_hash_find(Z_ARRVAL_PP(server), "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME"), (void**)&script_filename) == FAILURE ){
    	php_printf("not found!\n");
    	//return;
    }else{
    	php_printf("HTTP_HOST:%s\n",Z_STRVAL_PP(http_host));
    	php_printf("SCRIPT_NAME:%s\n",Z_STRVAL_PP(script_name));
    	php_printf("SCRIPT_FILENAME:%s\n",Z_STRVAL_PP(script_filename));
    	zval *retval;
    	zval **params[1];
    	MAKE_STD_ZVAL(*params[0]);
    	ZVAL_STRING(*params[0], Z_STRVAL_PP(script_filename), 1);
    	CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "basename", &retval, 1, params);
    	php_printf("basename:%s\n",Z_STRVAL_P(retval));
    	zval_ptr_dtor(&retval);
    }
    return;
	//
	zval *ret;
	zval *preg;
	zval *host;
	zval **params[2];
	MAKE_STD_ZVAL(preg);
	ZVAL_STRING(preg, "/^((\\[[0-9a-f:]+\\])|(\\d{1,3}(\\.\\d{1,3}){3})|[a-z0-9\\-\\.]+)(:\\d+)?$/i", 1);
	MAKE_STD_ZVAL(host);
	ZVAL_STRING(host, "http://www.baidu.com", 1);
	params[0] = &preg;
	params[1] = &host;
	CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "preg_match", &ret, 2, params);
	php_printf("type:%d\n",Z_TYPE_P(ret));
	php_printf("value:%ld\n",Z_LVAL_P(ret));
	php_printf("s:%s\n",Z_STRVAL_P(preg));
	//output log
	php_printf("Info: Config Class Initialized\n");
}
/**
* Fetch a config file item
*
* @param	string	$item	Config item name
* @param	string	$index	Index name
* @return	string|null	The configuration item or NULL if the item doesn't exist
*
* public function item($item, $index = '')
*/
//notice: if item or index is long, will return null.
PHP_METHOD(cii_config, item)
{
	char *item, *index = NULL;
	uint item_len, index_len;
	zval **value;
	zval *config;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &item, &item_len, &index, &index_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	config = zend_read_property(cii_config_ce, getThis(), "config", 6, 1 TSRMLS_CC);

	if(!index){
		if( zend_hash_find(Z_ARRVAL_P(config), item, item_len+1, (void**)&value) == FAILURE ){
			RETURN_NULL();
		}else{
			RETURN_ZVAL(*value, 1, 0);
		}
	}
	
	if( zend_hash_find(Z_ARRVAL_P(config), index, index_len+1, (void**)&value) == FAILURE ){
		RETURN_NULL();
	}else{
		zval **item_value;
		if( zend_hash_find(Z_ARRVAL_PP(value), item, item_len+1, (void**)&item_value) == FAILURE ){
			RETURN_NULL();
		}else{
			RETURN_ZVAL(*item_value, 1, 0);
		}	
	}
}
/**
* Set a config file item
*
* @param	string	$item	Config item key
* @param	string	$value	Config item value
* @return	void
*
* public function set_item($item, $value)
*/
//notice:what about item is long?
PHP_METHOD(cii_config, set_item)
{
	char *item;
	uint item_len;
	zval *value;
	zval **value_ptr;
	zval *config;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &item, &item_len, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	config = zend_read_property(cii_config_ce, getThis(), "config", 6, 1 TSRMLS_CC);

	value_ptr = &value;
	CII_IF_ISREF_THEN_SEPARATE_ELSE_ADDREF(value_ptr);	
	zend_hash_update(Z_ARRVAL_P(config), item, item_len+1, value_ptr, sizeof(zval *), NULL);
}

zend_function_entry cii_config_methods[] = {
	PHP_ME(cii_config, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_config, item, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_config, set_item, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_config)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "cii_config", cii_config_methods);
	cii_config_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * List of all loaded config values
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_config_ce, ZEND_STRL("config"), ZEND_ACC_PUBLIC TSRMLS_CC); 
}