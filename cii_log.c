#include "cii_log.h"

zend_class_entry *cii_log_ce;
/**
* Class constructor
*
* @return	void
*
* public function __construct()
*/
PHP_METHOD(cii_log, __construct)
{
	/*
	*	output log
	*/
	cii_write_log(3, "Log Class Initialized");
}
/*
* Write Log File
*/
ZEND_API int cii_write_log(int level, char *message)
{
	/*
	* 0 = Disables logging, Error logging TURNED OFF
	* 1 = Error Messages (including PHP errors)
	* 2 = Debug Messages
	* 3 = Informational Messages
	* 4 = All Messages
	*/
	static int log_threshold = 4;
	/*
	*	if disables logging
	*/
	if( !log_threshold ){
		return 0;
	}
	/*
	*	if level overflow
	*/
	if( level > log_threshold ){
		return 0;
	}
	/*
	*	get time
	*/
	time_t t = time(NULL);
	/*
	*	set filename
	*/
	char filename[19];   
	strftime(filename, 19, "log-%Y-%m-%d.php", localtime(&t));
	/*
	*	set filepath
	*/
	char *filepath;
	if( !CII_G(apppath) ){
		cii_get_apppath();
	}
	spprintf(&filepath, 0, "%s%s%s", CII_G(apppath), "logs/", filename);
	/*
	*	set level title
	*/
	char *level_upper;
	if( level == 1 ){
		level_upper = "ERROR";
	}else if( level == 2 ){
		level_upper = "DEBUG";
	}else if( level == 3 ){
		level_upper = "INFO";
	}else if( level == 4 ){
		level_upper = "ALL";
	}
	/*
	*	get time
	*/
	char time[20];
	strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));
	/*
	*	open file
	*/
	FILE *f;
	if( (f = fopen(filepath, "a")) == NULL ){
		php_error(E_WARNING, "Cannot open log file: logs/%s", filename);
		efree(filepath);
		return 0;
	}
	/*
	*	write log
	*/
	char *log;
	spprintf(&log, 0, "%s%s%s%s%s%s", level_upper, " - ", time, " --> ", message, "\r\n");
	fputs(log, f);
	/*
	*	free used memory
	*/
	efree(log);
	fclose(f);
	efree(filepath);
	/*
	* return state
	*/
	return 1;
}
/**
* Write Log File
*
* Generally this function will be called using the global log_message() function
*
* @param	string	the error level: 'error', 'debug' or 'info'
* @param	string	the error message
* @return	bool
*
* public function write_log($level, $msg)
*/
PHP_METHOD(cii_log, write_log)
{
	char *level;
	uint level_len;
	char *message;
	uint message_len;
	char *log_threshold[4] = {"error", "debug", "info", "all"};
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ss" ,&level, &level_len, &message, &message_len) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	char *level_lower = zend_str_tolower_dup(level, level_len);
	int p = -1;
	int i;
	for (i = 0; i < 4; i++)
	{
		if( !strcmp(level_lower, log_threshold[i]) ){
			p = i+1;
			break;
		}
	}
	efree(level_lower);
	if( p >= 0 ){
		char retval;
		retval = cii_write_log(p, message);
		if( return_value_used ){
			RETURN_BOOL(retval);
		}
	}else{
		RETURN_BOOL(0);
	}
}	

zend_function_entry cii_log_methods[] = {
	PHP_ME(cii_log, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(cii_log, write_log, NULL, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

PHP_MINIT_FUNCTION(cii_log)
{
	/**
	 * Logging Class
	 */
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "CII_Log", cii_log_methods);
	cii_log_ce = zend_register_internal_class(&ce TSRMLS_CC);
}