#include "cii_helper.h"

#define CII_HELPER_FUNCTION \
	PHP_FE(element,NULL) \
	PHP_FE(elements,NULL)

/*
*****************************************************************************************
**                                    array_helper                                     **
*****************************************************************************************
*/

/*
ZEND_BEGIN_ARG_INFO_EX(cii_helper_element_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0,item)
	ZEND_ARG_ARRAY_INFO(0,array,1)
	ZEND_ARG_INFO(0,dft)
ZEND_END_ARG_INFO()
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
				/*php_printf("%p\n",*return_value_ptr);
				php_printf("%p\n",return_value);
				zval_ptr_dtor(return_value_ptr);
				return_value = *pdata;
				return_value_ptr = &return_value;
				php_printf("%p\n",*return_value_ptr);
				php_printf("%p\n",return_value);
				php_printf("%p\n",*pdata);

				php_printf("%d\n",Z_TYPE_P(return_value));
				php_printf("%s\n",Z_STRVAL_P(return_value));*/
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
	zval *items;
	HashTable *array;
	zval *dft = NULL;
	zval **pdata;
	HashPosition pos;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zH|z", &items, &array, &dft) == FAILURE){
		return;
	}
	if(return_value_used){
		char *key;
		uint key_len;
		ulong idx;
		zval **value;
		if(Z_TYPE_P(items)!=IS_ARRAY){
			convert_to_array(items);
		}
		if(!dft){
			ALLOC_INIT_ZVAL(dft);
		}
		/*php_printf("%d\n",Z_TYPE_P(dft));
		php_printf("%s\n",Z_STRVAL_P(dft));
		php_printf("%d\n",(dft->refcount__gc));
		php_printf("%d\n",(dft->is_ref__gc));*/
		array_init(return_value);
		for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(items), &pos);
			zend_hash_has_more_elements_ex(Z_ARRVAL_P(items), &pos) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_P(items), &pos) )
		{
			key = NULL;
			key_len = 0;
			idx = 0;
			if( zend_hash_get_current_data_ex(Z_ARRVAL_P(items), (void**)&pdata, &pos) == SUCCESS ){
				switch(Z_TYPE_P(*pdata)){
					case IS_STRING:
						key = Z_STRVAL_P(*pdata);
						key_len = Z_STRLEN_P(*pdata) + 1;
						break;
					case IS_LONG:
						idx = Z_LVAL_P(*pdata);
						break;
					case IS_NULL:
						key = "";
						key_len = 1;
						break;
					default:
						php_error_docref(NULL TSRMLS_CC, E_WARNING, "The first argument should be an array or a string or an integer");
				}
			}
			if(key_len){
				if( zend_hash_find(array, key, key_len, (void**)&value) == SUCCESS ){
					zval *temp;
					if(PZVAL_IS_REF(*value)){
						MAKE_STD_ZVAL(temp);
						ZVAL_COPY_VALUE(temp,*value);
						zval_copy_ctor(temp);
						value = &temp;
					}else{
						Z_ADDREF_P(*value);
					}
					zend_hash_update(Z_ARRVAL_P(return_value), key, key_len, value, sizeof(zval *), NULL);
				}else{
					Z_ADDREF_P(dft);
					zend_hash_update(Z_ARRVAL_P(return_value), key, key_len, &dft, sizeof(zval *), NULL);
				}
			}else{
				if( zend_hash_index_find(array, idx, (void**)&value) == SUCCESS ){
					zval *temp;
					if(PZVAL_IS_REF(*value)){
						MAKE_STD_ZVAL(temp);
						ZVAL_COPY_VALUE(temp,*value);
						zval_copy_ctor(temp);
						value = &temp;
					}else{
						Z_ADDREF_P(*value);
					}
					zend_hash_index_update(Z_ARRVAL_P(return_value), idx, value, sizeof(zval *), NULL);
				}else{
					Z_ADDREF_P(dft);
					zend_hash_index_update(Z_ARRVAL_P(return_value), idx, &dft, sizeof(zval *), NULL);
				}
			}
		/*php_printf("%d\n",Z_TYPE_P(dft));
		php_printf("%s\n",Z_STRVAL_P(dft));
		php_printf("%d\n",(dft->refcount__gc));
		php_printf("%d\n",(dft->is_ref__gc));*/
		}
	}
}

/**
* Random Element - Takes an array as input and returns a random element
*
* @param	array
* @return	mixed	depends on what the array contains
*
* random_element($array)
*/
PHP_FUNCTION(random_element){
	HashTable *array;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "H", &array) == FAILURE){
		return;
	}
}