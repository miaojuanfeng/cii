#include "cii_output.h"

zend_class_entry *cii_output_ce;

/**
* Class constructor
*
* Determines whether zLib output compression will be used.
*
* @return	void
*
* public function __construct()
*/
PHP_METHOD(cii_output, __construct)
{
	/*
	*	output log
	*/
	php_printf("Info: Output Class Initialized\n");
}
/*
*	append_output
*/
ZEND_API void cii_append_output(zend_class_entry *cii_output_ce, zval *cii_output_obj, char *output_str){
	zval *final_output = zend_read_property(cii_output_ce, cii_output_obj, "final_output", 12, 1 TSRMLS_CC);
	
	char *output_new;
	spprintf(&output_new, 0, "%s%s", Z_STRVAL_P(final_output), output_str);

	zval *new_final_output;
	MAKE_STD_ZVAL(new_final_output);
	ZVAL_STRING(new_final_output, output_new, 0);
	//php_printf("Z_STRVAL_P: %s\n", Z_STRVAL_P(new_final_output));
	//php_printf("Z_STRLEN_P: %d\n", Z_STRLEN_P(new_final_output));
	//php_printf("output_new: %s\n", output_new);
	zend_update_property(cii_output_ce, cii_output_obj, "final_output", 12, new_final_output TSRMLS_CC);

	zval_ptr_dtor(&new_final_output);
}
/**
* Append Output
*
* Appends data onto the output string.
*
* @param	string	$output	Data to append
* @return	CI_Output
*
* public function append_output($output)
*/
PHP_METHOD(cii_output, append_output)
{
	char *output = NULL;
	uint output_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s" ,&output, &output_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	if( output && output_len ){
		cii_append_output(cii_output_ce, getThis(), output);
	}
	if( return_value_used){
		RETURN_ZVAL(getThis(), 1, 0);
	}
}

zend_function_entry cii_output_methods[] = {
	PHP_ME(cii_output, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_output, append_output, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_output)
{
	/**
	 * Output Class
	 *
	 * Responsible for sending final output to the browser.
	 */
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Output", cii_output_methods);
	cii_output_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * Final output string
	 *
	 * @var	string
	 */
	zend_declare_property_string(cii_output_ce, ZEND_STRL("final_output"), "", ZEND_ACC_PUBLIC TSRMLS_CC);
}