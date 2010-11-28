/* This file is part of gPHPEdit, a GNOME2 PHP Editor.

   Copyright (C) 2003, 2004, 2005 Andy Jeffries <andy at gphpedit.org>
   Copyright (C) 2009 Anoop John <anoop dot john at zyxware.com>
   Copyright (C) 2009, 2010 José Rostagno (for vijona.com.ar) 

   For more information or to find the latest release, visit our 
   website at http://www.gphpedit.org/

   gPHPEdit is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   gPHPEdit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with gPHPEdit. If not, see <http://www.gnu.org/licenses/>.

   The GNU General Public License is contained in the file COPYING.
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <gtk/gtk.h>
#include "debug.h"
#include "classbrowser_backend.h"
#include "classbrowser_parse.h"
#include "main_window_callbacks.h"
#include "gvfs_utils.h"

#include "tab_php.h"
#include "tab_css.h"
#include "tab_cxx.h"
#include "tab_cobol.h"
#include "tab_python.h"
#include "tab_perl.h"
/* object signal enumeration */
enum {
	DONE_REFRESH,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL];

/*
* classbrowser_backend private struct
*/
struct ClassbrowserBackendDetails
{
  GHashTable *functionlist;
  GHashTable *php_variables_tree;
  GHashTable *php_class_tree;
  guint identifierid;

  GHashTable *completion_list;
  GString *completion_string;
  gchar *completion_prefix;
  gint file_type;
};

/* internal struct */
typedef struct
{
  gchar *filename;
  gboolean accessible;
  GTimeVal modified_time;
}
ClassBrowserFile;

typedef struct
{
  gchar *varname;
  gchar *functionname;
  gchar *filename;
  gboolean remove;
  guint identifierid;
  gint file_type;
}
ClassBrowserVar;

/*
 * classbrowser_backend_get_type
 * register ClassbrowserBackend type and returns a new GType
*/

static void classbrowser_backend_class_init (ClassbrowserBackendClass *klass);
static void classbrowser_backend_dispose (GObject *gobject);

#define CLASSBROWSER_BACKEND_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object),\
					    CLASSBROWSER_BACKEND_TYPE,\
					    ClassbrowserBackendDetails))

void classbrowser_remove_dead_wood(ClassbrowserBackend *classback);
void do_parse_file(ClassbrowserBackend *classback, Document *document);
void free_function_list_item (gpointer data, gpointer user_data);
#ifdef HAVE_CTAGS_EXUBERANT
void call_ctags(ClassbrowserBackend *classback, gchar *filename);
void process_cobol_word(ClassbrowserBackend *classback, gchar *name, gchar *filename, gchar *type, gchar *line);
void process_cxx_word(ClassbrowserBackend *classback, gchar *name, gchar *filename, gchar *type, gchar *line, gchar *param);
void process_python_word(ClassbrowserBackend *classback, gchar *name,gchar *filename,gchar *type,gchar *line, gchar *param);
void process_perl_word(ClassbrowserBackend *classback, gchar *name, gchar *filename, gchar *type, gchar *line, gchar *param);
#endif

/* http://library.gnome.org/devel/gobject/unstable/gobject-Type-Information.html#G-DEFINE-TYPE:CAPS */
G_DEFINE_TYPE(ClassbrowserBackend, classbrowser_backend, G_TYPE_OBJECT);

void
classbrowser_backend_class_init (ClassbrowserBackendClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);
  object_class->dispose = classbrowser_backend_dispose;

/*
if load is ok return TRUE. if load isn't complete return FALSE
*/
	signals[DONE_REFRESH] =
		g_signal_new ("done_refresh",
		              G_TYPE_FROM_CLASS (object_class),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (ClassbrowserBackendClass, done_refresh),
		              NULL, NULL,
		               g_cclosure_marshal_VOID__BOOLEAN ,
		               G_TYPE_NONE, 1, G_TYPE_BOOLEAN, NULL);

	g_type_class_add_private (klass, sizeof (ClassbrowserBackendDetails));
}

void
classbrowser_backend_init (ClassbrowserBackend *classback)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  classbackdet->identifierid = 0;
}

