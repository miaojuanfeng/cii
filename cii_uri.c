#include "cii_uri.h"

zend_class_entry *cii_uri_ce;

/**
*	Class constructor
*
*	@return	void
*
*	public function __construct()
*/
PHP_METHOD(cii_uri, __construct)
{
	/*
	*	init cii_uri::segments
	*/
	zval *segments;
	MAKE_STD_ZVAL(segments);
	array_init(segments);
	zend_update_property(cii_uri_ce, getThis(), ZEND_STRL("segments"), segments TSRMLS_CC);
	zval_ptr_dtor(&segments);
	/*
	*	init cii_uri::rsegments
	*/
	zval *rsegments;
	MAKE_STD_ZVAL(rsegments);
	array_init(rsegments);
	zend_update_property(cii_uri_ce, getThis(), ZEND_STRL("rsegments"), rsegments TSRMLS_CC);
	zval_ptr_dtor(&rsegments);
	/*
	*	fetch uri
	*/
	zval **query;
	zval **server = &PG(http_globals)[TRACK_VARS_SERVER];

    /*//debug
	zval *query_temp;
    MAKE_STD_ZVAL(query_temp);
    ZVAL_STRING(query_temp, "/home/index/name/age/addr", 1);
    query = &query_temp;
    //debug*/

    if( /*1 ||*/ zend_hash_find(Z_ARRVAL_PP(server), "QUERY_STRING", 13, (void**)&query) != FAILURE &&
    	Z_TYPE_PP(query) == IS_STRING && Z_STRLEN_PP(query) > 1){

    	zval zstr;
		char *p = Z_STRVAL_PP(query);
		uint p_len = Z_STRLEN_PP(query);
		/*
    	*	trim(query, '/')  or ltrim(query, '/') and rtrim(query, '/')
    	*/
		if( p[0] == '/' && --p_len > 0 ){
			p++;
		}
		if( p_len > 0 && p[p_len-1] == '/'){
			p_len--;
		}
		ZVAL_STRINGL(&zstr, p, p_len, 1);

		if( p_len > 0 ){
			/*
			*	update cii_uri::uri_string
			*/
			zval *uri_string;
			MAKE_STD_ZVAL(uri_string);
			ZVAL_COPY_VALUE(uri_string, &zstr);
			zval_copy_ctor(uri_string);
			zend_update_property(cii_uri_ce, getThis(), ZEND_STRL("uri_string"), uri_string TSRMLS_CC);
			zval_ptr_dtor(&uri_string);
			/*
			*	explode uri
			*/
			zval zdelim;
	    	ZVAL_STRINGL(&zdelim, "/", 1, 1);
	    	zval *uri_arr;
	    	MAKE_STD_ZVAL(uri_arr);
	    	array_init(uri_arr);
	    	php_explode(&zdelim, &zstr, uri_arr, LONG_MAX);
	    	/*
			*	update cii_uri::segments and cii_uri::rsegments
			*/
	    	uint i = 1;
	    	HashPosition pos;
	    	zval **value;
	    	zval *segments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("segments"), 1 TSRMLS_CC);
	    	zval *rsegments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("rsegments"), 1 TSRMLS_CC);
	    	for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(uri_arr), &pos);
			    zend_hash_has_more_elements_ex(Z_ARRVAL_P(uri_arr), &pos) == SUCCESS;
			    zend_hash_move_forward_ex(Z_ARRVAL_P(uri_arr), &pos)){
					if(zend_hash_get_current_data_ex(Z_ARRVAL_P(uri_arr), (void**)&value, &pos) == FAILURE){
						continue;
					}
					Z_ADDREF_PP(value);
		    		zend_hash_index_update(Z_ARRVAL_P(segments),  i, value, sizeof(zval *), NULL);
		    		i++;
	    	}
	    	zval_dtor(&zdelim);
	    	zval_ptr_dtor(&uri_arr);
		}
		zval_dtor(&zstr);
		zval_ptr_dtor(query);
    }
	/*
	*	output log
	*/
	php_printf("Info: URI Class Initialized\n");
}
/**
* 	Segment Array
*
*	@return	array	CI_URI::$segments
*
*	public function segment_array()
*/
PHP_METHOD(cii_uri, segment_array)
{
	zval *segments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("segments"), 1 TSRMLS_CC);
	RETURN_ZVAL(segments, 1, 0);
}
/**
* 	Routed Segment Array
*
* 	@return	array	CI_URI::$rsegments
*
*	public function rsegment_array()
*/
PHP_METHOD(cii_uri, rsegment_array)
{
	zval *rsegments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("rsegments"), 1 TSRMLS_CC);
	RETURN_ZVAL(rsegments, 1, 0);
}
/**
* 	Total number of segments
*
* 	@return	int
*
*	public function total_segments()
*/
PHP_METHOD(cii_uri, total_segments)
{
	zval *segments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("segments"), 1 TSRMLS_CC);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(segments)));
}
/**
* 	Total number of routed segments
*
* 	@return	int
*
*	public function total_rsegments()
*/
PHP_METHOD(cii_uri, total_rsegments)
{
	zval *rsegments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("rsegments"), 1 TSRMLS_CC);
	RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(rsegments)));
}

zend_function_entry cii_uri_methods[] = {
	PHP_ME(cii_uri, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_uri, segment_array, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, rsegment_array, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, total_segments, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, total_rsegments, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_uri)
{
	/**
	 * URI Class
	 *
	 * Parses URIs and determines routing
	 */
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_URI", cii_uri_methods);
	cii_uri_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * Current URI string
	 *
	 * @var	string
	 */
	zend_declare_property_stringl(cii_uri_ce, ZEND_STRL("uri_string"), "", 0, ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * List of URI segments
	 *
	 * Starts at 1 instead of 0.
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_uri_ce, ZEND_STRL("segments"), ZEND_ACC_PUBLIC TSRMLS_CC);
	/**
	 * List of routed URI segments
	 *
	 * Starts at 1 instead of 0.
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_uri_ce, ZEND_STRL("rsegments"), ZEND_ACC_PUBLIC TSRMLS_CC);
}