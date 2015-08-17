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
	php_printf("ref:%d\n",Z_REFCOUNT_P(CII_G(config)));
	php_printf("isref:%d\n",Z_ISREF_P(CII_G(config)));
	
	//php_printf("ref:%d\n",Z_REFCOUNT_P(CII_G(config)));
	//php_printf("isref:%d\n",Z_ISREF_P(CII_G(config)));
	//zend_update_property(cii_config_ce, getThis(), ZEND_STRL("config"), CII_G(config) TSRMLS_CC);
	zval *temp;
	MAKE_STD_ZVAL(temp);
	ZVAL_STRING(temp,"hello",1);
	add_property_zval_ex(getThis(), ZEND_STRS("config"), temp TSRMLS_CC);

zval *config = zend_read_property(cii_config_ce, getThis(), "config", 6, 1 TSRMLS_CC);
	//don't know why when update property it will separate if zval is ref
	zval **config_ptr_ptr = &config;
	zval_ptr_dtor(config_ptr_ptr);
	*config_ptr_ptr = CII_G(config);
	php_printf("c-ref:%d\n",Z_REFCOUNT_P(config));
	php_printf("c-isref:%d\n",Z_ISREF_P(config));

	zval_ptr_dtor(&retval);

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