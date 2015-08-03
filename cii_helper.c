#include "cii_helper.h"

#define CII_HELPER_FUNCTION \
	PHP_FE(element,NULL) \
	PHP_FE(elements,NULL)

/*
*****************************************************************************************
**                                    array_helper                                     **
*****************************************************************************************
*/

/**
* Element
*
* Lets you determine whether an array index is set and whether it has a value.
* If the element is empty it returns NULL (or whatever you specify as the default value.)
*
* @param	string
* @param	array
* @param	mixed
* @return	mixed	depends on what the array contains
*
* element($item, $array, $default = NULL)
*/
PHP_FUNCTION(element){
	zval *item;
	HashTable *array;
	zval *dft = NULL;
	zval **pdata;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zH|z", &item, &array, &dft) == FAILURE){
		return;
	}
	switch(Z_TYPE_P(item)){
		case IS_STRING:
			if( zend_hash_find(array, Z_STRVAL_P(item), Z_STRLEN_P(item)+1, (void**)&pdata) == SUCCESS ){
				Z_ADDREF_P(*pdata);
				zval_ptr_dtor(&return_value);
				return_value = *pdata;
				php_printf("%d\n",return_value->refcount__gc);
				php_printf("%d\n",return_value->is_ref__gc);
				//RETVAL_ZVAL(*pdata, 1, 0);
				return;
			}
			break;
		case IS_LONG:
			if( zend_hash_index_find(array, Z_LVAL_P(item), (void**)&pdata) == SUCCESS ){
				RETVAL_ZVAL(*pdata, 1, 0);
				return;
			}
			break;
		case IS_NULL:
			if( zend_hash_find(array, "", 1, (void**)&pdata) == SUCCESS ){
				RETVAL_ZVAL(*pdata, 1, 0);
				return;
			}
			break;
		default:
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "The first argument should be either a string or an integer");	
	}
	if(dft){
		RETVAL_ZVAL(dft, 1, 0);
	}
}

/**
* Elements
*
* Returns only the array items specified. Will return a default value if
* it is not set.
*
* @param	array
* @param	array
* @param	mixed
* @return	mixed	depends on what the array contains
*
* elements($items, $array, $default = NULL)
*/
PHP_FUNCTION(elements){
	HashTable *items;
	HashTable *array;
	zval *dft = NULL;
	zval **pdata;
	HashPosition pos;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "HH|z", &items, &array, &dft) == FAILURE){
		return;
	}
	for(zend_hash_internal_pointer_reset_ex(items, &pos);
		zend_hash_has_more_elements_ex(items, &pos) == SUCCESS;
		zend_hash_move_forward_ex(items, &pos) )
	{
		char *key;
		uint key_len = 0;
		zval **value;
		if( zend_hash_get_current_data_ex(items, (void**)&pdata, &pos) == SUCCESS ){
			key = Z_STRVAL_P(*pdata);
			key_len = Z_STRLEN_P(*pdata);
		}
		if(key_len){
			array_init(return_value);
			if( zend_hash_find(array, key, key_len+1, (void**)&value) == SUCCESS ){
				//zend_hash_update(Z_ARRVAL_P(return_value), key, key_len+1, *value);
			}
		}
		/*switch(Z_TYPE_P(item)){
			case IS_STRING:
				if( zend_hash_find(array, Z_STRVAL_P(item), Z_STRLEN_P(item)+1, (void**)&pdata) == SUCCESS ){
					RETVAL_ZVAL(*pdata, 1, 0);
					return;
				}
				break;
			case IS_LONG:
				if( zend_hash_index_find(array, Z_LVAL_P(item), (void**)&pdata) == SUCCESS ){
					RETVAL_ZVAL(*pdata, 1, 0);
					return;
				}
				break;
			case IS_NULL:
				if( zend_hash_find(array, "", 1, (void**)&pdata) == SUCCESS ){
					RETVAL_ZVAL(*pdata, 1, 0);
					return;
				}
				break;
			default:
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "The first argument should be either a string or an integer");	
		}*/
	}
	if(dft){
		RETVAL_ZVAL(dft, 1, 0);
	}
}