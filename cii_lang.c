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
	zval *is_loaded = zend_read_property(cii_lang_ce, this, ZEND_STRL("is_loaded"), 1 TSRMLS_CC);
	if(Z_TYPE_P(language)!=IS_ARRAY){
		convert_to_array(language);
	}
	if(Z_TYPE_P(is_loaded)!=IS_ARRAY){
		convert_to_array(is_loaded);
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
* public function load($langfile, $idiom = '', $return = FALSE, $add_suffix = TRUE, $alt_path = '')
*/
PHP_METHOD(cii_lang, load)
{
	
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
* public function line($line, $log_errors = TRUE)
*/
PHP_METHOD(cii_lang, line)
{
	
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
	/**
	 * List of loaded language files
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_lang_ce, ZEND_STRL("is_loaded"), ZEND_ACC_PUBLIC TSRMLS_CC);
}