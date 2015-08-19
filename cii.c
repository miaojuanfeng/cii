/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cii.h"
#include "cii_controller.c"
#include "cii_model.c"
#include "cii_loader.c"
#include "cii_helper.c"
#include "cii_lang.c"
#include "cii_config.c"


ZEND_DECLARE_MODULE_GLOBALS(cii)

/* True global resources - no need for thread safety here */
static int le_cii;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("cii.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_cii_globals, cii_globals)
    STD_PHP_INI_ENTRY("cii.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_cii_globals, cii_globals)
PHP_INI_END()
*/
/* }}} */

static void php_cii_globals_ctor(zend_cii_globals *cii_globals)
{
	MAKE_STD_ZVAL(cii_globals->config);
	array_init(cii_globals->config);
}

static void php_cii_globals_dtor(zend_cii_globals *cii_globals)
{
	zval_ptr_dtor(&cii_globals->config);
}

PHP_MINIT_FUNCTION(cii)
{
	ZEND_INIT_MODULE_GLOBALS(cii, php_cii_globals_ctor, php_cii_globals_dtor);
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	ZEND_MINIT(cii_controller)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_model)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_loader)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_lang)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_config)(INIT_FUNC_ARGS_PASSTHRU);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cii)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}

PHP_RINIT_FUNCTION(cii)
{
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(cii)
{
#ifndef ZTS
	php_cii_globals_dtor(&cii_globals);
#endif	
	return SUCCESS;
}

PHP_MINFO_FUNCTION(cii)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "cii support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}

ZEND_BEGIN_ARG_INFO_EX(cii_get_instance_arginfo, 0, 1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cii_get_config_arginfo, 0, 1, 0)
	ZEND_ARG_ARRAY_INFO(0, replace, 1)
ZEND_END_ARG_INFO()

PHP_FUNCTION(cii_get_instance)
{
	GET_CII_CONTROLLER_INSTANCE_BY_REF();
}
/**
* Loads the main config.php file
*
* This function lets us grab the config file even if the Config class
* hasn't been instantiated yet
*
* @param	array
* @return	array
*
* function &get_config(Array $replace = array())
*/
PHP_FUNCTION(cii_get_config)
{
	HashTable *replace = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|H!", &replace) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if(!(Z_ARRVAL_P(CII_G(config))->nNumOfElements)){
		zval **cfg;
		char *file;
		uint file_len = spprintf(&file, 0, "%s", "/usr/local/nginx/html/cii/configs/config.php");

		CII_ALLOC_ACTIVE_SYMBOL_TABLE();

		cii_loader_import(file, file_len, 0 TSRMLS_CC);

		if( zend_hash_find(EG(active_symbol_table), "config", 7, (void**)&cfg) == FAILURE || 
			Z_TYPE_PP(cfg) != IS_ARRAY ){
			php_error(E_WARNING, "Your config file does not appear to be formatted correctly.");	
		}

		*CII_G(config) = **cfg;
		zval_copy_ctor(CII_G(config));	

		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		efree(file);
	}

	if(replace){
		HashPosition pos;
		char *key;
		uint key_len;
		ulong idx;
		zval **value;
		uint key_type;
		for(zend_hash_internal_pointer_reset_ex(replace, &pos);
		    zend_hash_has_more_elements_ex(replace, &pos) == SUCCESS;
		    zend_hash_move_forward_ex(replace, &pos)){
			if( (key_type = zend_hash_get_current_key_ex(replace, &key, &key_len, &idx, 0, &pos)) == HASH_KEY_NON_EXISTENT){
				continue;
			}
			if(zend_hash_get_current_data_ex(replace, (void**)&value, &pos) == FAILURE){
				continue;
			}
			CII_IF_ISREF_THEN_SEPARATE_ELSE_ADDREF(value);
			switch(key_type){
				case HASH_KEY_IS_STRING:
					zend_hash_update(Z_ARRVAL_P(CII_G(config)), key, key_len, value, sizeof(zval *), NULL);
					break;
				case HASH_KEY_IS_LONG:
					zend_hash_index_update(Z_ARRVAL_P(CII_G(config)), idx, value, sizeof(zval *), NULL);
					break;
			}
		}   	
	}

	if(return_value_used){
		zval_ptr_dtor(return_value_ptr);
		(*return_value_ptr) = CII_G(config);
		Z_ADDREF_P(*return_value_ptr);
	}
}
/**
* Is HTTPS?
*
* Determines if the application is accessed via an encrypted
* (HTTPS) connection.
*
* @return	bool
*
* function is_https()
*/
PHP_FUNCTION(cii_is_https)
{
	zval **https, **http_x_forwarded_proto, **http_front_end_https;

	zval **server = &PG(http_globals)[TRACK_VARS_SERVER];

	if( zend_hash_find(Z_ARRVAL_PP(server), "HTTPS", 6, (void**)&https) != FAILURE){
		char *strlower = zend_str_tolower_dup(Z_STRVAL_PP(https), Z_STRLEN_PP(https));
		if( strcmp(strlower, "off") ){
			RETURN_TRUE;
		}
	}else if( zend_hash_find(Z_ARRVAL_PP(server), "HTTP_X_FORWARDED_PROTO", 23, (void**)&http_x_forwarded_proto) != FAILURE){
		if( !strcmp(Z_STRVAL_PP(http_x_forwarded_proto), "https") ){
			RETURN_TRUE;
		}
	}else if( zend_hash_find(Z_ARRVAL_PP(server), "HTTP_FRONT_END_HTTPS", 21, (void**)&http_front_end_https) != FAILURE){
		char *strlower = zend_str_tolower_dup(Z_STRVAL_PP(http_front_end_https), Z_STRLEN_PP(http_front_end_https));
		if( strcmp(strlower, "off") ){
			RETURN_TRUE;
		}
	}
	RETURN_FALSE;
}


