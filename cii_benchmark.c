#include "cii_benchmark.h"

zend_class_entry *cii_benchmark_ce;

PHP_METHOD(cii_benchmark, __construct)
{
	/*
	*	init cii_benchmark::marker
	*/
	zval *marker;
	MAKE_STD_ZVAL(marker);
	array_init(marker);
	zend_update_property(cii_uri_ce, getThis(), ZEND_STRL("marker"), marker TSRMLS_CC);
	zval_ptr_dtor(&marker);
	/*
	*	output log
	*/
	php_printf("Info: Benchmark Class Initialized\n");
}
/**
* 	Set a benchmark marker
*
* 	Multiple calls to this function can be made so that several
* 	execution points can be timed.
*
* 	@param	string	$name	Marker name
* 	@return	void
*
*	public function mark($name)
*/
PHP_METHOD(cii_benchmark, mark)
{
	char *name;
	uint name_len;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE ){
		return;
	}

	zval *marker = zend_read_property(cii_benchmark_ce, getThis(), ZEND_STRL("marker"), 1 TSRMLS_CC);

	zval *microtime_retval;
	zval *microtime_para1;
	zval **microtime_paras[1];
	MAKE_STD_ZVAL(microtime_para1);
	ZVAL_BOOL(microtime_para1, 1);
	microtime_paras[0] = &microtime_para1;
	CII_CALL_USER_FUNCTION_EX(EG(function_table), NULL, "microtime", &microtime_retval, 1, microtime_paras);
	zval_ptr_dtor(&microtime_para1);

	zend_hash_update(Z_ARRVAL_P(marker), name, name_len+1, &microtime_retval, sizeof(zval *), NULL);
}

zend_function_entry cii_benchmark_methods[] = {
	PHP_ME(cii_benchmark, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_benchmark, mark, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_benchmark)
{
	/**
	 * Benchmark Class
	 *
	 * This class enables you to mark points and calculate the time difference
	 * between them. Memory consumption can also be displayed.
	 */
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Benchmark", cii_benchmark_methods);
	cii_benchmark_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * List of all benchmark markers
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_benchmark_ce, ZEND_STRL("marker"), ZEND_ACC_PUBLIC TSRMLS_CC);
}