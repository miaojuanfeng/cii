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
	* 	init cii_config::config
	*/
	zval *config = cii_get_config();
	/*
	*	 we will use zend_update_property() or add_property_zval_ex() function to update class property.
	* 	these function will separate zval when zval's is_ref__gc is true.
	* 	so i do a hack here:
	* 	--  Z_UNSET_ISREF_P();
	* 	--  zend_update_property();
	* 	--  Z_SET_ISREF_P();
	* 	it's a good way to lead zend_update_property() not separate zval.
	*/
	Z_UNSET_ISREF_P(config);
	zend_update_property(cii_config_ce, getThis(), ZEND_STRL("config"), config TSRMLS_CC);
	Z_SET_ISREF_P(config);
	/*
	* 	set default base_url
	*/
	zval **base_url_value;
	if( zend_hash_find(Z_ARRVAL_P(config), "base_url", sizeof("base_url"), (void**)&base_url_value) == FAILURE ||
		Z_TYPE_PP(base_url_value) == IS_STRING && Z_STRLEN_PP(base_url_value) == 0 ){
		zval *server = PG(http_globals)[TRACK_VARS_SERVER];
		zval **http_host;
		zval **script_name;
		zval *base_url;
		MAKE_STD_ZVAL(base_url);
		if( zend_hash_find(Z_ARRVAL_P(server), "HTTP_HOST", sizeof("HTTP_HOST"), (void**)&http_host) != FAILURE &&
			zend_hash_find(Z_ARRVAL_P(server), "SCRIPT_NAME", sizeof("SCRIPT_NAME"), (void**)&script_name) != FAILURE ){
			char *base_url_string, orig, *p, *is_https;
			p = strrchr(Z_STRVAL_PP(script_name), '/')+1;
			orig = *p;
			*p = '\0';
			if( !cii_is_https() ){
				is_https = "http://";
			}else{
				is_https = "https://";
			}
			spprintf(&base_url_string, 0, "%s%s%s", is_https, Z_STRVAL_PP(http_host), Z_STRVAL_PP(script_name));
			*p = orig;
			ZVAL_STRING(base_url, base_url_string, 0);
		}else{
			ZVAL_STRING(base_url, "http://localhost/", 1);
		}
		zend_hash_update(Z_ARRVAL_P(config), "base_url", 9, &base_url, sizeof(zval *), NULL);
	}
	/*
	* output log
	*/
	cii_write_log(3, "Config Class Initialized");
}
/*
*	cii_config_item
*/
ZEND_API zval* cii_config_item(char *item, uint item_len, char *index, uint index_len)
{
	zval **value;
	zval *config = cii_get_config();
	if( !index ){
		if( zend_hash_find(Z_ARRVAL_P(config), item, item_len+1, (void**)&value) == FAILURE ){
			return NULL;
		}else{
			return *value;
		}
	}
	if( zend_hash_find(Z_ARRVAL_P(config), index, index_len+1, (void**)&value) == FAILURE ){
		return NULL;
	}else{
		zval **item_value;
		if( zend_hash_find(Z_ARRVAL_PP(value), item, item_len+1, (void**)&item_value) == FAILURE ){
			return NULL;
		}else{
			return *item_value;
		}	
	}
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
	char *item = NULL;
	char *index = NULL;
	uint item_len = 0;
	uint index_len = 0;
	zval *retval;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &item, &item_len, &index, &index_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if( retval = cii_config_item(item, item_len, index, index_len) ){
		RETURN_ZVAL(retval, 1, 0);
	}
}
/*
*	cii_config_slash_item
*/
ZEND_API char* cii_config_slash_item(char *item, uint item_len, char *index, uint index_len, char *need_free)
{
	zval *retval;
	*need_free = 0;
	if( retval = cii_config_item(item, item_len, index, index_len) ){
		if( Z_TYPE_P(retval) != IS_STRING ){
			convert_to_string(retval);
		}
		if( !Z_STRLEN_P(retval) ){
			return "";
		}
		if( *(Z_STRVAL_P(retval)+Z_STRLEN_P(retval)-1) != '/' ){
			char *slash_item;
			spprintf(&slash_item, 0, "%s%s", Z_STRVAL_P(retval), "/");
			*need_free = 1;
			return slash_item;
		}
		return Z_STRVAL_P(retval);
	}
	return NULL;
}	
/**
* Fetch a config file item with slash appended (if not empty)
*
* @param	string		$item	Config item name
* @param	string		$index	Index name
* @return	string|null	The configuration item or NULL if the item doesn't exist
*
* public function slash_item($item, $index = '')
*/
//notice:what about item is long?
PHP_METHOD(cii_config, slash_item)
{
	char *item = NULL;
	char *index = NULL;
	uint item_len = 0;
	uint index_len = 0;
	char *retval, need_free;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &item, &item_len, &index, &index_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	retval = cii_config_slash_item(item, item_len, index, index_len, &need_free);
	if( retval ){
		if( need_free ){
			RETURN_STRING(retval, 0);
		}else{
			RETURN_STRING(retval, 1);
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
	zval *config = cii_get_config();
	value_ptr = &value;
	CII_IF_ISREF_THEN_SEPARATE_ELSE_ADDREF(value_ptr);	
	zend_hash_update(Z_ARRVAL_P(config), item, item_len+1, value_ptr, sizeof(zval *), NULL);
}

zend_function_entry cii_config_methods[] = {
	PHP_ME(cii_config, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_config, item, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_config, slash_item, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_config, set_item, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_config)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Config", cii_config_methods);
	cii_config_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * List of all loaded config values
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_config_ce, ZEND_STRL("config"), ZEND_ACC_PUBLIC TSRMLS_CC); 
}