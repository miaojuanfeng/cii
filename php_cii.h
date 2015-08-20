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

#ifndef PHP_CII_H
#define PHP_CII_H

extern zend_module_entry cii_module_entry;
#define phpext_cii_ptr &cii_module_entry

#define PHP_CII_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_CII_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_CII_API __attribute__ ((visibility("default")))
#else
#	define PHP_CII_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif


ZEND_BEGIN_MODULE_GLOBALS(cii)
  zval *classes;
  zval *is_loaded;
	zval *config;
  zval *apppath;
ZEND_END_MODULE_GLOBALS(cii)


/* In every utility function you add that needs to use variables 
   in php_cii_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as CII_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define CII_G(v) TSRMG(cii_globals_id, zend_cii_globals *, v)
#else
#define CII_G(v) (cii_globals.v)
#endif

#define CII_ALLOC_ACTIVE_SYMBOL_TABLE() \
    HashTable *old_active_symbol_table; \
    old_active_symbol_table = EG(active_symbol_table); \
    ALLOC_HASHTABLE(EG(active_symbol_table)); \
    zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0)

#define CII_DESTROY_ACTIVE_SYMBOL_TABLE() \
    zend_hash_destroy(EG(active_symbol_table)); \
    FREE_HASHTABLE(EG(active_symbol_table)); \
    EG(active_symbol_table) = old_active_symbol_table 

#define CII_IF_ISREF_THEN_SEPARATE_ELSE_ADDREF(value) \
    do{ \
        if(PZVAL_IS_REF(*value)){ \
            zval *temp; \
            MAKE_STD_ZVAL(temp); \
            ZVAL_COPY_VALUE(temp,*value); \
            zval_copy_ctor(temp); \
            value = &temp; \
        }else{ \
            Z_ADDREF_P(*value); \
        } \
    }while(0)

#define CII_CALL_USER_FUNCTION_EX(function_table, object_ptr, function_name, retval_ptr, param_count, params) \
    do{ \
        zval *func_name; \
        MAKE_STD_ZVAL(func_name); \
        ZVAL_STRING(func_name, function_name, 1); \
        if( call_user_function_ex(function_table, object_ptr, func_name, retval_ptr, param_count, params, 0, NULL TSRMLS_CC) == FAILURE ){ \
            php_error(E_ERROR, "Call function failed: %s", function_name); \
        } \
        zval_ptr_dtor(&func_name); \
    }while(0)

#define CII_CALL_USER_METHOD_EX(object_ptr, function_name, retval_ptr, param_count, params) \
    do{ \
        zval *func_name; \
        MAKE_STD_ZVAL(func_name); \
        ZVAL_STRING(func_name, function_name, 1); \
        if( call_user_function_ex(NULL, object_ptr, func_name, retval_ptr, param_count, params, 0, NULL TSRMLS_CC) == FAILURE ){ \
            php_error(E_ERROR, "Call method failed: %s", function_name); \
        } \
        zval_ptr_dtor(&func_name); \
    }while(0)    

#endif	/* PHP_CII_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