/*
* disposes the Gobject
*/
void classbrowser_backend_dispose (GObject *object)
{
  ClassbrowserBackend *classback = CLASSBROWSER_BACKEND(object);
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  /* free object resources*/
  if (classbackdet->functionlist) g_hash_table_destroy (classbackdet->functionlist);
  if (classbackdet->php_variables_tree) g_hash_table_destroy (classbackdet->php_variables_tree);
  if (classbackdet->php_class_tree) g_hash_table_destroy (classbackdet->php_class_tree);

  /* Chain up to the parent class */
  G_OBJECT_CLASS (classbrowser_backend_parent_class)->dispose (object);
}

ClassbrowserBackend *classbrowser_backend_new (void)
{
	ClassbrowserBackend *classback;
  classback = g_object_new (CLASSBROWSER_BACKEND_TYPE, NULL);

	return classback; /* return new object */
}

void free_php_variables_tree_item (gpointer data) 
{
  ClassBrowserVar *var=(ClassBrowserVar *)data;
  if (var->varname) g_free(var->varname);
  if (var->functionname) g_free(var->functionname);
  if (var->filename) g_free(var->filename);
  if (var) g_slice_free(ClassBrowserVar, var);
}

void free_php_class_tree_item (gpointer data)
{
  ClassBrowserClass *class = (ClassBrowserClass *) data;
  if (class->filename) g_free(class->filename);
  if (class->classname) g_free(class->classname);
  if (class) g_slice_free(ClassBrowserClass, class);
}

void free_php_function_list_item (gpointer data)
{
  ClassBrowserFunction *function = (ClassBrowserFunction *) data;
  g_free(function->filename);
  g_free(function->functionname);
  if (function->paramlist) {
    g_free(function->paramlist);
  }
  if (function->classname) {
    g_free(function->classname);
  }
  g_slice_free(ClassBrowserFunction,function);
}

void add_global_var(ClassbrowserBackendDetails *classbackdet, const gchar *var_name)
{
  ClassBrowserVar *var;
    var = g_slice_new(ClassBrowserVar);
    var->varname = g_strdup(var_name);
    var->functionname = NULL; /* NULL for global variables*/
    var->filename = NULL; /*NULL FOR PHP GLOBAL VARIABLES*/
    var->remove = FALSE;
    var->identifierid = classbackdet->identifierid++;
    var->file_type = TAB_PHP;
    g_hash_table_insert (classbackdet->php_variables_tree, g_strdup(var_name), var); /* key = variables name value var struct */
    gphpedit_debug_message(DEBUG_CLASSBROWSER, "%s\n", var_name);
}

/* release resources used by classbrowser */
void classbrowser_php_class_set_remove_item (gpointer key, gpointer value, gpointer data){
  ClassBrowserClass *class=(ClassBrowserClass *)value;
  if (class) {
    class->remove = TRUE;
  }
}

void list_php_files_open (gpointer data, gpointer user_data){
  do_parse_file(user_data, data);
}

/*
* do_parse_file
* parse an editor
* if editor is PHP uses our internal code
* otherwise use CTAGS EXUBERANT if it's avariable.
*/
void do_parse_file(ClassbrowserBackend *classback, Document *document){
    g_return_if_fail(document);
    gboolean untitled;
    g_object_get(document, "untitled", &untitled, NULL);
    if (OBJECT_IS_DOCUMENT_SCINTILLA(document) && !untitled){
    const gchar *short_filename;
    g_object_get(document, "short_filename", &short_filename, NULL);
    gphpedit_debug_message(DEBUG_CLASSBROWSER, "Parsing: %s\n", short_filename);
    gchar *filename = documentable_get_filename(DOCUMENTABLE(document));
      if (is_php_file_from_filename(filename)) {
        classbrowser_parse_file(classback, filename); 
#ifdef HAVE_CTAGS_EXUBERANT
      } else {
        /* CTAGS don't support CSS files */
        if (!is_css_file(filename)) call_ctags(classback, filename);
#endif
      }
    g_free(filename);
    }  
}

/* release resources used by classbrowser */
void classbrowser_php_function_set_remove_item (gpointer key, gpointer value, gpointer data){
  ClassBrowserFunction *function=(ClassBrowserFunction *)value;
  if (function) {
    function->remove = TRUE;
  }
}

void classbrowser_backend_start_update(ClassbrowserBackendDetails *classbackdet)
{
  if (!classbackdet->functionlist){
     /* create new tree */
    classbackdet->functionlist= g_hash_table_new_full (g_str_hash, g_str_equal, g_free, free_php_function_list_item);
  } else {
    g_hash_table_foreach (classbackdet->functionlist, classbrowser_php_function_set_remove_item, NULL);
  }

  if (!classbackdet->php_class_tree){
     /* create new tree */
    classbackdet->php_class_tree= g_hash_table_new_full (g_str_hash, g_str_equal, g_free, free_php_class_tree_item);
  } else {
    g_hash_table_foreach (classbackdet->php_class_tree, classbrowser_php_class_set_remove_item, NULL);
  }
}

