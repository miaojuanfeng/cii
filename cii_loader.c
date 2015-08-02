#include "cii_loader.h"

zend_class_entry *cii_loader_ce;

//usefulless.bing mei you sen me ruan yong
ZEND_BEGIN_ARG_INFO_EX(cii_loader_view_arginfo,0,0,1)
	ZEND_ARG_INFO(0,view)
	ZEND_ARG_ARRAY_INFO(0,data,1)
	ZEND_ARG_INFO(0,return)
ZEND_END_ARG_INFO()

#define CII_STORE_EG_ENVIRON() \
	{ \
		zval ** __old_return_value_pp   = EG(return_value_ptr_ptr); \
		zend_op ** __old_opline_ptr  	= EG(opline_ptr); \
		zend_op_array * __old_op_array  = EG(active_op_array);

#define CII_RESTORE_EG_ENVIRON() \
		EG(return_value_ptr_ptr) = __old_return_value_pp;\
		EG(opline_ptr)			 = __old_opline_ptr; \
		EG(active_op_array)		 = __old_op_array; \
	}

int cii_loader_import(char *path, int len, int use_path TSRMLS_DC) {
	zend_file_handle file_handle;
	zend_op_array 	*op_array;
	char realpath[MAXPATHLEN];

	if (!VCWD_REALPATH(path, realpath)) {
		return 0;
	}

	file_handle.filename = path;
	file_handle.free_filename = 0;
	file_handle.type = ZEND_HANDLE_FILENAME;
	file_handle.opened_path = NULL;
	file_handle.handle.fp = NULL;

	op_array = zend_compile_file(&file_handle, ZEND_INCLUDE TSRMLS_CC);

	if (op_array && file_handle.handle.stream.handle) {
		int dummy = 1;

		if (!file_handle.opened_path) {
			file_handle.opened_path = path;
		}

		zend_hash_add(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path)+1, (void *)&dummy, sizeof(int), NULL);
	}
	zend_destroy_file_handle(&file_handle TSRMLS_CC);

	if (op_array) {
		zval *result = NULL;

		CII_STORE_EG_ENVIRON();

		EG(return_value_ptr_ptr) = &result;
		EG(active_op_array) 	 = op_array;

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2)) || (PHP_MAJOR_VERSION > 5)
		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}
#endif
		zend_execute(op_array TSRMLS_CC);

		destroy_op_array(op_array TSRMLS_CC);
		efree(op_array);
		if (!EG(exception)) {
			if (EG(return_value_ptr_ptr) && *EG(return_value_ptr_ptr)) {
				zval_ptr_dtor(EG(return_value_ptr_ptr));
			}
		}
		CII_RESTORE_EG_ENVIRON();
	    return 1;
	}

	return 0;
}

PHP_METHOD(cii_loader,__construct){
	zval *ob_level;
	MAKE_STD_ZVAL(ob_level);
	ZVAL_LONG(ob_level,php_output_get_level(TSRMLS_C));
	zend_update_property(cii_loader_ce,getThis(),ZEND_STRL("_cii_ob_level"),ob_level TSRMLS_CC);
	zval_ptr_dtor(&ob_level);
	php_printf("Info: Loader Class Initialized\n");
}

