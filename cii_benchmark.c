#include "cii_benchmark.h"
#ifdef PHP_WIN32
	#include "win32/time.h"
#elif defined(NETWARE)
	#include <sys/timeval.h>
	#include <sys/time.h>
#else
	#include <sys/time.h>
#endif

#include "ext/standard/php_math.h"

#define MICRO_IN_SEC 1000000.00

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

static double cii_microtime(){
	struct timeval tp = {0};
	if( gettimeofday(&tp, NULL) ){
		return 0;
	}
	return (double)(tp.tv_sec + tp.tv_usec / MICRO_IN_SEC);
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
		RETURN_FALSE;
	}
	
	zval *microtime;
	MAKE_STD_ZVAL(microtime);
	ZVAL_DOUBLE(microtime, cii_microtime());

	zval *marker = zend_read_property(cii_benchmark_ce, getThis(), ZEND_STRL("marker"), 1 TSRMLS_CC);
	zend_hash_update(Z_ARRVAL_P(marker), name, name_len+1, &microtime, sizeof(zval*), NULL);
}
/**
* Elapsed time
*
* Calculates the time difference between two marked points.
*
* If the first parameter is empty this function instead returns the
* {elapsed_time} pseudo-variable. This permits the full system
* execution time to be shown in a template. The output class will
* swap the real value for this variable.
*
* @param	string	$point1		A particular marked point
* @param	string	$point2		A particular marked point
* @param	int	$decimals	Number of decimal places
*
* @return	string	Calculated elapsed time on success,
*			an '{elapsed_string}' if $point1 is empty
*			or an empty string if $point1 is not found.
*
* public function elapsed_time($point1 = '', $point2 = '', $decimals = 4)
*/
PHP_METHOD(cii_benchmark, elapsed_time)
{
	char *point1 = NULL, *point2 = NULL;
	uint point1_len, point2_len;
	long decimals = 4;

	if( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss|l", &point1, &point1_len, &point2, &point2_len, &decimals) == FAILURE ){
		RETURN_EMPTY_STRING();
	}
	if( !point1_len ){
		RETURN_STRING("{elapsed_time}", 1);
	}
	zval *marker = zend_read_property(cii_benchmark_ce, getThis(), ZEND_STRL("marker"), 1 TSRMLS_CC);
	if( !zend_hash_exists(Z_ARRVAL_P(marker), point1, point1_len+1) ){
		RETURN_EMPTY_STRING();
	}
	if( !zend_hash_exists(Z_ARRVAL_P(marker), point2, point2_len+1) ){
		zval *microtime;
		MAKE_STD_ZVAL(microtime);
		ZVAL_DOUBLE(microtime, cii_microtime());
		zend_hash_update(Z_ARRVAL_P(marker), point2, point2_len+1, &microtime, sizeof(zval*), NULL);
	}
	zval **start, **end;
	zend_hash_find(Z_ARRVAL_P(marker), point1, point1_len+1, (void**)&start);
	zend_hash_find(Z_ARRVAL_P(marker), point2, point2_len+1, (void**)&end);
	RETURN_STRING(_php_math_number_format((double)(Z_DVAL_PP(end)-Z_DVAL_PP(start)), decimals, '.', ','), 0);
}

zend_function_entry cii_benchmark_methods[] = {
	PHP_ME(cii_benchmark, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_benchmark, mark, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_benchmark, elapsed_time, NULL, ZEND_ACC_PUBLIC)
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