//FIXME: this function can be optimized by not requesting to reparse files on tab change
//when the parse only selected tab is set - Anoop
void classbrowser_backend_update(ClassbrowserBackend *classback, gboolean only_current_file)
{

  gphpedit_debug(DEBUG_CLASSBROWSER);
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  if (!classbackdet->php_variables_tree){
     /* create new tree */
     classbackdet->php_variables_tree=g_hash_table_new_full (g_str_hash, g_str_equal, g_free, free_php_variables_tree_item);
      
     /*add php global vars*/
     add_global_var(classbackdet, "$GLOBALS");
     add_global_var(classbackdet, "$HTTP_POST_VARS");
     add_global_var(classbackdet, "$HTTP_RAW_POST_DATA");
     add_global_var(classbackdet, "$http_response_header");
     add_global_var(classbackdet, "$this");
     add_global_var(classbackdet, "$_COOKIE");
     add_global_var(classbackdet, "$_POST");
     add_global_var(classbackdet, "$_REQUEST");
     add_global_var(classbackdet, "$_SERVER");
     add_global_var(classbackdet, "$_SESSION");
     add_global_var(classbackdet, "$_GET");
     add_global_var(classbackdet, "$_FILES");
     add_global_var(classbackdet, "$_ENV");
     add_global_var(classbackdet, "__CLASS__");
     add_global_var(classbackdet, "__DIR__");
     add_global_var(classbackdet, "__FILE__");
     add_global_var(classbackdet, "__FUNCTION__");
     add_global_var(classbackdet, "__METHOD__");
     add_global_var(classbackdet, "__NAMESPACE__");
  }
  
  classbrowser_backend_start_update(classbackdet);
  DocumentManager *docmg = document_manager_new();
  if (only_current_file){
      do_parse_file(classback, document_manager_get_current_document(docmg));
  } else {
    g_slist_foreach (document_manager_get_document_list(docmg), list_php_files_open, classback); 
  }
  g_object_unref(docmg);
  classbrowser_remove_dead_wood(classback);
  g_signal_emit (G_OBJECT (classback), signals[DONE_REFRESH], 0, TRUE); /* emit process and update UI */
}

gboolean classbrowser_remove_class (gpointer key, gpointer value, gpointer user_data)
{
  ClassBrowserClass *class= (ClassBrowserClass *)value;
  if (class) return class->remove;
  return FALSE;
}

gboolean classbrowser_remove_function (gpointer key, gpointer value, gpointer user_data)
{
  ClassBrowserFunction *function= (ClassBrowserFunction *)value;
  if (function) return function->remove;
  return FALSE;
}

void classbrowser_remove_dead_wood(ClassbrowserBackend *classback)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  g_hash_table_foreach_remove (classbackdet->functionlist, classbrowser_remove_function, classback);
  g_hash_table_foreach_remove (classbackdet->php_class_tree, classbrowser_remove_class, classback);
}

void classbrowser_varlist_add(ClassbrowserBackend *classback, gchar *varname, gchar *funcname, gchar *filename, gint file_type)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  ClassBrowserVar *var;
  var=g_hash_table_lookup (classbackdet->php_variables_tree, varname);

  if (var){
    var->remove = FALSE;
  } else {
    var = g_slice_new0(ClassBrowserVar);
    var->varname = g_strdup(varname);
    if (funcname) var->functionname = g_strdup(funcname);
    var->filename = g_strdup(filename);
    var->remove = FALSE;
    var->identifierid = classbackdet->identifierid++;
    var->file_type = file_type;
    g_hash_table_insert (classbackdet->php_variables_tree, g_strdup(varname), var); /* key =variables name value var struct */

    gphpedit_debug_message(DEBUG_CLASSBROWSER, "filename: %s var name: %s\n", filename, varname);
  }
}