PHP_METHOD(cii_loader,view){
	char *view;
	uint len, need_free = 0;
	HashTable *data = NULL;
	uint is_return = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|Hl" ,&view, &len,&data,&is_return) == FAILURE) {
		return;
	}

	if (!len) {
		RETURN_FALSE;
	} else {
		int  retval = 0;

		/*if (!IS_ABSOLUTE_PATH(file, len)) {
			yaf_loader_t *loader = yaf_loader_instance(NULL, NULL, NULL TSRMLS_CC);
			if (!loader) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s need to be initialize first", yaf_loader_ce->name);
				RETURN_FALSE;
			} else {
				zval *library = zend_read_property(yaf_loader_ce, loader, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), 1 TSRMLS_CC);
				len = spprintf(&file, 0, "%s%c%s", Z_STRVAL_P(library), DEFAULT_SLASH, file);
				need_free = 1;
			}
		}*/
		char *file;
		uint len;

		len = spprintf(&file, 0, "%s%s%s", "/usr/local/httpd/htdocs/", view, ".php");

		retval = (zend_hash_exists(&EG(included_files), file, len + 1));
		if (retval) {
			if (need_free) {
				efree(file);
			}
			RETURN_TRUE;
		}

		if(data){
			char *key;
			int key_len;
			ulong idx;
			zval **value;
			//using Bucket* pos to make sure not modify data's hashtable internal pointer
			HashPosition pos;
			for(zend_hash_internal_pointer_reset_ex(data, &pos);
			    zend_hash_has_more_elements_ex(data, &pos) == SUCCESS;
			    zend_hash_move_forward_ex(data, &pos)){
				if(zend_hash_get_current_key_ex(data, &key, &key_len, &idx, 0, &pos) != HASH_KEY_IS_STRING){
					continue;
				}

				if(zend_hash_get_current_data_ex(data, (void**)&value, &pos) == FAILURE){
					continue;
				}
				ZEND_SET_SYMBOL_WITH_LENGTH(EG(active_symbol_table), key, key_len,*value, Z_REFCOUNT_P(*value) + 1, PZVAL_IS_REF(*value));	
			}
		}

		if(is_return){
			if(php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC) == SUCCESS){
				retval = cii_loader_import(file, len, 0 TSRMLS_CC);
				php_output_get_contents(return_value TSRMLS_CC);
				php_output_discard(TSRMLS_C);
				if(!retval){
					php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load the requested file: %s", file);
				}
				return;
			}else{
				php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "failed to create buffer");
				return;
			}
		}else{
			retval = cii_loader_import(file, len, 0 TSRMLS_CC);
			if(!retval){
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load the requested file: %s", file);
			}
		}

		if (need_free) {
			efree(file);
		}

		RETURN_BOOL(retval);
	}
}

PHP_METHOD(cii_loader,model){
	char *model;
	int model_len, need_free = 0;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &model, &model_len) == FAILURE){
		RETURN_NULL();	
	}

	if (!model_len) {
		RETURN_FALSE;
	} else {
		int  retval = 0;

		/*if (!IS_ABSOLUTE_PATH(file, len)) {
			yaf_loader_t *loader = yaf_loader_instance(NULL, NULL, NULL TSRMLS_CC);
			if (!loader) {
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "%s need to be initialize first", yaf_loader_ce->name);
				RETURN_FALSE;
			} else {
				zval *library = zend_read_property(yaf_loader_ce, loader, ZEND_STRL(YAF_LOADER_PROPERTY_NAME_LIBRARY), 1 TSRMLS_CC);
				len = spprintf(&file, 0, "%s%c%s", Z_STRVAL_P(library), DEFAULT_SLASH, file);
				need_free = 1;
			}
		}*/
		char *file;
		uint len;

		len = spprintf(&file, 0, "%s%s%s", "/usr/local/httpd/htdocs/", model, ".php");

		retval = (zend_hash_exists(&EG(included_files), file, len + 1));
		if (retval) {
			if (need_free) {
				efree(file);
			}
			RETURN_TRUE;
		}

		retval = cii_loader_import(file, len, 0 TSRMLS_CC);

		if (need_free) {
			efree(file);
		}

		//
		zend_class_entry **ce;
		if(zend_hash_find(CG(class_table),model,model_len+1,(void**)&ce)==SUCCESS){
			zval *new_object;
			MAKE_STD_ZVAL(new_object);
			object_init_ex(new_object,*ce);
			//
			zval *CII;
			CII = get_cii_controller_instance(TSRMLS_C);
			zend_update_property(cii_controller_ce,CII,ZEND_STRL(model),new_object TSRMLS_CC);
			zval *new_object_method_construct;
			MAKE_STD_ZVAL(new_object_method_construct);
			ZVAL_STRING(new_object_method_construct,"__construct",1);
			call_user_function(NULL, &new_object, new_object_method_construct, *return_value_ptr, 0, NULL TSRMLS_CC);
		}else{
			php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to locate the model you have specified: %s", model);
		}
		RETURN_BOOL(retval);
	}
}

zend_function_entry cii_loader_methods[] = {
	PHP_ME(cii_loader,__construct,NULL,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(cii_loader,view,cii_loader_view_arginfo,ZEND_ACC_PUBLIC)
	PHP_ME(cii_loader,model,NULL,ZEND_ACC_PUBLIC)
	{NULL,NULL,NULL}
};

PHP_MINIT_FUNCTION(cii_loader){
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"cii_loader",cii_loader_methods);
	cii_loader_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_property_null(cii_loader_ce,ZEND_STRL("_cii_ob_level"),ZEND_ACC_PROTECTED TSRMLS_CC);
}
