/* This file is part of gPHPEdit, a GNOME PHP Editor.

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

#include "document_manager.h"
#include "debug.h"
#include "main_window_callbacks.h"
#include "gvfs_utils.h"
#include "gphpedit-close-button.h"
#include "gphpedit-statusbar.h"
#include "classbrowser_ui.h"
#include "document_loader.h"

/*
* document_manager private struct
*/
struct DocumentManagerDetails
{
  Document *current_document;
  GSList *editors;
};

#define DOCUMENT_MANAGER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object),\
					    DOCUMENT_MANAGER_TYPE,\
					    DocumentManagerDetails))

static void document_manager_finalize (GObject *object);
void document_manager_create_new(void);
void document_loader_done_refresh_cb (DocumentLoader *doclod, gboolean result, gpointer user_data);

/*
 * register DocumentManager type and returns a new GType
*/
G_DEFINE_TYPE(DocumentManager, document_manager, G_TYPE_OBJECT);  

/*
* overide default contructor to make a singleton.
* see http://blogs.gnome.org/xclaesse/2010/02/11/how-to-make-a-gobject-singleton/
*/
static GObject*
document_manager_constructor (GType type,
                 guint n_construct_params,
                 GObjectConstructParam *construct_params)
{
  static GObject *self = NULL;

  if (self == NULL)
    {
      self = G_OBJECT_CLASS (document_manager_parent_class)->constructor (
          type, n_construct_params, construct_params);
      g_object_add_weak_pointer (self, (gpointer) &self);
      return self;
    }

  return g_object_ref (self);
}


static void
document_manager_class_init (DocumentManagerClass *klass)
{
  GObjectClass *object_class;

  object_class = G_OBJECT_CLASS (klass);
  object_class->finalize = document_manager_finalize;
  object_class->constructor = document_manager_constructor;
  g_type_class_add_private (klass, sizeof (DocumentManagerDetails));
}

static void
document_manager_init (DocumentManager * object)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  DocumentManagerDetails *docdet;
  docdet = DOCUMENT_MANAGER_GET_PRIVATE(object);
  docdet->current_document = NULL;
  docdet->editors = NULL;
}

static void
document_manager_finalize (GObject *object)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  DocumentManager *doc = DOCUMENT_MANAGER(object);
  DocumentManagerDetails *docdet;
	docdet = DOCUMENT_MANAGER_GET_PRIVATE(doc);
  /* save current session */
  document_manager_session_save(DOCUMENT_MANAGER(object));
  document_manager_close_all_tabs(doc);  
  //free class data
	G_OBJECT_CLASS (document_manager_parent_class)->finalize (object);
}

DocumentManager *document_manager_new (void)
{
	DocumentManager *doc;
  doc = g_object_new (DOCUMENT_MANAGER_TYPE, NULL);
	return doc; /* return new object */
}

DocumentManager *document_manager_new_full (char **argv, gint argc)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
	DocumentManager *docmg;
  docmg = g_object_new (DOCUMENT_MANAGER_TYPE, NULL);
  
  /* load command line files */
  guint i;
  if (argc>1) {
    i = 1;
    while (argv[i] != NULL) {
      document_manager_switch_to_file_or_open(docmg, argv[i],0);
      ++i;
    }
  } else {
  document_manager_session_reopen(docmg);
  }
 	return docmg; /* return new object */
}

/* internal */
GtkWidget *get_close_tab_widget(Document *document) {
  GtkWidget *hbox;
  GtkWidget *close_button;
  hbox = gtk_hbox_new(FALSE, 0);
  close_button = gphpedit_close_button_new ();
  gtk_widget_set_tooltip_text(close_button, _("Close Tab"));

  g_signal_connect(G_OBJECT(close_button), "clicked", G_CALLBACK(on_tab_close_activate), document);
  /* load file icon */
  GtkWidget *label;
  GdkPixbuf *file_icon;
  g_object_get(document, "editor_label", &label, "file_icon", &file_icon, NULL);
  GtkWidget *icon= gtk_image_new_from_pixbuf (file_icon);
  gtk_widget_show (icon);
  gtk_box_pack_start(GTK_BOX(hbox), icon, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);
  gtk_box_pack_end(GTK_BOX(hbox), close_button, FALSE, FALSE, 0);
  gtk_widget_show(close_button);
  gtk_widget_show(hbox);
  return hbox;
}

