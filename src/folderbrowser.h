/* This file is part of gPHPEdit, a GNOME2 PHP Editor.

   Copyright (C) 2003, 2004, 2005 Andy Jeffries <andy at gphpedit.org>
   Copyright (C) 2009 Anoop John <anoop dot john at zyxware.com>
   Copyright (C) 2009 José Rostagno(for vijona.com.ar)

   For more information or to find the latest release, visit our
   website at http://www.gphpedit.org/

   gPHPEdit is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   gPHPEdit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with gPHPEdit.  If not, see <http://www.gnu.org/licenses/>.

   The GNU General Public License is contained in the file COPYING.
*/
#ifndef FOLDER_BROWSER_H
#define FOLDER_BROWSER_H

#include <gtk/gtk.h>

#define MIME_ISDIR(string) (strcmp(string, "inode/directory")==0)
#define IS_MIME(stringa,stringb) (g_content_type_equals (stringa, stringb))
#define IS_TEXT(stringa) (g_content_type_is_a (stringa, "text/*"))
#define IS_APPLICATION(stringa) (g_content_type_is_a (stringa, "application/*") && !IS_MIME(stringa,"application/x-php") && !IS_MIME(stringa,"application/javascript") && !IS_MIME(stringa,"application/x-perl"))
#define DEFAULT_DIR (N_("Workspace's directory"))
#define IS_DEFAULT_DIR(a) (strcmp(a,DEFAULT_DIR)==0)
#define CURRENTFOLDER gtk_button_get_label(GTK_BUTTON(main_window.button_dialog))
#define FOLDER_INFOFLAGS "standard::is-backup,standard::display-name,standard::icon,standard::content-type"

enum {
  ICON_COLUMN,
  FILE_COLUMN,
  MIME_COLUMN,
  N_COL
};
void update_folderbrowser (gchar *newpath);
void init_folderbrowser(GtkTreeStore *pTree, gchar *filename, GtkTreeIter *iter, GtkTreeIter *iter2);
void update_folderbrowser_signal (GFileMonitor *monitor,GFile *file,GFile *other_file, GFileMonitorEvent event_type, gpointer user_data);
void tree_double_clicked(GtkTreeView *tree_view,GtkTreePath *path,GtkTreeViewColumn *column,gpointer user_data);
gint filebrowser_sort_func(GtkTreeModel * model, GtkTreeIter * a, GtkTreeIter * b, gpointer user_data);
gchar *trunc_on_char(gchar * string, gchar which_char);
#endif
