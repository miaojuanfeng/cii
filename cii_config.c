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
	/*
	* init cii_config::config
	*/
	zval *get_config_result = cii_get_config();
	//CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "cii_get_config", &get_config_result, 0, NULL);
	/*
	* we will use zend_update_property() or add_property_zval_ex() function to update class property.
	* these function will separate zval when zval's is_ref__gc is true.
	* so i do a hack here:
	* --  Z_UNSET_ISREF_P();
	* --  zend_update_property();
	* --  Z_SET_ISREF_P();
	* it's a good way to lead zend_update_property() not separate zval.
	*/
	Z_UNSET_ISREF_P(get_config_result);
	zend_update_property(cii_config_ce, getThis(), ZEND_STRL("config"), get_config_result TSRMLS_CC);
	Z_SET_ISREF_P(get_config_result);
	/*
	* set default base_url
	*/
	//preg_match('/^((\[[0-9a-f:]+\])|(\d{1,3}(\.\d{1,3}){3})|[a-z0-9\-\.]+)(:\d+)?$/i', $_SERVER['HTTP_HOST']);
	zval **base_url_value;
	/*zend_hash_find(Z_ARRVAL_P(get_config_result), "base_url", sizeof("base_url"), (void**)&base_url_value);
	php_printf("base_url type:%d\n",Z_TYPE_P(*base_url_value));
	php_printf("base_url value:%s\n",Z_STRVAL_P(*base_url_value));
	php_printf("base_url len:%d\n",Z_STRLEN_P(*base_url_value));*/
	if( zend_hash_find(Z_ARRVAL_P(get_config_result), "base_url", sizeof("base_url"), (void**)&base_url_value) == FAILURE ||
		Z_TYPE_PP(base_url_value) == IS_STRING && Z_STRLEN_PP(base_url_value) == 0 && !strcmp(Z_STRVAL_PP(base_url_value), "") ){
		zval **server = &PG(http_globals)[TRACK_VARS_SERVER];
		zval **http_host;
		zval *default_base_url;
		MAKE_STD_ZVAL(default_base_url);
		//php_printf("base_url is empty\n");
		if( zend_hash_find(Z_ARRVAL_PP(server), "HTTP_HOST", sizeof("HTTP_HOST"), (void**)&http_host) != FAILURE ){
			//php_printf("http_host:%s\n",Z_STRVAL_PP(http_host));
			zval *preg_match_result;
			zval *preg;
			zval *host;
			zval **params[2];
			MAKE_STD_ZVAL(preg);
			ZVAL_STRING(preg, "/^((\\[[0-9a-f:]+\\])|(\\d{1,3}(\\.\\d{1,3}){3})|[a-z0-9\\-\\.]+)(:\\d+)?$/i", 1);
			MAKE_STD_ZVAL(host);
			ZVAL_STRING(host, Z_STRVAL_PP(http_host), 1);
			//php_printf("http_host:%s\n",Z_STRVAL_P(preg));
			//php_printf("http_host:%s\n",Z_STRVAL_P(host));
			params[0] = &preg;
			params[1] = &host;
			CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "preg_match", &preg_match_result, 2, params);
			zval_ptr_dtor(&preg);
			zval_ptr_dtor(&host);
			//php_printf("type:%d\n",Z_TYPE_P(preg_match_result));
			//php_printf("value:%ld\n",Z_LVAL_P(preg_match_result));
			if( Z_LVAL_P(preg_match_result) ){
				zval **script_name, **script_filename;
			    if( zend_hash_find(Z_ARRVAL_PP(server), "SCRIPT_NAME", sizeof("SCRIPT_NAME"), (void**)&script_name) == FAILURE ||
			    	zend_hash_find(Z_ARRVAL_PP(server), "SCRIPT_FILENAME", sizeof("SCRIPT_FILENAME"), (void**)&script_filename) == FAILURE ){
			    	//php_printf("not found!\n");
			    	//return;
			    	goto set_default_base_url;
			    }else{
			    	char *base_url;
			    	//php_printf("HTTP_HOST:%s\n",Z_STRVAL_PP(http_host));
			    	//php_printf("SCRIPT_NAME:%s\n",Z_STRVAL_PP(script_name));
			    	//php_printf("SCRIPT_FILENAME:%s\n",Z_STRVAL_PP(script_filename));
			    	zval *basename_retval;
			    	zval *basename_param_value;
			    	zval **basename_params[1];
			    	MAKE_STD_ZVAL(basename_param_value);
			    	ZVAL_STRING(basename_param_value, Z_STRVAL_PP(script_filename), 1);
			    	basename_params[0] = &basename_param_value;
			    	CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "basename", &basename_retval, 1, basename_params);
			    	zval_ptr_dtor(&basename_param_value);
			    	//php_printf("basename_retval:%s\n",Z_STRVAL_P(basename_retval));
			    	zval *strpos_retval;
			    	zval **strpos_params[2];
			    	strpos_params[0] = script_name;
			    	strpos_params[1] = &basename_retval;
			    	CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "strpos", &strpos_retval, 2, strpos_params);
			    	//php_printf("strpos_retval:%ld\n",Z_LVAL_P(strpos_retval));
			    	zval *substr_retval;
			    	zval **substr_params[3];
			    	zval *substr_param_start;
			    	MAKE_STD_ZVAL(substr_param_start);
			    	ZVAL_LONG(substr_param_start, 0);
			    	substr_params[0] = script_name;
			    	substr_params[1] = &substr_param_start;
			    	substr_params[2] = &strpos_retval;
			    	CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "substr", &substr_retval, 3, substr_params);
			    	zval_ptr_dtor(&substr_param_start);
			    	//php_printf("substr_retval:%s\n",Z_STRVAL_P(substr_retval));
			    	zval *cii_is_https_retval;
			    	CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "cii_is_https", &cii_is_https_retval, 0, NULL);

			    	char *is_php;
			    	if(Z_LVAL_P(cii_is_https_retval)){
			    		is_php = "https";
			    	}else{
			    		is_php = "http";
			    	}

			    	spprintf(&base_url, 0, "%s%s%s%s", is_php, "://", Z_STRVAL_PP(http_host), Z_STRVAL_P(substr_retval));
			    	ZVAL_STRING(default_base_url, base_url, 0);

			    	zval_ptr_dtor(&basename_retval);
			    	zval_ptr_dtor(&strpos_retval);
			    	zval_ptr_dtor(&substr_retval);
			    	zval_ptr_dtor(&cii_is_https_retval);
			    }
			}else{
				zval_ptr_dtor(&preg_match_result);
				goto set_default_base_url;
			}
			zval_ptr_dtor(&preg_match_result);
		}else{
set_default_base_url:
			ZVAL_STRING(default_base_url, "http://localhost/", 1);
		}
		zend_hash_update(Z_ARRVAL_P(get_config_result), "base_url", 9, &default_base_url, sizeof(zval *), NULL);
	}
	zval_ptr_dtor(&get_config_result);
	/*
	* output log
	*/
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

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &item, &item_len, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	//can replaced by CII_G(config)
	zval *config = zend_read_property(cii_config_ce, getThis(), "config", 6, 1 TSRMLS_CC);

	value_ptr = &value;
	CII_IF_ISREF_THEN_SEPARATE_ELSE_ADDREF(value_ptr);	
	zend_hash_update(Z_ARRVAL_P(config), item, item_len+1, value_ptr, sizeof(zval *), NULL);
}
/**
* Fetch a config file item with slash appended (if not empty)
*
* @param	string		$item	Config item name
* @return	string|null	The configuration item or NULL if the item doesn't exist
*
* public function slash_item($item)
*/
//notice:what about item is long?
PHP_METHOD(cii_config, slash_item)
{
	char *item;
	uint item_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &item, &item_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	//can replaced by CII_G(config)
	zval *config = zend_read_property(cii_config_ce, getThis(), "config", 6, 1 TSRMLS_CC);
	zval **value;

	if( zend_hash_find(Z_ARRVAL_P(config), item, item_len+1, (void**)&value) == FAILURE ){
		/*
		* return_value aleady is null.
		* no need to reset by RETURN_NULL()
		*/
		return;
	}else if( Z_TYPE_PP(value) == IS_STRING && Z_STRLEN_PP(value) == 0 && !strcmp(Z_STRVAL_PP(value), "") ){
		RETURN_STRLING("", 1);
	}else{
		char *retval;
		zval *rtrim_retval;
		zval *rtrim_param_0, *rtrim_param_1;
		zval **rtrim_params[2];
		MAKE_STD_ZVAL(rtrim_param_0);
		ZVAL_ZVAL(rtrim_param_0, *value, 1, 0);
		MAKE_STD_ZVAL(rtrim_param_1);
		ZVAL_STRING(rtrim_param_1, "/", 1);
		rtrim_params[0] = &rtrim_param_0;
		rtrim_params[1] = &rtrim_param_1;
		CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "rtrim", &rtrim_retval, 2, rtrim_params);

		spprintf(&retval, 0, "%s%s", Z_STRVAL_P(rtrim_retval), "/");
		
		zval_ptr_dtor(&rtrim_param_0);
		zval_ptr_dtor(&rtrim_param_1);
		zval_ptr_dtor(&rtrim_retval);

		RETURN_STRING(retval, 0);
	}
}

zend_function_entry cii_config_methods[] = {
	PHP_ME(cii_config, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_config, item, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_config, set_item, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_config, slash_item, NULL, ZEND_ACC_PUBLIC)
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