/* internal */
void document_save_update_cb (Document *doc, gpointer user_data){
  DocumentManager *docmg = DOCUMENT_MANAGER(user_data);
  DocumentManagerDetails *docmgdet;
	docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  update_app_title(docmgdet->current_document);
  classbrowser_update(GPHPEDIT_CLASSBROWSER(main_window.classbrowser));
}

void document_save_start_cb (Document *doc, gpointer user_data){
  /* show status in statusbar */
  const gchar *short_filename;
  g_object_get(doc, "short_filename", &short_filename, NULL);
  gphpedit_statusbar_flash_message (GPHPEDIT_STATUSBAR(main_window.appbar),0,_("Saving %s"), short_filename);
}

void document_type_changed_cb (Document *doc, gint type, gpointer user_data)
{
	DocumentManager *docmg = DOCUMENT_MANAGER(user_data);
	DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
	/* only process if document is current_document */
	if (doc==docmgdet->current_document) update_status_combobox(docmgdet->current_document);
}

void document_need_reload_cb (Document *doc, gpointer user_data)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  DocumentLoader *loader = document_loader_new (GTK_WINDOW(main_window.window));
  g_signal_connect(G_OBJECT(loader), "done_refresh", G_CALLBACK(document_loader_done_refresh_cb), user_data);
  document_loader_reload_file(loader, doc);
}

void document_loader_done_loading_cb (DocumentLoader *doclod, gboolean result, Document *doc, gpointer user_data)
{
	DocumentManager *docmg = DOCUMENT_MANAGER(user_data);
	DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  if (result) {
    gboolean untitled;
    docmgdet->editors = g_slist_append(docmgdet->editors, doc);
    GtkWidget *document_tab;
    document_tab = get_close_tab_widget(doc);
    GtkWidget *document_widget;
    g_object_get(doc, "untitled", &untitled, "editor_widget", &document_widget, NULL);
    gtk_widget_show(document_widget);
    gtk_notebook_append_page (GTK_NOTEBOOK (main_window.notebook_editor), document_widget, document_tab);
    gtk_notebook_set_current_page (GTK_NOTEBOOK (main_window.notebook_editor), -1);
    docmgdet->current_document = doc;
    document_grab_focus(doc);
    update_app_title(doc);
    if (!untitled) document_manager_session_save(docmg);
    classbrowser_update(GPHPEDIT_CLASSBROWSER(main_window.classbrowser));
    g_signal_connect(G_OBJECT(doc), "save_update", G_CALLBACK(document_save_update_cb), docmg);
    g_signal_connect(G_OBJECT(doc), "save_start", G_CALLBACK(document_save_start_cb), NULL);
    g_signal_connect(G_OBJECT(doc), "type_changed", G_CALLBACK(document_type_changed_cb), docmg);
    g_signal_connect(G_OBJECT(doc), "need_reload", G_CALLBACK(document_need_reload_cb), docmg);
  }
  g_object_unref(doclod);
}

void document_loader_need_mounting_cb (DocumentLoader *doclod, gpointer user_data)
{
  gphpedit_statusbar_flash_message (GPHPEDIT_STATUSBAR(main_window.appbar),0,"%s", 
    _("Error filesystem not mounted. Mounting filesystem, this will take a few seconds..."));
}

void document_loader_help_file_not_found_cb (DocumentLoader *doclod, gpointer user_data)
{
  gphpedit_statusbar_flash_message (GPHPEDIT_STATUSBAR(main_window.appbar),0,"%s",_("Could not find the required command in the online help"));
  g_object_unref(doclod);
}

