<?php
/*
 * PHP-GTK - The PHP language bindings for GTK+
 *
 * Copyright (C) 2001,2002 Andrei Zmievski <andrei@php.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/* $Id$ */

$function_tpl = "
PHP_FUNCTION(%s)
{
%s	if (!php_gtk_parse_args(ZEND_NUM_ARGS(), \"%s\"%s))
		return;

%s%s
}\n\n";

$function_call_tpl = "%s(%s)";

$method_tpl = "
PHP_FUNCTION(%s)
{
%s	NOT_STATIC_METHOD();

	if (!php_gtk_parse_args(ZEND_NUM_ARGS(), \"%s\"%s))
		return;

%s%s
}\n\n";

$method1_call_tpl = "%s(%s(PHP_GTK_GET(this_ptr))%s)";
$method2_call_tpl = "%s(PHP_%s_GET(this_ptr)%s)";

$constructor_tpl = "
PHP_FUNCTION(%s)
{
%s	NOT_STATIC_METHOD();

	if (!php_gtk_parse_args(ZEND_NUM_ARGS(), \"%s\"%s)) {
		php_gtk_invalidate(this_ptr);
		return;
	}

%s	wrapped_obj = (%s *)%s(%s);
	if (!wrapped_obj) {
		php_error(E_WARNING, \"%%s(): could not create %s object\",
				  get_active_function_name(TSRMLS_C));
		php_gtk_invalidate(this_ptr);
		return;
	}
%s%s
}\n\n";

$gtk_object_init_tpl = "
	php_gtk_object_init(wrapped_obj, this_ptr);\n";

$non_gtk_object_init_tpl = "
	php_gtk_set_object(this_ptr, wrapped_obj, le_%s);\n";

$function_entry_tpl = "\t{\"%s\",	PHP_FN(%s),	%s},\n";
$functions_decl_tpl = "
static function_entry php_%s_functions[] = {
";

$register_getter_tpl = "\tphp_gtk_register_prop_getter(%s, %s_get_property);\n";

$prop_check_tpl =
"%sif (!strcmp(prop_name, \"%s\")) {
	%s
	}";

$prop_getter_tpl = "
static void %s_get_property(zval *return_value, zval *object, char *prop_name, int *result)
{
	*result = SUCCESS;

%s

	*result = FAILURE;
}\n\n";

$init_class_tpl = "
	INIT_OVERLOADED_CLASS_ENTRY(ce, \"%s\", php_%s_functions, NULL, %s, php_gtk_set_property);\n";

$struct_class_tpl = "
	INIT_CLASS_ENTRY(ce, \"%s\", php_%s_functions);\n";

$get_type_tpl = "
PHP_FUNCTION(%s_get_type)
{
	if (!php_gtk_parse_args(ZEND_NUM_ARGS(), \"\"))
		return;

	RETURN_LONG(%s_get_type());
}\n\n";

$register_classes_tpl = "
void php_%s_register_classes(void)
{
	TSRMLS_FETCH();
%s
}\n";


//$register_class_tpl = "\t%s = zend_register_internal_class_ex(&ce, %s, NULL TSRMLS_CC);\n";
$register_class_tpl = "
	%s = php_gtk_register_class(\"%s\", php_%s_functions, %s, %d, 0, %s TSRMLS_CC);\n";

$class_entry_tpl = "PHP_GTK_EXPORT_CE(%s);\n";

$class_prop_list_header = "static char *php_%s_properties[] = {\n";
$class_prop_list_footer = "\tNULL\n};\n\n";

$struct_init_tpl = "
zval *PHP_GTK_EXPORT_FUNC(php_%s_new)(%s *obj)
{
	zval *result;
	TSRMLS_FETCH();

    if (!obj) {
        MAKE_STD_ZVAL(result);
        ZVAL_NULL(result);
        return result;
    }

    MAKE_STD_ZVAL(result);
    object_init_ex(result, %s);

%s
    return result;
}\n\n";

$struct_construct_tpl = "
PHP_FUNCTION(%s)
{
%s	NOT_STATIC_METHOD();

	if (!php_gtk_parse_args(ZEND_NUM_ARGS(), \"%s\"%s)) {
		php_gtk_invalidate(this_ptr);
		return;
	}

%s}\n\n";

$struct_get_tpl = "
zend_bool PHP_GTK_EXPORT_FUNC(php_%s_get)(zval *wrapper, %s *obj)
{
    zval **item;
	TSRMLS_FETCH();

    if (!php_gtk_check_class(wrapper, %s))
        return 0;

%s
    return 1;
}\n\n";

/* vim: set et sts=4: */
?>
