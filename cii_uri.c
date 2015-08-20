#include "cii_uri.h"

zend_class_entry *cii_uri_ce;

PHP_METHOD(cii_uri, __construct)
{
	/*
	* init cii_uri::segments
	*/
	zval *segments;
	MAKE_STD_ZVAL(segments);
	array_init(segments);
	zend_update_property(cii_uri_ce, getThis(), ZEND_STRL("segments"), segments TSRMLS_CC);
	zval_ptr_dtor(&segments);
	/*
	* init cii_uri::rsegments
	*/
	zval *rsegments;
	MAKE_STD_ZVAL(rsegments);
	array_init(rsegments);
	zend_update_property(cii_uri_ce, getThis(), ZEND_STRL("rsegments"), rsegments TSRMLS_CC);
	zval_ptr_dtor(&rsegments);
	/*
	* fetch uri
	*/
	zval **query;
	zval **server = &PG(http_globals)[TRACK_VARS_SERVER];

    //debug
	zval *query_temp;
    MAKE_STD_ZVAL(query_temp);
    ZVAL_STRING(query_temp, "/home", 1);
    query = &query_temp;
    //debug

    if( 1 || zend_hash_find(Z_ARRVAL_PP(server), "QUERY_STRING", 13, (void**)&query) != FAILURE &&
    	Z_TYPE_PP(query) == IS_STRING && Z_STRLEN_PP(query) > 1){

    	zval zdelim;
    	zval zstr;
    	zval *uri_string;
    	zval *uri_arr;
		char *p = Z_STRVAL_PP(query);
		uint p_len = Z_STRLEN_PP(query);

		if( p[0] == '/' && --p_len > 0 ){
			p++;
		}
		if( p_len > 0 && p[p_len-1] == '/'){
			p_len--;
		}
		ZVAL_STRINGL(&zstr, p, p_len, 1);
		/*
		* update cii_uri::uri_string
		*/
		MAKE_STD_ZVAL(uri_string);
		*uri_string = zstr;
		zval_copy_ctor(uri_string);
    	zend_update_property(cii_uri_ce, getThis(), ZEND_STRL("uri_string"), uri_string TSRMLS_CC);
    	zval_ptr_dtor(&uri_string);

    	ZVAL_STRINGL(&zdelim, "/", 1, 1);

    	MAKE_STD_ZVAL(uri_arr);
    	array_init(uri_arr);
    	php_explode(&zdelim, &zstr, uri_arr, LONG_MAX);
    	//php_printf("type: %d\n",Z_TYPE_P(uri_arr));
    	//php_printf("num: %d\n",Z_ARRVAL_P(uri_arr)->nNumOfElements);
    	uint i;
    	HashPosition pos = Z_ARRVAL_P(uri_arr)->pListHead;
    	zval *segments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("segments"), 1 TSRMLS_CC);
    	zval *rsegments = zend_read_property(cii_uri_ce, getThis(), ZEND_STRL("rsegments"), 1 TSRMLS_CC);
    	for(i = 1; i <= Z_ARRVAL_P(uri_arr)->nNumOfElements; i++){
    		zend_hash_index_update(Z_ARRVAL_P(segments),  i, pos->pData, sizeof(zval *), NULL);
    		zend_hash_index_update(Z_ARRVAL_P(rsegments), i, pos->pData, sizeof(zval *), NULL);
    		pos = pos->pListNext;
    	}	

    	zval_dtor(&zdelim);
    	zval_dtor(&zstr);
    	zval_ptr_dtor(&uri_arr);

    	/*zval *router_obj;
    	MAKE_STD_ZVAL(router_obj);
    	object_init_ex(router_obj, cii_router_ce);

    	zend_update_property()*/
    	
    }

    zval_ptr_dtor(query);
	/*
	* output log
	*/
	php_printf("Info: URI Class Initialized\n");
}

zend_function_entry cii_uri_methods[] = {
	PHP_ME(cii_uri, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
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