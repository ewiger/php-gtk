/*
 * PHP-GTK - The PHP language bindings for GTK+
 *
 * Copyright (C) 2001 Andrei Zmievski <andrei@php.net>
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

#include "php_gtk.h"

#if HAVE_PHP_GTK

int le_gtk;
HashTable php_gtk_prop_getters;
HashTable php_gtk_prop_setters;
HashTable php_gtk_rsrc_hash;

static const char *php_gtk_wrapper_key = "php_gtk::wrapper";

void php_gtk_object_init(GtkObject *obj, zval *wrapper)
{
	gtk_object_ref(obj);
	gtk_object_sink(obj);

	php_gtk_set_object(wrapper, obj, le_gtk);
}

void php_gtk_set_object(zval *wrapper, void *obj, int rsrc_type)
{
	zval *handle;
	zval **rsrc;

	MAKE_STD_ZVAL(handle);
	Z_TYPE_P(handle) = IS_LONG;
	if (rsrc_type == le_gtk) {
		Z_LVAL_P(handle) = zend_list_insert(obj, rsrc_type);
		zval_add_ref(&wrapper);
		gtk_object_set_data_full(obj, php_gtk_wrapper_key, wrapper, php_gtk_destroy_notify);
	} else {
		if (zend_hash_index_find(&php_gtk_rsrc_hash, (long)obj, (void **)&rsrc) == SUCCESS)
			Z_LVAL_P(handle) = Z_LVAL_PP(rsrc);
		else {
			Z_LVAL_P(handle) = zend_list_insert(obj, rsrc_type);
			zval_add_ref(&handle);
			zend_hash_index_update(&php_gtk_rsrc_hash, (long)obj, (void *)&handle, sizeof(zval *), NULL);
		}
	}
	zend_hash_index_update(Z_OBJPROP_P(wrapper), 0, &handle, sizeof(zval *), NULL);
}

void *php_gtk_get_object(zval *wrapper, int rsrc_type)
{
	void *obj;
	zval **handle;
	int type;
	
	if (Z_TYPE_P(wrapper) != IS_OBJECT) {
		php_error(E_ERROR, "Wrapper is not an object");
	}
	if (zend_hash_index_find(Z_OBJPROP_P(wrapper), 0, (void **)&handle) == FAILURE) {
		php_error(E_ERROR, "Underlying object missing");
	}
	obj = zend_list_find(Z_LVAL_PP(handle), &type);
	if (!obj || type != rsrc_type) {
		php_error(E_ERROR, "Underlying object missing or of invalid type");
	}

	return obj;
}

int php_gtk_get_enum_value(GtkType enum_type, zval *enum_val, int *result)
{
	if (!enum_val)
		return 0;

	if (Z_TYPE_P(enum_val) == IS_LONG) {
		*result = Z_LVAL_P(enum_val);
		return 1;
	} else if (Z_TYPE_P(enum_val) == IS_STRING) {
		GtkEnumValue *info = gtk_type_enum_find_value(enum_type, Z_STRVAL_P(enum_val));
		if (!info) {
			php_error(E_WARNING, "Could not translate enum value '%s'", Z_STRVAL_P(enum_val));
			return 0;
		}
		*result = info->value;
		return 1;
	}

	php_error(E_WARNING, "enum values must be integers or strings");
	return 0;
}

int php_gtk_get_flag_value(GtkType flag_type, zval *flag_val, int *result)
{
	if (!flag_val)
		return 0;

	if (Z_TYPE_P(flag_val) == IS_LONG) {
		*result = Z_LVAL_P(flag_val);
		return 1;
	} else if (Z_TYPE_P(flag_val) == IS_STRING) {
		GtkFlagValue *info = gtk_type_flags_find_value(flag_type, Z_STRVAL_P(flag_val));
		if (!info) {
			php_error(E_WARNING, "Could not translate flag value '%s'", Z_STRVAL_P(flag_val));
			return 0;
		}
		*result = info->value;
		return 1;
	} else if (Z_TYPE_P(flag_val) == IS_ARRAY) {
		zval **flag;

		for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(flag_val));
			 zend_hash_get_current_data(Z_ARRVAL_P(flag_val), (void **)&flag) == SUCCESS;
			 zend_hash_move_forward(Z_ARRVAL_P(flag_val))) {
			if (Z_TYPE_PP(flag) == IS_LONG)
				*result |= Z_LVAL_PP(flag);
			else if (Z_TYPE_PP(flag) == IS_STRING) {
				GtkFlagValue *info = gtk_type_flags_find_value(flag_type, Z_STRVAL_PP(flag));
				if (!info) {
					php_error(E_WARNING, "Could not translate flag value '%s'", Z_STRVAL_PP(flag));
					return 0;
				}
				*result |= info->value;
			} else {
				php_error(E_WARNING, "flag arrays can contain only integers or strings");
				return 0;
			}
		}

		return 1;
	}

	php_error(E_WARNING, "flag values must be integers or strings");
	return 0;
}

/* Generic callback marshal. */
void php_gtk_callback_marshal(GtkObject *o, gpointer data, guint nargs, GtkArg *args)
{
	zval *gtk_args;
	zval *callback_data = (zval *)data;
	zval **callback, **extra = NULL, **pass_object = NULL;
	zval **callback_filename = NULL, **callback_lineno = NULL;
	zval *wrapper = NULL;
	zval *params;
	zval *retval = NULL;
	zval *tmp;
	zval ***signal_args;
	char *callback_name;
	ELS_FETCH();

	/* Callback is always passed as the first element. */
	zend_hash_index_find(Z_ARRVAL_P(callback_data), 0, (void **)&callback);

	/*
	 * If there is more than one element, it will be an array of:
	 *  [1] an array of extra arguments
	 *  [2] a flag indidicating whether the object should be passed to the
	 *      callback
	 *  [3] the filename where the callback was specified
	 *  [4] the line number where the callback was specified
	 */
	if (zend_hash_num_elements(Z_ARRVAL_P(callback_data)) > 1) {
		zend_hash_index_find(Z_ARRVAL_P(callback_data), 1, (void **)&extra);
		zend_hash_index_find(Z_ARRVAL_P(callback_data), 2, (void **)&pass_object);
		zend_hash_index_find(Z_ARRVAL_P(callback_data), 3, (void **)&callback_filename);
		zend_hash_index_find(Z_ARRVAL_P(callback_data), 4, (void **)&callback_lineno);
	}

	if (!php_gtk_is_callable(*callback, 0, &callback_name)) {
		if (callback_filename)
			php_error(E_WARNING, "Unable to call signal callback '%s' specified in %s on line %d", callback_name, Z_STRVAL_PP(callback_filename), Z_LVAL_PP(callback_lineno));
		else
			php_error(E_WARNING, "Unable to call callback '%s'", callback_name);
		efree(callback_name);
		return;
	}

	gtk_args = php_gtk_args_as_hash(nargs, args);
	
	/*
	 * If pass_object flag is not specified, or it's specified and true, and we
	 * have the actual object, construct the wrapper around it.
	 */
	if ((!pass_object || Z_LVAL_PP(pass_object)) && o)
		wrapper = php_gtk_new(o);

	/*
	 * If there is a wrapper, construct array of parameters, set wrapper as
	 * the first parameter, and append the array of GTK signal arguments to it.
	 */
	if (wrapper) {
		MAKE_STD_ZVAL(params);
		array_init(params);
		zend_hash_next_index_insert(Z_ARRVAL_P(params), &wrapper, sizeof(zval *), NULL);
		php_array_merge(Z_ARRVAL_P(params), Z_ARRVAL_P(gtk_args), 0);
		zval_ptr_dtor(&gtk_args);
	} else
		/* Otherwise, the only parameters will be GTK signal arguments. */
		params = gtk_args;

	/*
	 * If there are extra arguments specified by user, add them to the parameter
	 * array.
	 */
	if (extra)
		php_array_merge(Z_ARRVAL_P(params), Z_ARRVAL_PP(extra), 0);
	
	signal_args = php_gtk_hash_as_array(params);

	call_user_function_ex(EG(function_table), NULL, *callback, &retval, zend_hash_num_elements(Z_ARRVAL_P(params)), signal_args, 1, NULL);

	if (retval) {
		if (nargs)
			php_gtk_ret_from_value(&args[nargs], retval);
		zval_ptr_dtor(&retval);
	}

	efree(signal_args);
	zval_ptr_dtor(&params);
}

