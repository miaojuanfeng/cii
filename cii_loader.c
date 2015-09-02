#include "cii_loader.h"

zend_class_entry *cii_loader_ce;
extern zend_class_entry *cii_output_ce;
extern ZEND_API void cii_append_output(zend_class_entry *cii_output_ce, zval *cii_output_obj, char *output_str);

//usefulless.bing mei you sen me ruan yong
/*ZEND_BEGIN_ARG_INFO_EX(cii_loader_view_arginfo,0,0,1)
	ZEND_ARG_INFO(0,view)
	ZEND_ARG_ARRAY_INFO(0,data,1)
	ZEND_ARG_INFO(0,return)
ZEND_END_ARG_INFO()*/
ZEND_BEGIN_ARG_INFO_EX(cii_model___get_arginfo, 0, 0, 1)
	ZEND_ARG_INFO(0, key)
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

PHP_FUNCTION(cii_model___get)
{
	char *key;
	uint key_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s" ,&key, &key_len) == FAILURE) {
		return;
	}
	zval *value = zend_read_property(CII_G(cii_controller_ce), CII_G(cii_controller), key, key_len, 1 TSRMLS_CC);
	RETURN_ZVAL(value, 1, 0);
}	


ZEND_API int cii_loader_import(char *path, int len, int use_path TSRMLS_DC) {
	zend_file_handle file_handle;
	zend_op_array 	*op_array;
	char realpath[MAXPATHLEN];

	if (!VCWD_REALPATH(path, realpath)) {
		php_error(E_ERROR, "Unable to load the requested file: %s", path);
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
	/*
	* init cii_loader::_ob_level
	*/
	zval *_ob_level;
	MAKE_STD_ZVAL(_ob_level);
	ZVAL_LONG(_ob_level, php_output_get_level(TSRMLS_C));
	zend_update_property(cii_loader_ce, getThis(), ZEND_STRL("_ob_level"), _ob_level TSRMLS_CC);
	zval_ptr_dtor(&_ob_level);
	/*
	* init cii_loader::_models
	*/
	zval *_models;
	MAKE_STD_ZVAL(_models);
	array_init(_models);
	zend_update_property(cii_loader_ce, getThis(), ZEND_STRL("_models"), _models TSRMLS_CC);
	zval_ptr_dtor(&_models);
	/*
	* output log
	*/
	php_printf("Info: Loader Class Initialized\n");
}

PHP_METHOD(cii_loader,view){
	char *view;
	uint view_len;
	HashTable *data = NULL;
	uint is_return = 0;
	zval **value;
	char *file;
	uint file_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|H!l" ,&view, &view_len, &data, &is_return) == FAILURE) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	if (!view_len) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	if( !CII_G(apppath) ){
		cii_get_apppath();
	}
	file_len = spprintf(&file, 0, "%s%s%s%s", CII_G(apppath), "views/", view, ".php");

	if(zend_hash_exists(&EG(included_files), file, file_len + 1)){
		efree(file);
		RETURN_ZVAL(getThis(), 1, 0);
	}

	/*if (EG(active_symbol_table)) {
		zend_rebuild_symbol_table(TSRMLS_C);
	}*/
	CII_ALLOC_ACTIVE_SYMBOL_TABLE();

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
			CII_IF_ISREF_THEN_SEPARATE_ELSE_ADDREF(value);
			zend_hash_update(EG(active_symbol_table), key, key_len, value, sizeof(zval *), NULL);
		}
	}

	if(php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC) == SUCCESS){
		if(is_return){	
			cii_loader_import(file, file_len, 0 TSRMLS_CC);
			php_output_get_contents(return_value TSRMLS_CC);
			php_output_discard(TSRMLS_C);
		}else{
			cii_loader_import(file, file_len, 0 TSRMLS_CC);
			zval *output;
			MAKE_STD_ZVAL(output);
			php_output_get_contents(output TSRMLS_CC);
			cii_append_output(cii_output_ce, load_class("Output", 0, NULL), Z_STRVAL_P(output));
			zval_ptr_dtor(&output);
			php_output_discard(TSRMLS_C);
		}
	}else{
		php_error(E_WARNING, "failed to create buffer");
	}

	efree(file);

	CII_DESTROY_ACTIVE_SYMBOL_TABLE();

	if(!is_return){
		RETURN_ZVAL(getThis(), 1, 0);
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
	char *file;
	uint file_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s!", &model, &model_len, &name, &name_len) == FAILURE){
		RETURN_ZVAL(getThis(), 1, 0);
	}

	if(!model_len){
		RETURN_ZVAL(getThis(), 1, 0);
	}

	if( !CII_G(apppath) ){
		cii_get_apppath();
	}
	file_len = spprintf(&file, 0, "%s%s%s%s", CII_G(apppath), "models/", model, ".php");

	if (zend_hash_exists(&EG(included_files), file, file_len + 1)){
		efree(file);
		RETURN_ZVAL(getThis(), 1, 0);
	}

	CII_ALLOC_ACTIVE_SYMBOL_TABLE();

	cii_loader_import(file, file_len, 0 TSRMLS_CC);

	CII_DESTROY_ACTIVE_SYMBOL_TABLE();

	//add new object property to cii_controller class
	zend_class_entry **ce;
	char *model_lower = zend_str_tolower_dup(model, model_len);
	if( zend_hash_find(CG(class_table), model_lower, model_len+1, (void**)&ce) == SUCCESS ){
		if(name && CII_G(cii_controller_ce) && CII_G(cii_controller)){
			if( Z_TYPE_P(zend_read_property(CII_G(cii_controller_ce), CII_G(cii_controller), name, name_len, 1 TSRMLS_CC)) != IS_NULL ){
				php_error(E_ERROR, "The model name you are loading is the name of a resource that is already being used: %s", name);
			}
		}else{
			name = model;
			name_len = model_len;
		}
		/*
		*	new ce object
		*/
		zval *new_object;
		MAKE_STD_ZVAL(new_object);
		object_init_ex(new_object, *ce);
		/*
		*	add new object to cii_controller
		*/
		if( CII_G(cii_controller_ce) && CII_G(cii_controller) ){
			zend_update_property(CII_G(cii_controller_ce), CII_G(cii_controller), name, name_len, new_object TSRMLS_CC);
		}
		/*
		*	add new model to cii_loader::_models
		*/
		zval *model_name;
		MAKE_STD_ZVAL(model_name);
		ZVAL_STRING(model_name, name, 1);
		zval *_models = zend_read_property(cii_loader_ce, getThis(), ZEND_STRL("_models"), 1 TSRMLS_CC);
		zend_hash_next_index_insert(Z_ARRVAL_P(_models), &model_name, sizeof(zval *), NULL);
		/*
		*	add __get method
		*/
		if( !zend_hash_exists(&(*ce)->function_table, "__get", 6) ){
			zend_function *func_pDest;
			zend_function func;
			func.internal_function.type = ZEND_INTERNAL_FUNCTION;
			func.internal_function.function_name = "__get";
			func.internal_function.scope = *ce;
			func.internal_function.fn_flags = ZEND_ACC_PUBLIC;
			func.internal_function.num_args = 0;
			func.internal_function.required_num_args = 0;
			func.internal_function.arg_info = (zend_arg_info*)cii_model___get_arginfo+1;
			func.internal_function.handler = ZEND_FN(cii_model___get);
			if( zend_hash_add(&(*ce)->function_table, "__get", 6, &func, sizeof(zend_function), (void**)&func_pDest) == FAILURE ){
				php_error(E_WARNING, "add __get method failed");
			}else{
				(*ce)->__get = func_pDest;
				(*ce)->__get->common.fn_flags &= ~ZEND_ACC_ALLOW_STATIC;
			}
		}
		/*
		*	cii_model __construct function
		*/
		php_printf("Info: Model Class Initialized\n");
		/*
		*	call new object construct function
		*/
		if (zend_hash_exists(&(*ce)->function_table, "__construct", 12)) {
			zval *retval;
			CII_CALL_USER_METHOD_EX(&new_object, "__construct", &retval, 0, NULL);
			zval_ptr_dtor(&retval);
		}
		zval_ptr_dtor(&new_object);
	}
	efree(model_lower);
	efree(file);
	RETURN_ZVAL(getThis(), 1, 0);
}
/**
* Helper Loader
*
* @param	string|string[]	$helpers	Helper name(s)
* @return	object
*/
PHP_METHOD(cii_loader, helper){
	zval *helper = NULL;
	char *file;
	uint file_len;
	HashPosition pos;
	zval **data;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z!" ,&helper) == FAILURE) {
		RETURN_ZVAL(getThis(), 1, 0);
	}

	if(!helper){
		RETURN_ZVAL(getThis(), 1, 0);
	} 

	if(Z_TYPE_P(helper)!=IS_ARRAY){
		convert_to_array(helper);
	}

	CII_ALLOC_ACTIVE_SYMBOL_TABLE();

	if( !CII_G(apppath) ){
		cii_get_apppath();
	}
	for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(helper), &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(helper), (void**)&data, &pos) == SUCCESS;
		zend_hash_move_forward_ex(Z_ARRVAL_P(helper), &pos) ){
			file_len = spprintf(&file, 0, "%s%s%s%s", CII_G(apppath), "helpers/", Z_STRVAL_P(*data), ".php");
			if(zend_hash_exists(&EG(included_files), file, file_len + 1)){
				efree(file);
				continue;
			}
			cii_loader_import(file, file_len, 0 TSRMLS_CC);
			efree(file);
	}

	CII_DESTROY_ACTIVE_SYMBOL_TABLE();

	RETURN_ZVAL(getThis(), 1, 0);
}
/**
* Database Loader
*
* @param	Array	$config		Database configuration options
* @return	object
*
* public function database($config = NULL)
*/
PHP_METHOD(cii_loader, database){
	zend_class_entry **mysqli_ce;
	zval **db_arr;
	zval **active_group;
	HashPosition pos;
	zval **config_arr;
	zval **params[4];

	if( zend_hash_find(EG(class_table), "mysqli", 7, (void**)&mysqli_ce) == FAILURE ){
		zend_error(E_ERROR, "mysqli class has not initialized yet");
	}

	char *file;
	uint file_len;

	if( !CII_G(apppath) ){
		cii_get_apppath();
	}
	file_len = spprintf(&file, 0, "%s%s", CII_G(apppath), "configs/database.php");

	if(zend_hash_exists(&EG(included_files), file, file_len + 1)){
		return;
	}

	CII_ALLOC_ACTIVE_SYMBOL_TABLE();

	cii_loader_import(file, file_len, 0 TSRMLS_CC);

	if( zend_hash_find(EG(active_symbol_table), "active_group", 13, (void**)&active_group) == FAILURE ){
		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		efree(file);
		php_error(E_ERROR, "You have not specified a database connection group via $active_group in your config file: %s", file);
	}

	if( zend_hash_find(EG(active_symbol_table), "db", 3, (void**)&db_arr) == FAILURE ){
		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		efree(file);
		php_error(E_ERROR, "No database connection settings were found in the database config file: %s", file);
	}

	if( zend_hash_find(Z_ARRVAL_P(*db_arr), Z_STRVAL_P(*active_group), Z_STRLEN_P(*active_group)+1, (void**)&config_arr) == FAILURE ){
		char *active_group_error = zend_strndup(Z_STRVAL_P(*active_group), Z_STRLEN_P(*active_group));
		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		efree(file);
		php_error(E_ERROR, "You have specified an invalid database connection group (%s) in your config/database.php file: %s", active_group_error, file);
	}

	zval *hostname, *username, *password, *database;
	MAKE_STD_ZVAL(hostname); MAKE_STD_ZVAL(username); MAKE_STD_ZVAL(password); MAKE_STD_ZVAL(database);
	ZVAL_NULL(hostname); ZVAL_NULL(username); ZVAL_NULL(password); ZVAL_NULL(database);

	for(zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(*config_arr), &pos);
		zend_hash_has_more_elements_ex(Z_ARRVAL_P(*config_arr), &pos) == SUCCESS;
	    zend_hash_move_forward_ex(Z_ARRVAL_P(*config_arr), &pos)){
		char *key;
		uint key_len;
		ulong idx;
		zval **value;

		zend_hash_get_current_key_ex(Z_ARRVAL_P(*config_arr), &key, &key_len, &idx, 0, &pos);
		zend_hash_get_current_data_ex(Z_ARRVAL_P(*config_arr), (void**)&value, &pos);

		zend_str_tolower(key,key_len);
		if( key_len == 9 && !memcmp(key, "hostname", 9) ){
			efree(hostname);
			hostname = *value;
		}else if( key_len == 9 && !memcmp(key, "username", 9) ){
			efree(username);
			username = *value;
		}else if( key_len == 9 && !memcmp(key, "password", 9) ){
			efree(password);
			password = *value;
		}else if( key_len == 9 && !memcmp(key, "database", 9) ){
			efree(database);
			database = *value;
		}
	}

	params[0] = &hostname;
	params[1] = &username;
	params[2] = &password;
	params[3] = &database;

	zval *db_obj;
	MAKE_STD_ZVAL(db_obj);
	object_init_ex(db_obj, *mysqli_ce);
	if( CII_G(cii_controller_ce) && CII_G(cii_controller) ){
		zend_update_property(CII_G(cii_controller_ce), CII_G(cii_controller), ZEND_STRL("db"), db_obj TSRMLS_CC);
	}	
	

	zval *func_name;
	zval *retval;
	MAKE_STD_ZVAL(func_name);
	ZVAL_STRING(func_name, "connect", 1);
	if( call_user_function_ex(NULL, &db_obj, func_name, &retval, 4, params, 0, NULL TSRMLS_CC) == FAILURE ){
		CII_DESTROY_ACTIVE_SYMBOL_TABLE();
		php_error(E_ERROR, "Call mysqli::connect function failed");
	}
	zval_ptr_dtor(&func_name);
	zval_ptr_dtor(&retval);
	zval_ptr_dtor(&db_obj);

	CII_DESTROY_ACTIVE_SYMBOL_TABLE();
	efree(file);
}

zend_function_entry cii_loader_methods[] = {
	PHP_ME(cii_loader, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_loader, view, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_loader, model, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_loader, helper, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(cii_loader, database, NULL, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

PHP_MINIT_FUNCTION(cii_loader){
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Loader", cii_loader_methods);
	cii_loader_ce = zend_register_internal_class(&ce TSRMLS_CC);
	/**
	 * Nesting level of the output buffering mechanism
	 *
	 * @var	int
	 */
	zend_declare_property_null(cii_loader_ce, ZEND_STRL("_ob_level"), ZEND_ACC_PROTECTED TSRMLS_CC);
	/**
	 * List of loaded models
	 *
	 * @var	array
	 */
	zend_declare_property_null(cii_loader_ce, ZEND_STRL("_models"), ZEND_ACC_PROTECTED TSRMLS_CC);
}