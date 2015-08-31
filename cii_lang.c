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
/*
*	load language file
*/
ZEND_API int cii_load_lang(zend_class_entry *cii_lang_ce, zval *cii_lang_obj, char *file, uint len, zval *langfile, zval *idiom){

	if (zend_hash_exists(&EG(included_files), file, len+1)) {
		return 1;
	}

	CII_ALLOC_ACTIVE_SYMBOL_TABLE();

	cii_loader_import(file, len, 0 TSRMLS_CC);

	zval **lang;
	if( zend_hash_find(EG(active_symbol_table), "lang", 5, (void**)&lang) == FAILURE ){
		if( idiom && Z_STRLEN_P(idiom) ){
			php_error(E_WARNING, "Language file contains no data: language/%s/%s.php", Z_STRVAL_P(idiom), Z_STRVAL_P(langfile));
		}else{
			php_error(E_WARNING, "Language file contains no data: language/%s.php", Z_STRVAL_P(langfile));
		}
		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		return 0;
	}

	zval *is_loaded = zend_read_property(cii_lang_ce, cii_lang_obj, ZEND_STRL("is_loaded"), 1 TSRMLS_CC);
	Z_ADDREF_P(idiom);
	zend_hash_update(Z_ARRVAL_P(is_loaded), Z_STRVAL_P(langfile), Z_STRLEN_P(langfile)+1, &idiom, sizeof(zval *), NULL);

	zval *language = zend_read_property(cii_lang_ce, cii_lang_obj, ZEND_STRL("language"), 1 TSRMLS_CC);
	php_array_merge(Z_ARRVAL_P(language), Z_ARRVAL_P(*lang), 0 TSRMLS_CC);

	CII_DESTROY_ACTIVE_SYMBOL_TABLE();

	return 1;
}
/**
* Load a language file
*
* @param	mixed	$langfile	Language file name
* @param	string	$idiom		Language name (english, etc.)
* @param	bool	$return		Whether to return the loaded array of translations
*
* @return	void|string[]	Array containing translations, if $return is set to TRUE
*
* public function load($langfile, $idiom = '')
*/
PHP_METHOD(cii_lang, load)
{
	zval *langfile;
	zval *idiom = NULL;
	char is_return = 0;
	char *file;
	uint len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z!b", &langfile, &idiom, &is_return) == FAILURE ){
		WRONG_PARAM_COUNT;
	}

	if( !idiom ){
		zval **defaultl;
		if( zend_hash_find(Z_ARRVAL_P(CII_G(configs)), "language", 9, (void**)&defaultl) != FAILURE ){
			idiom = *defaultl;
		}
	}
	
	if( !CII_G(apppath) ){
		cii_get_apppath();
	}

	if( Z_TYPE_P(langfile) == IS_ARRAY ){
		HashPosition pos;
		zval **value;
		for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(langfile), &pos);
		    zend_hash_has_more_elements_ex(Z_ARRVAL_P(langfile), &pos) == SUCCESS;
		    zend_hash_move_forward_ex(Z_ARRVAL_P(langfile), &pos)){

			if(zend_hash_get_current_data_ex(Z_ARRVAL_P(langfile), (void**)&value, &pos) == FAILURE){
				continue;
			}
			if( idiom && Z_STRLEN_P(idiom) ){
				len = spprintf(&file, 0, "%s%s%s%s%s%s", CII_G(apppath), "language/", Z_STRVAL_P(idiom), "/", Z_STRVAL_PP(value), ".php");
			}else{
				len = spprintf(&file, 0, "%s%s%s%s", CII_G(apppath), "language/", Z_STRVAL_PP(value), ".php");
			}
			cii_load_lang(cii_lang_ce, getThis(), file, len, *value, idiom);
			efree(file);
		}
	}else if( Z_TYPE_P(langfile) == IS_STRING ){
		if( idiom && Z_STRLEN_P(idiom) ){
			len = spprintf(&file, 0, "%s%s%s%s%s%s", CII_G(apppath), "language/", Z_STRVAL_P(idiom), "/", Z_STRVAL_P(langfile), ".php");
		}else{
			len = spprintf(&file, 0, "%s%s%s%s", CII_G(apppath), "language/", Z_STRVAL_P(langfile), ".php");
		}
		cii_load_lang(cii_lang_ce, getThis(), file, len, langfile, idiom);
		efree(file);
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
		php_error(E_NOTICE, "Could not find the language line %s", line);
		return;
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
	INIT_CLASS_ENTRY(ce, "CII_Lang", cii_lang_methods);
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