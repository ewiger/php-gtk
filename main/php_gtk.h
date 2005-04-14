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
 
/* $Id$: */

#ifndef _PHP_GTK_H
#define _PHP_GTK_H

#include "php.h"
#include "php_ini.h"
#ifdef PHP_WIN32
#include "config.w32.h"
#else
#include "config.h"
#endif

#if HAVE_PHP_GTK

#include "zend_objects_API.h"
#include "zend_exceptions.h"

#define PHP_GTK_VERSION "2.0.0"

#ifdef PHP_WIN32
# ifdef GTK_SHARED
#  define PHP_GTK_API __declspec(dllimport)
# else
#  define PHP_GTK_API __declspec(dllexport)
# endif
#else
# define PHP_GTK_API
#endif

#include "php_gtk_module.h"

#define PANGO_ENABLE_BACKEND
#define PANGO_ENABLE_ENGINE
#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#define phpg_return_val_if_fail_quiet(expr, val) G_STMT_START{ \
    if G_LIKELY(expr) { } else                     \
    {                                \
    	return (val);                          \
    }; }G_STMT_END

#define phpg_return_val_if_fail(expr, val) g_return_val_if_fail(expr, val)

#define phpg_return_if_fail_quiet(expr) G_STMT_START{ \
    if G_LIKELY(expr) { } else                     \
    {                                \
    	return;                          \
    }; }G_STMT_END

#define phpg_return_if_fail(expr) g_return_if_fail(expr)

#define PHP_GTK_EXPORT_CE(ce) zend_class_entry *ce
#define PHP_GTK_EXPORT_FUNC(func) func

typedef void (*phpg_dtor_t)(void *);

typedef struct {
	zval *callback;
	zval *user_args;
	char *src_filename;
	long src_lineno;
} phpg_cb_data_t;

static inline phpg_cb_data_t* phpg_cb_data_new(zval *callback, zval *user_args TSRMLS_DC)
{
	phpg_cb_data_t *cbd = emalloc(sizeof(phpg_cb_data_t));
	cbd->callback = callback;
	cbd->user_args = user_args;
	cbd->src_filename = estrdup(zend_get_executed_filename(TSRMLS_C));
	cbd->src_lineno = zend_get_executed_lineno(TSRMLS_C);
	return cbd;
}
void phpg_cb_data_destroy(gpointer data);

#define PHPG_OBJ_HEADER \
	zend_object zobj;   \
	HashTable *pi_hash;

typedef struct {
	PHPG_OBJ_HEADER
} phpg_head_t;

typedef struct {
	PHPG_OBJ_HEADER
	GObject *obj;
	phpg_dtor_t dtor;
	GSList *closures;
	gboolean is_owned;
} phpg_gobject_t;

typedef struct {
	PHPG_OBJ_HEADER
	GType gtype;
	gpointer boxed;
	gboolean free_on_destroy;
} phpg_gboxed_t;

typedef int (*boxed_from_zval_t)(const zval *value, GValue *gvalue TSRMLS_DC);
typedef int (*boxed_to_zval_t)(const GValue *gvalue, zval **value TSRMLS_DC);
typedef struct {
	boxed_from_zval_t from_zval;
	boxed_to_zval_t   to_zval;
} phpg_gboxed_marshal_t;

/* Private structure */
typedef struct _phpg_closure_t phpg_closure_t;

#define PHPG_GET(zobj) \
	zend_object_store_get_object((zobj) TSRMLS_CC)

#define PHPG_GOBJECT(zobj) (GObject *)phpg_gobject_get(zobj TSRMLS_CC)->obj

#define PHPG_GBOXED(zobj) phpg_gboxed_get(zobj TSRMLS_CC)->boxed

/*
 * Property read/write function types
 */
typedef int (*prop_read_func_t)(void *object, zval *return_value TSRMLS_DC);
typedef int (*prop_write_func_t)(void *object, zval *rvalue TSRMLS_DC);

typedef struct {
	const char *name;
	prop_read_func_t read;
	prop_write_func_t write;
} prop_info_t;

#define PHPG_PROP_READ_FN(class, name) \
	phpg_##class##_read_##name
#define PHPG_PROP_READER(class, name) \
	static int PHPG_PROP_READ_FN(class, name)(void *object, zval *return_value)

typedef void (*prop_getter_t)(zval *return_value, zval *object, char *property, int *result);
typedef int (*prop_setter_t)(zval *object, char *property, zval *value);
typedef zend_object_value (*create_object_func_t)(zend_class_entry *ce TSRMLS_DC);

/*
 * PHP-GTK Sub-extensions related stuff
 */
#define EXT_INIT_ARGS			int module_number TSRMLS_DC
#define EXT_SHUTDOWN_ARGS		void