void document_manager_add_new_document(DocumentManager *docmg, gint type, const gchar *filename, gint goto_line)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;

  DocumentLoader *loader = document_loader_new (GTK_WINDOW(main_window.window));
  g_signal_connect(G_OBJECT(loader), "done_loading", G_CALLBACK(document_loader_done_loading_cb), docmg);
  g_signal_connect(G_OBJECT(loader), "need_mounting", G_CALLBACK(document_loader_need_mounting_cb), docmg);
  g_signal_connect(G_OBJECT(loader), "help_file_not_found", G_CALLBACK(document_loader_help_file_not_found_cb), docmg);
  document_loader_load (loader, type, (gchar *) filename, goto_line);
}

Document *document_manager_find_document_from_widget (DocumentManager *docmg, void *widget)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return NULL;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  GSList *walk;
  Document *document;
  GtkWidget *document_widget;
  for (walk = docmgdet->editors; walk != NULL; walk = g_slist_next (walk)) {
    document = walk->data;
    g_object_get(document, "editor_widget", &document_widget, NULL);
    if (document_widget == GTK_WIDGET(widget)) return walk->data;
  }
  return NULL;
}

Document *document_manager_find_document_from_filename (DocumentManager *docmg, gchar *filename)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return NULL;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  GSList *walk;
  Document *document;

  for (walk = docmgdet->editors; walk != NULL; walk = g_slist_next (walk)) {
    document = walk->data;
    gchar *doc_filename = document_get_filename(document);
    if (g_strcmp0(doc_filename,filename)==0) {
      g_free(doc_filename);
      return document;
    }
    g_free(doc_filename);
  }
  return NULL;
}

Document *document_manager_get_current_document (DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return NULL;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  return docmgdet->current_document;
}

gboolean document_manager_set_current_document_from_widget (DocumentManager *docmg, GtkWidget *child)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return FALSE;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  Document *data = document_manager_find_document_from_widget (docmg, (void *) child);
  if (data){
    // Store it in the global main_window.current_editor value
    docmgdet->current_document = data;
    document_grab_focus(data);
  } else {
    return FALSE;
  }
  return TRUE;
}

void document_manager_get_context_help(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  gchar *buffer = document_get_current_selected_text(docmgdet->current_document);
  if (buffer){
    document_manager_add_new_document(docmg, TAB_HELP, buffer, 0);
  }
}

void document_manager_get_document_preview(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  gchar *filename = document_get_filename(docmgdet->current_document);
  document_manager_add_new_document(docmg, TAB_PREVIEW, filename, 0);
  g_free(filename);
}

// session_save relies on the fact that all tabs can be closed without 
// prompting, they should already be saved.  Also, the title won't be set
// afterwards.
void document_manager_session_save(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  GSList *walk;
  Document *document;
  GString *session_file_contents;
  gboolean save_session;

  gchar *uri = g_build_filename (g_get_user_config_dir (), "gphpedit", "session", NULL);
  GFile *file = g_file_new_for_commandline_arg(uri);
  g_free(uri);
  GError *error=NULL;

  g_object_get (main_window.prefmg, "save_session", &save_session, NULL);
  if (save_session && (g_slist_length(docmgdet->editors) > 0)) {
    session_file_contents=g_string_new(NULL);
    for(walk = docmgdet->editors; walk!= NULL; walk = g_slist_next(walk)) {
      document = walk->data;
      gchar *entry = document_get_session_entry(document);
      if (document == docmgdet->current_document) {
        session_file_contents = g_string_append(session_file_contents,"*");
      }
      session_file_contents = g_string_append(session_file_contents, entry);
      g_free(entry);
    }
    if(!g_file_replace_contents (file, session_file_contents->str, session_file_contents->len, NULL, FALSE, G_FILE_CREATE_NONE, NULL, NULL, &error)){
      g_print(_("Error Saving session file: %s\n"),error->message);
      g_error_free (error);
    }
    if (session_file_contents) g_string_free(session_file_contents, TRUE);
  } else {
      if (save_session){
        if (!g_file_delete (file,NULL,&error)){
          if (error->code!=G_FILE_ERROR_NOENT && error->code!=1){
            g_print(_("GIO Error deleting file: %s, code %d\n"), error->message,error->code);
          }
          g_error_free (error);
        }
      }
  }
  g_object_unref(file);
}

