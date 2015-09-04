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
	*	init cii_hooks::objects
	*/
	zval *objects;
	MAKE_STD_ZVAL(objects);
	array_init(objects);
	zend_update_property(cii_hooks_ce, getThis(), ZEND_STRL("objects"), objects TSRMLS_CC);
	zval_ptr_dtor(&objects);
	/*
	*	output log
	*/
	cii_write_log(3, "Hooks Class Initialized");
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
ZEND_API int cii_run_hook(zend_class_entry *cii_hooks_ce, zval *cii_hooks_obj, HashTable *data)
{
	if( data->nNumOfElements ){
		zval **class    = NULL;
		zval **function = NULL;
		zval **filename = NULL;
		zval **filepath = NULL;
		zval **initparams   = NULL;
		zval **funcparams   = NULL;
		/*
		*	get hook's data
		*/
		zend_hash_find(data, "class", 	   6, (void**)&class);
		zend_hash_find(data, "function",   9, (void**)&function);
		zend_hash_find(data, "filename",   9, (void**)&filename);
		zend_hash_find(data, "filepath",   9, (void**)&filepath);
		zend_hash_find(data, "initparams", 11, (void**)&initparams);
		zend_hash_find(data, "funcparams", 11, (void**)&funcparams);
		/*
		*	check hook's format
		*/
		if( class && Z_TYPE_PP(class) != IS_STRING ||
			function && Z_TYPE_PP(function) != IS_STRING ||
			filename && Z_TYPE_PP(filename) != IS_STRING ||
			filepath && Z_TYPE_PP(filepath) != IS_STRING ){
			php_error(E_WARNING, "class value or function value or filename value or filepath value should be string");
			return 0;
		}
		if( initparams && Z_TYPE_PP(initparams) != IS_ARRAY ||
		 	funcparams && Z_TYPE_PP(funcparams) != IS_ARRAY ){
			php_error(E_WARNING, "initparams value or funcparams value should be array");
			return 0;
		}
		/*
		*	check hook's data
		*/
		if( class ){ if( !function ){ return 0; } }
		if( !function ){ return 0; }
		char *filename_call;
		char *filepath_call;
		if( filename ){ filename_call = Z_STRVAL_PP(filename); }else{ return 0; }
		if( filepath ){ filepath_call = Z_STRVAL_PP(filepath); }else{ filepath_call = ""; }
		/*
		*	init initparams and funcparams
		*/
		zval ***initparams_call = NULL, ***initparam_p;
		uint init_arg_num = 0;
		if( initparams ){
			init_arg_num = Z_ARRVAL_PP(initparams)->nNumOfElements;
			initparams_call = initparam_p = (zval***)emalloc(sizeof(zval**) * init_arg_num);
			HashPosition pos;
			zval **value;
			for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(initparams), &pos);
		    	zend_hash_has_more_elements_ex(Z_ARRVAL_PP(initparams), &pos) == SUCCESS;
		    	zend_hash_move_forward_ex(Z_ARRVAL_PP(initparams), &pos)){
				if( zend_hash_get_current_data_ex(Z_ARRVAL_PP(initparams), (void**)&value, &pos) == FAILURE ){
					continue;
				}
				*initparam_p++ = value;
			}
		}
		zval ***funcparams_call = NULL, ***funcparam_p;
		uint func_arg_num = 0;
		if( funcparams ){
			func_arg_num = Z_ARRVAL_PP(funcparams)->nNumOfElements;
			funcparams_call = funcparam_p = (zval***)emalloc(sizeof(zval**) * func_arg_num);
			HashPosition pos;
			zval **value;
			for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_PP(funcparams), &pos);
		    	zend_hash_has_more_elements_ex(Z_ARRVAL_PP(funcparams), &pos) == SUCCESS;
		    	zend_hash_move_forward_ex(Z_ARRVAL_PP(funcparams), &pos)){
				if( zend_hash_get_current_data_ex(Z_ARRVAL_PP(funcparams), (void**)&value, &pos) == FAILURE ){
					continue;
				}
				*funcparam_p++ = value;
			}
		}
		/*
		*	If we have stored the object
		*/
		if( class ){
			zval *objects = zend_read_property(cii_hooks_ce, cii_hooks_obj, "objects", 7, 1 TSRMLS_CC);
			zval **hook_stored_obj;
			if( zend_hash_find(Z_ARRVAL_P(objects), Z_STRVAL_PP(class), Z_STRLEN_PP(class)+1, (void**)&hook_stored_obj) != FAILURE ){
				char *function_lower = zend_str_tolower_dup(Z_STRVAL_PP(function), Z_STRLEN_PP(function));
				if (zend_hash_exists(&zend_get_class_entry(*hook_stored_obj)->function_table, function_lower, Z_STRLEN_PP(function)+1)) {
					zval *hook_stored_retval;
					CII_CALL_USER_METHOD_EX(hook_stored_obj, function_lower, &hook_stored_retval, func_arg_num, funcparams_call);
					zval_ptr_dtor(&hook_stored_retval);
					if( initparams_call ){
						efree(initparams_call);
					}
					if( funcparams_call ){
						efree(funcparams_call);
					}
					efree(function_lower);
					return 1;
				}else{
					php_error(E_WARNING, "Unable to load the function you specified: %s", Z_STRVAL_PP(function));
					if( initparams_call ){
						efree(initparams_call);
					}
					if( funcparams_call ){
						efree(funcparams_call);
					}
					efree(function_lower);
					return 0;
				}
			}
		}	
		/*
		*	load hook's data
		*/
		char *file;
		uint file_len;
		if( !CII_G(apppath) ){
			cii_get_apppath();
		}
		file_len = spprintf(&file, 0, "%s%s%s%s", CII_G(apppath), "hooks/", filepath_call, filename_call);
		CII_ALLOC_ACTIVE_SYMBOL_TABLE();
		cii_loader_import(file, file_len, 0 TSRMLS_CC);
		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		efree(file);
		/*
		*	call hook's function
		*/
		if( class ){
			/*
			*	hook is a class
			*/
			zend_class_entry **hook_call_ce;
			char *class_lower = zend_str_tolower_dup(Z_STRVAL_PP(class), Z_STRLEN_PP(class));
			if( zend_hash_find(CG(class_table), class_lower, Z_STRLEN_PP(class)+1, (void**)&hook_call_ce) == SUCCESS ){
				zval *hook_call_obj;
				MAKE_STD_ZVAL(hook_call_obj);
				object_init_ex(hook_call_obj, *hook_call_ce);
				if (zend_hash_exists(&(*hook_call_ce)->function_table, "__construct", 12)) {
					zval *hook_call_retval;
					CII_CALL_USER_METHOD_EX(&hook_call_obj, "__construct", &hook_call_retval, init_arg_num, initparams_call);
					zval_ptr_dtor(&hook_call_retval);
				}
				char *function_lower = zend_str_tolower_dup(Z_STRVAL_PP(function), Z_STRLEN_PP(function));
				if (zend_hash_exists(&(*hook_call_ce)->function_table, function_lower, Z_STRLEN_PP(function)+1)) {
					zval *hook_call_retval;
					CII_CALL_USER_METHOD_EX(&hook_call_obj, function_lower, &hook_call_retval, func_arg_num, funcparams_call);
					zval_ptr_dtor(&hook_call_retval);
				}else{
					php_error(E_WARNING, "Unable to load the function you specified: %s", Z_STRVAL_PP(function));
					efree(class_lower);
					if( initparams_call ){
						efree(initparams_call);
					}
					if( funcparams_call ){
						efree(funcparams_call);
					}
					efree(function_lower);
					zval_ptr_dtor(&hook_call_obj);
					return 0;
				}
				/*
				*	add to objects
				*/
				zval *objects = zend_read_property(cii_hooks_ce, cii_hooks_obj, "objects", 7, 1 TSRMLS_CC);
				Z_ADDREF_P(hook_call_obj);
				zend_hash_update(Z_ARRVAL_P(objects), Z_STRVAL_PP(class), Z_STRLEN_PP(class)+1, &hook_call_obj, sizeof(zval *), NULL);
				/*
				*	free used memory
				*/
				efree(function_lower);
				zval_ptr_dtor(&hook_call_obj);
			}else{
				php_error(E_WARNING, "Unable to load the class you specified: %s", Z_STRVAL_PP(class));
				efree(class_lower);
				if( initparams_call ){
					efree(initparams_call);
				}
				if( funcparams_call ){
					efree(funcparams_call);
				}
				return 0;
			}
			efree(class_lower);
		}else{
			/*
			*	hook is a function
			*/
			char *function_lower = zend_str_tolower_dup(Z_STRVAL_PP(function), Z_STRLEN_PP(function));
			if (zend_hash_exists(EG(function_table), function_lower, Z_STRLEN_PP(function)+1)) {
				zval *hook_call_retval;
				CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, function_lower, &hook_call_retval, func_arg_num, funcparams_call);
				zval_ptr_dtor(&hook_call_retval);
			}else{
				php_error(E_WARNING, "Unable to load the function you specified: %s", Z_STRVAL_PP(function));
				if( initparams_call ){
					efree(initparams_call);
				}
				if( funcparams_call ){
					efree(funcparams_call);
				}
				efree(function_lower);
				return 0;
			}
			efree(function_lower);
		}	
		/*
		*	free used memory
		*/
		if( initparams_call ){
			efree(initparams_call);
		}
		if( funcparams_call ){
			efree(funcparams_call);
		}
		return 1;
	}
	return 0;
}
/**
* Call Hook
*
* Runs a particular hook. Called by cii.c.
*
* @param	char	which	Hook name
* @return	bool	TRUE on success or FALSE on failure
*/
ZEND_API int cii_call_hook(zend_class_entry *cii_hooks_ce, zval *cii_hooks_obj, char *which, uint which_len)
{
	zval *enabled = zend_read_property(cii_hooks_ce, cii_hooks_obj, ZEND_STRL("enabled"), 1 TSRMLS_CC);
	if( Z_TYPE_P(enabled) != IS_BOOL || Z_BVAL_P(enabled) == 0 ){
		return 0;
	}
	zval *hooks = zend_read_property(cii_hooks_ce, cii_hooks_obj, ZEND_STRL("hooks"), 1 TSRMLS_CC);
	zval **call_hook;
	if( zend_hash_find(Z_ARRVAL_P(hooks), which, which_len, (void**)&call_hook) == FAILURE ){
		return 0;
	}
	/*
	*	call hook function
	*/
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
			if( !cii_run_hook(cii_hooks_ce, cii_hooks_obj, Z_ARRVAL_PP(value)) ){
				return 0;
			}
		}
		return 1;
	}else{
		return cii_run_hook(cii_hooks_ce, cii_hooks_obj, Z_ARRVAL_PP(call_hook));
	}
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
	RETURN_BOOL(cii_call_hook(cii_hooks_ce, getThis(), which, which_len+1));
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
	zend_declare_property_null(cii_hooks_ce, ZEND_STRL("objects"), ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * In progress flag
	 *
	 * Determines whether hook is in progress, used to prevent infinte loops
	 *
	 * @var	bool
	 */
	zend_declare_property_bool(cii_hooks_ce, ZEND_STRL("_in_progress"), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
}