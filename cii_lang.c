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
	zval *this = getThis();
	zval *language = zend_read_property(cii_lang_ce, this, ZEND_STRL("language"), 1 TSRMLS_CC);
	if(Z_TYPE_P(language)!=IS_ARRAY){
		convert_to_array(language);
	}
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
	char *langfile;
	uint file_len;
	char *idiom;
	uint idiom_len;
	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss", &langfile, &file_len, &idiom, &idiom_len) == FAILURE ){
		return;
	}

	if(!file_len || !idiom_len){
		return;
	}else{
		char *file;
		uint len;
		zval *language;
		HashTable *old_active_symbol_table;
		zval **lang;

		len = spprintf(&file, 0, "%s%s%s%s%s", "/usr/local/httpd/htdocs/cii/language/", idiom, "/", langfile, ".php");

		if (zend_hash_exists(&EG(included_files), file, len + 1)) {
			RETURN_TRUE;
		}

		old_active_symbol_table = EG(active_symbol_table);
		ALLOC_HASHTABLE(EG(active_symbol_table));
		zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);

		if(cii_loader_import(file, len, 0 TSRMLS_CC)){
			if( zend_hash_find(EG(active_symbol_table), "lang", 5, (void**)&lang) == FAILURE ){
				php_error(E_WARNING, "Language file contains no data: language/%s/%s.php", idiom, langfile);
			}

			language = zend_read_property(cii_lang_ce, getThis(), ZEND_STRL("language"), 1 TSRMLS_CC);
			php_array_merge(Z_ARRVAL_P(language), Z_ARRVAL_P(*lang), 0 TSRMLS_CC);
		}

		efree(file);

		zend_hash_destroy(EG(active_symbol_table));
		FREE_HASHTABLE(EG(active_symbol_table));
		EG(active_symbol_table) = old_active_symbol_table;
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
	INIT_CLASS_ENTRY(ce,"cii_lang",cii_lang_methods);
	cii_lang_ce = zend_register_internal_class(&ce TSRMLS_CC);

	/**
	 * List of translations
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_lang_ce, ZEND_STRL("language"), ZEND_ACC_PUBLIC TSRMLS_CC);
}