void classbrowser_classlist_add(ClassbrowserBackend *classback, gchar *classname, gchar *filename, gint line_number, gint file_type)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);

  ClassBrowserClass *class;
  gchar *keyname = g_strdup_printf("%s%s",classname,filename);
  class = g_hash_table_lookup (classbackdet->php_class_tree, keyname);

  if ((class)){
    class->line_number = line_number;
    class->remove= FALSE;
    g_free(keyname);
  } else {
    class = g_slice_new0(ClassBrowserClass);
    class->classname = g_strdup(classname);
    class->filename = g_strdup(filename);
    class->line_number = line_number;
    class->remove = FALSE;
    class->identifierid = classbackdet->identifierid++;
    class->file_type = file_type;
    g_hash_table_insert (classbackdet->php_class_tree, keyname, class);
    gphpedit_debug_message(DEBUG_CLASSBROWSER,"filename: %s class name: %s\n", filename, classname);
  }
}

#ifdef HAVE_CTAGS_EXUBERANT
gchar *get_ctags_token(gchar *text,gint *advancing){
  int i;
  int k=0;
  gchar *name;
  gchar *part = text;
  name=part;
  for (i=0;i<strlen(text);i++){
    /* process until get a space*/
    if (*(part+i)==' '){
      while (*(part+i+k)==' ') k++; /*count spaces*/
      break;
    }
  }
  name=g_malloc0(i+1);
  strncpy(name,part,i);
  *advancing=i+k; /* skip spaces*/
  return name;
}

gchar *get_ctags_param(gchar *text,gint *advancing){
  int i;
  gchar *name;
  gchar *part = text;
  name=part;
  for (i=0;i<strlen(text);i++){
    if (*(part+i)=='('){
      break;
    }
  }
  int len = strlen(part) - i -1;
  if (len < 0) return NULL;
  name = g_malloc0(strlen(part) - i);
  strncpy(name, part + i + 1, strlen(part) - i -2);
//  g_print("res: %s\n", name);
  return name;
}

void call_ctags(ClassbrowserBackend *classback, gchar *filename){
  if (!filename) return;
  gboolean result;
  gchar *stdout;
  gint exit_status;
  GError *error=NULL;
  gchar *stdouterr;
  gchar *path=filename_get_path(filename);
  gchar *command_line=g_strdup_printf("ctags -x '%s'",path);
  gphpedit_debug_message(DEBUG_CLASSBROWSER,"%s", command_line);
  result = g_spawn_command_line_sync (command_line, &stdout, &stdouterr, &exit_status, &error);
  g_free(command_line);
  g_free(path);

  if (result) {
//   g_print("ctags:%s ->(%s)\n",stdout,stdouterr);

  gchar *copy;
  gchar *token;
  gchar *name;
  gchar *type;
  gchar *line;
  gchar *param;
  copy = stdout;
    while ((token = strtok(copy, "\n"))) {
        gint ad=0;
        name=get_ctags_token(token,&ad);
//        g_print("name:%s ",name);
        token+=ad;
        type=get_ctags_token(token,&ad);
//        g_print("type:%s ",type);
        token+=ad;
        line=get_ctags_token(token,&ad);
//        g_print("line:%s\n",line);
        param = get_ctags_param(token,&ad);
        if (is_cobol_file(filename)) {
            process_cobol_word(classback, name, filename, type, line);
        } else if (is_cxx_file(filename)) {
            process_cxx_word(classback, name, filename, type, line, param);
        } else if (is_python_file(filename)) {
            process_python_word(classback, name, filename, type, line, param);
        } else if (is_perl_file(filename)) {
            process_perl_word(classback, name, filename, type, line, param);
        }
        g_free(name);
        g_free(line);
        g_free(type);
        g_free(param);
        copy = NULL;
      }
    //we have all functions in the same GTree and we distinguish by filetype (PHP,COBOL,C/C++,PERL,PYTHON,ect).
    g_free(stdouterr);
    g_free(stdout);
  }
}
#endif

