/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: fanhengguang@126.com                                                             |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_xml2array.h"

#include <libxml/parser.h>
#include <libxml/tree.h>

static zval php_xml2array_loop(xmlNode *node);
static void php_xml2array_parse(zval* return_value, char * xml_str, long xml_len);
static void php_xml2array_add_val (zval *ret,const xmlChar *name, zval *r, char *son_key);
static void php_xml2array_get_properties (xmlNodePtr cur_node, zval * nodes, char *name);

/* True global resources - no need for thread safety here */
static int le_xml2array;

PHP_FUNCTION(xml2array)
{
	char *arg = NULL;
	long arg_len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	php_xml2array_parse(return_value, arg, arg_len);
}

static void php_xml2array_parse(zval* return_value, char * xml_str, long xml_len) {

	xmlKeepBlanksDefault(0);
	xmlDoc *doc = xmlParseMemory(xml_str, xml_len);
	xmlNode *root_element;

	if (doc == NULL) {
		return;
	} else {
		root_element = xmlDocGetRootElement(doc);
		zval z;
		z = php_xml2array_loop(root_element);
		*return_value = z;
		xmlFreeDoc(doc);
	}

	xmlCleanupParser();
}


static zval php_xml2array_loop(xmlNodePtr a_node) {
	xmlNodePtr cur_node;
	zval ret;
	array_init(&ret);

	if (a_node->children == NULL) {
		zval r;
		ZVAL_STRING(&r, "");
		php_xml2array_add_val(&ret, a_node->name, &r, NULL);
	} else {
		for (cur_node = a_node->children; cur_node; cur_node = cur_node->next) {
			char *cur_name = NULL;
			zval r;
			if (cur_node->type == XML_ELEMENT_NODE) {
				cur_name = (char*)cur_node->name;
				r =  php_xml2array_loop(cur_node);
				php_xml2array_get_properties (cur_node, &r, cur_name);
			} else if (cur_node->type == XML_CDATA_SECTION_NODE || cur_node->type == XML_TEXT_NODE) {
				xmlChar *z = xmlNodeGetContent(cur_node);
				ZVAL_STRING(&r, z);
				xmlFree(z);
			}

			php_xml2array_add_val(&ret, a_node->name, &r, cur_name);
		}
	}
	return ret;
}

static void php_xml2array_get_properties (xmlNodePtr cur_node, zval *nodes, char *name) {
	if (cur_node->properties) {
		xmlAttrPtr attr =  NULL;

		zval *tmp;
		zend_string *name_zend_str = zend_string_init(name, strlen(name), 0);
		tmp = zend_symtable_find(Z_ARRVAL_P(nodes), name_zend_str);
		if (tmp == NULL) {
			zend_string_free(name_zend_str);
			return;//this should not happen
		}
		zval *target = tmp;
		if (Z_TYPE_P(tmp) != IS_ARRAY) {
			zval value_zval;
			array_init(&value_zval);
			target = &value_zval;
			zval copy;
			ZVAL_COPY(&copy, tmp);
			add_assoc_zval(&value_zval, "value", &copy);
			zend_symtable_update(Z_ARRVAL_P(nodes), name_zend_str, &value_zval);
		}
		zend_string_free(name_zend_str);
		for(attr = cur_node->properties; NULL != attr; attr = attr->next) {
			xmlChar * prop = NULL;
			prop = xmlGetProp(cur_node, attr->name);
			char *attr_name = (char*)attr->name;
			zend_string *attr_name_zend_str = zend_string_init(attr_name, strlen(attr_name), 0);
			zval attr_zval;
			ZVAL_STRING(&attr_zval, prop);
			zend_symtable_update(Z_ARRVAL_P(target), attr_name_zend_str, &attr_zval);
			zend_string_release(attr_name_zend_str);
			xmlFree(prop);
		}
	}
}

/**
 * @ret 父亲zval
 * @name 父亲name
 * @r 子zval
 * @son_name 子name
 */
static void php_xml2array_add_val (zval *ret,const xmlChar *name, zval *r, char *son_key) {
	zval *tmp = NULL;
	char *key = (char *)name;

	zend_string *key_zend_str = zend_string_init(key, strlen(key), 0);
	tmp =  zend_symtable_find(Z_ARRVAL_P(ret),  key_zend_str);
	zend_string_free(key_zend_str);

	if(son_key != NULL && tmp != NULL) {
		zend_string *son_key_zend_str = zend_string_init(son_key, strlen(son_key), 0);
		zval *son_val =  zend_symtable_find(Z_ARRVAL_P(r),  son_key_zend_str);
		zval *tmp_val = zend_symtable_find(Z_ARRVAL_P(tmp),  son_key_zend_str);

		zval son_val_copy;
		ZVAL_COPY(&son_val_copy, son_val);

		if (tmp_val != NULL) {//已经包含同名子元素
			if (Z_TYPE_P(tmp_val)  == IS_ARRAY && zend_hash_index_exists(Z_ARRVAL_P(tmp_val), 0)) {//第二次添加同名子元素
				add_next_index_zval(tmp_val, &son_val_copy);
			} else {//第一次添加同名子元素
				zval son_arr;
				array_init(&son_arr);
				zval copy;
				ZVAL_COPY(&copy, tmp_val);
				add_next_index_zval(&son_arr, &copy);
				add_next_index_zval(&son_arr, &son_val_copy);
				zend_symtable_update(Z_ARRVAL_P(tmp), son_key_zend_str, &son_arr);
			}
		} else {
			add_assoc_zval(tmp, son_key, &son_val_copy);
		}
		zval_dtor(r);
		zend_string_free(son_key_zend_str);
	} else {
		add_assoc_zval(ret, key, r);
	}
}


PHP_MINIT_FUNCTION(xml2array)
{
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(xml2array)
{
	return SUCCESS;
}

PHP_RINIT_FUNCTION(xml2array)
{
#if defined(COMPILE_DL_XML2ARRAY) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(xml2array)
{
	return SUCCESS;
}

PHP_MINFO_FUNCTION(xml2array)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "xml2array support", "enabled");
	php_info_print_table_end();

}

const zend_function_entry xml2array_functions[] = {
	PHP_FE(xml2array,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in xml2array_functions[] */
};

zend_module_entry xml2array_module_entry = {
	STANDARD_MODULE_HEADER,
	"xml2array",
	xml2array_functions,
	PHP_MINIT(xml2array),
	PHP_MSHUTDOWN(xml2array),
	PHP_RINIT(xml2array),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(xml2array),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(xml2array),
	PHP_XML2ARRAY_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_XML2ARRAY
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(xml2array)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