void php_gtk_handler_marshal(gpointer a, gpointer data, int nargs, GtkArg *args)
{
	zval *callback_data = (zval *)data;
	zval **callback, **extra = NULL;
	zval **callback_filename = NULL, **callback_lineno = NULL;
	zval ***handler_args = NULL;
	int num_handler_args = 0;
	zval *retval = NULL;
	char *callback_name;
	ELS_FETCH();

	/* Callback is always passed as the first element. */
	zend_hash_index_find(Z_ARRVAL_P(callback_data), 0, (void **)&callback);
	zend_hash_index_find(Z_ARRVAL_P(callback_data), 1, (void **)&extra);
	zend_hash_index_find(Z_ARRVAL_P(callback_data), 2, (void **)&callback_filename);
	zend_hash_index_find(Z_ARRVAL_P(callback_data), 3, (void **)&callback_lineno);

	if (!php_gtk_is_callable(*callback, 0, &callback_name)) {
		php_error(E_WARNING, "Unable to call handler callback '%s' specified in %s on line %d", callback_name, Z_STRVAL_PP(callback_filename), Z_LVAL_PP(callback_lineno));
		efree(callback_name);
		return;
	}

	handler_args = php_gtk_hash_as_array(*extra);
	num_handler_args = zend_hash_num_elements(Z_ARRVAL_PP(extra));

	call_user_function_ex(EG(function_table), NULL, *callback, &retval, num_handler_args, handler_args, 1, NULL);

	*GTK_RETLOC_BOOL(args[0]) = FALSE;
	if (retval) {
		if (zval_is_true(retval))
			*GTK_RETLOC_BOOL(args[0]) = TRUE;
		else
			*GTK_RETLOC_BOOL(args[0]) = FALSE;
		zval_ptr_dtor(&retval);
	}

	if (handler_args)
		efree(handler_args);
}

