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
/*
*	display
*/
ZEND_API int cii_display(char *output, uint output_len, char **output_new, uint *output_new_len TSRMLS_DC){
	char *p = NULL;
	char *free_output_new = NULL;
	char retval = 0;
	*output_new = output;
	*output_new_len = output_len;
	/*
	*	replace {elapsed_time}
	*/
	do{
		if( p = strstr(*output_new, "{elapsed_time}") ){
			p[13] = '\0';
			p[0]  = '\0';
			if( retval ){
				free_output_new = *output_new;
			}
			*output_new_len = spprintf(output_new, 0, "%s%s%s", *output_new, "0.0012", &p[14]);
			if( free_output_new ){
				efree(free_output_new);
				free_output_new = NULL;
			}
			retval = 1;
		}
	}while(p);
	/*
	*	replace {memory_usage}
	*/
	do{
		if( p = strstr(*output_new, "{memory_usage}") ){
			p[13] = '\0';
			p[0]  = '\0';
			if( retval ){
				free_output_new = *output_new;
			}
			char *memory = _php_math_number_format((double)zend_memory_usage(0 TSRMLS_DC)/1024/1024, 2, '.', ',');
			*output_new_len = spprintf(output_new, 0, "%s%s%s%s", *output_new, memory, "MB", &p[14]);
			efree(memory);
			if( free_output_new ){
				efree(free_output_new);
				free_output_new = NULL;
			}
			retval = 1;
		}
	}while(p);
	/*
	*	return state. 1 means replaced or 0 means not replaced
	*/
	return retval;
}
/**
* Display Output
*
* Processes and sends finalized output data to the browser along
* with any server headers and profile data. It also stops benchmark
* timers so the page rendering speed and memory usage can be shown.
*
* Note: All "view" data is automatically put into $this->final_output
*	 by controller class.
*
* @uses	CI_Output::$final_output
* @param	string	$output	Output data override
* @return	void
*
* public function display($output = '')
*/
PHP_METHOD(cii_output, display)
{
	zval *output = NULL;
	char *output_new;
	uint output_new_len;
	char retval;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z!" ,&output) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if( output && Z_TYPE_P(output) != IS_STRING ){
		convert_to_string(output);
	}

	if( output && Z_TYPE_P(output) == IS_STRING ){
		retval = cii_display(Z_STRVAL_P(output), Z_STRLEN_P(output), &output_new, &output_new_len TSRMLS_CC);
	}else{
		zval *output = zend_read_property(cii_output_ce, getThis(), "final_output", 12, 1 TSRMLS_CC);
		retval = cii_display(Z_STRVAL_P(output), Z_STRLEN_P(output), &output_new, &output_new_len TSRMLS_CC);
	}
	PHPWRITE(output_new, output_new_len);
	if( retval ){
		efree(output_new);
	}
}

zend_function_entry cii_output_methods[] = {
	PHP_ME(cii_output, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_output, append_output, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_output, display, NULL, ZEND_ACC_PUBLIC)
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