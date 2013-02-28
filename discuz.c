/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id: header 297205 2010-03-30 21:09:07Z johannes $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_discuz.h"

#include "ext/standard/base64.h"
#include "ext/standard/md5.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/microtime.h"
#include <string.h>
#include <time.h>

PHPAPI static void php_md5(char *, const char *, size_t);
PHPAPI static void php_discuz_authcode(unsigned char *, const unsigned char *, size_t, const char *);
PHPAPI static void php_discuz_auth_encode(smart_str *, const char *, size_t, const char *, size_t, int);
PHPAPI static int php_discuz_auth_decode(char *, const char *, size_t, const char *, size_t);
PHPAPI static void php_discuz_auth_initkey(char *, char *, char *, const char *, const char *, size_t);

/* If you declare any globals in php_discuz.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(discuz)
*/

/* True global resources - no need for thread safety here */
static int le_discuz;

/* {{{ discuz_functions[]
 *
 * Every user visible function must have an entry in discuz_functions[].
 */
const zend_function_entry discuz_functions[] = {
	PHP_FE(confirm_discuz_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(discuz_auth_encode,	NULL)
	PHP_FE(discuz_auth_decode,	NULL)
	{NULL, NULL, NULL}	/* Must be the last line in discuz_functions[] */
};
/* }}} */

/* {{{ discuz_module_entry
 */
zend_module_entry discuz_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"discuz",
	discuz_functions,
	PHP_MINIT(discuz),
	PHP_MSHUTDOWN(discuz),
	PHP_RINIT(discuz),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(discuz),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(discuz),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_DISCUZ
ZEND_GET_MODULE(discuz)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("discuz.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_discuz_globals, discuz_globals)
    STD_PHP_INI_ENTRY("discuz.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_discuz_globals, discuz_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_discuz_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_discuz_init_globals(zend_discuz_globals *discuz_globals)
{
	discuz_globals->global_value = 0;
	discuz_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(discuz)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(discuz)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(discuz)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(discuz)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(discuz)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "discuz support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_discuz_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_discuz_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "discuz", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

/* {{{ proto string discuz_auth_encode(string str, string key, int expiry)
    */
PHP_FUNCTION(discuz_auth_encode)
{
	char *str = NULL;
	char *key = NULL;
	int argc = ZEND_NUM_ARGS();
	int str_len;
	int key_len;
	long expiry;

	if (zend_parse_parameters(argc TSRMLS_CC, "ssl", &str, &str_len, &key, &key_len, &expiry) == FAILURE) 
		return;
	
	if(argc < 3) expiry = 0;

	smart_str buf = {0};//final data buffer
	php_discuz_auth_encode(&buf, str, str_len, key, key_len, expiry);
	ZVAL_STRINGL(return_value, buf.c, buf.len, 1);
	smart_str_free(&buf);
}
/* }}} */

/* {{{ proto string discuz_auth_decode(string str, string key)
    */
PHP_FUNCTION(discuz_auth_decode)
{
	char *str = NULL;
	char *key = NULL;
	int argc = ZEND_NUM_ARGS();
	int str_len;
	int key_len;

	if (zend_parse_parameters(argc TSRMLS_CC, "ss", &str, &str_len, &key, &key_len) == FAILURE) 
		return;

	char *buf = (char *)safe_emalloc(str_len, sizeof(char), 1);
	int len;
	len = php_discuz_auth_decode(buf, str, str_len, key, key_len);
	RETURN_STRINGL(buf, len, 1);
	efree(buf);
}
/* }}} */

PHPAPI static php_discuz_auth_encode(smart_str *buf, const char *str, int str_len, const char *key, int keylen, long expiry)
{
	char keya[33], keyb[33], keyc[DISCUZ_AUTH_CKEYLEN+1], cryptkey[65]={0};
        char ret[100], retmd5[33];
        char *pc;
	struct timeval tp = {0};
        smart_str strbuf = {0};
        if (gettimeofday(&tp, NULL)) {
                return;
        }
        snprintf(ret, 100, "%.8F %ld", tp.tv_usec / MICRO_IN_SEC, tp.tv_sec);

        php_md5(retmd5, ret, strlen(ret));
        bzero(keyc, sizeof(keyc));
        memcpy(keyc, retmd5 + 32 - DISCUZ_AUTH_CKEYLEN, DISCUZ_AUTH_CKEYLEN);

	php_discuz_auth_initkey(keya, keyb, cryptkey, keyc, key, keylen);
	//$string = sprintf('%010d', $expiry ? $expiry + time() : 0).substr(md5($string.$keyb), 0, 16).$string;
	smart_str_appendl(&strbuf, str, str_len);
	smart_str_appendl(&strbuf, keyb, 32);
	php_md5(retmd5, strbuf.c, strbuf.len);
	smart_str_free(&strbuf);

	snprintf(ret, sizeof(ret), "%010d", expiry ? expiry + time(NULL) : 0);
	smart_str_appendl(&strbuf, ret, 10);
	smart_str_appendl(&strbuf, retmd5, 16);
	smart_str_appendl(&strbuf, str, str_len);

	pc = (unsigned char *)safe_emalloc(strbuf.len, sizeof(char), 1);
	php_discuz_authcode(pc, strbuf.c, strbuf.len, cryptkey);

	//return $keyc.str_replace('=', '', base64_encode($result));
	char *result;
	int retlen;
	result = php_base64_encode((unsigned char*)pc, strbuf.len, &retlen);
	efree(pc);	

	int index, to;
	for(index = 0, to = 0; index <= retlen; index++) {
		if(*(result+index) != '=') {
			*(result+to) = *(result+index);
			to++;
		}
	}
	smart_str_appendl(buf, keyc, DISCUZ_AUTH_CKEYLEN);
	smart_str_appendl(buf, result, retlen - (index - to));
	smart_str_free(&strbuf);
}

PHPAPI static int php_discuz_auth_decode(char *buf, const char *str, size_t str_len, const char *key, size_t keylen){
	char keya[33], keyb[33], keyc[DISCUZ_AUTH_CKEYLEN+1], cryptkey[65]={0};
	unsigned char *base64result;
	int retlen;
	time_t expiry;
	char md5salt[16], md5buf[33], *authcodebuf;
	smart_str strbuf = {0};

	memcpy(keyc, str, DISCUZ_AUTH_CKEYLEN);
	php_discuz_auth_initkey(keya, keyb, cryptkey, keyc, key, keylen);

	base64result = php_base64_decode((unsigned char*)(str + DISCUZ_AUTH_CKEYLEN), str_len-DISCUZ_AUTH_CKEYLEN, &retlen);

	authcodebuf = (unsigned char *)safe_emalloc(retlen, sizeof(char), 1);
	php_discuz_authcode((unsigned char *)authcodebuf, base64result, retlen, cryptkey);

	//expiry
	bzero(md5buf, sizeof(md5buf));
	memcpy(md5buf, authcodebuf, 10);
	expiry = atoi(md5buf);
	//md5
	memcpy(md5salt, authcodebuf + 10, 16);
	//if((substr($result, 0, 10) == 0 || substr($result, 0, 10) - time() > 0) && substr($result, 10, 16) == substr(md5(substr($result, 26).$keyb), 0, 16)) {
	smart_str_appendl(&strbuf, authcodebuf + 26, retlen - 26);
	smart_str_appendl(&strbuf, keyb, 32);
	
	php_md5(md5buf, strbuf.c, strbuf.len);
	smart_str_free(&strbuf);

	if((expiry == 0 || expiry - time(NULL) > 0) && memcmp(md5salt, md5buf, 16) == 0){
		memcpy(buf, authcodebuf + 26, retlen-26);
		efree(authcodebuf);
		return retlen-26;
	}
	efree(authcodebuf);
	return 0;	
}


PHPAPI static void php_discuz_auth_initkey(char *keya, char *keyb, char *cryptkey, const char *keyc, const char *key, size_t keylen){
	char md5buf[33], buf[65] = {0};

	php_md5(md5buf, key, keylen);
	memcpy(buf, md5buf, 16);
	php_md5(keya, buf, 16);
	php_md5(keyb, md5buf + 16, 16);
	//$cryptkey = $keya.md5($keya.$keyc);
	memcpy(buf, keya, 32);
	memcpy(buf+32, keyc, DISCUZ_AUTH_CKEYLEN);
	php_md5(md5buf, buf, 32 + DISCUZ_AUTH_CKEYLEN);

	memcpy(cryptkey, keya, 32);
	memcpy(cryptkey + 32, md5buf, 32);
}

PHPAPI static void php_discuz_authcode(unsigned char *buf, const unsigned char *str, size_t str_len, const char *key){
	size_t key_len = 64;
	unsigned char box[256], rndkey[256], tmp;
	int a, i, j;

	for(i = 0; i < 256; i++){
		box[i] = i;
	}

	for(i = 0; i < 256; i++){
		rndkey[i] = key[i % key_len];
	}

	for(i = 0, j = 0; i < 256; i++){
		j = (j + box[i] + rndkey[i]) % 256;
		tmp = box[i];
		box[i] = box[j];
		box[j] = tmp;
	}

	for(a = 0, j = 0, i = 0; i < str_len; i++) {
		a = (a + 1) % 256;
		j = (j + box[a]) % 256;
		tmp = box[a];
		box[a] = box[j];
		box[j] = tmp;
		buf[i] = *(str+i) ^ (box[(box[a] + box[j]) % 256]);
	}
}

PHPAPI static void php_md5(char *buf, const char *str, size_t len){
	PHP_MD5_CTX context;
	unsigned char digest[16];

	PHP_MD5Init(&context);
	PHP_MD5Update(&context, str, len);
	PHP_MD5Final(digest, &context);
	make_digest_ex(buf, digest, 16);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