void classbrowser_functionlist_add(ClassbrowserBackend *classback, gchar *classname, gchar *funcname, gchar *filename, gint file_type, guint line_number, gchar *param_list)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);

  ClassBrowserClass *class;
  ClassBrowserFunction *function;
  gchar *key = g_strdup_printf("%s%s%s",funcname,classname,filename);
  if ((function = g_hash_table_lookup (classbackdet->functionlist, key))){
    if (function->paramlist) g_free(function->paramlist);
    if (param_list) function->paramlist = g_strdup(param_list);
    function->line_number = line_number;
    function->remove = FALSE;
    g_free(key);
  } else {
    function = g_slice_new0(ClassBrowserFunction);
    function->functionname = g_strdup(funcname);
    if (param_list) function->paramlist = g_strdup(param_list);
    function->filename = g_strdup(filename);
    function->line_number = line_number;
    function->remove = FALSE;
    function->identifierid = classbackdet->identifierid++;
    function->file_type = file_type;
    gchar *keyname=g_strdup_printf("%s%s",classname,filename);
    if (classname && (class = g_hash_table_lookup (classbackdet->php_class_tree, keyname))) {
      function->class_id = class->identifierid;
      function->classname = g_strdup(classname);
    }
    g_free(keyname);
    g_hash_table_insert (classbackdet->functionlist, key, function);
    gphpedit_debug_message(DEBUG_CLASSBROWSER,"filename: %s fucntion: %s\n", filename, funcname);
  }
}

GList *classbrowser_backend_get_function_list(ClassbrowserBackend *classback)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  return g_hash_table_get_values (classbackdet->functionlist);
}

GList *classbrowser_backend_get_class_list(ClassbrowserBackend *classback)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  return g_hash_table_get_values (classbackdet->php_class_tree);
}

static void make_class_completion_string (gpointer key, gpointer value, gpointer data) {
  ClassbrowserBackendDetails *classbackdet = (ClassbrowserBackendDetails *)data;
  ClassBrowserClass *class;
  class=(ClassBrowserClass *)value;
  if (class->file_type != classbackdet->file_type) return ;
  if (!classbackdet->completion_string) {
    classbackdet->completion_string = g_string_new(g_strchug(class->classname));
    classbackdet->completion_string = g_string_append(classbackdet->completion_string, "?4"); /* add corresponding image*/
  } else {
    g_string_append_printf (classbackdet->completion_string," %s?4", g_strchug(class->classname)); /* add corresponding image*/
  }
}

gchar *classbrowser_backend_get_autocomplete_php_classes_string(ClassbrowserBackend *classback){
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  classbackdet->completion_string = NULL;
  classbackdet->file_type = TAB_PHP;

  g_hash_table_foreach (classbackdet->php_class_tree, make_class_completion_string, classbackdet);

  return g_string_free(classbackdet->completion_string, FALSE);
}

typedef struct {
 gchar *prefix;
 GString *completion_result;
 gint file_type;
} var_find;

static void make_completion_string (gpointer key, gpointer value, gpointer data)
{
  var_find *search_data = (var_find *)data;
  ClassBrowserVar *var;
  var=(ClassBrowserVar *)value;
  if (g_str_has_prefix(key,search_data->prefix) && search_data->file_type == var->file_type){
    if (!search_data->completion_result) {
      search_data->completion_result = g_string_new(var->varname);
      search_data->completion_result = g_string_append(search_data->completion_result, "?3");
    } else {
      search_data->completion_result = g_string_append(search_data->completion_result, " ");
      search_data->completion_result = g_string_append(search_data->completion_result, var->varname);
      search_data->completion_result = g_string_append(search_data->completion_result, "?3"); /* add corresponding image*/
    }
  }
}

/*
* classbrowser_backend_autocomplete_php_variables
*/
gchar *classbrowser_backend_autocomplete_php_variables(ClassbrowserBackend *classback, gchar *buffer)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  gchar *result = NULL;

  var_find *search_data= g_slice_new(var_find);
  search_data->prefix=buffer;
  search_data->completion_result = NULL;
  search_data->file_type = TAB_PHP;
  g_hash_table_foreach (classbackdet->php_variables_tree, make_completion_string,search_data);
  if (search_data->completion_result){
    result = g_string_free(search_data->completion_result, FALSE); /*release resources*/
  }
  g_slice_free(var_find, search_data);  
  gphpedit_debug_message(DEBUG_CLASSBROWSER,"prefix: %s autocomplete list:%s\n", buffer, result);
  return result;
}

void make_result_member_string (gpointer key, gpointer value, gpointer user_data)
{
  ClassBrowserFunction *function = (ClassBrowserFunction *) value;
  ClassbrowserBackendDetails *classbackdet = (ClassbrowserBackendDetails *) user_data;

  if ((g_str_has_prefix(function->functionname, classbackdet->completion_prefix) && function->file_type==classbackdet->file_type)) {
    if (!classbackdet->completion_string) {
      classbackdet->completion_string = g_string_new(function->functionname);
      classbackdet->completion_string = g_string_append(classbackdet->completion_string, "?1");
    } else {
       g_string_append_printf(classbackdet->completion_string, " %s?1", function->functionname);
    }
  }
}

