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
#include "cii_loader.c"
#include "cii_lang.c"
#include "cii_config.c"
#include "cii_uri.c" 
#include "cii_router.c"
#include "cii_benchmark.c"
#include "cii_hooks.c"
#include "cii_output.c"
#include "cii_log.c"
#include "cii_input.c"   


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
	cii_globals->cii_controller_ce = NULL;
	cii_globals->cii_controller = NULL;
	cii_globals->classes = NULL;
	cii_globals->is_loaded = NULL;
	cii_globals->configs = NULL;
	cii_globals->apppath = NULL;
	cii_globals->output_replace_elapsed_time = 0;
	cii_globals->output_replace_memory_usage = 0;
	cii_globals->output_replace_memory_peak = 0;
}

static void php_cii_globals_dtor(zend_cii_globals *cii_globals)
{
	if( cii_globals->cii_controller ) zval_ptr_dtor(&cii_globals->cii_controller);
	if( cii_globals->classes ) zval_ptr_dtor(&cii_globals->classes);
	if( cii_globals->configs ) zval_ptr_dtor(&cii_globals->configs);
	if( cii_globals->is_loaded ) zval_ptr_dtor(&cii_globals->is_loaded);
	if( cii_globals->apppath ) efree(cii_globals->apppath);
}

PHP_MINIT_FUNCTION(cii)
{
	ZEND_INIT_MODULE_GLOBALS(cii, php_cii_globals_ctor, php_cii_globals_dtor);
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	ZEND_MINIT(cii_loader)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_lang)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_config)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_uri)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_router)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_benchmark)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_hooks)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_output)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_log)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MINIT(cii_input)(INIT_FUNC_ARGS_PASSTHRU);
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

