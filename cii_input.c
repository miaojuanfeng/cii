#include "cii_input.h"

zend_class_entry *cii_input_ce;

/**
 * Class constructor
 *
 * @return	void
 *
 * public function __construct()
 */
PHP_METHOD(cii_input, __construct)
{
	/*
	*	output log
	*/
	cii_write_log(3, "Input Class Initialized");
}
/**
* cii_input_get
*/
ZEND_API zval* cii_input_get(zval *index)
{
	zval *get = PG(http_globals)[TRACK_VARS_GET];
	if( index ){
		zval **value;
		if( Z_TYPE_P(index) == IS_STRING && zend_hash_find(Z_ARRVAL_P(get), Z_STRVAL_P(index), Z_STRLEN_P(index)+1, (void**)&value) != FAILURE ){
			return *value;
		}else if( Z_TYPE_P(index) == IS_LONG && zend_hash_index_find(Z_ARRVAL_P(get), Z_LVAL_P(index), (void**)&value) != FAILURE ){
			return *value;
		}else{
			return NULL;
		}
	}
	return get;
}
/**
* Fetch an item from the GET array
*
* @param	mixed	$index		Index for item to be fetched from $_GET
* @param	bool	$xss_clean	Whether to apply XSS filtering
* @return	mixed
*
* public function get($index = NULL, $xss_clean = NULL)
*/
PHP_METHOD(cii_input, get)
{
	if( return_value_used ){
		zval *index = NULL;
		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z!", &index) == FAILURE){
			WRONG_PARAM_COUNT;
		}
		zval *retval;
		if( retval = cii_input_get(index) ){
			RETURN_ZVAL(retval, 1, 0);
		}
	}
}
/**
* cii_input_post
*/
ZEND_API zval* cii_input_post(zval *index)
{
	zval *post = PG(http_globals)[TRACK_VARS_POST];
	if( index ){
		zval **value;
		if( Z_TYPE_P(index) == IS_STRING && zend_hash_find(Z_ARRVAL_P(post), Z_STRVAL_P(index), Z_STRLEN_P(index)+1, (void**)&value) != FAILURE ){
			return *value;
		}else if( Z_TYPE_P(index) == IS_LONG && zend_hash_index_find(Z_ARRVAL_P(post), Z_LVAL_P(index), (void**)&value) != FAILURE ){
			return *value;
		}else{
			return NULL;
		}
	}
	return post;
}
/**
* Fetch an item from the POST array
*
* @param	mixed	$index		Index for item to be fetched from $_POST
* @param	bool	$xss_clean	Whether to apply XSS filtering
* @return	mixed
*
* public function post($index = NULL, $xss_clean = NULL)
*/
PHP_METHOD(cii_input, post)
{
	if( return_value_used ){
		zval *index = NULL;
		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z!", &index) == FAILURE){
			WRONG_PARAM_COUNT;
		}
		zval *retval;
		if( retval = cii_input_post(index) ){
			RETURN_ZVAL(retval, 1, 0);
		}
	}
}
/**
* Fetch an item from POST data with fallback to GET
*
* @param	string	$index		Index for item to be fetched from $_POST or $_GET
* @param	bool	$xss_clean	Whether to apply XSS filtering
* @return	mixed
*
* public function post_get($index, $xss_clean = NULL)
*/
PHP_METHOD(cii_input, post_get)
{
	if( return_value_used ){
		zval *index = NULL;
		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z!", &index) == FAILURE){
			WRONG_PARAM_COUNT;
		}
		zval *retval;
		if( retval = cii_input_post(index) ){
			RETURN_ZVAL(retval, 1, 0);
		}
		if( retval = cii_input_get(index) ){
			RETURN_ZVAL(retval, 1, 0);
		}
	}
}
/**
* Fetch an item from GET data with fallback to POST
*
* @param	string	$index		Index for item to be fetched from $_GET or $_POST
* @param	bool	$xss_clean	Whether to apply XSS filtering
* @return	mixed
*
* public function get_post($index, $xss_clean = NULL)
*/
PHP_METHOD(cii_input, get_post)
{
	if( return_value_used ){
		zval *index = NULL;
		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z!", &index) == FAILURE){
			WRONG_PARAM_COUNT;
		}
		zval *retval;
		if( retval = cii_input_get(index) ){
			RETURN_ZVAL(retval, 1, 0);
		}
		if( retval = cii_input_post(index) ){
			RETURN_ZVAL(retval, 1, 0);
		}
	}
}
/**
* Fetch an item from the COOKIE array
*
* @param	mixed	$index		Index for item to be fetched from $_COOKIE
* @param	bool	$xss_clean	Whether to apply XSS filtering
* @return	mixed
*
* public function cookie($index = NULL, $xss_clean = NULL)
*/
PHP_METHOD(cii_input, cookie)
{
	if( return_value_used ){
		zval *index = NULL;
		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z!", &index) == FAILURE){
			WRONG_PARAM_COUNT;
		}
		zval *cookie = PG(http_globals)[TRACK_VARS_COOKIE];
		if( index ){
			zval **value;
			if( Z_TYPE_P(index) == IS_STRING && zend_hash_find(Z_ARRVAL_P(cookie), Z_STRVAL_P(index), Z_STRLEN_P(index)+1, (void**)&value) != FAILURE ){
				RETURN_ZVAL(*value, 1, 0);
			}else if( Z_TYPE_P(index) == IS_LONG && zend_hash_index_find(Z_ARRVAL_P(cookie), Z_LVAL_P(index), (void**)&value) != FAILURE ){
				RETURN_ZVAL(*value, 1, 0);
			}else{
				return;
			}
		}
		RETURN_ZVAL(cookie, 1, 0);
	}	
}
/**
* Fetch an item from the SERVER array
*
* @param	mixed	$index		Index for item to be fetched from $_SERVER
* @param	bool	$xss_clean	Whether to apply XSS filtering
* @return	mixed
*
* public function server($index, $xss_clean = NULL)
*/
PHP_METHOD(cii_input, server)
{
	if( return_value_used ){
		zval *index = NULL;
		if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z!", &index) == FAILURE){
			WRONG_PARAM_COUNT;
		}
		zval *server = PG(http_globals)[TRACK_VARS_SERVER];
		if( index ){
			zval **value;
			if( Z_TYPE_P(index) == IS_STRING && zend_hash_find(Z_ARRVAL_P(server), Z_STRVAL_P(index), Z_STRLEN_P(index)+1, (void**)&value) != FAILURE ){
				RETURN_ZVAL(*value, 1, 0);
			}else if( Z_TYPE_P(index) == IS_LONG && zend_hash_index_find(Z_ARRVAL_P(server), Z_LVAL_P(index), (void**)&value) != FAILURE ){
				RETURN_ZVAL(*value, 1, 0);
			}else{
				return;
			}
		}
		RETURN_ZVAL(server, 1, 0);
	}
}
/**
* Fetch User Agent string
*
* @return	string|null	User Agent string or NULL if it doesn't exist
*
* public function user_agent($xss_clean = NULL)
*/
PHP_METHOD(cii_input, user_agent)
{
	if( return_value_used ){
		zval *server = PG(http_globals)[TRACK_VARS_SERVER];
		zval **value;
		if( zend_hash_find(Z_ARRVAL_P(server), "HTTP_USER_AGENT", 16, (void**)&value) != FAILURE ){
			RETURN_ZVAL(*value, 1, 0);
		}
	}
}

zend_function_entry cii_input_methods[] = {
	PHP_ME(cii_input, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_input, get, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_input, post, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_input, post_get, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_input, get_post, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_input, cookie, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_input, server, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_input, user_agent, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_input)
{
	/**
	 * Input Class
	 *
	 * Pre-processes global input data for security
	 */
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Input", cii_input_methods);
	cii_input_ce = zend_register_internal_class(&ce TSRMLS_CC);
}