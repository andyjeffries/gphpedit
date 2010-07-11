/* This file is part of gPHPEdit, a GNOME2 PHP Editor.
 
   Copyright (C) 2003, 2004, 2005 Andy Jeffries <andy at gphpedit.org>
   Copyright (C) 2009 Anoop John <anoop dot john at zyxware.com>
    
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
#include "stdlib.h"
#include "syntax_check.h"
#include "main_window.h"
#include <glib/gstdio.h>
#include "gvfs_utils.h"
#include <unistd.h>
#include "main_window_callbacks.h"
#include "pluginmenu.h"

gchar *run_php_lint(gchar *command_line)
{
  gboolean result;
  gchar *stdout;
  gint exit_status;
  GError *error;
  gchar *stdouterr;
  error = NULL;

  result = g_spawn_command_line_sync (command_line,
                                      &stdout, &stdouterr, &exit_status, &error);

  if (!result) {
    return NULL;
  }
  gchar *res =g_strdup_printf ("%s\n%s",stdouterr,stdout);
  g_free(stdouterr);
  g_free(stdout);
  return res;
}


void syntax_add_lines(gchar *output)
{
  GtkTreeIter   iter;
  gchar *copy;
  gchar *token;
  gchar *line_number;
  gchar *first_error = NULL;
  gint line_start;
  gint line_end;
  gint indent;
  first_error = 0;
  copy = output;
  gtk_scintilla_set_indicator_current(GTK_SCINTILLA(main_window.current_editor->scintilla), 20);
  gtk_scintilla_indic_set_style(GTK_SCINTILLA(main_window.current_editor->scintilla), 20, INDIC_SQUIGGLE);
  gtk_scintilla_indic_set_fore(GTK_SCINTILLA(main_window.current_editor->scintilla), 20, 0x0000ff);

  GtkListStore *lint_store =  gtk_list_store_new (1, G_TYPE_STRING);
  /*clear tree */
  gtk_list_store_clear(lint_store);

  if (main_window.current_editor->type==TAB_PHP){
    while ((token = strtok(copy, "\n"))) {
      if ((g_str_has_prefix(token, "PHP Warning:  ") || g_str_has_prefix(token,"PHP Parse error:  syntax error, ")) && (!g_str_has_prefix(token, "Content-type"))) { 
        if (g_str_has_prefix(token,"PHP Parse error:  syntax error, ")){
        token+=strlen("PHP Parse error:  syntax error, ");
        } else if (g_str_has_prefix(token, "PHP Warning:  ")){
        token+=strlen("PHP Warning:  ");
        }
        gtk_list_store_append (lint_store, &iter);
        gtk_list_store_set (lint_store, &iter, 0, token, -1);

        line_number = strrchr(token, ' ');
        line_number++;
        if (atoi(line_number)>0) {
          if (!first_error) {
            first_error = line_number;
          }
          indent = gtk_scintilla_get_line_indentation(GTK_SCINTILLA(main_window.current_editor->scintilla), atoi(line_number)-1);
    
          line_start = gtk_scintilla_position_from_line(GTK_SCINTILLA(main_window.current_editor->scintilla), atoi(line_number)-1);
          line_start += (indent/get_preferences_manager_indentation_size(main_window.prefmg));
    
          line_end = gtk_scintilla_get_line_end_position(GTK_SCINTILLA(main_window.current_editor->scintilla), atoi(line_number)-1);
          gtk_scintilla_indicator_fill_range(GTK_SCINTILLA(main_window.current_editor->scintilla), line_start, line_end-line_start);
        }
        else {
          g_print("Line number is 0\n");
        }
      }
      copy = NULL;
    }
    if (output && g_str_has_prefix(output,"\nNo syntax errors detected")){
      output++;
      gtk_list_store_append (lint_store, &iter);
      gtk_list_store_set (lint_store, &iter, 0, output, -1);
    }
  } else if (main_window.current_editor->type==TAB_PERL){
      gint quote=0;
      gint a=0;
      gchar *cop=copy;
      while (*cop!='\0'){
      if(*cop=='"' && quote==0) quote++;
      else if(*cop=='"' && quote!=0) quote--;
      if (*cop=='\n' && quote==1) *(copy +a)=' ';
      cop++;
      a++;
//      g_print("char:%c, quote:%d,pos:%d\n",*cop,quote,a);
      }      
      while ((token = strtok(copy, "\n"))) {
        gtk_list_store_append (lint_store, &iter);
        gtk_list_store_set (lint_store, &iter, 0, token, -1);
        gchar number[15];  
        int i=15;
        line_number = strstr(token, "line ");
        if (line_number){
        line_number+=5;
        while (*line_number!=',' && *line_number!='.' && i!=0){
        number[15-i]=*line_number;
        line_number++;
        i--;
        }
        number[i]='\0';
        }
        gint num=atoi(number);
        if (num>0) {
          if (!first_error) {
            first_error = number;
          }
          indent = gtk_scintilla_get_line_indentation(GTK_SCINTILLA(main_window.current_editor->scintilla), num-1);
    
          line_start = gtk_scintilla_position_from_line(GTK_SCINTILLA(main_window.current_editor->scintilla), num-1);
          line_start += (indent/get_preferences_manager_indentation_size(main_window.prefmg));
    
          line_end = gtk_scintilla_get_line_end_position(GTK_SCINTILLA(main_window.current_editor->scintilla), num-1);
          gtk_scintilla_indicator_fill_range(GTK_SCINTILLA(main_window.current_editor->scintilla), line_start, line_end-line_start);
        }
        else {
          g_print("Line number is 0\n");
        }
      number[0]='a'; /*force new number */
      copy = NULL;
    }
  }
  if (first_error) {
    goto_line(first_error);
  }
  gtk_syntax_check_window_set_model(main_window.win, lint_store);
}


