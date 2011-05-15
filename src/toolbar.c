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

#include "main_window.h"
#include "main_window_callbacks.h"

#include <gdk/gdkkeysyms.h>
#include "toolbar.h"

#define TOOLBAR_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), \
						GOBJECT_TYPE_TOOLBAR,              \
						ToolBarPrivate))

struct _ToolBarPrivate 
{
/* main toolbar widgets */
  GtkWidget *button_new;
  GtkWidget *button_open;
  GtkWidget *button_save;
  GtkWidget *button_save_as;
  GtkWidget *button_undo;
  GtkWidget *button_redo;
  GtkWidget *button_cut;
  GtkWidget *button_copy;
  GtkWidget *button_paste;
  GtkWidget *button_find;
  GtkWidget *button_replace;
  GtkToolItem *toolbar_separator;
  GtkWidget *button_indent;
  GtkWidget *button_unindent;
  GtkWidget *button_zoom_in;
  GtkWidget *button_zoom_out;
  GtkWidget *button_zoom_100;
};

G_DEFINE_TYPE(ToolBar, TOOLBAR, GTK_TYPE_TOOLBAR)

static void main_toolbar_init (ToolBar *toolbar);

static void
TOOLBAR_class_init (ToolBarClass *klass)
{
	GObjectClass *object_class;

	object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (ToolBarPrivate));
}

/**
* sincronice toolbar icon size menu item with current toolbar icon size value
*/
static void sincronice_menu_items_size (GtkToolbar *toolbar){
  /*sincronice with menu items*/
  if (gtk_toolbar_get_icon_size (toolbar)==GTK_ICON_SIZE_SMALL_TOOLBAR){
    menubar_set_toolbar_size(MENUBAR(main_window.menu), TRUE);
  }else {
    menubar_set_toolbar_size(MENUBAR(main_window.menu), FALSE);
  }
}

/**
* create_toolbar_stock_item
* creates a new toolbar stock item
*/

static inline void create_toolbar_stock_item(GtkWidget **toolitem,ToolBar *toolbar,const gchar *stock_id, gchar *tooltip_text){
  *toolitem = GTK_WIDGET(gtk_tool_button_new_from_stock(stock_id));
  gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM (*toolitem), tooltip_text);
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM (*toolitem), -1);
  gtk_widget_show (*toolitem);
}


static void
main_toolbar_init (ToolBar *toolbar)
{
  ToolBarPrivate *priv = TOOLBAR_GET_PRIVATE(toolbar);

  /* Add the File operations to the Main Toolbar */
  create_toolbar_stock_item(&priv->button_new,toolbar,GTK_STOCK_NEW, _("New File"));

  create_toolbar_stock_item(&priv->button_open,toolbar,GTK_STOCK_OPEN, _("Open File"));

  create_toolbar_stock_item(&priv->button_save,toolbar,GTK_STOCK_SAVE, _("Save current File"));

  create_toolbar_stock_item(&priv->button_save_as,toolbar,GTK_STOCK_SAVE_AS, _("Save File As..."));

  g_signal_connect (G_OBJECT (priv->button_open), "clicked", G_CALLBACK (on_open1_activate), NULL);
  g_signal_connect (G_OBJECT (priv->button_new), "clicked", G_CALLBACK (on_new1_activate), NULL);
  g_signal_connect (G_OBJECT (priv->button_save), "clicked", G_CALLBACK (on_save1_activate), NULL);
  g_signal_connect (G_OBJECT (priv->button_save_as), "clicked", G_CALLBACK (on_save_as1_activate), NULL);

  priv->toolbar_separator=gtk_separator_tool_item_new();
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM (priv->toolbar_separator), -1);
  gtk_widget_show (GTK_WIDGET(priv->toolbar_separator));
  /* Add the Undo operations to the Main Toolbar */
  create_toolbar_stock_item(&priv->button_undo,toolbar,GTK_STOCK_UNDO, _("Undo last change"));
  create_toolbar_stock_item(&priv->button_redo,toolbar,GTK_STOCK_REDO, _("Redo last change"));
  g_signal_connect (G_OBJECT (priv->button_undo), "clicked", G_CALLBACK (on_undo1_activate), NULL);
  g_signal_connect (G_OBJECT (priv->button_redo), "clicked", G_CALLBACK (on_redo1_activate), NULL);
  priv->toolbar_separator=gtk_separator_tool_item_new();
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM (priv->toolbar_separator), -1);
  gtk_widget_show (GTK_WIDGET(priv->toolbar_separator));

  /* Add the Clipboard operations to the Main Toolbar */

  create_toolbar_stock_item(&priv->button_cut,toolbar,GTK_STOCK_CUT, _("Cut Current Selection"));
  g_signal_connect (G_OBJECT (priv->button_cut), "clicked", G_CALLBACK (on_cut1_activate), NULL);

  create_toolbar_stock_item(&priv->button_copy,toolbar,GTK_STOCK_COPY, _("Copy Current Selection"));
  g_signal_connect (G_OBJECT (priv->button_copy), "clicked", G_CALLBACK (on_copy1_activate), NULL);

  create_toolbar_stock_item(&priv->button_paste,toolbar,GTK_STOCK_PASTE, _("Paste text from clipboard"));
  g_signal_connect (G_OBJECT (priv->button_paste), "clicked", G_CALLBACK (on_paste1_activate), NULL);
  
  priv->toolbar_separator=gtk_separator_tool_item_new();
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM (priv->toolbar_separator), -1);
  gtk_widget_show (GTK_WIDGET(priv->toolbar_separator));
  /* Add the Search operations to the Main Toolbar */
  create_toolbar_stock_item(&priv->button_find,toolbar,GTK_STOCK_FIND, _("Find text"));
  g_signal_connect (G_OBJECT (priv->button_find), "clicked", G_CALLBACK (on_find1_activate), NULL);

  create_toolbar_stock_item(&priv->button_replace,toolbar,GTK_STOCK_FIND_AND_REPLACE, _("Replace Text"));
  g_signal_connect (G_OBJECT (priv->button_replace), "clicked", G_CALLBACK (on_replace1_activate), NULL);
  
  priv->toolbar_separator=gtk_separator_tool_item_new();
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM (priv->toolbar_separator), -1);
  gtk_widget_show (GTK_WIDGET(priv->toolbar_separator));

  /* Add the indent/unindent operations to the Main Toolbar */
  /*indent block*/
  create_toolbar_stock_item(&priv->button_indent,toolbar,GTK_STOCK_INDENT, _("Indent Selected Text"));
  g_signal_connect (G_OBJECT (priv->button_indent), "clicked", G_CALLBACK (block_indent), NULL);
  /*unindent block*/
  create_toolbar_stock_item(&priv->button_unindent,toolbar,GTK_STOCK_UNINDENT, _("Unindent Selected Text"));
  g_signal_connect (G_OBJECT (priv->button_unindent), "clicked", G_CALLBACK (block_unindent), NULL);
  
  priv->toolbar_separator=gtk_separator_tool_item_new();
  gtk_toolbar_insert(GTK_TOOLBAR(toolbar), GTK_TOOL_ITEM (priv->toolbar_separator), -1);
  gtk_widget_show (GTK_WIDGET(priv->toolbar_separator));
  /* Add Zoom operations to the main Toolbar */
  /* zoom in */
  create_toolbar_stock_item(&priv->button_zoom_in,toolbar,GTK_STOCK_ZOOM_IN, _("Increases Zoom level"));
  g_signal_connect (G_OBJECT (priv->button_zoom_in), "clicked", G_CALLBACK (zoom_in), NULL);
  /* zoom out */
  create_toolbar_stock_item(&priv->button_zoom_out,toolbar,GTK_STOCK_ZOOM_OUT, _("Decreases Zoom level"));
  g_signal_connect (G_OBJECT (priv->button_zoom_out), "clicked", G_CALLBACK (zoom_out), NULL);
  /* zoom 100% */
  create_toolbar_stock_item(&priv->button_zoom_100,toolbar,GTK_STOCK_ZOOM_100, _("Zoom 100%"));
  g_signal_connect (G_OBJECT (priv->button_zoom_100), "clicked", G_CALLBACK (zoom_100), NULL);

}