#define PHP_GTK_XINIT(ext)		php_gtk_xinit_##ext
#define PHP_GTK_XSHUTDOWN(ext)	php_gtk_xshutdown_##ext

#define PHP_GTK_XINIT_FUNCTION(ext)		int PHP_GTK_XINIT(ext)(EXT_INIT_ARGS)
#define PHP_GTK_XSHUTDOWN_FUNCTION(ext)	int PHP_GTK_XSHUTDOWN(ext)(EXT_SHUTDOWN_ARGS)

typedef struct _php_gtk_ext_entry php_gtk_ext_entry;
struct _php_gtk_ext_entry {
	char *name;
	int (*ext_startup_func)(EXT_INIT_ARGS);
	int (*ext_shutdown_func)(EXT_SHUTDOWN_ARGS);
	int ext_started;
	void *handle;
};

#define PHP_GTK_GET_EXTENSION(name) \
    ZEND_DLEXPORT php_gtk_ext_entry *get_extension(void) { return &name##_ext_entry; }

#include "ext/gtk+/php_gtk+.h"

/*
 * True globals.
 * */
extern zend_llist php_gtk_ext_registry;
PHP_GTK_API extern GHashTable *php_gtk_class_hash;
extern HashTable php_gtk_rsrc_hash;
extern HashTable php_gtk_prop_getters;
extern HashTable php_gtk_prop_setters;
extern HashTable php_gtk_type_hash;
extern HashTable php_gtk_callback_hash;
extern HashTable php_gtk_prop_desc;
extern HashTable phpg_prop_info;

/* IDs for type identification */
extern const gchar *phpg_class_id;
extern GQuark phpg_class_key;
extern GType G_TYPE_PHP_VALUE;

/* Exceptions */
extern PHP_GTK_API zend_class_entry *phpg_generic_exception;
extern PHP_GTK_API zend_class_entry *phpg_construct_exception;
extern PHP_GTK_API zend_class_entry *phpg_type_exception;
extern PHP_GTK_API zend_class_entry *phpg_gerror_exception;

extern PHP_GTK_API zend_object_handlers php_gtk_handlers;

/* Function declarations. */

int php_gtk_startup_all_extensions(int module_number);
int php_gtk_startup_extensions(php_gtk_ext_entry **ext, int ext_count, int module_number);

zval *phpg_read_property(zval *object, zval *member, int type TSRMLS_DC);
void phpg_write_property(zval *object, zval *member, zval *value TSRMLS_DC);
HashTable* phpg_get_properties(zval *object TSRMLS_DC);
PHP_GTK_API void phpg_get_properties_helper(zval *object, HashTable *ht TSRMLS_DC, ...);

#define STRS(s) #s, sizeof(#s)-1

PHP_GTK_API void *php_gtk_get_object(zval *wrapper);
PHP_GTK_API int php_gtk_get_simple_enum_value(zval *enum_val, int *result);
PHP_GTK_API int php_gtk_get_enum_value(GType enum_type, zval *enum_val, int *result);
PHP_GTK_API void php_gtk_callback_marshal(GtkObject *o, gpointer data, guint nargs, GtkArg *args);
void php_gtk_handler_marshal(gpointer a, gpointer data, int nargs, GtkArg *args);
zval *php_gtk_args_as_hash(int nargs, GtkArg *args);
GtkArg *php_gtk_hash_as_args(zval *hash, GType type, gint *nargs);
int php_gtk_args_from_hash(GtkArg *args, int nparams, zval *hash);
zval *php_gtk_arg_as_value(GtkArg *arg);
int php_gtk_arg_from_value(GtkArg *arg, zval *value);
zval *php_gtk_ret_as_value(GtkArg *ret);
void php_gtk_ret_from_value(GtkArg *ret, zval *value);
int php_gtk_get_flag_value(GType flag_type, zval *flag_val, int *result);
/*
PHP_GTK_API zval php_gtk_get_property(zend_property_reference *property_reference);
PHP_GTK_API int php_gtk_set_property(zend_property_reference *property_reference, zval *value);
void php_gtk_call_function(INTERNAL_FUNCTION_PARAMETERS, zend_property_reference *property_reference);
*/

PHP_GTK_API zend_class_entry* phpg_register_class(const char *class_name, function_entry *class_functions, zend_class_entry *parent, zend_uint ce_flags, prop_info_t *prop_info, create_object_func_t create_obj_func, GType gtype TSRMLS_DC);
PHP_GTK_API zend_class_entry* phpg_register_interface(const char *iface_name, function_entry *iface_methods, GType gtype TSRMLS_DC);
PHP_GTK_API zend_class_entry* phpg_create_class(GType gtype);
PHP_GTK_API void phpg_init_object(void *pobj, zend_class_entry *ce);
PHP_GTK_API zend_bool phpg_parse_ctor_props(GType gtype, zval **php_args, GParameter *params, guint *n_params, char **prop_names TSRMLS_DC);

PHP_GTK_API void phpg_register_prop_getter(zend_class_entry *ce, prop_getter_t getter);
PHP_GTK_API void php_gtk_register_prop_setter(zend_class_entry *ce, prop_setter_t setter);
PHP_GTK_API void php_gtk_register_callback(char *class_and_method, GtkSignalFunc call_function);
PHP_GTK_API void php_gtk_object_init(GtkObject *obj, zval *wrapper);

/* Utility functions. */
PHP_GTK_API void phpg_destroy_notify(gpointer user_data);
PHP_GTK_API int php_gtk_parse_args(int argc, char *format, ...);
PHP_GTK_API int php_gtk_parse_args_quiet(int argc, char *format, ...);
PHP_GTK_API int php_gtk_parse_varargs(int argc, int min_args, zval **varargs, char *format, ...);
PHP_GTK_API int php_gtk_parse_args_hash(zval *hash, char *format, ...);
PHP_GTK_API int php_gtk_parse_args_hash_quiet(zval *hash, char *format, ...);
PHP_GTK_API int php_gtk_check_class(zval *wrapper, zend_class_entry *expected_ce);
PHP_GTK_API void php_gtk_invalidate(zval *wrapper);
zend_bool php_gtk_is_callable(zval *callable, zend_bool syntax_only, char **callable_name);
zval *php_gtk_array_as_hash(zval ***values, int num_values, int start, int length);
zval*** php_gtk_hash_as_array(zval *hash);
zval*** php_gtk_hash_as_array_offset(zval *hash, int offset, int *total);
zval ***php_gtk_func_args(int argc);
PHP_GTK_API zval *php_gtk_func_args_as_hash(int argc, int start, int length);
PHP_GTK_API void php_gtk_build_value(zval **result, char *format, ...);
char *php_gtk_zval_type_name(zval *arg);
PHP_GTK_API void phpg_warn_deprecated(char *msg TSRMLS_DC);

PHP_GTK_API  void php_gtk_signal_connect_impl(INTERNAL_FUNCTION_PARAMETERS, int pass_object, int after);
PHP_GTK_API zval* php_gtk_simple_signal_callback(GtkObject *o, gpointer data, zval *gtk_args );

static inline zend_bool phpg_object_check(zval *zobj, zend_class_entry *ce TSRMLS_DC)
{
    phpg_return_val_if_fail(zobj != NULL, FALSE);
    phpg_return_val_if_fail(ce != NULL, FALSE);
	phpg_return_val_if_fail_quiet(Z_TYPE_P(zobj) == IS_OBJECT
								  && instanceof_function(Z_OBJCE_P(zobj), ce TSRMLS_CC), FALSE);
	return TRUE;
}

static inline zend_class_entry* phpg_class_from_gtype(GType gtype)
{
	zend_class_entry *ce = NULL;

	ce = g_type_get_qdata(gtype, phpg_class_key);
	if (!ce) {
		ce = phpg_create_class(gtype);
	}
	
	assert(ce != NULL);
	return ce;
}
#define NOT_STATIC_METHOD() \
	if (!this_ptr) { \
		php_error(E_WARNING, "%s::%s() is not a static method", get_active_class_name(NULL TSRMLS_CC), get_active_function_name(TSRMLS_C)); \
		return; \
	}

#define PHPG_THROW_EXCEPTION(exception, message) \
    do { \
		TSRMLS_FETCH(); \
		zend_throw_exception(exception, message, 0 TSRMLS_CC); \
		return; \
	} while (0)

#define PHPG_THROW_CONSTRUCT_EXCEPTION(type) \
	PHPG_THROW_EXCEPTION(phpg_construct_exception, "could not construct " #type " object");

#define PHPG_THROW_EXCEPTION_WITH_RETURN(exception, message, retval) \
    do { \
		TSRMLS_FETCH(); \
		zend_throw_exception(exception, message, 0 TSRMLS_CC); \
		return retval; \
	} while (0)


#define MAKE_ZVAL_IF_NULL(z) \
   do { \
	   if (z == NULL) { MAKE_STD_ZVAL(z) }; \
   } while (0);


void phpg_register_exceptions();
gboolean phpg_handler_marshal(gpointer user_data);
zend_bool phpg_handle_gerror(GError **error TSRMLS_DC);
PHP_GTK_API zval* phpg_throw_gerror_exception(const char *domain, long code, const char *message TSRMLS_DC);

PHP_GTK_API PHP_FUNCTION(no_constructor);
PHP_GTK_API PHP_FUNCTION(no_direct_constructor);

extern char *php_gtk_zval_type_name(zval *arg);

PHP_GTK_API void phpg_register_enum(GType gtype, const char *strip_prefix, zend_class_entry *ce);
PHP_GTK_API void phpg_register_flags(GType gtype, const char *strip_prefix, zend_class_entry *ce);
PHP_GTK_API void phpg_register_int_constant(zend_class_entry *ce, char *name, int name_len, long value);
PHP_GTK_API void phpg_register_string_constant(zend_class_entry *ce, char *name, int name_len, char *value, int value_len);

void phpg_gtype_register_self(TSRMLS_D);
PHP_GTK_API void phpg_gtype_new(zval *zobj, GType type TSRMLS_DC);
PHP_GTK_API GType phpg_gtype_from_zval(zval *value);


/* GValue */
PHP_GTK_API int phpg_gvalue_to_zval(const GValue *gval, zval **value, zend_bool copy_boxed TSRMLS_DC);
PHP_GTK_API int phpg_gvalue_from_zval(GValue *gval, zval *value TSRMLS_DC);
PHP_GTK_API int phpg_param_gvalue_to_zval(const GValue *gval, zval **value, zend_bool copy_boxed, const GParamSpec *pspec TSRMLS_DC);
PHP_GTK_API int phpg_param_gvalue_from_zval(GValue *gval, zval *value, const GParamSpec *pspec TSRMLS_DC);
PHP_GTK_API zval *phpg_gvalues_to_array(const GValue *values, uint n_values);
PHP_GTK_API int phpg_gvalue_get_enum(GType enum_type, zval *enum_val, gint *result);
PHP_GTK_API int phpg_gvalue_get_flags(GType flags_type, zval *flags_val, gint *result);

/* GObject */
PHP_GTK_API zend_object_value phpg_create_gobject(zend_class_entry *ce TSRMLS_DC);
PHP_GTK_API void phpg_gobject_new(zval **zobj, GObject *obj TSRMLS_DC);
PHP_GTK_API void phpg_gobject_set_wrapper(zval *zobj, GObject *obj TSRMLS_DC);
PHP_GTK_API void phpg_gobject_watch_closure(zval *zobj, GClosure *closure TSRMLS_DC);
void phpg_gobject_register_self(TSRMLS_D);
zend_bool phpg_gobject_construct(zval *this_ptr TSRMLS_DC);

static inline phpg_gobject_t* phpg_gobject_get(zval *zobj TSRMLS_DC)
{
    phpg_gobject_t *pobj = zend_object_store_get_object(zobj TSRMLS_CC);
    if (pobj->obj == NULL) {
        php_error(E_ERROR, "Internal object missing in %s wrapper", Z_OBJCE_P(zobj)->name);
    }
    return pobj;
}


/* GBoxed */
void phpg_gboxed_register_self(TSRMLS_D);
PHP_GTK_API void phpg_gboxed_register_custom(GType type, boxed_from_zval_t from_func, boxed_to_zval_t to_func);
PHP_GTK_API phpg_gboxed_marshal_t* phpg_gboxed_lookup_custom(GType type);
PHP_GTK_API void phpg_gboxed_new(zval **zobj, GType gtype, gpointer boxed, gboolean copy, gboolean own_ref TSRMLS_DC);
PHP_GTK_API zend_class_entry* phpg_register_boxed(const char *class_name, function_entry *class_methods, prop_info_t *prop_info, create_object_func_t create_obj_func, GType gtype TSRMLS_DC);
PHP_GTK_API zend_object_value phpg_create_gboxed(zend_class_entry *ce TSRMLS_DC);
PHP_GTK_API zend_bool phpg_gboxed_check(zval *zobj, GType gtype, zend_bool full_check TSRMLS_DC);

static inline phpg_gboxed_t* phpg_gboxed_get(zval *zobj TSRMLS_DC)
{
    phpg_gboxed_t *pobj = zend_object_store_get_object(zobj TSRMLS_CC);
    if (pobj->boxed == NULL) {
        php_error(E_ERROR, "Internal object missing in %s wrapper", Z_OBJCE_P(zobj)->name);
    }
    return pobj;
}


/* Closures */
PHP_GTK_API GClosure* phpg_closure_new(zval *callback, zval *user_args, zend_bool use_signal_object TSRMLS_DC);
PHP_GTK_API void phpg_watch_closure(zval *obj, GClosure *closure TSRMLS_DC);

PHP_GTK_API extern PHP_GTK_EXPORT_CE(gtype_ce);
PHP_GTK_API extern PHP_GTK_EXPORT_CE(gobject_ce);
PHP_GTK_API extern PHP_GTK_EXPORT_CE(gboxed_ce);

PHP_GTK_API ZEND_EXTERN_MODULE_GLOBALS(gtk);

#endif /* HAVE_PHP_GTK */

#endif	/* _PHP_GTK_H */