/**
* Reference to the CI_Controller method.
*
* Returns current CI instance object
*
* @return object
*
* function &get_instance()
*/
PHP_FUNCTION(cii_get_instance)
{
	if( return_value_used && CII_G(cii_controller_ce) && CII_G(cii_controller) ){
		zval_ptr_dtor(return_value_ptr);
		(*return_value_ptr) = CII_G(cii_controller);
		Z_ADDREF_P(*return_value_ptr);
	}
}
/*
*	cii_get_apppath
*/
static void cii_get_apppath()
{
	CII_G(apppath) = estrdup("/usr/local/nginx/html/cii/");
}
/*
*	cii_get_config
*/
static zval* cii_get_config()
{
	if( !CII_G(configs) ){
		zval **cfg;
		char *file;
		uint file_len;

		if( !CII_G(apppath) ){
			cii_get_apppath();
		}
		file_len = spprintf(&file, 0, "%s%s", CII_G(apppath), "configs/config.php");

		CII_ALLOC_ACTIVE_SYMBOL_TABLE();

		cii_loader_import(file, file_len, 0 TSRMLS_CC);

		if( zend_hash_find(EG(active_symbol_table), "config", 7, (void**)&cfg) == FAILURE || 
			Z_TYPE_PP(cfg) != IS_ARRAY ){
			php_error(E_WARNING, "Your config file does not appear to be formatted correctly.");	
		}

		MAKE_STD_ZVAL(CII_G(configs));
		*CII_G(configs) = **cfg;
		zval_copy_ctor(CII_G(configs));	

		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		efree(file);
	}
	return CII_G(configs);
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

	cii_get_config();

	if( replace ){
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
					zend_hash_update(Z_ARRVAL_P(CII_G(configs)), key, key_len, value, sizeof(zval *), NULL);
					break;
				case HASH_KEY_IS_LONG:
					zend_hash_index_update(Z_ARRVAL_P(CII_G(configs)), idx, value, sizeof(zval *), NULL);
					break;
			}
		}   	
	}
	if(return_value_used){
		zval_ptr_dtor(return_value_ptr);
		(*return_value_ptr) = CII_G(configs);
		Z_ADDREF_P(*return_value_ptr);
	}
}
/*
*	is_loaded
*/
ZEND_API zval* cii_is_loaded(char *class)
{
	if( !CII_G(is_loaded) ){
		MAKE_STD_ZVAL(CII_G(is_loaded));
		array_init(CII_G(is_loaded));
	}
	if(class){
		char *lower_class;
		zval *zclass;

		MAKE_STD_ZVAL(zclass);
		ZVAL_STRING(zclass, class, 1);

		lower_class = zend_str_tolower_dup(class, strlen(class));
		zend_hash_update(Z_ARRVAL_P(CII_G(is_loaded)), lower_class, strlen(lower_class)+1, &zclass, sizeof(zval *), NULL);

		efree(lower_class);
	}
	return CII_G(is_loaded);
}
/**
* Keeps track of which libraries have been loaded. This function is
* called by the load_class() function above
*
* @param	string
* @return	array
*
* function &is_loaded($class = '')
*/
PHP_FUNCTION(cii_is_loaded)
{
	char *class = NULL;
	uint class_len;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!", &class, &class_len) == FAILURE ){
		WRONG_PARAM_COUNT;
	}
	zval_ptr_dtor(return_value_ptr);
	(*return_value_ptr) = cii_is_loaded(class);
	Z_ADDREF_P(*return_value_ptr);
}
/*
*	load_class
*/
ZEND_API zval* cii_load_class(char *class, uint class_len, uint param_count, zval **params[])
{
	if( !CII_G(classes) ){
		MAKE_STD_ZVAL(CII_G(classes));
		array_init(CII_G(classes));
	}
	zval **exist_object;
	if( zend_hash_find(Z_ARRVAL_P(CII_G(classes)), class, class_len+1, (void**)&exist_object) == SUCCESS ){
		return *exist_object;
	}

	if( !CII_G(apppath) ){
		cii_get_apppath();
	}

	CII_ALLOC_ACTIVE_SYMBOL_TABLE();
	/*
	*	check library file name
	*/
	char *p = NULL, orig;
	char *fullfile;
	uint fullfile_len;
	if( class_len > 12 && (p = class + (class_len-12)) && !strcmp(p, "_library.php") ){
		orig = *p;
		*p = '\0';
		fullfile_len = spprintf(&fullfile, 0, "%s%s", class, "_library.php");
	}else if( class_len > 8 && (p = class + (class_len-8)) && !strcmp(p, "_library") ){
		orig = *p;
		*p = '\0';
		fullfile_len = spprintf(&fullfile, 0, "%s%s", class, "_library.php");
	}else if( class_len > 4 && (p = class + (class_len-4)) && !strcmp(p, ".php") ){
		orig = *p;
		*p = '\0';
		fullfile_len = spprintf(&fullfile, 0, "%s%s", class, "_library.php");
	}else{
		fullfile_len = spprintf(&fullfile, 0, "%s%s", class, "_library.php");
	}
	if(p && !*p){
		*p = orig;
	}
	/*
	*	set library file name
	*/
	char *file;
	uint file_len;
	file_len = spprintf(&file, 0, "%s%s%s", CII_G(apppath), "libraries/", fullfile);
	cii_loader_import(file, file_len, 0 TSRMLS_CC);
	efree(file);
	efree(fullfile);

	char *class_fullname;
	uint class_fullname_len;
	zval *retval;
	char *subclass_prefix;
	char need_free = 0;
	if( retval = cii_config_item("subclass_prefix", 15, NULL, 0) ){
		subclass_prefix = zend_str_tolower_dup(Z_STRVAL_P(retval), Z_STRLEN_P(retval));
		need_free = 1;
	}else{
		subclass_prefix = "";
	}
	char *lower_class = zend_str_tolower_dup(class, class_len);
	class_fullname_len = spprintf(&class_fullname, 0, "%s%s", subclass_prefix, lower_class);

	zend_class_entry **class_ce;
	if( zend_hash_find(EG(class_table), class_fullname, class_fullname_len+1, (void**)&class_ce) == FAILURE ){
		php_error(E_WARNING, "Unable to locate the specified class: %s%s", subclass_prefix, class);
		efree(class_fullname);
		if( need_free ){
			efree(subclass_prefix);
		}
		efree(lower_class);
		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		return NULL;
	}
	/*
	*	free used memory
	*/
	if( need_free ){
		efree(subclass_prefix);
	}
	efree(lower_class);

	cii_is_loaded(class);

	zval *class_obj;
	MAKE_STD_ZVAL(class_obj);
	object_init_ex(class_obj, *class_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), class, strlen(class)+1, &class_obj, sizeof(zval *), NULL);

	if (zend_hash_exists(&(*class_ce)->function_table, "__construct", 12)) {
		zval *class_construct_retval;
		CII_CALL_USER_METHOD_EX(&class_obj, "__construct", &class_construct_retval, param_count, params);
		zval_ptr_dtor(&class_construct_retval);
	}

	efree(class_fullname);
	CII_DESTROY_ACTIVE_SYMBOL_TABLE();

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
*
* function &load_class($class, $directory = 'libraries', $param = NULL)
*/
PHP_FUNCTION(cii_load_class)
{
	char *class;
	uint class_len;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &class, &class_len) == FAILURE ){
		WRONG_PARAM_COUNT;
	}
	zval *retval;
	if( retval = cii_load_class(class, class_len, 0, NULL) ){
		zval_ptr_dtor(return_value_ptr);
		(*return_value_ptr) = retval;
		Z_ADDREF_P(*return_value_ptr);
	}	
}
/*
* Is HTTPS?
*/
ZEND_API int cii_is_https(){
	zval **https, **http_x_forwarded_proto, **http_front_end_https;

	zval **server = &PG(http_globals)[TRACK_VARS_SERVER];

	if( zend_hash_find(Z_ARRVAL_PP(server), "HTTPS", 6, (void**)&https) != FAILURE){
		char *strlower = zend_str_tolower_dup(Z_STRVAL_PP(https), Z_STRLEN_PP(https));
		if( strcmp(strlower, "off") ){
			return 1;
		}
	}else if( zend_hash_find(Z_ARRVAL_PP(server), "HTTP_X_FORWARDED_PROTO", 23, (void**)&http_x_forwarded_proto) != FAILURE){
		if( !strcmp(Z_STRVAL_PP(http_x_forwarded_proto), "https") ){
			return 1;
		}
	}else if( zend_hash_find(Z_ARRVAL_PP(server), "HTTP_FRONT_END_HTTPS", 21, (void**)&http_front_end_https) != FAILURE){
		char *strlower = zend_str_tolower_dup(Z_STRVAL_PP(http_front_end_https), Z_STRLEN_PP(http_front_end_https));
		if( strcmp(strlower, "off") ){
			return 1;
		}
	}
	return 0;
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
	RETURN_BOOL(cii_is_https());
}
/**
* Returns the specified config item
*
* @param	string
* @return	mixed
*
* function config_item($item)
*/
PHP_FUNCTION(cii_config_item)
{
	char *item;
	uint item_len;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &item, &item_len) == FAILURE ){
		WRONG_PARAM_COUNT;
	}
	zval *retval;
	if( retval = cii_config_item(item, item_len, NULL, 0) ){
		RETURN_ZVAL(retval, 1, 0);
	}
}
/**
* Error Logging Interface
*
* We use this as a simple mechanism to access the logging
* class and send messages to be logged.
*
* @param	string	the error level: 'error', 'debug' or 'info'
* @param	string	the error message
* @return	void
*
* function log_message($level, $message)
*/
PHP_FUNCTION(cii_log_message)
{
	char *level;
	uint level_len;
	char *message;
	uint message_len;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss" ,&level, &level_len, &message, &message_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	cii_user_write_log(level, level_len, message, message_len);
}


