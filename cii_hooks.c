#include "cii_hooks.h"

zend_class_entry *cii_hooks_ce;

/**
* Class constructor
*
* @return	void
*
* public function __construct()
*/
PHP_METHOD(cii_hooks, __construct)
{
	/*
	*	init cii_hooks::hooks
	*/
	zval *hooks;
	MAKE_STD_ZVAL(hooks);
	array_init(hooks);
	zend_update_property(cii_hooks_ce, getThis(), ZEND_STRL("hooks"), hooks TSRMLS_CC);
	zval_ptr_dtor(&hooks);
	/*
	*	init cii_hooks::_objects
	*/
	zval *_objects;
	MAKE_STD_ZVAL(_objects);
	array_init(_objects);
	zend_update_property(cii_hooks_ce, getThis(), ZEND_STRL("_objects"), _objects TSRMLS_CC);
	zval_ptr_dtor(&_objects);
	/*
	*	output log
	*/
	php_printf("Info: Hooks Class Initialized\n");
	/*
	*	init cii_hooks::hooks
	*/
	zval *config = cii_get_config();
	zval **enable_hooks;
	if( zend_hash_find(Z_ARRVAL_P(config), "enable_hooks", 13, (void**)&enable_hooks) == FAILURE ||
		Z_TYPE_PP(enable_hooks) != IS_BOOL || Z_BVAL_PP(enable_hooks) == 0){
		return;
	}

	char *file;
	uint file_len;
	file_len = spprintf(&file, 0, "%s%s", CII_G(apppath), "configs/hooks.php");

	CII_ALLOC_ACTIVE_SYMBOL_TABLE();
	cii_loader_import(file, file_len, 0 TSRMLS_CC);
	zval **hook;
	if( zend_hash_find(EG(active_symbol_table), "hook", 5, (void**)&hook) == FAILURE || 
		Z_TYPE_PP(hook) != IS_ARRAY ){
		php_error(E_WARNING, "Your hooks file does not appear to be formatted correctly.");	
	}else{
		zend_update_property(cii_hooks_ce, getThis(), ZEND_STRL("hooks"), *hook TSRMLS_CC);
	}
	CII_DESTROY_ACTIVE_SYMBOL_TABLE();
	efree(file);
	/*
	*	init cii_hooks::enabled
	*/
	zval *enabled;
	MAKE_STD_ZVAL(enabled);
	ZVAL_BOOL(enabled, 1);
	zend_update_property(cii_hooks_ce, getThis(), ZEND_STRL("enabled"), enabled TSRMLS_CC);
	zval_ptr_dtor(&enabled);
}
/**
* Run Hook
*
* Runs a particular hook
*
* @param	array	$data	Hook details
* @return	bool	TRUE on success or FALSE on failure
*/
ZEND_API int cii_run_hook(HashTable *data)
{
	if( data->nNumOfElements ){
		zval **class    = NULL;
		zval **function = NULL;
		zval **filename = NULL;
		zval **filepath = NULL;
		zval **params   = NULL;

		zend_hash_find(data, "class", 	 6, (void**)&class);
		zend_hash_find(data, "function", 9, (void**)&function);
		zend_hash_find(data, "filename", 9, (void**)&filename);
		zend_hash_find(data, "filepath", 9, (void**)&filepath);
		zend_hash_find(data, "params", 	 7, (void**)&params);

		if( class 	 && Z_TYPE_PP(class)    == IS_STRING ) php_printf("class: %s\n", Z_STRVAL_PP(class));
		if( function && Z_TYPE_PP(function) == IS_STRING ) php_printf("function: %s\n", Z_STRVAL_PP(function));
		if( filename && Z_TYPE_PP(filename) == IS_STRING ) php_printf("filename: %s\n", Z_STRVAL_PP(filename));
		if( filepath && Z_TYPE_PP(filepath) == IS_STRING ) php_printf("filepath: %s\n", Z_STRVAL_PP(filepath));
		if( params   && Z_TYPE_PP(params)   == IS_ARRAY  ) php_printf("params: %d\n", Z_ARRVAL_PP(params)->nNumOfElements);

		return 1;
	}
	return 0;
}
/**
* Call Hook
*
* Calls a particular hook. Called by CodeIgniter.php.
*
* @uses	CI_Hooks::_run_hook()
*
* @param	string	$which	Hook name
* @return	bool	TRUE on success or FALSE on failure
*
* public function call_hook($which = '')
*/
PHP_METHOD(cii_hooks, call_hook)
{
	char *which = "";
	uint which_len = 1;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s!", &which, &which_len) == FAILURE ){
		WRONG_PARAM_COUNT;
	}
	zval *enabled = zend_read_property(cii_hooks_ce, getThis(), ZEND_STRL("enabled"), 1 TSRMLS_CC);
	if( Z_TYPE_P(enabled) != IS_BOOL || Z_BVAL_P(enabled) == 0 ){
		RETURN_FALSE;
	}
	zval *hooks = zend_read_property(cii_hooks_ce, getThis(), ZEND_STRL("hooks"), 1 TSRMLS_CC);;
	zval **call_hook;
	if( zend_hash_find(Z_ARRVAL_P(hooks), which, which_len, (void**)&call_hook) == FAILURE ){
		RETURN_FALSE;
	}
	/*
	*	call hook function
	*/
	//php_printf("type: %d\n", Z_TYPE_PP((zval**)Z_ARRVAL_PP(call_hook)->pListHead->pData));
	//php_printf("n: %d\n", Z_ARRVAL_PP(call_hook)->nNumOfElements);
	if( Z_ARRVAL_PP(call_hook)->pListHead && Z_TYPE_PP((zval**)Z_ARRVAL_PP(call_hook)->pListHead->pData) == IS_ARRAY ){
		HashPosition pos;
		zval **value;
		for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(call_hook), &pos);
		    zend_hash_has_more_elements_ex(Z_ARRVAL_PP(call_hook), &pos) == SUCCESS;
		    zend_hash_move_forward_ex(Z_ARRVAL_PP(call_hook), &pos)){
			if( zend_hash_get_current_data_ex(Z_ARRVAL_PP(call_hook), (void**)&value, &pos) == FAILURE ||
				Z_TYPE_PP(value) != IS_ARRAY ){
				continue;
			}
			if( !cii_run_hook(Z_ARRVAL_PP(value)) ){
				RETURN_BOOL(0);
			}
		}
		RETURN_BOOL(1);
	}else{
		RETURN_BOOL(cii_run_hook(Z_ARRVAL_PP(call_hook)));
	}
}

zend_function_entry cii_hooks_methods[] = {
	PHP_ME(cii_hooks, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_hooks, call_hook, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_hooks)
{
	/**
	* Hooks Class
	*
	* Provides a mechanism to extend the base system without hacking.
	*/
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Hooks", cii_hooks_methods);
	cii_hooks_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * Determines whether hooks are enabled
	 *
	 * @var	bool
	 */
	zend_declare_property_bool(cii_hooks_ce, ZEND_STRL("enabled"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * List of all hooks set in config/hooks.php
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_hooks_ce, ZEND_STRL("hooks"), ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * Array with class objects to use hooks methods
	 *
	 * @var array
	 */
	zend_declare_property_null(cii_hooks_ce, ZEND_STRL("_objects"), ZEND_ACC_PROTECTED TSRMLS_CC);
	/**
	 * In progress flag
	 *
	 * Determines whether hook is in progress, used to prevent infinte loops
	 *
	 * @var	bool
	 */
	zend_declare_property_bool(cii_hooks_ce, ZEND_STRL("_in_progress"), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
}