void document_manager_session_reopen(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  char *filename;
  int focus_tab=-1;
  gboolean focus_this_one = FALSE;

  gchar *uri = g_build_filename (g_get_user_config_dir (), "gphpedit", "session", NULL);
  GFile *file = g_file_new_for_commandline_arg(uri);

  if(g_file_query_exists(file,NULL)){
    int number =0;
    gchar *content=read_text_file_sync(uri);
    gchar **strings;
    strings = g_strsplit (content,"\n",0);
      int i=0;
      while (strings[i]){
        /* strings[i] contains possibly:
          file:///blah\n
          *file:///blah\n
          phphelp:function\n
          *phphelp:function\n
          preview:function\n
          *preview:function\n

        */
        if (strings[i][0]==0) break;
        number++;
        filename = strings[i];
        str_replace(filename, 10, 0);
        if (strings[i][0]=='*') {
          filename++;
          focus_this_one = TRUE;
        }
        if (g_str_has_prefix(filename, "phphelp:")){
          filename += 8;
          document_manager_add_new_document(docmg, TAB_HELP, filename, 0);
        } else if (g_str_has_prefix(filename, "preview:")){
          filename += 8;
          document_manager_add_new_document(docmg, TAB_PREVIEW, filename, 0);
        } else {
          if (filename){
          document_manager_switch_to_file_or_open(docmg, filename, 0);
          }
        }
        if (focus_this_one && (docmgdet->current_document)) {
            GtkWidget *document_widget;
            g_object_get(docmgdet->current_document, "editor_widget", &document_widget, NULL);
            focus_tab = gtk_notebook_page_num(GTK_NOTEBOOK(main_window.notebook_editor), document_widget);
        }
        focus_this_one=FALSE;
        i++;    
      }
      g_strfreev (strings);
    if (number==0){ 
      //session file exists but is empty
      //add a new untitled
     document_manager_add_new_document(docmg, TAB_FILE, NULL, 0);
    }
    g_free(content);
    gtk_notebook_set_current_page( GTK_NOTEBOOK(main_window.notebook_editor), focus_tab);
  } else {
    //add a new untitled
    document_manager_add_new_document(docmg, TAB_FILE, NULL, 0);
  }

  GError *error=NULL;
  if (!g_file_delete (file,NULL,&error)){
      if (error->code!=G_FILE_ERROR_NOENT && error->code!=1){
        g_print(_("GIO Error deleting file: %s, code %d\n"), error->message,error->code);
      }
      g_error_free (error);
  }
  g_free(uri);
  g_object_unref (file);
}

void document_manager_switch_to_file_or_open(DocumentManager *docmg, gchar *filename, gint line_number)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  Document *document;
  GSList *walk;
  // need to check if filename is local before adding to the listen
  filename = g_strdup(filename);
  for (walk = docmgdet->editors; walk!=NULL; walk = g_slist_next(walk)) {
    document = walk->data;
    gchar *docfilename;
    GFile *file;
    g_object_get(document, "GFile", &file, NULL);
    docfilename = g_file_get_uri(file);
    gchar *filename_uri = filename_get_uri(filename);
    if (g_strcmp0(docfilename, filename_uri)==0) {
      GtkWidget *document_widget;
      g_object_get(document, "editor_widget", &document_widget, NULL);
      gtk_notebook_set_current_page( GTK_NOTEBOOK(main_window.notebook_editor), gtk_notebook_page_num(GTK_NOTEBOOK(main_window.notebook_editor),document_widget));
      docmgdet->current_document = document;
      document_goto_line(docmgdet->current_document, line_number);
      g_free(docfilename);
      return ;
    }
    g_free(filename_uri);
    g_free(docfilename);
  }
  document_manager_add_new_document(docmg, TAB_FILE, filename, line_number);
  register_file_opened(filename);
  g_free(filename);
  return ;
}