PHP_FUNCTION(cii_test)
{
	zval **carrier = &PG(http_globals)[TRACK_VARS_SERVER];
	/*HashPosition p = Z_ARRVAL_PP(carrier)->pListHead;
	while(p){
    	php_printf("server:%s\n",p->arKey);
    	p = p->pListNext;
    }*/
    zval **query;
    if( zend_hash_find(Z_ARRVAL_PP(carrier), "QUERY_STRING", 13, (void**)&query) == FAILURE ){
    	zval *temp;
    	MAKE_STD_ZVAL(temp);
    	ZVAL_STRING(temp,"/home/index/123",1);
    	query = &temp;
    	//return;
    }
    php_printf("QUERY_STRING:%s\n",Z_STRVAL_PP(query));
    zval *cut;
    MAKE_STD_ZVAL(cut);
    ZVAL_STRING(cut,"/",1);
    zval **params[2];
    params[0] = &cut;
    params[1] = query;
    zval *func_name;
    MAKE_STD_ZVAL(func_name);
    ZVAL_STRING(func_name,"explode",1);
    zval *retval;
    if( call_user_function_ex(EG(function_table), NULL, func_name, &retval, 2, params, 0, NULL TSRMLS_CC) == FAILURE ){
		php_error(E_ERROR, "Call function failed");
	}
	php_printf("%d\n",Z_TYPE_P(retval));
	HashPosition p = Z_ARRVAL_P(retval)->pListHead;
	while(p){
    	php_printf("explode:%ld\n",p->h);
    	p = p->pListNext;
    }
    zval_ptr_dtor(query);
    zval_ptr_dtor(&retval);
    zval_ptr_dtor(&func_name);
    zval_ptr_dtor(&cut);
}

const zend_function_entry cii_functions[] = {
	PHP_FE(cii_get_instance, cii_get_instance_arginfo)
	PHP_FE(cii_get_config, cii_get_config_arginfo)
	PHP_FE(cii_test, NULL)
	PHP_FE(cii_is_https, NULL)
	CII_HELPER_FUNCTION
	PHP_FE_END
};

zend_module_entry cii_module_entry = {
	STANDARD_MODULE_HEADER,
	"cii",
	cii_functions,
	PHP_MINIT(cii),
	PHP_MSHUTDOWN(cii),
	PHP_RINIT(cii),
	PHP_RSHUTDOWN(cii),
	PHP_MINFO(cii),
	PHP_CII_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CII
ZEND_GET_MODULE(cii)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */