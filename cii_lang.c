#include "cii_lang.h"

zend_class_entry *cii_lang_ce;
/**
* Class constructor
*
* @return	void
*
* public function __construct()
*/
PHP_METHOD(cii_lang, __construct)
{
	/*
	*	init cii_lang::language
	*/
	zval *language;
	MAKE_STD_ZVAL(language);
	array_init(language);
	zend_update_property(cii_lang_ce, getThis(), ZEND_STRL("language"), language TSRMLS_CC);
	zval_ptr_dtor(&language);
	/*
	*	init cii_lang::is_loaded
	*/
	zval *is_loaded;
	MAKE_STD_ZVAL(is_loaded);
	array_init(is_loaded);
	zend_update_property(cii_lang_ce, getThis(), ZEND_STRL("is_loaded"), is_loaded TSRMLS_CC);
	zval_ptr_dtor(&is_loaded);
	/*
	*	output log
	*/
	php_printf("Info: Language Class Initialized\n");
}
/**
* Load a language file
*
* @param	mixed	$langfile	Language file name
* @param	string	$idiom		Language name (english, etc.)
* @param	bool	$return		Whether to return the loaded array of translations
* @param 	bool	$add_suffix	Whether to add suffix to $langfile
* @param 	string	$alt_path	Alternative path to look for the language file
*
* @return	void|string[]	Array containing translations, if $return is set to TRUE
*
* public function load($langfile, $idiom = '')
*/
PHP_METHOD(cii_lang, load)
{
	char *langfile, *idiom;
	uint file_len = 0, idiom_len = 0;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &langfile, &file_len, &idiom, &idiom_len) == FAILURE ){
		return;
	}

	if( !file_len ){
		return;
	}else{
		char *file;
		uint len;
		char need_free_idiom = 0;

		if( !idiom_len ){
			zval *config;
			CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "cii_get_config", &config, 0, NULL);
			zval_ptr_dtor(&config);
			zval **cfg_lang;
			if( zend_hash_find(Z_ARRVAL_P(CII_G(config)), "language", 9, (void**)&cfg_lang) == FAILURE ){
				idiom = estrndup("english", 8);
				idiom_len = 8;
			}else{
				idiom = estrndup(Z_STRVAL_PP(cfg_lang), Z_STRLEN_PP(cfg_lang));
				idiom_len = Z_STRLEN_PP(cfg_lang);
			}
			need_free_idiom = 1;
		}

		len = spprintf(&file, 0, "%s%s%s%s%s%s", Z_STRVAL_P(CII_G(apppath)), "language/", idiom, "/", langfile, ".php");

		if (zend_hash_exists(&EG(included_files), file, len+1)) {
			efree(file);
			RETURN_TRUE;
		}

		CII_ALLOC_ACTIVE_SYMBOL_TABLE();

		if(cii_loader_import(file, len, 0 TSRMLS_CC)){
			zval **lang;

			if( zend_hash_find(EG(active_symbol_table), "lang", 5, (void**)&lang) == FAILURE ){
				php_error(E_WARNING, "Language file contains no data: language/%s/%s.php", idiom, langfile);
			}

			zval *z_idiom;
			MAKE_STD_ZVAL(z_idiom);
			ZVAL_STRING(z_idiom, idiom, idiom_len+1);
			zval *is_loaded = zend_read_property(cii_lang_ce, getThis(), ZEND_STRL("is_loaded"), 1 TSRMLS_CC);
			zend_hash_update(Z_ARRVAL_P(is_loaded), langfile, file_len+1, &z_idiom, sizeof(zval *), NULL);

			zval *language = zend_read_property(cii_lang_ce, getThis(), ZEND_STRL("language"), 1 TSRMLS_CC);
			php_array_merge(Z_ARRVAL_P(language), Z_ARRVAL_P(*lang), 0 TSRMLS_CC);
		}

		CII_DESTROY_ACTIVE_SYMBOL_TABLE();

		efree(file);
		if(need_free_idiom){
			efree(idiom);
		}
	}
	RETURN_TRUE;
}
/**
* Language line
*
* Fetches a single line of text from the language array
*
* @param	string	$line		Language line key
* @param	bool	$log_errors	Whether to log an error message if the line is not found
* @return	string	Translation
* 
* public function line($line)
*/
PHP_METHOD(cii_lang, line)
{
	char *line;
	uint line_len;
	zval **value;
	zval *language;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &line, &line_len) == FAILURE ){
		return;
	}
	language = zend_read_property(cii_lang_ce, getThis(), ZEND_STRL("language"), 1 TSRMLS_CC);
	if( zend_hash_find(Z_ARRVAL_P(language), line, line_len+1, (void**)&value) == FAILURE ){
		php_error(E_WARNING, "Could not find the language line %s", line);
	}
	RETURN_ZVAL(*value, 1, 0);
}

zend_function_entry cii_lang_methods[] = {
	PHP_ME(cii_lang, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_lang, load, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_lang, line, NULL, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

PHP_MINIT_FUNCTION(cii_lang){
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"CII_Lang",cii_lang_methods);
	cii_lang_ce = zend_register_internal_class(&ce TSRMLS_CC);

	/**
	 * List of translations
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_lang_ce, ZEND_STRL("language"), ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * List of loaded language files
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_lang_ce, ZEND_STRL("is_loaded"), ZEND_ACC_PUBLIC TSRMLS_CC);
}