void document_loader_done_refresh_cb (DocumentLoader *doclod, gboolean result, gpointer user_data)
{
  g_object_unref(doclod);
}

void document_manager_document_reload(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  gboolean saved;
  g_object_get(docmgdet->current_document, "saved", &saved, NULL);
  if (!saved) {
    gint result = yes_no_dialog (_("Question"), _("Are you sure you wish to reload the current file, losing your changes?"));
    if (result==GTK_RESPONSE_NO) return ;
  }
  const gchar *short_filename;
  g_object_get(docmgdet->current_document, "short_filename", &short_filename, NULL);
  gphpedit_statusbar_flash_message (GPHPEDIT_STATUSBAR(main_window.appbar),0,_("Reloading %s"), short_filename);
  document_reload(docmgdet->current_document);
}

void document_manager_open_selected(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  GSList *li;
  Document *document;
  gchar *ac_buffer;
  gchar *file;

  ac_buffer = document_get_current_selected_text(docmgdet->current_document);
  if (!ac_buffer){
    for(li = docmgdet->editors; li!= NULL; li = g_slist_next(li)) {
      document = li->data;
      if (document) {
         gchar *filename = document_get_filename(document);
         file = filename_parent_uri(filename);
         g_free(filename);
         gphpedit_debug_message(DEBUG_MAIN_WINDOW,"file: %s", filename);
         if (!strstr(ac_buffer, "://") && file) {
          gchar *filetemp= g_strdup_printf("%s/%s",file, ac_buffer);
          g_free(file);
          file=g_strdup(filetemp);
          g_free(filetemp);
        }
        else if (strstr(ac_buffer, "://")) {
            if (file) g_free(file);
            file = g_strdup(ac_buffer);
        }
        if(filename_file_exist(file)) document_manager_switch_to_file_or_open(docmg, file,0);
        if (file) g_free(file);
      }
    }
  }
  g_free(ac_buffer);
}

GSList *document_manager_get_document_list (DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return NULL;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  return docmgdet->editors;
}

gint document_manager_get_document_count (DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return 0;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  if (!docmgdet->editors) return 0;
  return g_slist_length(docmgdet->editors);
}

// This procedure relies on the fact that all tabs will be closed without prompting
// for whether they need saving beforehand.  If in doubt, call can_all_tabs_be_saved
// and pay attention to the return value.
void document_manager_close_all_tabs(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  GSList *walk;
  Document *document;

  for(walk = docmgdet->editors; walk!= NULL; walk = g_slist_next(walk)) {
    document = walk->data;
    if (document) {
     document_manager_close_page(docmg, document);
    }
  }
  docmgdet->editors = NULL;
  docmgdet->current_document=NULL;
}

// Returns true if all tabs are either saved or closed
gboolean document_manager_can_all_tabs_be_saved(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return TRUE;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  GSList *walk;
  Document *document;
  gboolean saved;

  for(walk = docmgdet->editors; walk!= NULL; walk = g_slist_next(walk)) {
    document = walk->data;
    GtkWidget *document_widget;
    g_object_get(document, "editor_widget", &document_widget, NULL);
    if (document_widget) {
      gboolean read_only, saved_status;
      g_object_get(document, "read_only", &read_only, "saved", &saved_status, NULL);
      if (!saved_status && !read_only) {
        saved = document_manager_try_save_page(docmg, document, FALSE);
        if (saved==FALSE) return FALSE;
      }
    }
  }
  return TRUE;
}

void document_manager_close_page(DocumentManager *docmg, Document *document)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  close_page(document);
  g_object_unref(document);
  docmgdet->editors = g_slist_remove(docmgdet->editors, document);
  if (!docmgdet->editors) {
    docmgdet->current_document = NULL;
  }
}