static void
TOOLBAR_init (ToolBar *toolbar)
{
//  ToolBarPrivate *priv = TOOLBAR_GET_PRIVATE(toolbar);
 /* set toolbar style */
  gtk_container_set_border_width (GTK_CONTAINER (toolbar), 0);
  gtk_toolbar_set_style (GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_set_show_arrow (GTK_TOOLBAR(toolbar), TRUE);

  GtkStyleContext *context = gtk_widget_get_style_context (GTK_WIDGET(toolbar));
  gtk_style_context_set_junction_sides (context, GTK_JUNCTION_BOTTOM);
  gtk_style_context_add_class (context, GTK_STYLE_CLASS_PRIMARY_TOOLBAR);
  gtk_style_context_save (context);

  /* sincronice toolbar icon size */
  sincronice_menu_items_size (GTK_TOOLBAR (toolbar));

  main_toolbar_init (toolbar);
}

GtkWidget *
toolbar_new (void)
{
  ToolBar *toolbar = g_object_new (GOBJECT_TYPE_TOOLBAR, NULL);
	return GTK_WIDGET(toolbar);
}

void toolbar_update_controls(ToolBar *toolbar, gboolean is_scintilla, gboolean isreadonly)
{
  if (!toolbar) return ;
  ToolBarPrivate *priv = TOOLBAR_GET_PRIVATE(toolbar);
  if (!priv) return ;
  if (is_scintilla){
    //activate toolbar items
    gtk_widget_set_sensitive (priv->button_cut, TRUE);
    gtk_widget_set_sensitive (priv->button_paste, TRUE);
    gtk_widget_set_sensitive (priv->button_undo, TRUE);
    gtk_widget_set_sensitive (priv->button_redo, TRUE);
    gtk_widget_set_sensitive (priv->button_replace, TRUE);
    gtk_widget_set_sensitive (priv->button_indent, TRUE);
    gtk_widget_set_sensitive (priv->button_unindent, TRUE);
    gtk_widget_set_sensitive (priv->button_save_as, TRUE);
    if (isreadonly){
      gtk_widget_set_sensitive (priv->button_save, FALSE);
    } else {
      gtk_widget_set_sensitive (priv->button_save, TRUE);
    } 
  } else {
      //deactivate toolbar items
    gtk_widget_set_sensitive (priv->button_cut, FALSE);
    gtk_widget_set_sensitive (priv->button_paste, FALSE);
    gtk_widget_set_sensitive (priv->button_undo, FALSE);
    gtk_widget_set_sensitive (priv->button_redo, FALSE);
    gtk_widget_set_sensitive (priv->button_replace, FALSE);
    gtk_widget_set_sensitive (priv->button_indent, FALSE);
    gtk_widget_set_sensitive (priv->button_unindent, FALSE);
    gtk_widget_set_sensitive (priv->button_save, FALSE);
    gtk_widget_set_sensitive (priv->button_save_as, FALSE);
  }
}