PHP_FUNCTION(cii_run)
{	
	if( !CII_G(apppath) ){
		cii_get_apppath();
	}
	if( !CII_G(classes) ){
		MAKE_STD_ZVAL(CII_G(classes));
		array_init(CII_G(classes));
	}
	if( !CII_G(is_loaded) ){
		MAKE_STD_ZVAL(CII_G(is_loaded));
		array_init(CII_G(is_loaded));
	}
	/*
	* load CII_Benchmark object
	*/
	zval *cii_benchmark_obj;
	MAKE_STD_ZVAL(cii_benchmark_obj);
	object_init_ex(cii_benchmark_obj, cii_benchmark_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "benchmark", 10, &cii_benchmark_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_benchmark_ce->function_table, "__construct", 12)) {
		zval *cii_benchmark_retval;
		CII_CALL_USER_METHOD_EX(&cii_benchmark_obj, "__construct", &cii_benchmark_retval, 0, NULL);
		zval_ptr_dtor(&cii_benchmark_retval);
	}	
	cii_is_loaded("benchmark");
	/*
	*	Start the timer... tick tock tick tock...
	*/
	zval *marker = zend_read_property(cii_benchmark_ce, cii_benchmark_obj, ZEND_STRL("marker"), 1 TSRMLS_CC);

	zval *total_execution_time_start;
	MAKE_STD_ZVAL(total_execution_time_start);
	ZVAL_DOUBLE(total_execution_time_start, cii_microtime());
	zend_hash_update(Z_ARRVAL_P(marker), "total_execution_time_start", 27, &total_execution_time_start, sizeof(zval*), NULL);

	zval *loading_time_base_classes_start;
	MAKE_STD_ZVAL(loading_time_base_classes_start);
	ZVAL_DOUBLE(loading_time_base_classes_start, cii_microtime());
	zend_hash_update(Z_ARRVAL_P(marker), "loading_time:_base_classes_start", 33, &loading_time_base_classes_start, sizeof(zval*), NULL);
	/*
	* load CII_Hooks object
	*/
	zval *cii_hooks_obj;
	MAKE_STD_ZVAL(cii_hooks_obj);
	object_init_ex(cii_hooks_obj, cii_hooks_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "hooks", 6, &cii_hooks_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_hooks_ce->function_table, "__construct", 12)) {
		zval *cii_hooks_retval;
		CII_CALL_USER_METHOD_EX(&cii_hooks_obj, "__construct", &cii_hooks_retval, 0, NULL);
		zval_ptr_dtor(&cii_hooks_retval);
	}	
	cii_is_loaded("hooks");
	/*
	* load CII_Config object
	*/
	zval *cii_config_obj;
	MAKE_STD_ZVAL(cii_config_obj);
	object_init_ex(cii_config_obj, cii_config_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "config", 7, &cii_config_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_config_ce->function_table, "__construct", 12)) {
		zval *cii_config_retval;
		CII_CALL_USER_METHOD_EX(&cii_config_obj, "__construct", &cii_config_retval, 0, NULL);
		zval_ptr_dtor(&cii_config_retval);
	}
	cii_is_loaded("config");
	/*
	* load CII_Log object
	*/
	zval *cii_log_obj;
	MAKE_STD_ZVAL(cii_log_obj);
	object_init_ex(cii_log_obj, cii_log_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "log", 4, &cii_log_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_log_ce->function_table, "__construct", 12)) {
		zval *cii_log_retval;
		CII_CALL_USER_METHOD_EX(&cii_log_obj, "__construct", &cii_log_retval, 0, NULL);
		zval_ptr_dtor(&cii_log_retval);
	}	
	cii_is_loaded("log");
	/*
	* load CII_URI object
	*/
	zval *cii_uri_obj;
	MAKE_STD_ZVAL(cii_uri_obj);
	object_init_ex(cii_uri_obj, cii_uri_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "uri", 4, &cii_uri_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_uri_ce->function_table, "__construct", 12)) {
		zval *cii_uri_retval;
		CII_CALL_USER_METHOD_EX(&cii_uri_obj, "__construct", &cii_uri_retval, 0, NULL);
		zval_ptr_dtor(&cii_uri_retval);
	}	
	cii_is_loaded("uri");
	/*
	* load CII_Router object
	*/
	zval *cii_router_obj;
	MAKE_STD_ZVAL(cii_router_obj);
	object_init_ex(cii_router_obj, cii_router_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "router", 7, &cii_router_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_router_ce->function_table, "__construct", 12)) {
		zval *cii_router_retval;
		CII_CALL_USER_METHOD_EX(&cii_router_obj, "__construct", &cii_router_retval, 0, NULL);
		zval_ptr_dtor(&cii_router_retval);
	}
	cii_is_loaded("router");
	/*
	* load CII_Output object
	*/
	zval *cii_output_obj;
	MAKE_STD_ZVAL(cii_output_obj);
	object_init_ex(cii_output_obj, cii_output_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "output", 7, &cii_output_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_output_ce->function_table, "__construct", 12)) {
		zval *cii_output_retval;
		CII_CALL_USER_METHOD_EX(&cii_output_obj, "__construct", &cii_output_retval, 0, NULL);
		zval_ptr_dtor(&cii_output_retval);
	}
	cii_is_loaded("output");
	/*
	* 	load CII_Input object
	*/
	zval *cii_input_obj;
	MAKE_STD_ZVAL(cii_input_obj);
	object_init_ex(cii_input_obj, cii_input_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "input", 6, &cii_input_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_input_ce->function_table, "__construct", 12)) {
		zval *cii_input_retval;
		CII_CALL_USER_METHOD_EX(&cii_input_obj, "__construct", &cii_input_retval, 0, NULL);
		zval_ptr_dtor(&cii_input_retval);
	}	
	cii_is_loaded("input");
	/*
	* 	load CII_Lang object
	*/
	zval *cii_lang_obj;
	MAKE_STD_ZVAL(cii_lang_obj);
	object_init_ex(cii_lang_obj, cii_lang_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "lang", 5, &cii_lang_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_lang_ce->function_table, "__construct", 12)) {
		zval *cii_lang_retval;
		CII_CALL_USER_METHOD_EX(&cii_lang_obj, "__construct", &cii_lang_retval, 0, NULL);
		zval_ptr_dtor(&cii_lang_retval);
	}	
	cii_is_loaded("lang");
	/*
	* load CII_Loader object
	*/
	zval *cii_loader_obj;
	MAKE_STD_ZVAL(cii_loader_obj);
	object_init_ex(cii_loader_obj, cii_loader_ce);
	zend_hash_update(Z_ARRVAL_P(CII_G(classes)), "loader", 7, &cii_loader_obj, sizeof(zval *), NULL);
	if (zend_hash_exists(&cii_loader_ce->function_table, "__construct", 12)) {
		zval *cii_loader_retval;
		CII_CALL_USER_METHOD_EX(&cii_loader_obj, "__construct", &cii_loader_retval, 0, NULL);
		zval_ptr_dtor(&cii_loader_retval);
	}
	cii_is_loaded("loader");
	/*
	*	Set a mark point for benchmarking
	*/
	zval *loading_time_base_classes_end;
	MAKE_STD_ZVAL(loading_time_base_classes_end);
	ZVAL_DOUBLE(loading_time_base_classes_end, cii_microtime());
	zend_hash_update(Z_ARRVAL_P(marker), "loading_time:_base_classes_end", 31, &loading_time_base_classes_end, sizeof(zval*), NULL);
	/*
	* load is_loaded objects
	*/
	zval *call_class = zend_read_property(cii_router_ce, cii_router_obj, ZEND_STRL("class"), 1 TSRMLS_CC);
	zval *call_method = zend_read_property(cii_router_ce, cii_router_obj, ZEND_STRL("method"), 1 TSRMLS_CC);

	char *file;
	uint file_len;
	file_len = spprintf(&file, 0, "%s%s%s%s", CII_G(apppath), "controllers/", Z_STRVAL_P(call_class), ".php");

	if (zend_hash_exists(&EG(included_files), file, file_len + 1)){
		efree(file);
		RETURN_ZVAL(getThis(), 1, 0);
	}

	CII_ALLOC_ACTIVE_SYMBOL_TABLE();
	cii_loader_import(file, file_len, 0 TSRMLS_CC);
	CII_DESTROY_ACTIVE_SYMBOL_TABLE();
	efree(file);

	zend_class_entry **run_class_ce;
	if( zend_hash_find(EG(class_table), Z_STRVAL_P(call_class), Z_STRLEN_P(call_class)+1, (void**)&run_class_ce) != FAILURE ){
		/*
		* load is_loaded objects
		*/
		zval *run_obj;
		MAKE_STD_ZVAL(run_obj);
		object_init_ex(run_obj, *run_class_ce);
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
				/*
				*	if object already contains property named key. and we use:
				*	zend_update_property(*run_class_ce, run_obj, key, key_len, *exist_object TSRMLS_CC);
				*	to update property,it will add a new property to object instead update the same name property.
				* 	so i use constant string instead variable string
				*	the reason still unknown
				*/
				switch(i){
					case 1:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "benchmark", 10) ){
							zend_update_property(*run_class_ce, run_obj, "benchmark", 9, *exist_object TSRMLS_CC);
						}
						break;
					case 2:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "hooks", 6) ){
							zend_update_property(*run_class_ce, run_obj, "hooks", 5, *exist_object TSRMLS_CC);
						}
						break;	
					case 3:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "config", 7) ){
							zend_update_property(*run_class_ce, run_obj, "config", 6, *exist_object TSRMLS_CC);
						}
						break;
					case 4:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "log", 4) ){
							zend_update_property(*run_class_ce, run_obj, "log", 3, *exist_object TSRMLS_CC);
						}
						break;	
					case 5:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "uri", 4) ){
							zend_update_property(*run_class_ce, run_obj, "uri", 3, *exist_object TSRMLS_CC);
						}
						break;
					case 6:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "router", 7) ){
							zend_update_property(*run_class_ce, run_obj, "router", 6, *exist_object TSRMLS_CC);
						}
						break;
					case 7:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "output", 7) ){
							zend_update_property(*run_class_ce, run_obj, "output", 6, *exist_object TSRMLS_CC);
						}
						break;
					case 8:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "input", 6) ){
							zend_update_property(*run_class_ce, run_obj, "input", 5, *exist_object TSRMLS_CC);
						}
						break;	
					case 9:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "lang", 5) ){
							zend_update_property(*run_class_ce, run_obj, "lang", 4, *exist_object TSRMLS_CC);
						}
						break;
					case 10:
						if( !zend_hash_exists(&(*run_class_ce)->properties_info, "load", 5) ){
							zend_update_property(*run_class_ce, run_obj, "load", 4, *exist_object TSRMLS_CC);
						}
						break;
				}
				i++;
		}
		/*
		*	add get_instance method
		*/
		if( !zend_hash_exists(&(*run_class_ce)->function_table, "get_instance", 13) ){
			zend_function func;
			func.internal_function.type = ZEND_INTERNAL_FUNCTION;
			func.internal_function.function_name = "get_instance";
			func.internal_function.scope = *run_class_ce;
			func.internal_function.fn_flags = ZEND_ACC_PUBLIC | ZEND_ACC_RETURN_REFERENCE;
			func.internal_function.num_args = 0;
			func.internal_function.required_num_args = 0;
			func.internal_function.arg_info = (zend_arg_info*)cii_get_instance_arginfo+1;
			func.internal_function.handler = ZEND_FN(cii_get_instance);
			if( zend_hash_add(&(*run_class_ce)->function_table, "get_instance", 13, &func, sizeof(zend_function), NULL) == FAILURE ){
				php_error(E_WARNING, "add get_instance method failed");
			}
		}
		/*
		*	add to global cii_controller and cii_controller_ce
		*/
		CII_G(cii_controller) = run_obj;
		CII_G(cii_controller_ce) = *run_class_ce;
		/*
		*	cii_controller __construct function
		*/
		cii_write_log(3, "Controller Class Initialized");
		/*
		*   Is there a "pre_controller" hook?
		*/
		cii_call_hook(cii_hooks_ce, cii_hooks_obj, "pre_controller", 15);
		/*
		*	Mark a start point so we can benchmark the controller
		*/
		char *mark_name_start;
		uint mark_start_len;
		mark_start_len = spprintf(&mark_name_start, 0, "%s%s%s%s%s", "controller_execution_time_( ", Z_STRVAL_P(call_class), " / ", Z_STRVAL_P(call_method), " )_start");
		zval *controller_execution_time_start;
		MAKE_STD_ZVAL(controller_execution_time_start);
		ZVAL_DOUBLE(controller_execution_time_start, cii_microtime());
		zend_hash_update(Z_ARRVAL_P(marker), mark_name_start, mark_start_len+1, &controller_execution_time_start, sizeof(zval*), NULL);
		efree(mark_name_start);
		/*
		* 	call run_obj's __construct method
		*/
		if ( zend_hash_exists(&(*run_class_ce)->function_table, "__construct", 12) ){
			zval *run_class_retval;
			CII_CALL_USER_METHOD_EX(&run_obj, "__construct", &run_class_retval, 0, NULL);
			zval_ptr_dtor(&run_class_retval);
		}
		/*
 		*  Is there a "post_controller_constructor" hook?
 		*/
 		cii_call_hook(cii_hooks_ce, cii_hooks_obj, "post_controller_constructor", 28);
		/*
		* 	call run_obj's method
		*/
		if ( zend_hash_exists(&(*run_class_ce)->function_table, Z_STRVAL_P(call_method), Z_STRLEN_P(call_method)+1) ){
			zval *run_method_retval;
			CII_CALL_USER_METHOD_EX(&run_obj, Z_STRVAL_P(call_method), &run_method_retval, 0, NULL);
			zval_ptr_dtor(&run_method_retval);
		}else{
			php_error(E_WARNING, "method does not exist: %s\n", Z_STRVAL_P(call_method));
		}
		/*
		*	Mark a benchmark end point
		*/
		char *mark_name_end;
		uint mark_end_len;
		mark_end_len = spprintf(&mark_name_end, 0, "%s%s%s%s%s", "controller_execution_time_( ", Z_STRVAL_P(call_class), " / ", Z_STRVAL_P(call_method), " )_end");
		zval *controller_execution_time_end;
		MAKE_STD_ZVAL(controller_execution_time_end);
		ZVAL_DOUBLE(controller_execution_time_end, cii_microtime());
		zend_hash_update(Z_ARRVAL_P(marker), mark_name_end, mark_end_len+1, &controller_execution_time_end, sizeof(zval*), NULL);
		efree(mark_name_end);
		/*
		*  Is there a "post_controller" hook?
		*/
		cii_call_hook(cii_hooks_ce, cii_hooks_obj, "post_controller", 16);
	}
	/*
	*	mark total_execution_time_end benchmark
	*/
	/*zval *total_execution_time_end;
	MAKE_STD_ZVAL(total_execution_time_end);
	ZVAL_DOUBLE(total_execution_time_end, cii_microtime());
	zend_hash_update(Z_ARRVAL_P(marker), "total_execution_time_end", 25, &total_execution_time_end, sizeof(zval*), NULL);*/
	/*
	*  Send the final rendered output to the browser
	*/
	char *output_new;
	uint output_new_len;
	char retval;
	zval *output = zend_read_property(cii_output_ce, cii_output_obj, "final_output", 12, 1 TSRMLS_CC);
	retval = cii_display(Z_STRVAL_P(output), Z_STRLEN_P(output), &output_new, &output_new_len TSRMLS_CC);
	PHPWRITE(output_new, output_new_len);
	if( retval ){
		efree(output_new);
	}
	/*
	*  Is there a "post_system" hook?
	*/
	cii_call_hook(cii_hooks_ce, cii_hooks_obj, "post_system", 12);
}
/**
* Stringify attributes for use in HTML tags.
*
* Helper function used to convert a string, array, or object
* of attributes to a string.
*
* @param	mixed	string, array, object
* @param	bool
* @return	string
*
* function _stringify_attributes($attributes, $js = FALSE)
*/
PHP_FUNCTION(cii_stringify_attributes)
{
	zval *attributes;
	char js = 0;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z!|b", &attributes, &js) == FAILURE ){
		WRONG_PARAM_COUNT;
	}
	if( !attributes ){
		return;
	}
	if( Z_TYPE_P(attributes) == IS_STRING ){
		char *retval;
		spprintf(&retval, 0, "%s%s", " ", Z_STRVAL_P(attributes));
		RETURN_STRING(retval, 0);
	}
	if( Z_TYPE_P(attributes) != IS_ARRAY ){
		convert_to_array(attributes);
	}
	HashPosition pos;
	char *atts = "";
	uint atts_len;
	char need_free = 0;
	char *key;
	uint key_len;
	ulong idx;
	zval **value;
	int key_type;
	for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(attributes), &pos);
	    zend_hash_has_more_elements_ex(Z_ARRVAL_P(attributes), &pos) == SUCCESS;
	    zend_hash_move_forward_ex(Z_ARRVAL_P(attributes), &pos)){

		key_type = zend_hash_get_current_key_ex(Z_ARRVAL_P(attributes), &key, &key_len, &idx, 0, &pos);
		if( zend_hash_get_current_data_ex(Z_ARRVAL_P(attributes), (void**)&value, &pos) == FAILURE ){
			continue;
		}
		if( Z_TYPE_PP(value) != IS_STRING ){
			convert_to_string(*value);
		}
		char *new_atts;
		if( key_type == HASH_KEY_IS_STRING ){
			if( !js ){
				atts_len = spprintf(&new_atts, 0, "%s%s%s%s%s%s", atts, " ", key, "=\"", Z_STRVAL_PP(value), "\"");
			}else{
				atts_len = spprintf(&new_atts, 0, "%s%s%s%s%s", atts, key, "=", Z_STRVAL_PP(value), ",");
			}
		}else if( key_type == HASH_KEY_IS_LONG ){
			if( !js ){
				atts_len = spprintf(&new_atts, 0, "%s%s%ld%s%s%s", atts, " ", idx, "=\"", Z_STRVAL_PP(value), "\"");
			}else{
				atts_len = spprintf(&new_atts, 0, "%s%ld%s%s%s", atts, idx, "=", Z_STRVAL_PP(value), ",");
			}
		}
		if( atts && need_free){
			efree(atts);
		}
		atts = new_atts;
		need_free = 1;
	}
	if( *(atts+atts_len-1) == ',' ){
		*(atts+atts_len-1) = '\0';
		atts_len--;
	}
	RETURN_STRINGL(atts, atts_len, 0);
}
/**
* Determines if the current version of PHP is equal to or greater than the supplied value
*
* @param	string
* @return	bool	TRUE if the current version is $version or higher
*
* function is_php($version)
*/
PHP_FUNCTION(cii_is_php)
{
	char *version;
	uint version_len;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &version, &version_len) == FAILURE ){
		WRONG_PARAM_COUNT;
	}
	zval php_version;
	if( !zend_get_constant("PHP_VERSION", 11, &php_version TSRMLS_CC) ){
		RETURN_FALSE;
	}
	int compare = php_version_compare(Z_STRVAL(php_version), version);
	zval_dtor(&php_version);
	RETURN_BOOL(compare != -1);
}
/**
* Is CLI?
*
* Test to see if a request was made from the command line.
*
* @return 	bool
*
* function is_cli()
*/
PHP_FUNCTION(cii_is_cli)
{
	zval php_sapi;
	zval stdin;
	if( zend_get_constant("STDIN", 5, &stdin TSRMLS_CC) ||
		zend_get_constant("PHP_SAPI", 8, &php_sapi TSRMLS_CC) && 
		Z_TYPE(php_sapi) == IS_STRING && 
		!strcmp(Z_STRVAL(php_sapi), "cli") ){
		
		RETURN_TRUE;
	}
	RETURN_FALSE;
}

const zend_function_entry cii_functions[] = {
	PHP_FE(cii_get_instance, cii_get_instance_arginfo)
	PHP_FE(cii_get_config, cii_get_config_arginfo)
	PHP_FE(cii_config_item, NULL)
	PHP_FE(cii_run, NULL)
	PHP_FE(cii_is_https, NULL)
	PHP_FE(cii_load_class, cii_load_class_arginfo)
	PHP_FE(cii_is_loaded, cii_is_loaded_arginfo)
	PHP_FE(cii_log_message, NULL)
	PHP_FE(cii_stringify_attributes, NULL)
	PHP_FE(cii_is_php, NULL)
	PHP_FE(cii_is_cli, NULL)
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