gchar *classbrowser_backend_autocomplete_member_function(ClassbrowserBackend *classback, gchar *prefix)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  classbackdet->completion_string = NULL;
  classbackdet->completion_prefix = prefix;
  classbackdet->file_type = TAB_PHP;

  g_hash_table_foreach (classbackdet->functionlist, make_result_member_string, classbackdet);
  if (classbackdet->completion_string){
    gphpedit_debug_message(DEBUG_CLASSBROWSER, "prefix: %s autocomplete list:%s\n", prefix, classbackdet->completion_string->str);
    return g_string_free(classbackdet->completion_string, FALSE);
  }

  gphpedit_debug_message(DEBUG_CLASSBROWSER, "prefix: %s autocomplete list:%s\n", prefix, "null");
  return NULL;
}

gboolean get_custom_calltip (gpointer key, gpointer value, gpointer user_data)
{
  ClassBrowserFunction *function = (ClassBrowserFunction *) value;
  ClassbrowserBackendDetails *classbackdet = (ClassbrowserBackendDetails *) user_data;

  if (g_utf8_collate(function->functionname, classbackdet->completion_prefix)==0 && function->file_type==classbackdet->file_type) {
    return TRUE;
  }
  return FALSE;
}

gchar *classbrowser_backend_custom_function_calltip(ClassbrowserBackend *classback, gchar *function_name, gint file_type)
{
/*FIXME::two functions diferent classes same name = bad calltip */
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  ClassBrowserFunction *function;
  gchar *calltip = NULL;
  classbackdet->completion_prefix = function_name;
  classbackdet->file_type = file_type;
  function = g_hash_table_find (classbackdet->functionlist, get_custom_calltip, classbackdet);
  if (function) {
    calltip = g_strdup_printf("%s (%s)",function->functionname,function->paramlist);
  }
  gphpedit_debug_message(DEBUG_CLASSBROWSER,"custom calltip: %s\n", calltip);
  return calltip;
}

void make_result_string (gpointer key, gpointer value, gpointer user_data)
{
  gchar *function_name = (gchar *)value;
  ClassbrowserBackendDetails *classbackdet = (ClassbrowserBackendDetails *) user_data;
  if (!classbackdet->completion_string) {
    classbackdet->completion_string = g_string_new(function_name);
  } else {
    classbackdet->completion_string = g_string_append(classbackdet->completion_string, " ");
    classbackdet->completion_string = g_string_append(classbackdet->completion_string, function_name);
  }
}

void add_result_item (gpointer key, gpointer value, gpointer user_data)
{
  ClassBrowserFunction *function = (ClassBrowserFunction *) value;
  ClassbrowserBackendDetails *classbackdet = (ClassbrowserBackendDetails *) user_data;
  if ((g_str_has_prefix(function->functionname, classbackdet->completion_prefix) && function->file_type==classbackdet->file_type)) {
    g_hash_table_insert (classbackdet->completion_list, function->functionname, g_strdup_printf("%s?1",function->functionname));
  }
}

gchar *classbrowser_backend_add_custom_autocompletion(ClassbrowserBackend *classback, gchar *prefix, gint file_type, GSList *list)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  GSList *li;
  gchar *function_name;
  classbackdet->completion_prefix = prefix;
  classbackdet->file_type = file_type;

  classbackdet->completion_list = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, (GDestroyNotify) g_free);
  g_hash_table_foreach (classbackdet->functionlist, add_result_item, classbackdet);
  /* add functions */
  for(li = list; li!= NULL; li = g_slist_next(li)) {
    function_name = li->data;
    g_hash_table_insert (classbackdet->completion_list, function_name, g_strdup(function_name));
  }

  classbackdet->completion_string = NULL;
  g_hash_table_foreach (classbackdet->completion_list, make_result_string, classbackdet);
  g_hash_table_destroy (classbackdet->completion_list);

  if (classbackdet->completion_string){
    gphpedit_debug_message(DEBUG_CLASSBROWSER, "prefix: %s autocomplete list:%s\n", prefix, classbackdet->completion_string->str);
    return g_string_free(classbackdet->completion_string, FALSE);
  }

  gphpedit_debug_message(DEBUG_CLASSBROWSER, "prefix: %s autocomplete list:%s\n", prefix, "null");
  return NULL;
}