void php_gtk_input_marshal(gpointer a, gpointer data, int nargs, GtkArg *args)
{
	zval *callback_data = (zval *)data;
	zval *params;
	zval *retval = NULL, **callback, **extra = NULL;
	zval **callback_filename = NULL, **callback_lineno = NULL;
	zval ***input_args;
	char *callback_name;
	ELS_FETCH();

	zend_hash_index_find(Z_ARRVAL_P(callback_data), 0, (void **)&callback);
	zend_hash_index_find(Z_ARRVAL_P(callback_data), 1, (void **)&extra);
	zend_hash_index_find(Z_ARRVAL_P(callback_data), 2, (void **)&callback_filename);
	zend_hash_index_find(Z_ARRVAL_P(callback_data), 3, (void **)&callback_lineno);

	if (!php_gtk_is_callable(*callback, 0, &callback_name)) {
		php_error(E_WARNING, "Unable to call input callback '%s' specified in %s on line %d", callback_name, Z_STRVAL_PP(callback_filename), Z_LVAL_PP(callback_lineno));
		efree(callback_name);
		return;
	}

	params = php_gtk_args_as_hash(nargs, args);
	if (extra)
		php_array_merge(Z_ARRVAL_P(params), Z_ARRVAL_PP(extra), 0);
	input_args = php_gtk_hash_as_array(params);

	call_user_function_ex(EG(function_table), NULL, *callback, &retval, zend_hash_num_elements(Z_ARRVAL_P(params)), input_args, 1, NULL);
	if (retval)
		zval_ptr_dtor(&retval);
	efree(input_args);
	zval_ptr_dtor(&params);
}

void php_gtk_destroy_notify(gpointer user_data)
{
	zval *value = (zval *)user_data;
	zval_ptr_dtor(&value);
}

