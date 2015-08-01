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


/* If you declare any globals in php_cii.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(cii)
*/

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

/* {{{ php_cii_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_cii_init_globals(zend_cii_globals *cii_globals)
{
	cii_globals->global_value = 0;
	cii_globals->global_string = NULL;
}*/

PHP_MINIT_FUNCTION(cii)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	ZEND_MODULE_STARTUP_N(cii_controller)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(cii_model)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(cii_loader)(INIT_FUNC_ARGS_PASSTHRU);
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

const zend_function_entry cii_functions[] = {
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
