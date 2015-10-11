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

    //debug
	zval *query_temp;
    MAKE_STD_ZVAL(query_temp);
    ZVAL_STRING(query_temp, "/welcome/index/name/miaojuanfeng/age/23", 1);
    query = &query_temp;
    //debug

    if( 1 || zend_hash_find(Z_ARRVAL_PP(server), "QUERY_STRING", 13, (void**)&query) != FAILURE &&
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
	cii_write_log(3, "URI Class Initialized");
}
/**
* Fetch URI Segment
*
* @see		CI_URI::$segments
* @param	int		$n		Index
* @param	mixed		$no_result	What to return if the segment index is not found
* @return	mixed
*
* public function segment($n, $no_result = NULL)
*/
PHP_METHOD(cii_uri, segment)
{
	long index;
	zval *no_result = NULL;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|z!", &index, &no_result) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	zval *segments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("segments"), 1 TSRMLS_CC);
	zval **value;
	if( zend_hash_index_find(Z_ARRVAL_P(segments), index, (void**)&value) != FAILURE ){
		RETURN_ZVAL(*value, 1, 0);
	}
	if( no_result ){
		RETURN_ZVAL(no_result, 1, 0);
	}
}
/**
* Fetch URI "routed" Segment
*
* Returns the re-routed URI segment (assuming routing rules are used)
* based on the index provided. If there is no routing, will return
* the same result as CI_URI::segment().
*
* @see		CI_URI::$rsegments
* @see		CI_URI::segment()
* @param	int		$n		Index
* @param	mixed		$no_result	What to return if the segment index is not found
* @return	mixed
*
* public function rsegment($n, $no_result = NULL)
*/
PHP_METHOD(cii_uri, rsegment)
{
	long index;
	zval *no_result = NULL;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|z!", &index, &no_result) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	zval *rsegments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("rsegments"), 1 TSRMLS_CC);
	zval **value;
	if( zend_hash_index_find(Z_ARRVAL_P(rsegments), index, (void**)&value) != FAILURE ){
		RETURN_ZVAL(*value, 1, 0);
	}
	if( no_result ){
		RETURN_ZVAL(no_result, 1, 0);
	}
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
/**
* Fetch URI string
*
* @return	string	CI_URI::$uri_string
*
* public function uri_string()
*/
PHP_METHOD(cii_uri, uri_string)
{
	zval *uri_string = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("uri_string"), 1 TSRMLS_CC);
	RETURN_ZVAL(uri_string, 1, 0);
}
/**
* Fetch Re-routed URI string
*
* @return	string
*
* public function ruri_string()
*/
PHP_METHOD(cii_uri, ruri_string)
{
	zval *rsegments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("rsegments"), 1 TSRMLS_CC);
	/*
	*	implode uri
	*/
	zval *delim;
	MAKE_STD_ZVAL(delim);
	ZVAL_STRINGL(delim, "/", 1, 1);
	zval *retval;
	MAKE_STD_ZVAL(retval);
	php_implode(delim, rsegments, retval TSRMLS_CC);
	zval_ptr_dtor(&delim);

	RETVAL_ZVAL(retval, 1, 0);
	zval_ptr_dtor(&retval);
}
/**
* cii_slash_segment
*/
ZEND_API char* cii_slash_segment(long n, char where, zval *segments)
{
	char *segment;
	if( !segments ){
		segment = "";
	}
	if( Z_TYPE_P(segments) != IS_ARRAY ){
		convert_to_array(segments);
	}
	zval **value = NULL;
	if( zend_hash_index_find(Z_ARRVAL_P(segments), n, (void**)&value) == FAILURE ){
		segment = "";
	}
	if( value ){
		segment = Z_STRVAL_PP(value);
	}else{
		segment = "";
	}
	char *retval;
	switch(where){
		case 't':
			spprintf(&retval, 0, "%s%c", segment, '/');
			break;
		case 'l':
			spprintf(&retval, 0, "%c%s", '/', segment);
			break;
		default:
			spprintf(&retval, 0, "%c%s%c", '/', segment, '/');
			break;
	}
	return retval;
}
/**
* Slash segment
*
* Fetches an URI segment with a slash.
*
* @param	int	$n	Index
* @param	string	$where	Where to add the slash ('t' == 'trailing' or 'l' == 'leading')
* @return	string
*
* public function slash_segment($n, $where = 't')
*/
PHP_METHOD(cii_uri, slash_segment)
{
	ulong n;
	char* where = "t";
	uint where_len = 1;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|s", &n, &where, &where_len) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	zval *segments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("segments"), 1 TSRMLS_CC);
	RETURN_STRING(cii_slash_segment(n, where[0], segments), 0);
}
/**
* Slash routed segment
*
* Fetches an URI routed segment with a slash.
*
* @param	int	$n	Index
* @param	string	$where	Where to add the slash ('t' == 'trailing' or 'l' == 'leading')
* @return	string
*
* public function slash_rsegment($n, $where = 't')
*/
PHP_METHOD(cii_uri, slash_rsegment)
{
	ulong n;
	char* where = "t";
	uint where_len = 1;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|s", &n, &where, &where_len) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	zval *rsegments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("rsegments"), 1 TSRMLS_CC);
	RETURN_STRING(cii_slash_segment(n, where[0], rsegments), 0);
}
/**
* Assoc to URI
*
* Generates a URI string from an associative array.
*
* @param	array	$array	Input array of key/value pairs
* @return	string	URI string
*
* public function assoc_to_uri($array)
*/
PHP_METHOD(cii_uri, assoc_to_uri)
{
	HashTable *array;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H", &array) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	zval *temp;
	MAKE_STD_ZVAL(temp);
	array_init(temp);
	HashPosition pos;
	char *key;
	uint key_len;
	int key_type;
	ulong idx;
	zval **value;
	zval *key_zval;
	for(zend_hash_internal_pointer_reset_ex(array, &pos);
	    zend_hash_has_more_elements_ex(array, &pos) == SUCCESS;
	    zend_hash_move_forward_ex(array, &pos)){
		key_type = zend_hash_get_current_key_ex(array, &key, &key_len, &idx, 0, &pos);	
		MAKE_STD_ZVAL(key_zval);
		if( key_type == HASH_KEY_IS_STRING ){
			ZVAL_STRINGL(key_zval, key, key_len-1, 1);
		}else if( key_type == HASH_KEY_IS_LONG ){
			ZVAL_LONG(key_zval, idx);
		}else{
			continue;
		}
		zend_hash_next_index_insert(Z_ARRVAL_P(temp), &key_zval, sizeof(zval *), NULL);
		zend_hash_get_current_data_ex(array, (void**)&value, &pos);
		CII_IF_ISREF_THEN_SEPARATE_ELSE_ADDREF(value);
		zend_hash_next_index_insert(Z_ARRVAL_P(temp), value, sizeof(zval *), NULL);
	}
	/*
	*	implode uri
	*/
	zval *delim;
	MAKE_STD_ZVAL(delim);
	ZVAL_STRINGL(delim, "/", 1, 1);
	zval *retval;
	MAKE_STD_ZVAL(retval);
	php_implode(delim, temp, retval TSRMLS_CC);
	zval_ptr_dtor(&delim);

	RETVAL_STRING(Z_STRVAL_P(retval), 1);
	zval_ptr_dtor(&temp);
	zval_ptr_dtor(&retval);
}	
/**
* URI to assoc
*
* Generates an associative array of URI data starting at the supplied
* segment index. For example, if this is your URI:
*
*	example.com/user/search/name/joe/location/UK/gender/male
*
* You can use this method to generate an array with this prototype:
*
*	array (
*		name => joe
*		location => UK
*		gender => male
*	 )
*
* @param	int	$n		Index (default: 3)
* @param	array	$default	Default values
* @return	array
*
* public function uri_to_assoc($n = 3, $default = array())
*/
PHP_METHOD(cii_uri, uri_to_assoc)
{
	long n = 3;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &n) == FAILURE){
		WRONG_PARAM_COUNT;
	}
	zval *segments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("segments"), 1 TSRMLS_CC);
	zval *temp;
	MAKE_STD_ZVAL(temp);
	array_init(temp);
	HashPosition pos;
	long i = 0, j = 1;
	char *key;
	uint key_len;
	zval **value;
	for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(segments), &pos);
	    zend_hash_has_more_elements_ex(Z_ARRVAL_P(segments), &pos) == SUCCESS;
	    zend_hash_move_forward_ex(Z_ARRVAL_P(segments), &pos)){
		i++;
		if( i < n ) continue;
		zend_hash_get_current_data_ex(Z_ARRVAL_P(segments), (void**)&value, &pos);
		if( j % 2 ){
			php_printf("key\n");
			key = Z_STRVAL_PP(value);
			key_len = Z_STRLEN_PP(value);
		}else{
			php_printf("value\n");
			CII_IF_ISREF_THEN_SEPARATE_ELSE_ADDREF(value);
			zend_hash_update(Z_ARRVAL_P(temp), key, key_len+1, value, sizeof(zval *), NULL);
		}
		j++;
	}
	if( !(j % 2) ){
		php_printf("null value\n");
		zval *z_null;
		MAKE_STD_ZVAL(z_null);
		ZVAL_NULL(z_null);
		zend_hash_update(Z_ARRVAL_P(temp), key, key_len+1, &z_null, sizeof(zval *), NULL);
	}
	RETVAL_ZVAL(temp, 1, 0);
	zval_ptr_dtor(&temp);
}

zend_function_entry cii_uri_methods[] = {
	PHP_ME(cii_uri, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_uri, segment, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, rsegment, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, segment_array, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, rsegment_array, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, total_segments, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, total_rsegments, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, uri_string, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, ruri_string, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, slash_segment, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, slash_rsegment, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, assoc_to_uri, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_uri, uri_to_assoc, NULL, ZEND_ACC_PUBLIC)
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