GString *save_as_temp_file(void)
{
  gchar *write_buffer = NULL;
  gsize text_length;
  gchar *rawfilename;
  GString *filename;
  int file_handle;

  file_handle = g_file_open_tmp("gphpeditXXXXXX",&rawfilename,NULL);
  if (file_handle != -1) {
    close(file_handle);
    filename = g_string_new(rawfilename);
    
    text_length = gtk_scintilla_get_length(GTK_SCINTILLA(main_window.current_editor->scintilla));
    write_buffer = g_malloc0(text_length+1); // Include terminating null

    if (write_buffer == NULL) {
      g_warning ("%s", _("Cannot allocate write buffer"));
      return NULL;
    }
    
    gtk_scintilla_get_text(GTK_SCINTILLA(main_window.current_editor->scintilla), text_length+1, write_buffer);
    GError *error=NULL;
    
    if (!g_file_set_contents (rawfilename, write_buffer,text_length+1,&error)){
      g_print(_("Error saving temp file: '%s'. GIO Error:%s"),rawfilename,error->message);
      g_error_free(error);
    }
    
    g_free (write_buffer);
    g_free(rawfilename);

    return filename;
  }
  
  return NULL;
}

void syntax_check_run(void)
{
  GString *command_line=NULL;
  gchar *output;
  gboolean using_temp;
  GString *filename;

  if (main_window.current_editor) {
    if (!filename_is_native(main_window.current_editor->filename->str)){
      return;
    }
    if (main_window.current_editor->saved==TRUE) {
      gchar *local_path=filename_get_path(main_window.current_editor->filename->str);
      filename = g_string_new(local_path);
      g_free(local_path);
      using_temp = FALSE;
    }
    else {
      filename = save_as_temp_file();
      using_temp = TRUE;
    }
    unquote(filename->str);
    if(main_window.current_editor->type==TAB_PHP){
    command_line = g_string_new(get_preferences_manager_php_binary_location(main_window.prefmg));
    command_line = g_string_append(command_line, " -q -l -d html_errors=Off -f '");
    command_line = g_string_append(command_line, filename->str);
    command_line = g_string_append(command_line, "'");
//    g_print("eject:%s\n", command_line->str);
    } else if (main_window.current_editor->type==TAB_PERL){
    command_line = g_string_new("perl -c ");
    command_line = g_string_append(command_line, "'");
    command_line = g_string_append(command_line, filename->str);
    command_line = g_string_append(command_line, "'");
//    g_print("eject:%s\n", command_line->str);
    } else {
      if (!run_syntax_plugin_by_ftype(get_plugin_manager(GTK_PLUGIN_MANAGER_MENU(main_window.menu->menuplugin)), main_window.current_editor, main_window.current_editor->type)){
//      g_print("syntax check not implement\n");
      }
      return;
    }
    gtk_widget_show(GTK_WIDGET(main_window.win));
    output = run_php_lint(command_line->str);
    g_string_free(command_line, TRUE);

    gtk_scintilla_indicator_clear_range(GTK_SCINTILLA(main_window.current_editor->scintilla), 0, gtk_scintilla_get_text_length(GTK_SCINTILLA(main_window.current_editor->scintilla)));

    if (output) {
      syntax_add_lines(output);
      g_free(output);
    }
    else {
        gtk_syntax_check_window_set_string(main_window.win, _("Error calling PHP CLI (is PHP command line binary installed? If so, check if it's in your path or set php_binary in Preferences)\n"));
    }
    
    if (using_temp) {
      g_unlink(filename->str);
    }
    g_string_free(filename, TRUE);
  }
  else {
   gtk_syntax_check_window_set_string(main_window.win, _("You don't have any files open to check\n"));
  }
}
