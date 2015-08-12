#include "cii_loader.h"

zend_class_entry *cii_loader_ce;

//usefulless.bing mei you sen me ruan yong
/*ZEND_BEGIN_ARG_INFO_EX(cii_loader_view_arginfo,0,0,1)
	ZEND_ARG_INFO(0,view)
	ZEND_ARG_ARRAY_INFO(0,data,1)
	ZEND_ARG_INFO(0,return)
ZEND_END_ARG_INFO()*/

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

ZEND_API int cii_loader_import(char *path, int len, int use_path TSRMLS_DC) {
	zend_file_handle file_handle;
	zend_op_array 	*op_array;
	char realpath[MAXPATHLEN];

	if (!VCWD_REALPATH(path, realpath)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to load the requested file: %s", path);
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
	zval *this = getThis();
	//init cii_loader::_cii_ob_level
	zval *ob_level;
	MAKE_STD_ZVAL(ob_level);
	ZVAL_LONG(ob_level, php_output_get_level(TSRMLS_C));
	zend_update_property(cii_loader_ce, this, ZEND_STRL("_cii_ob_level"), ob_level TSRMLS_CC);
	zval_ptr_dtor(&ob_level);
	//init cii_loader::_cii_models
	zval *_cii_models = zend_read_property(cii_loader_ce, this, ZEND_STRL("_cii_models"), 1 TSRMLS_CC);
	if(Z_TYPE_P(_cii_models)!=IS_ARRAY){
		convert_to_array(_cii_models);
	}
	php_printf("Info: Loader Class Initialized\n");
}

PHP_METHOD(cii_loader,view){
	char *view;
	uint len;
	HashTable *data = NULL;
	uint is_return = 0;

	zval **value;

	HashTable *old_active_symbol_table;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|H!l" ,&view, &len, &data, &is_return) == FAILURE) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	if (!len) {
		RETURN_ZVAL(getThis(), 1, 0);
	} else {
		char *file;
		uint len;

		len = spprintf(&file, 0, "%s%s%s", "/usr/local/httpd/htdocs/cii/views/", view, ".php");

		if(zend_hash_exists(&EG(included_files), file, len + 1)){
			RETURN_ZVAL(getThis(), 1, 0);
		}

		/*if (EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}*/
		old_active_symbol_table = EG(active_symbol_table);
		ALLOC_HASHTABLE(EG(active_symbol_table));
		zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);

		if(data){
			char *key;
			int key_len;
			ulong idx;
			
			//using HashPosition pos to make sure not modify data's hashtable internal pointer
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
				if(PZVAL_IS_REF(*value)){
					zval *temp;
					MAKE_STD_ZVAL(temp);
					ZVAL_COPY_VALUE(temp,*value);
					zval_copy_ctor(temp);
					value = &temp;
				}else{
					Z_ADDREF_P(*value);
				}
				zend_hash_update(EG(active_symbol_table), key, key_len, value, sizeof(zval *), NULL);
			}
		}

		if(is_return){
			if(php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC) == SUCCESS){
				cii_loader_import(file, len, 0 TSRMLS_CC);
				php_output_get_contents(return_value TSRMLS_CC);
				php_output_discard(TSRMLS_C);

				zend_hash_destroy(EG(active_symbol_table));
				FREE_HASHTABLE(EG(active_symbol_table));
				EG(active_symbol_table) = old_active_symbol_table;

				return;
			}else{
				php_error_docref(NULL TSRMLS_CC, E_WARNING, "failed to create buffer");
				return;
			}
		}else{
			cii_loader_import(file, len, 0 TSRMLS_CC);

			zend_hash_destroy(EG(active_symbol_table));
			FREE_HASHTABLE(EG(active_symbol_table));
			EG(active_symbol_table) = old_active_symbol_table;

			RETURN_ZVAL(getThis(), 1, 0);
		}
	}
}
/**
* Model Loader
*
* Loads and instantiates models.
*
* @param	string	$model		Model name
* @param	string	$name		An optional object name to assign to
* @return	object
*
* public function model($model, $name = '')
*/
PHP_METHOD(cii_loader, model){
	char *model;
	uint model_len;
	char *name = NULL;
	uint name_len;
	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &model, &model_len, &name, &name_len) == FAILURE){
		RETURN_ZVAL(getThis(), 1, 0);
	}

	if(!model_len){
		RETURN_ZVAL(getThis(), 1, 0);
	}else{
		char *file;
		uint len;

		len = spprintf(&file, 0, "%s%s%s", "/usr/local/httpd/htdocs/cii/models/", model, ".php");

		if (zend_hash_exists(&EG(included_files), file, len + 1)){
			RETURN_ZVAL(getThis(), 1, 0);
		}

		cii_loader_import(file, len, 0 TSRMLS_CC);

		//add new object property to cii_controller class
		zend_class_entry **ce;
		if(zend_hash_find(CG(class_table),model,model_len+1,(void**)&ce)==SUCCESS){
			zval *new_object;
			zval *CII = GET_CII_CONTROLLER_INSTANCE();
			zval *new_object_method_construct;

			if(name){
				if( Z_TYPE_P(zend_read_property(cii_controller_ce, CII, name, name_len, 1 TSRMLS_CC)) != IS_NULL ){
					php_error(E_ERROR, "The model name you are loading is the name of a resource that is already being used: %s", name);
					RETURN_ZVAL(getThis(), 1, 0);
				}
			}else{
				name = model;
				name_len = model_len;
			}
			//add new object to cii_controller
			MAKE_STD_ZVAL(new_object);
			object_init_ex(new_object, *ce);
			zend_update_property(cii_controller_ce, CII, name, name_len, new_object TSRMLS_CC);
			//add new model to cii_loader::_cii_model
			zval *_cii_models = zend_read_property(cii_loader_ce, getThis(), ZEND_STRL("_cii_models"), 1 TSRMLS_CC);
			zval *model_name;
			MAKE_STD_ZVAL(model_name);
			ZVAL_STRING(model_name, name, 1);
			zend_hash_next_index_insert(Z_ARRVAL_P(_cii_models), &model_name, sizeof(zval *), NULL);
			//call new object construct function
			MAKE_STD_ZVAL(new_object_method_construct);
			ZVAL_STRING(new_object_method_construct, "__construct", 1);
			call_user_function(NULL, &new_object, new_object_method_construct, *return_value_ptr, 0, NULL TSRMLS_CC);
		}
	}
	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(cii_loader, helper){
	zval *helper = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z" ,&helper) == FAILURE) {
		return;
	}

	if (!helper) {
		RETURN_FALSE;
	} else {
		int  retval = 0;

		char *file;
		uint len;

		HashPosition pos;
		zval **data;

		if(Z_TYPE_P(helper)!=IS_ARRAY){
			convert_to_array(helper);
		}

		for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(helper), &pos);
			zend_hash_get_current_data_ex(Z_ARRVAL_P(helper), (void**)&data, &pos) == SUCCESS;
			zend_hash_move_forward_ex(Z_ARRVAL_P(helper), &pos) ){

				len = spprintf(&file, 0, "%s%s%s", "/usr/local/httpd/htdocs/cii/helpers/", Z_STRVAL_P(*data), ".php");

				retval = (zend_hash_exists(&EG(included_files), file, len + 1));
				if (retval) {
					continue;
				}

				retval = cii_loader_import(file, len, 0 TSRMLS_CC);

		}
		RETURN_BOOL(retval);
	}
}

zend_function_entry cii_loader_methods[] = {
	PHP_ME(cii_loader,__construct,NULL,ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
	PHP_ME(cii_loader,view,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(cii_loader,model,NULL,ZEND_ACC_PUBLIC)
	PHP_ME(cii_loader,helper,NULL,ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

PHP_MINIT_FUNCTION(cii_loader){
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce,"cii_loader",cii_loader_methods);
	cii_loader_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * Nesting level of the output buffering mechanism
	 *
	 * @var	int
	 */
	zend_declare_property_null(cii_loader_ce,ZEND_STRL("_cii_ob_level"),ZEND_ACC_PROTECTED TSRMLS_CC);
	/**
	 * List of loaded models
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_loader_ce,ZEND_STRL("_cii_models"),ZEND_ACC_PROTECTED TSRMLS_CC);
}