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
	php_printf("Info: Log Class Initialized\n");
}
/*
* php_strtoupper
*/
static char* zend_str_toupper_dup(char *s, size_t len)
{
	unsigned char *c, *e, *r, *p;

	r = p = (char*)emalloc(sizeof(char)*len+1);


	c = (unsigned char *)s;
	e = (unsigned char *)c+len;

	while (c < e) {
		*p = toupper(*c);
		p++;
		c++;
	}
	*p = '\0';
	return r;
}
/*
* Write Log File
*/
ZEND_API int cii_write_log(char *level, char *message)
{
	time_t t = time(NULL);
	char filename[19];   
	strftime(filename, 19, "log-%Y-%m-%d.php", localtime(&t));

	char *filepath;
	if( !CII_G(apppath) ){
		cii_get_apppath();
	}
	spprintf(&filepath, 0, "%s%s%s", CII_G(apppath), "logs/", filename);
	FILE *f;
	f = fopen(filepath, "a");
	char time[20];
	strftime(time, 20, "%Y-%m-%d %H:%M:%S", localtime(&t));
	char *level_upper = zend_str_toupper_dup(level, strlen(level));
	char *log;
	spprintf(&log, 0, "%s%s%s%s%s%s", level_upper, " - ", time, " --> ", message, "\r\n");
	fputs(log, f);
	fclose(f);
	efree(filepath);
	efree(log);
	efree(level_upper);
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
	cii_write_log("Info", "load");
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