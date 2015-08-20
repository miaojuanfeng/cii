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
#include "cii_router.c"


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
	MAKE_STD_ZVAL(cii_globals->classes);
	array_init(cii_globals->classes);
	MAKE_STD_ZVAL(cii_globals->config);
	array_init(cii_globals->config);
	MAKE_STD_ZVAL(cii_globals->is_loaded);
	array_init(cii_globals->is_loaded);
	MAKE_STD_ZVAL(cii_globals->apppath);
	ZVAL_STRING(cii_globals->apppath, "", 1);
}

static void php_cii_globals_dtor(zend_cii_globals *cii_globals)
{
	zval_ptr_dtor(&cii_globals->classes);
	zval_ptr_dtor(&cii_globals->config);
	zval_ptr_dtor(&cii_globals->is_loaded);
	zval_ptr_dtor(&cii_globals->apppath);
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
	ZEND_MINIT(cii_router)(INIT_FUNC_ARGS_PASSTHRU);
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

ZEND_BEGIN_ARG_INFO_EX(cii_load_class_arginfo, 0, 1, 0)
	ZEND_ARG_INFO(0, class)
	ZEND_ARG_INFO(0, param_count)
	ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(cii_is_loaded_arginfo, 0, 1, 0)
	ZEND_ARG_INFO(0, class)
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

ZEND_API zval* is_loaded(char *class){

	if(class){
		char *lower_class;
		zval *zclass;

		MAKE_STD_ZVAL(zclass);
		ZVAL_STRING(zclass, class, 1);

		lower_class = zend_str_tolower_dup(class, sizeof(class));
		zend_hash_update(Z_ARRVAL_P(CII_G(is_loaded)), lower_class, strlen(lower_class)+1, &zclass, sizeof(zval *), NULL);

		efree(lower_class);
	}

	return CII_G(is_loaded);
}

ZEND_API zval* load_class(char *class, uint param_count, zval **params[]){

	zval **exist_object;
	if( zend_hash_find(Z_ARRVAL_P(CII_G(classes)), class, strlen(class)+1, (void**)&exist_object) == SUCCESS ){
		//php_printf("object found!\n");
		return *exist_object;
	}

	char *lower_class;
	char *file;
	uint file_len;
	zend_class_entry **class_ce;
	zval *class_obj;
	MAKE_STD_ZVAL(class_obj);
	lower_class = zend_str_tolower_dup(class, sizeof(class));
	php_printf("class: %s\n", lower_class);

	file_len = spprintf(&file, 0, "%s%s%s%s", Z_STRVAL_P(CII_G(apppath)), "libraries/", class, ".php");

	php_printf("file: %s\n", file);

	CII_ALLOC_ACTIVE_SYMBOL_TABLE();

	cii_loader_import(file, file_len, 0 TSRMLS_CC);
	efree(file);

	if( zend_hash_find(EG(class_table), lower_class, strlen(lower_class)+1, (void**)&class_ce) == FAILURE ){
		php_error(E_WARNING, "Unable to locate the specified class: %s", class);
		efree(lower_class);
		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		return class_obj;
	}

	is_loaded(class);

	object_init_ex(class_obj, *class_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), class, strlen(class)+1, &class_obj, sizeof(zval *), NULL);

	/*zval *retval;
	zval *func_name;
	MAKE_STD_ZVAL(func_name);
	ZVAL_STRING(func_name, "__construct", 1);
	call_user_function_ex(NULL, &class_obj, func_name, &retval, param_count, params, 0, NULL TSRMLS_CC);
	zval_ptr_dtor(&func_name);
	zval_ptr_dtor(&retval);*/

	CII_DESTROY_ACTIVE_SYMBOL_TABLE();
	efree(lower_class);

	return class_obj;
}
/**
* Class registry
*
* This function acts as a singleton. If the requested class does not
* exist it is instantiated and set to a static variable. If it has
* previously been instantiated the variable is returned.
*
* @param	string	the class name being requested
* @param	string	the directory where the class should be found
* @param	string	an optional argument to pass to the class constructor
* @return	object
*/
PHP_FUNCTION(cii_load_class)
{
	if(return_value_used){
		zval_ptr_dtor(return_value_ptr);
		(*return_value_ptr) = load_class("Calendar", 0, NULL);
		Z_ADDREF_P(*return_value_ptr);
	}	
}
/**
* Keeps track of which libraries have been loaded. This function is
* called by the load_class() function above
*
* @param	string
* @return	array
*/
PHP_FUNCTION(cii_is_loaded)
{
	if(return_value_used){
		zval_ptr_dtor(return_value_ptr);
		(*return_value_ptr) = is_loaded(NULL);
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


PHP_FUNCTION(cii_run)
{
	ZVAL_STRING(CII_G(apppath), "/usr/local/nginx/html/cii/", 1);
	//Z_ADDREF_P(CII_G(apppath));
	zend_hash_update(EG(zend_constants), "BASEPATH", 9, CII_G(apppath), sizeof(zval *), NULL); return;

	zval **carrier = &PG(http_globals)[TRACK_VARS_SERVER];
	/*HashPosition p = Z_ARRVAL_PP(carrier)->pListHead;
	while(p){
    	php_printf("server:%s\n",p->arKey);
    	p = p->pListNext;
    }*/
    /*zval delim, *retval, temp;
	ZVAL_STRING(&delim, "/", 1);
	ZVAL_STRING(&temp, "/asdas/asdasd/sadwqeq/sdas", 1);
	MAKE_STD_ZVAL(retval);
	array_init(retval);
	php_printf("value: %s\n",Z_STRVAL(delim));
	php_printf("value: %s\n",Z_STRVAL(temp));
	php_explode(&delim, &temp, retval, LONG_MAX);
	php_printf("type: %d\n",Z_TYPE_P(retval));*/

	zval **class = NULL;
	zval **function = NULL;
	char *file;
	uint file_len;
    zval **query;
    zval *uri_arr = NULL;
    zval *class_temp = NULL;
    zval *function_temp = NULL;
    //zval *query_temp;
   // MAKE_STD_ZVAL(query_temp);
   // ZVAL_STRING(query_temp, "/home/index/", 1);
   // query = &query_temp;
    if( zend_hash_find(Z_ARRVAL_PP(carrier), "QUERY_STRING", 13, (void**)&query) != FAILURE &&
    	Z_TYPE_PP(query) == IS_STRING && Z_STRLEN_PP(query) > 1){
    	zval zdelim, zstr;
    	ZVAL_STRINGL(&zdelim, "/", 1, 1);
    	{
    		char *p = Z_STRVAL_PP(query);
    		uint p_len = Z_STRLEN_PP(query);
    		//php_printf("cmp: %d\n", p[0] == '/');
    		if( p[0] == '/' && --p_len > 0 ){
    			p++;
    		}
    		if( p_len > 0 && p[p_len-1] == '/'){
    			p_len--;
    		}
    		//php_printf("p: %c\n", p[0]);
    		//php_printf("len: %d\n", p_len);
    		//php_printf("plen: %c\n", p[p_len-1]);
    		ZVAL_STRINGL(&zstr, p, p_len, 1);
    	}	
    	MAKE_STD_ZVAL(uri_arr);
    	array_init(uri_arr);
    	php_explode(&zdelim, &zstr, uri_arr, LONG_MAX);
    	php_printf("type: %d\n",Z_TYPE_P(uri_arr));
    	php_printf("num: %d\n",Z_ARRVAL_P(uri_arr)->nNumOfElements);
    	zval_dtor(&zdelim);
    	zval_dtor(&zstr);

    	/*zval *router_obj;
    	MAKE_STD_ZVAL(router_obj);
    	object_init_ex(router_obj, cii_router_ce);

    	zend_update_property()*/
    	
    	if( Z_ARRVAL_P(uri_arr)->nNumOfElements >= 1 ){
    		zend_hash_index_find(Z_ARRVAL_P(uri_arr), 0, (void**)&class);
    		//php_printf("class: %s\n", Z_STRVAL_PP(class));
    	}
    	if( Z_ARRVAL_P(uri_arr)->nNumOfElements >= 2 ){
    		zend_hash_index_find(Z_ARRVAL_P(uri_arr), 1, (void**)&function);
    		//php_printf("function: %s\n", Z_STRVAL_PP(function));
    	}
    	if( Z_ARRVAL_P(uri_arr)->nNumOfElements > 2 ){

    	}
    	if( !class || !function ){
    		goto set_default_class_and_function;
    	}
    }else{
set_default_class_and_function:
    	if( !class ){
    		MAKE_STD_ZVAL(class_temp);
    		ZVAL_STRING(class_temp, "welcome", 1);
    		class = &class_temp;
    	}
    	if( !function ){
    		MAKE_STD_ZVAL(function_temp);
    		ZVAL_STRING(function_temp, "index", 1);
    		function = &function_temp;
    	}
    }
    php_printf("class: %s\n", Z_STRVAL_PP(class));
    php_printf("function: %s\n", Z_STRVAL_PP(function));



	file_len = spprintf(&file, 0, "%s%s%s%s", Z_STRVAL_P(CII_G(apppath)), "controllers/", Z_STRVAL_PP(class), ".php");
	CII_ALLOC_ACTIVE_SYMBOL_TABLE();

	//if(php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC) == SUCCESS){
		cii_loader_import(file, file_len, 0 TSRMLS_CC);
	//	php_output_discard(TSRMLS_C);
	//}else{
	//	php_error(E_WARNING, "failed to create buffer");
	//}
	efree(file);

	zend_class_entry **controller_ce;
	if( zend_hash_find(EG(class_table), Z_STRVAL_PP(class), Z_STRLEN_PP(class)+1, (void**)&controller_ce) == FAILURE ){
		php_error(E_WARNING, "class not exists");
	}else{
		zval *controller_obj;
		MAKE_STD_ZVAL(controller_obj);
		object_init_ex(controller_obj, *controller_ce);
		zval *ret;
		CII_CALL_USER_METHOD_EX(&controller_obj, "__construct", &ret, 0, NULL);
		zval_ptr_dtor(&ret);
		zval *retval;
		CII_CALL_USER_METHOD_EX(&controller_obj, Z_STRVAL_PP(function), &retval, 0, NULL);
		zval_ptr_dtor(&retval);
	}
	CII_DESTROY_ACTIVE_SYMBOL_TABLE();


    if(class_temp){
    	zval_ptr_dtor(&class_temp);
    }
    if(function_temp){
    	zval_ptr_dtor(&function_temp);
    }	
    if(uri_arr){
    	zval_ptr_dtor(&uri_arr);
    }
}

const zend_function_entry cii_functions[] = {
	PHP_FE(cii_get_instance, cii_get_instance_arginfo)
	PHP_FE(cii_get_config, cii_get_config_arginfo)
	PHP_FE(cii_run, NULL)
	PHP_FE(cii_is_https, NULL)
	PHP_FE(cii_load_class, cii_load_class_arginfo)
	PHP_FE(cii_is_loaded, cii_is_loaded_arginfo)
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