gboolean document_manager_try_save_page(DocumentManager *docmg, Document *document, gboolean close_if_can)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return TRUE;
  gint ret;
  const gchar *short_filename;
  g_object_get(document, "short_filename", &short_filename, NULL);

  GtkWidget *confirm_dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW(main_window.window),
    GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_WARNING, GTK_BUTTONS_NONE,
      /*TRANSLATORS: this is a pango markup string you must keep the format tags. */
    _("<b>The file '%s' has not been saved since your last changes.</b>\n<small>Are you sure you want to close it and lose these changes?</small>"),
     short_filename);

  gtk_dialog_add_button (GTK_DIALOG(confirm_dialog),_("Close and _lose changes"), 0);
  gboolean untitled;
  g_object_get(document, "untitled", &untitled, NULL);
  if (untitled){
    gtk_dialog_add_button (GTK_DIALOG(confirm_dialog), GTK_STOCK_SAVE_AS, 1);
  } else {
    gtk_dialog_add_button (GTK_DIALOG(confirm_dialog), GTK_STOCK_SAVE, 1);
  }
  gtk_dialog_add_button (GTK_DIALOG(confirm_dialog), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
  gtk_dialog_set_default_response (GTK_DIALOG(confirm_dialog), 1); /* set save as default */

  ret = gtk_dialog_run (GTK_DIALOG (confirm_dialog));
  gtk_widget_destroy(confirm_dialog);
  switch (ret) {
    case 0:
      if (close_if_can) {
          document_manager_close_page(docmg, document);
      }
      return TRUE;
    case 1:
      on_save1_activate(NULL);
      // If chose neither of these, dialog either cancelled or closed. Do nothing.
  }
  return FALSE;
}


void document_manager_save_all(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  GSList *li;
  Document *doc;
  for(li = docmgdet->editors; li!= NULL; li = g_slist_next(li)) {
    doc = li->data;
    gboolean untitled;
    g_object_get(doc, "untitled", &untitled, NULL);
    if (untitled){
      gphpedit_debug_message(DEBUG_MAIN_WINDOW,"%s","Untitled found. Save not implemented"); //FIXME:
    } else {
      document_save(doc);
    }
  }
  classbrowser_update(GPHPEDIT_CLASSBROWSER(main_window.classbrowser));
}

gboolean document_manager_set_current_document_from_position(DocumentManager *docmg, gint page_num)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return FALSE;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  Document *new_current_editor;
  new_current_editor = DOCUMENT(g_slist_nth_data(docmgdet->editors, page_num));
  docmgdet->current_document = new_current_editor;
  if (new_current_editor) return TRUE;
  return FALSE;
}

gboolean document_manager_try_close_document(DocumentManager *docmg, Document *document)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  gboolean read_only, is_empty, saved_status;
  g_object_get(document, "read_only", &read_only, "is_empty", &is_empty, "saved", &saved_status, NULL);

  if (!read_only && !saved_status && !is_empty) {
    return document_manager_try_save_page(main_window.docmg, document, TRUE);
  }
  document_manager_close_page(main_window.docmg, document);
  return TRUE;
}

gboolean document_manager_try_close_current_document(DocumentManager *docmg)
{
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return FALSE;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  return document_manager_try_close_document(docmg, docmgdet->current_document);
}

void document_manager_refresh_properties_all(DocumentManager *docmg)
{
  gint type;
  gphpedit_debug(DEBUG_DOC_MANAGER);
  if (!docmg) return ;
  DocumentManagerDetails *docmgdet = DOCUMENT_MANAGER_GET_PRIVATE(docmg);
  GSList *walk;
  for (walk = docmgdet->editors; walk!=NULL; walk = g_slist_next(walk)) {
    Document *document = walk->data;
    document_refresh_properties(document);
    tab_check_type_file(document);
    g_object_get(document, "type", &type, NULL);
    if (type==TAB_FILE) set_document_to_type(document, type);
  }
}