#ifdef HAVE_CTAGS_EXUBERANT
static inline gboolean is_cobol_banned_word(gchar *word)
{
  return (g_strcmp0(word,"AUTHOR")==0 || g_strcmp0(word,"OBJECT-COMPUTER")==0 || g_strcmp0(word,"DATE-WRITTEN")==0 || g_strcmp0(word,"PROGRAM-ID")==0 || g_strcmp0(word,"SOURCE-COMPUTER")==0 || g_strcmp0(word,"END-PERFORM")==0 || g_strcmp0(word, "END-EVALUATE")==0 || g_strcmp0(word,"FILE-CONTROL")==0 || g_strcmp0(word,"SPECIAL-NAMES")==0 || g_strcmp0(word,"END-IF")==0 || g_strcmp0(word,"END-START")==0);
}

void process_cobol_word(ClassbrowserBackend *classback, gchar *name,gchar *filename,gchar *type,gchar *line)
{
 if (g_strcmp0(type,"paragraph")==0 && !is_cobol_banned_word(name)) {
  classbrowser_functionlist_add(classback, NULL, name, filename, TAB_COBOL, atoi(line), NULL);
 }
}

void process_cxx_word(ClassbrowserBackend *classback, gchar *name,gchar *filename,gchar *type,gchar *line, gchar *param)
{
 if (g_strcmp0(type,"function")==0) {
    classbrowser_functionlist_add(classback, NULL, name, filename, TAB_CXX, atoi(line), param);
 } else if (g_strcmp0(type,"macro")==0) {
    classbrowser_functionlist_add(classback, NULL, name, filename, TAB_CXX, atoi(line), NULL);
 }
}

typedef struct {
 gint line;
 ClassBrowserClass *class;
 gint file_type;
} class_find;

/*
  search class for the member based on line number.
  we have a line number, so search back for last defined class.
*/
void search_class (gpointer key, gpointer value, gpointer data){
  class_find *cl = (class_find *) data;
  ClassBrowserClass *class = (ClassBrowserClass *) value;
  if (class->line_number < cl->line) {
    if (cl->class) {
      if (class->line_number > cl->class->line_number) cl->class = class;
    } else {
      cl->class = class;
    }
  }
}

gchar *get_fixed_param(gchar *param)
{
  gchar *parameters = NULL;
  /* some function came with a trailing ')' we must remove it */
  gchar *par = param;
  if (g_str_has_suffix(param, ")")) {
    gint len = strlen(param) - 1;
    if (len != 0) {
      parameters = g_malloc0(len);
      strncpy(parameters, param, len);
      par = parameters;
    } else {
      par = NULL;
    }
  } else {
    par = g_strdup(param);
  }
  return par;
}

void process_python_word(ClassbrowserBackend *classback, gchar *name, gchar *filename, gchar *type, gchar *line, gchar *param)
{
  ClassbrowserBackendDetails *classbackdet;
	classbackdet = CLASSBROWSER_BACKEND_GET_PRIVATE(classback);
  gchar *parameters = NULL;

 if (g_strcmp0(type,"function")==0) {
    /* some function came with a trailing ')' we must remove it */
    parameters = get_fixed_param(param);
    classbrowser_functionlist_add(classback, NULL, name, filename, TAB_PYTHON, atoi(line), parameters);
    g_free(parameters);
 } else if (g_strcmp0(type,"class")==0){
    classbrowser_classlist_add(classback, name, filename, atoi(line), TAB_PYTHON);
 } else if (g_strcmp0(type,"member")==0){
  gchar *classname = NULL;
  class_find *search_data= g_slice_new(class_find);
  search_data->line = atoi(line);
  search_data->class = NULL;
  search_data->file_type = TAB_PYTHON;
  g_hash_table_foreach (classbackdet->php_class_tree, search_class, search_data);
  if (search_data->class) classname = search_data->class->classname;
  parameters = get_fixed_param(param);
  classbrowser_functionlist_add(classback, classname, name, filename, TAB_PYTHON, atoi(line), parameters);
  g_free(parameters);
  g_slice_free(class_find, search_data);
 }
}

void process_perl_word(ClassbrowserBackend *classback, gchar *name, gchar *filename, gchar *type, gchar *line, gchar *param)
{
  //FIXME: no package support
 if (g_strcmp0(type,"subroutine")==0) {
    classbrowser_functionlist_add(classback, NULL, name, filename, TAB_PERL, atoi(line), param);
 }
}
#endif