zval *php_gtk_new(GtkObject *obj)
{
	zval *wrapper;
	zend_class_entry *ce;
	GtkType type;
	gchar *type_name;
	
	if (!obj) {
		MAKE_STD_ZVAL(wrapper);
		ZVAL_NULL(wrapper);
		return wrapper;
	}

    if ((wrapper = (zval *)gtk_object_get_data(obj, php_gtk_wrapper_key))) {
		zval_add_ref(&wrapper);
		return wrapper;
	}

	MAKE_STD_ZVAL(wrapper);

	type = GTK_OBJECT_TYPE(obj);
	while ((ce = g_hash_table_lookup(php_gtk_class_hash, gtk_type_name(type))) == NULL)
		type = gtk_type_parent(type);

	object_init_ex(wrapper, ce);
	gtk_object_ref(obj);
	php_gtk_set_object(wrapper, obj, le_gtk);

	return wrapper;
}

zval *php_gtk_args_as_hash(int nargs, GtkArg *args)
{
	zval *hash;
	zval *item;
	int i;

	MAKE_STD_ZVAL(hash);
	array_init(hash);
	for (i = 0; i < nargs; i++) {
		item = php_gtk_arg_as_value(&args[i]);
		if (!item) {
			MAKE_STD_ZVAL(item);
			ZVAL_NULL(item);
		}
		zend_hash_next_index_insert(Z_ARRVAL_P(hash), &item, sizeof(zval *), NULL);
	}

	return hash;
}

zval *php_gtk_arg_as_value(GtkArg *arg)
{
	zval *value;

	MAKE_STD_ZVAL(value);
	switch (GTK_FUNDAMENTAL_TYPE(arg->type)) {
		case GTK_TYPE_INVALID:
		case GTK_TYPE_NONE:
			ZVAL_NULL(value);
			break;

		case GTK_TYPE_CHAR:
		case GTK_TYPE_UCHAR:
			ZVAL_STRINGL(value, &GTK_VALUE_CHAR(*arg), 1, 1);
			break;

		case GTK_TYPE_BOOL:
			ZVAL_BOOL(value, GTK_VALUE_BOOL(*arg));
			break;

		case GTK_TYPE_ENUM:
		case GTK_TYPE_FLAGS:
		case GTK_TYPE_INT:
			ZVAL_LONG(value, GTK_VALUE_INT(*arg));
			break;

		case GTK_TYPE_LONG:
			ZVAL_LONG(value, GTK_VALUE_INT(*arg));
			break;

		case GTK_TYPE_UINT:
			ZVAL_LONG(value, GTK_VALUE_UINT(*arg));
			break;

		case GTK_TYPE_ULONG:
			ZVAL_LONG(value, GTK_VALUE_ULONG(*arg));
			break;

		case GTK_TYPE_FLOAT:
			ZVAL_DOUBLE(value, GTK_VALUE_FLOAT(*arg));
			break;

		case GTK_TYPE_DOUBLE:
			ZVAL_DOUBLE(value, GTK_VALUE_DOUBLE(*arg));
			break;

		case GTK_TYPE_STRING:
			if (GTK_VALUE_STRING(*arg) != NULL) {
				ZVAL_STRING(value, GTK_VALUE_STRING(*arg), 1);
			} else
				ZVAL_NULL(value);
			break;

		case GTK_TYPE_ARGS:
			value = php_gtk_args_as_hash(GTK_VALUE_ARGS(*arg).n_args,
										 GTK_VALUE_ARGS(*arg).args);
			break;

		case GTK_TYPE_OBJECT:
			if (GTK_VALUE_OBJECT(*arg) != NULL)
				value = php_gtk_new(GTK_VALUE_OBJECT(*arg));
			else
				ZVAL_NULL(value);
			break;

		case GTK_TYPE_POINTER:
			php_error(E_WARNING, "%s(): internal error: GTK_TYPE_POINTER unsupported",
					  get_active_function_name());
			ZVAL_NULL(value);
			break;

		case GTK_TYPE_BOXED:
			if (arg->type == GTK_TYPE_GDK_EVENT)
				value = php_gdk_event_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_GDK_WINDOW)
				value = php_gdk_window_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_GDK_COLOR)
				value = php_gdk_color_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_GDK_COLORMAP)
				value = php_gdk_colormap_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_GDK_VISUAL)
				value = php_gdk_visual_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_GDK_FONT)
				value = php_gdk_font_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_GDK_DRAG_CONTEXT)
				value = php_gdk_drag_context_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_ACCEL_GROUP)
				value = php_gtk_accel_group_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_STYLE)
				value = php_gtk_style_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_SELECTION_DATA)
				value = php_gtk_selection_data_new(GTK_VALUE_BOXED(*arg));
			else if (arg->type == GTK_TYPE_CTREE_NODE) {
				if (GTK_VALUE_BOXED(*arg))
					value = php_gtk_ctree_node_new(GTK_VALUE_BOXED(*arg));
				else
					ZVAL_NULL(value);
			} else
				ZVAL_NULL(value);
			break;

		case GTK_TYPE_FOREIGN:
		case GTK_TYPE_CALLBACK:
		case GTK_TYPE_SIGNAL:
			php_error(E_WARNING, "%s(): internal error: GTK_TYPE_FOREIGN, GTK_TYPE_CALLBACK, or GTK_TYPE_SIGNAL under development",
					  get_active_function_name());
			break;
	}

	return value;
}

void php_gtk_ret_from_value(GtkArg *ret, zval *value)
{
	switch (GTK_FUNDAMENTAL_TYPE(ret->type)) {
		case GTK_TYPE_NONE:
		case GTK_TYPE_INVALID:
			break;

		case GTK_TYPE_BOOL:
			convert_to_boolean(value);
			*GTK_RETLOC_BOOL(*ret) = Z_BVAL_P(value);
			break;

		case GTK_TYPE_CHAR:
			convert_to_string(value);
			*GTK_RETLOC_BOOL(*ret) = Z_STRVAL_P(value)[0];
			break;

		case GTK_TYPE_ENUM:
			if (!php_gtk_get_enum_value(ret->type, value, GTK_RETLOC_ENUM(*ret)))
				*GTK_RETLOC_ENUM(*ret) = 0;
			break;

		case GTK_TYPE_FLAGS:
			if (!php_gtk_get_flag_value(ret->type, value, GTK_RETLOC_FLAGS(*ret)))
				*GTK_RETLOC_FLAGS(*ret) = 0;
			break;

		case GTK_TYPE_INT:
			convert_to_long(value);
			*GTK_RETLOC_INT(*ret) = Z_LVAL_P(value);
			break;

		case GTK_TYPE_UINT:
			convert_to_long(value);
			*GTK_RETLOC_UINT(*ret) = Z_LVAL_P(value);
			break;

		case GTK_TYPE_LONG:
			convert_to_long(value);
			*GTK_RETLOC_LONG(*ret) = Z_LVAL_P(value);
			break;

		case GTK_TYPE_ULONG:
			convert_to_long(value);
			*GTK_RETLOC_ULONG(*ret) = Z_LVAL_P(value);
			break;

		case GTK_TYPE_FLOAT:
			convert_to_double(value);
			*GTK_RETLOC_FLOAT(*ret) = Z_DVAL_P(value);
			break;

		case GTK_TYPE_DOUBLE:
			convert_to_double(value);
			*GTK_RETLOC_DOUBLE(*ret) = Z_DVAL_P(value);
			break;

		case GTK_TYPE_STRING:
			convert_to_string(value);
			*GTK_RETLOC_STRING(*ret) = g_strdup(Z_STRVAL_P(value));
			break;

		case GTK_TYPE_OBJECT:
			if (Z_TYPE_P(value) == IS_OBJECT && php_gtk_check_class(value, g_hash_table_lookup(php_gtk_class_hash, "GtkObject")))
				*GTK_RETLOC_OBJECT(*ret) = PHP_GTK_GET(value);
			else
				*GTK_RETLOC_OBJECT(*ret) = NULL;
			break;

		case GTK_TYPE_BOXED:
			if (ret->type == GTK_TYPE_GDK_EVENT) {
				if (php_gtk_check_class(value, gdk_event_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GDK_EVENT_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_GDK_WINDOW) {
				if (php_gtk_check_class(value, gdk_window_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GDK_WINDOW_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_GDK_COLOR) {
				if (php_gtk_check_class(value, gdk_color_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GDK_COLOR_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_GDK_COLORMAP) {
				if (php_gtk_check_class(value, gdk_colormap_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GDK_COLORMAP_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_GDK_VISUAL) {
				if (php_gtk_check_class(value, gdk_visual_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GDK_VISUAL_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_GDK_FONT) {
				if (php_gtk_check_class(value, gdk_font_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GDK_FONT_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_GDK_DRAG_CONTEXT) {
				if (php_gtk_check_class(value, gdk_drag_context_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GDK_DRAG_CONTEXT_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_ACCEL_GROUP) {
				if (php_gtk_check_class(value, gtk_accel_group_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GTK_ACCEL_GROUP_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_STYLE) {
				if (php_gtk_check_class(value, gtk_style_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GTK_STYLE_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_SELECTION_DATA) {
				if (php_gtk_check_class(value, gtk_selection_data_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GTK_SELECTION_DATA_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else if (ret->type == GTK_TYPE_CTREE_NODE) {
				if (php_gtk_check_class(value, gtk_ctree_node_ce))
					*GTK_RETLOC_BOXED(*ret) = PHP_GTK_CTREE_NODE_GET(value);
				else
					*GTK_RETLOC_BOXED(*ret) = NULL;
			} else
				*GTK_RETLOC_BOXED(*ret) = NULL;
			break;

		case GTK_TYPE_POINTER:
			php_error(E_WARNING, "internal error: GTK_TYPE_POINTER not supported");
			break;

		default:
			g_assert_not_reached();
			break;
	}
}

/* Generic get/set property handlers. */
zval php_gtk_get_property(zend_property_reference *property_reference)
{
	zval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	zval object = *property_reference->object;
	prop_getter_t *getter;
	zend_class_entry *ce;
	int found;

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		if ((Z_TYPE_P(overloaded_property) != OE_IS_OBJECT ||
			Z_TYPE(overloaded_property->element) != IS_STRING) ||
			Z_TYPE(object) != IS_OBJECT) {
			convert_to_null(&result);
			return result;
		}

		found = FAILURE;
		for (ce = Z_OBJCE(object); ce != NULL && found != SUCCESS; ce = ce->parent) {
			if (zend_hash_index_find(&php_gtk_prop_getters, (long)ce, (void **)&getter) == SUCCESS) {
				(*getter)(&result, &object, &element, &found);
			}
		}
		if (found == FAILURE) {
			convert_to_null(&result);
			return result;
		}
		object = result;

		zval_dtor(&overloaded_property->element);
	}

    return result;
}

int php_gtk_set_property(zend_property_reference *property_reference, zval *value)
{
	zval result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	zend_llist_element *stop_element;
	zval object = *property_reference->object;
	prop_getter_t *getter;
	prop_setter_t *setter;
	zend_class_entry *ce;
	int retval, found;

	/*
	 * We want to stop at the last overloaded object reference - the rest can
	 * contain array references, that's fine.
	 */
	for (stop_element=property_reference->elements_list->tail;
		 stop_element && Z_TYPE_P((zend_overloaded_element *)stop_element->data) == OE_IS_ARRAY;
		 stop_element=stop_element->prev);

	for (element=property_reference->elements_list->head; element && element!=stop_element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		if (Z_TYPE_P(overloaded_property) != OE_IS_OBJECT ||
			Z_TYPE(overloaded_property->element) != IS_STRING ||
			Z_TYPE(object) != IS_OBJECT) {
			return FAILURE;
		}

		found = FAILURE;
		for (ce = Z_OBJCE(object); ce != NULL && found != SUCCESS; ce = ce->parent) {
			if (zend_hash_index_find(&php_gtk_prop_getters, (long)ce, (void **)&getter) == SUCCESS) {
				(*getter)(&result, &object, &element, &found);
			}
		}
		if (found == FAILURE)
			return FAILURE;
		object = result;

		zval_dtor(&overloaded_property->element);
	}

	retval = FAILURE;
	overloaded_property = (zend_overloaded_element *) element->data;
	if (zend_hash_index_find(&php_gtk_prop_setters, (long)object.value.obj.ce, (void **)&setter) == SUCCESS) {
		retval = (*setter)(&object, &element, value);
	}

	zval_dtor(&overloaded_property->element);
	return retval;
}


zval php_gtk_get_property_exp(zend_property_reference *property_reference)
{
	zval result;
	zval **prop_result;
	zval property;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	zval object = *property_reference->object;
	prop_getter_t *getter;
	zend_class_entry *ce;
	int found;

	for (element=property_reference->elements_list->head; element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;

		found = FAILURE;
		if (Z_TYPE_P(overloaded_property) == OE_IS_OBJECT) {
			/* Trying to access a property on a non-object. */
			if (Z_TYPE(object) != IS_OBJECT) {
				ZVAL_NULL(&result);
				return result;
			}

			if (Z_OBJCE(object)->handle_property_get) {
				for (ce = Z_OBJCE(object); ce != NULL && found != SUCCESS; ce = ce->parent) {
					if (zend_hash_index_find(&php_gtk_prop_getters, (long)ce, (void **)&getter) == SUCCESS) {
						(*getter)(&result, &object, &element, &found);
					}
				}
			}
			if (found == FAILURE &&
				(found = zend_hash_find(Z_OBJPROP(object),
							   Z_STRVAL(overloaded_property->element),
							   Z_STRLEN(overloaded_property->element)+1,
							   (void **)&prop_result)) == SUCCESS) {
				result = **prop_result;
			}
		} else if (Z_TYPE_P(overloaded_property) == OE_IS_ARRAY) {
			/* Trying to access index on a non-array. */
			if (Z_TYPE(object) != IS_ARRAY) {
				ZVAL_NULL(&result);
				return result;
			}

			if (Z_TYPE(overloaded_property->element) == IS_STRING) {
				found = zend_hash_find(Z_ARRVAL(object),
									   Z_STRVAL(overloaded_property->element),
									   Z_STRLEN(overloaded_property->element)+1,
									   (void **)&prop_result);
			} else if (Z_TYPE(overloaded_property->element) == IS_LONG) {
				found = zend_hash_index_find(Z_ARRVAL(object),
											 Z_LVAL(overloaded_property->element),
											 (void **)&prop_result);
			}
			if (found == SUCCESS)
				result = **prop_result;
		}

		if (found == FAILURE) {
			ZVAL_NULL(&result);
			if (element == property_reference->elements_list->tail)
				return result;
		}
		object = result;

		zval_dtor(&overloaded_property->element);
	}

    return result;
}


int php_gtk_set_property_exp(zend_property_reference *property_reference, zval *value)
{
	zval result;
	zval **prop_result;
	zend_overloaded_element *overloaded_property;
	zend_llist_element *element;
	zend_llist_element *stop_element;
	zval object = *property_reference->object;
	prop_getter_t *getter;
	prop_setter_t *setter;
	zend_class_entry *ce;
	int retval, found;

	/*
	 * We want to stop at the last overloaded object reference - the rest can
	 * contain array references, that's fine.
	 */
	for (stop_element=property_reference->elements_list->tail;
		 stop_element && Z_TYPE_P((zend_overloaded_element *)stop_element->data) == OE_IS_ARRAY;
		 stop_element=stop_element->prev);

	for (element=property_reference->elements_list->head; element && element!=stop_element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		if (Z_TYPE_P(overloaded_property) != OE_IS_OBJECT ||
			Z_TYPE(overloaded_property->element) != IS_STRING ||
			(Z_TYPE(object) != IS_OBJECT)) {
			return FAILURE;
		}

		found = FAILURE;
		if (Z_OBJCE(object)->handle_property_get) {
			for (ce = Z_OBJCE(object); ce != NULL && found != SUCCESS; ce = ce->parent) {
				if (zend_hash_index_find(&php_gtk_prop_getters, (long)ce, (void **)&getter) == SUCCESS) {
					(*getter)(&result, &object, &element, &found);
				}
			}
		}
		if (found == FAILURE) {
			if (zend_hash_find(Z_OBJPROP(object),
							   Z_STRVAL(overloaded_property->element),
							   Z_STRLEN(overloaded_property->element)+1,
							   (void **)&prop_result) == SUCCESS)
				result = **prop_result;
			else
				return FAILURE;
		}
		object = result;

		zval_dtor(&overloaded_property->element);
	}

	retval = FAILURE;
	overloaded_property = (zend_overloaded_element *) element->data;
	if (Z_OBJCE(object)->handle_property_set) {
		for (ce = Z_OBJCE(object); ce != NULL && retval != SUCCESS; ce = ce->parent) {
			if (zend_hash_index_find(&php_gtk_prop_setters, (long)ce, (void **)&setter) == SUCCESS) {
				retval = (*setter)(&object, &element, value);
			}
		}
	}
	if (retval == FAILURE) {
		if (Z_OBJCE(object)->handle_property_get) {
			for (ce = Z_OBJCE(object); ce != NULL && retval != SUCCESS; ce = ce->parent) {
				if (zend_hash_index_find(&php_gtk_prop_getters, (long)ce, (void **)&getter) == SUCCESS) {
					(*getter)(&result, &object, &element, &retval);
				}
			}
		}

		if (retval == FAILURE) {
			zval *local_value;

			MAKE_STD_ZVAL(local_value);
			*local_value = *value;
			zval_copy_ctor(local_value);
			retval = add_property_zval_ex(&object,
										  Z_STRVAL(overloaded_property->element),
										  Z_STRLEN(overloaded_property->element)+1, local_value);
		} else {
			php_error(E_WARNING, "Cannot assign to overloaded property '%s'",
					  Z_STRVAL(overloaded_property->element));
		}
	}

	zval_dtor(&overloaded_property->element);
	return retval;
}


void php_gtk_call_function(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference)
{
	zval result;
	zend_overloaded_element *overloaded_property;
	zval method_name = ((zend_overloaded_element *)property_reference->elements_list->tail->data)->element;
	zend_llist_element *element;
	zend_llist_element *stop_element;
	zval *object = property_reference->object;
	prop_getter_t *getter;
	zend_class_entry *ce;
	int found;

	/*
	 * We want to stop at the last overloaded object reference - the rest can
	 * contain array and method references, that's fine.
	 */
	for (stop_element=property_reference->elements_list->tail;
		 stop_element &&
		 (Z_TYPE_P((zend_overloaded_element *)stop_element->data) == OE_IS_ARRAY ||
		  Z_TYPE_P((zend_overloaded_element *)stop_element->data) == OE_IS_METHOD);
		 stop_element=stop_element->prev);

	for (element=property_reference->elements_list->head; element && element!=stop_element; element=element->next) {
		overloaded_property = (zend_overloaded_element *) element->data;
		if (Z_TYPE_P(overloaded_property) != OE_IS_OBJECT ||
			Z_TYPE(overloaded_property->element) != IS_STRING ||
			Z_TYPE_P(object) != IS_OBJECT) {
			php_error(E_WARNING, "Error invoking method '%s'", Z_STRVAL(method_name));
			return;
		}

		found = FAILURE;
		for (ce = Z_OBJCE_P(object); ce != NULL && found != SUCCESS; ce = ce->parent) {
			if (zend_hash_index_find(&php_gtk_prop_getters, (long)ce, (void **)&getter) == SUCCESS) {
				(*getter)(&result, object, &element, &found);
			}
		}
		if (found == FAILURE) {
			php_error(E_WARNING, "Error invoking method '%s' on property '%s'", Z_STRVAL(method_name), Z_STRVAL(overloaded_property->element));
			return;
		}
		*object = result;

		zval_dtor(&overloaded_property->element);
	}

	zval_dtor(&method_name);
}


#endif  /* HAVE_PHP_GTK */
