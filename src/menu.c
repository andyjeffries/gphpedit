/* This file is part of gPHPEdit, a GNOME2 PHP Editor.

   Copyright (C) 2003, 2004, 2005 Andy Jeffries <andy at gphpedit.org>
   Copyright (C) 2009 Anoop John <anoop dot john at zyxware.com>
   Copyright (C) 2009 José Rostagno (for vijona.com.ar) 

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
#include "plugin.h"

/*needed for menu hints*/
guint context_id;
guint message_id;

/*
 *classbrowser_status
 *return 0  if classbrowser is hidden
 *return 1 if classbrowser is show
*/
static int classbrowser_status(void){
	gint hidden;
	GConfClient *config;
        config=gconf_client_get_default ();
	hidden = gconf_client_get_int (config,"/gPHPEdit/main_window/classbrowser_hidden",NULL);
	return hidden;
}
/*
 * tog_classbrowser
 * Show/hide side panel
*/
static void tog_classbrowser(GtkCheckMenuItem *checkmenuitem, gpointer user_data){
    classbrowser_show_hide(NULL);
}
/*
 * tog_statusbar
 * Show/hide application statusbar
*/
static void tog_statusbar(GtkWidget *widget, gpointer user_data)
{
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
    gtk_widget_show(main_window.appbar);
  } else {
    gtk_widget_hide(main_window.appbar);
  }
}
/*
 * tog_maintoolbar
 * Show/hide application maintoolbar
*/

static void tog_maintoolbar(GtkWidget *widget, gpointer user_data)
{
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
    gtk_widget_show(main_window.toolbar_main->toolbar);
  } else {
    gtk_widget_hide(main_window.toolbar_main->toolbar);
  }
}

/*
 * tog_maintoolbar
 * Show/hide application findtoolbar
*/
static void tog_findtoolbar(GtkWidget *widget, gpointer user_data)
{
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
    gtk_widget_show(main_window.toolbar_find->toolbar);
  } else {
    gtk_widget_hide(main_window.toolbar_find->toolbar);
  }
}
/*
 * tog_fullscreen
 * Enable/disable fullscreen mode
*/
static void tog_fullscreen(GtkWidget *widget, gpointer user_data)
{
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
    gtk_window_fullscreen (GTK_WINDOW(main_window.window));
  } else {
    gtk_window_unfullscreen (GTK_WINDOW(main_window.window));
  }
}
/*
 * ShowPreview
 * Preview Current Document
*/
static void showpreview (GtkWidget *widget, gpointer user_data)
{
	tab_create_new(TAB_PREVIEW, main_window.current_editor->filename);
}
/*
 *size_change
 * Changes toolbar icon sizes
*/
static void size_change(GtkWidget *widget, gpointer user_data)
{
if (gtk_check_menu_item_get_active ((GtkCheckMenuItem *)main_window.menu->sizebig)){
gtk_toolbar_set_icon_size (GTK_TOOLBAR (main_window.toolbar_main->toolbar), GTK_ICON_SIZE_LARGE_TOOLBAR);
} else {
gtk_toolbar_set_icon_size (GTK_TOOLBAR (main_window.toolbar_main->toolbar), GTK_ICON_SIZE_SMALL_TOOLBAR);
}
}
/*
 *show_hint
 * Show a new mennu hint in the statusbar, and set widget state to prelight
*/
gboolean show_hint(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data){
    gtk_widget_set_state (widget, GTK_STATE_PRELIGHT);
    context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR(main_window.appbar), (const gchar *) user_data);
    message_id= gtk_statusbar_push (GTK_STATUSBAR(main_window.appbar),context_id, (const gchar *) user_data);
    return false;
}
/*
 *delete_hint
 * deletes the menu hint from the statusbar, and set widget state to normal
*/

gboolean delete_hint(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data){
    gtk_widget_set_state (widget, GTK_STATE_NORMAL);
    gtk_statusbar_remove (GTK_STATUSBAR(main_window.appbar), context_id, message_id);
    return false;
}
/*
 * install_menu_hint
 * connect menu hint signals
*/
void install_menu_hint(GtkWidget *widget, gchar *message){
  g_signal_connect(G_OBJECT(widget), "enter-notify-event", G_CALLBACK(show_hint), message);
  g_signal_connect(G_OBJECT(widget), "leave-notify-event", G_CALLBACK(delete_hint), NULL);
}
#ifdef PACKAGE_BUGREPORT
/*
 * bugreport
 * launch default system browser with bug report page
*/
static void bugreport(void){
    GdkScreen *screen;
    screen = gtk_widget_get_screen (GTK_WIDGET (main_window.window));
    gtk_show_uri (screen, PACKAGE_BUGREPORT, GDK_CURRENT_TIME, NULL);
}
#endif
#ifdef TRANSLATE_URL
/*
 * translate
 * launch default system browser with tranlation page
*/
static void translate(void){
    GdkScreen *screen;
    screen = gtk_widget_get_screen (GTK_WIDGET (main_window.window));
    gtk_show_uri (screen, TRANSLATE_URL, GDK_CURRENT_TIME, NULL);
}
#endif
/*
 * create_stock_menu_item
 * creates a new stock menu item, append it to menu, add menu hint, optionally add accelerator and return the new menuitem
*/
static void create_stock_menu_item(GtkWidget **menuitem,GtkWidget *menu,const gchar *stock_id, gchar *menu_hint, guint accel_key, GdkModifierType accel_mods){
  *menuitem = gtk_image_menu_item_new_from_stock(stock_id, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), *menuitem);
  install_menu_hint(*menuitem,menu_hint);
  if (!(accel_key==0 && accel_mods==0)){
  gtk_widget_add_accelerator(*menuitem, "activate", main_window.menu->accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
  }
}
/*
 * create_mnemonic_menu_item
 * creates a new mnemonic menu item, append it to menu, add menu hint, optionally add accelerator and return the new menuitem
*/
static void create_mnemonic_menu_item(GtkWidget **menuitem,GtkWidget *menu,gchar *mnemonic, gchar *menu_hint, guint accel_key, GdkModifierType accel_mods){
*menuitem = gtk_menu_item_new_with_mnemonic(mnemonic);
install_menu_hint(*menuitem,menu_hint);
if (!(accel_key==0 && accel_mods==0))
	gtk_widget_add_accelerator(*menuitem, "activate", main_window.menu->accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
gtk_menu_shell_append(GTK_MENU_SHELL(menu), *menuitem);
}
/*
 * create_mnemonic_menu_item
 * creates a check menu item, append it to menu, add menu hint, optionally add accelerator, set default state and return the new menuitem
*/
static void create_check_menu_item(GtkWidget **menuitem,GtkWidget *menu,gchar *mnemonic, gchar *menu_hint, guint accel_key, GdkModifierType accel_mods,gboolean active){
  *menuitem = gtk_check_menu_item_new_with_label(mnemonic);
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(*menuitem), active);
  install_menu_hint(*menuitem, menu_hint);
  if (!(accel_key==0 && accel_mods==0))
 	 gtk_widget_add_accelerator(*menuitem, "activate", main_window.menu->accel_group, accel_key, accel_mods, GTK_ACCEL_VISIBLE);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu),*menuitem);
}
/*
* create_separator_item
* creates a new separator item, add it to menu and return the separator item
*/
static void create_separator_item(GtkWidget **separator,GtkWidget *menu){
*separator = gtk_separator_menu_item_new();
gtk_menu_shell_append(GTK_MENU_SHELL(menu), *separator);
}
/*
* create_menu
* create a new menu and insert it in menubar
*/
static void create_menu(GtkWidget **menu, GtkWidget **menuitem, const gchar *caption, GtkWidget *menubar){
  *menu = gtk_menu_new();
  *menuitem = gtk_menu_item_new_with_mnemonic(caption);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(*menuitem),   *menu);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), *menuitem);
}
/*
* create_help_menu
* create help menu widgets and fill help menu
*/
static void fill_help_menu(void){
  /* help menu */
  create_mnemonic_menu_item(&main_window.menu->phphelp,main_window.menu->menuhelp,_("_PHP Help"), _("Look for help on the currently selected function"), GDK_F1, 0);
  g_signal_connect(G_OBJECT(main_window.menu->phphelp), "activate", G_CALLBACK(context_help), NULL);
  #ifdef PACKAGE_BUGREPORT
  create_mnemonic_menu_item(&main_window.menu->bugreport,main_window.menu->menuhelp,_("_Report a bug in gPHPEdit"), _("Go to bug report page to report a bug"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->bugreport), "activate", G_CALLBACK(bugreport), NULL);
  #endif
  #ifdef TRANSLATE_URL
  create_mnemonic_menu_item(&main_window.menu->translate,main_window.menu->menuhelp,_("_Translate this application"), _("Start translating this application"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->translate), "activate", G_CALLBACK(translate), NULL);
  #endif
  create_stock_menu_item(&main_window.menu->abouthelp,main_window.menu->menuhelp,GTK_STOCK_ABOUT, _("Shows info about gPHPEdit"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->abouthelp), "activate", G_CALLBACK(on_about1_activate), NULL);
}

/*
* create_edit_menu
* create edit menu widgets and fill edit menu
*/
static void fill_menu_edit(void){

  create_stock_menu_item(&main_window.menu->undo,main_window.menu->menuedit,GTK_STOCK_UNDO, _("Undo last action"), GDK_z, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->undo), "activate", G_CALLBACK(on_undo1_activate), NULL);
  create_stock_menu_item(&main_window.menu->redo,main_window.menu->menuedit,GTK_STOCK_REDO, _("Redo last action"), GDK_z, GDK_SHIFT_MASK | GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->redo), "activate", G_CALLBACK(on_redo1_activate), NULL);

  /* separator */
  create_separator_item(&main_window.menu->sep, main_window.menu->menuedit);

  create_stock_menu_item(&main_window.menu->cut,main_window.menu->menuedit,GTK_STOCK_CUT, _("Cut Selected Text"), GDK_x, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->cut), "activate", G_CALLBACK(on_cut1_activate), NULL);
create_stock_menu_item(&main_window.menu->copy,main_window.menu->menuedit,GTK_STOCK_COPY, _("Copy Selected Text"), GDK_c, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->copy), "activate", G_CALLBACK(on_copy1_activate), NULL);
create_stock_menu_item(&main_window.menu->paste,main_window.menu->menuedit,GTK_STOCK_PASTE, _("Paste Text from clipboard"), GDK_v, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->paste), "activate", G_CALLBACK(on_paste1_activate), NULL);
create_stock_menu_item(&main_window.menu->selectall,main_window.menu->menuedit,GTK_STOCK_SELECT_ALL, _("Select all Text in current file"), GDK_a, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->selectall), "activate", G_CALLBACK(on_selectall1_activate), NULL);

  /* separator */
  create_separator_item(&main_window.menu->sep3,main_window.menu->menuedit);

create_stock_menu_item(&main_window.menu->find,main_window.menu->menuedit,GTK_STOCK_FIND, _("Find text in current file"), GDK_f, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->find), "activate", G_CALLBACK(on_find1_activate), NULL);
create_stock_menu_item(&main_window.menu->replace,main_window.menu->menuedit,GTK_STOCK_FIND_AND_REPLACE, _("Find and replace text in current file"), GDK_h, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->replace), "activate", G_CALLBACK(on_replace1_activate), NULL);
  
  /* separator */
  create_separator_item(&main_window.menu->sep4,main_window.menu->menuedit);

create_stock_menu_item(&main_window.menu->indent,main_window.menu->menuedit,GTK_STOCK_INDENT, _("Indent the currently selected block"), GDK_i, GDK_SHIFT_MASK | GDK_MOD1_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->indent), "activate", G_CALLBACK(block_indent), NULL);
create_stock_menu_item(&main_window.menu->unindent,main_window.menu->menuedit,GTK_STOCK_UNINDENT, _("Unindent the currently selected block"), GDK_i,  GDK_SHIFT_MASK | GDK_CONTROL_MASK |GDK_MOD1_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->unindent), "activate", G_CALLBACK(block_unindent), NULL);

  /* separator */
  create_separator_item(&main_window.menu->sept,main_window.menu->menuedit);

  create_mnemonic_menu_item(&main_window.menu->upper ,main_window.menu->menuedit,_("_ToUpper"), _("Convert the current selection text to upper case"), GDK_u, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->upper), "activate", G_CALLBACK(selectiontoupper), NULL);
  create_mnemonic_menu_item(&main_window.menu->lower,main_window.menu->menuedit,_("_ToLower"), _("Convert the current selection text to lower case"), GDK_l, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->lower), "activate", G_CALLBACK(selectiontolower), NULL);

}

/*
* create_file_menu
* create file menu widgets and fill file menu
*/
static void fill_menu_file(void){
  int i;
  create_stock_menu_item(&main_window.menu->newi,main_window.menu->menunew,GTK_STOCK_NEW, _("Creates a new file"), GDK_n, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->newi), "activate", G_CALLBACK(on_new1_activate), NULL);
  create_stock_menu_item(&main_window.menu->open,main_window.menu->menunew,GTK_STOCK_OPEN, _("Open a file"), GDK_o, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->open), "activate", G_CALLBACK(on_open1_activate), NULL);
  create_mnemonic_menu_item(&main_window.menu->opensel ,main_window.menu->menunew,_("_Open selected file"), _("Open a file with the name currently selected in the editor"), GDK_o, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->opensel), "activate", G_CALLBACK(on_openselected1_activate), NULL);
  
  main_window.menu->reciente = gtk_menu_item_new_with_mnemonic(_("_Recent Files"));
  gtk_container_add (GTK_CONTAINER (main_window.menu->menunew), main_window.menu->reciente);
  main_window.menu->menureciente = gtk_menu_new();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (main_window.menu->reciente), main_window.menu->menureciente);
  /* create empty widgets */
  for (i=0;i<NUM_REOPEN_MAX;i++){
  main_window.menu->recent[i]= gtk_menu_item_new_with_mnemonic(_("_Recent"));
  g_signal_connect(G_OBJECT(main_window.menu->recent[i]), "activate", G_CALLBACK(reopen_recent), (gpointer)i);
  gtk_menu_shell_append(GTK_MENU_SHELL(main_window.menu->menureciente), main_window.menu->recent[i]);
  }
  create_mnemonic_menu_item(&main_window.menu->reload ,main_window.menu->menunew,_("_Reload current file"), _("Reload the file currently selected in the editor"), GDK_r, GDK_SHIFT_MASK | GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->reload), "activate", G_CALLBACK(on_reload1_activate), NULL); 
  /* separator */
  create_separator_item(&main_window.menu->sep,main_window.menu->menunew);

  create_stock_menu_item(&main_window.menu->save,main_window.menu->menunew,GTK_STOCK_SAVE, _("Save the file currently selected in the editor"), GDK_s, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->save), "activate", G_CALLBACK(on_save1_activate), NULL);
  create_stock_menu_item(&main_window.menu->saveas,main_window.menu->menunew,GTK_STOCK_SAVE_AS, _("Save the file currently selected in the editor"), GDK_s, GDK_SHIFT_MASK | GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->saveas), "activate", G_CALLBACK(on_save_as1_activate), NULL);
  create_mnemonic_menu_item(&main_window.menu->saveall ,main_window.menu->menunew,_("Save A_ll"), _("Save all open unsaved files"), GDK_a, GDK_SHIFT_MASK | GDK_MOD1_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->saveall), "activate", G_CALLBACK(on_saveall1_activate), NULL);
  create_mnemonic_menu_item(&main_window.menu->rename ,main_window.menu->menunew,_("_Rename"), _("Rename the current file 'on-the-fly'"), GDK_r, GDK_SHIFT_MASK | GDK_MOD1_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->rename), "activate", G_CALLBACK(on_rename1_activate), NULL);
  create_stock_menu_item(&main_window.menu->close,main_window.menu->menunew,GTK_STOCK_CLOSE, _("Close the current file"), GDK_w, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->close), "activate", G_CALLBACK(on_close1_activate), NULL);

  /* separator */
  create_separator_item(&main_window.menu->sep1,main_window.menu->menunew);

  create_stock_menu_item(&main_window.menu->quit,main_window.menu->menunew,GTK_STOCK_QUIT, _("Quit the application"), GDK_q, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->quit), "activate", G_CALLBACK(on_quit1_activate), NULL);
}
/*
* create_view_menu
* create view menu widgets and fill view menu
*/
static void fill_menu_view(void){
  create_check_menu_item(&main_window.menu->viewstatusbar,main_window.menu->menuview,_("Statusbar"), _("Show/Hide Application Statusbar"), 0, 0,TRUE);
  g_signal_connect(G_OBJECT(main_window.menu->viewstatusbar), "activate", G_CALLBACK(tog_statusbar),NULL);
  create_check_menu_item(&main_window.menu->viewmaintoolbar,main_window.menu->menuview,_("Main Toolbar"), _("Show/Hide Application Main Toolbar"), 0, 0,TRUE);
  g_signal_connect(G_OBJECT(main_window.menu->viewmaintoolbar), "activate", G_CALLBACK(tog_maintoolbar),NULL);
  create_check_menu_item(&main_window.menu->viewfindtoolbar,main_window.menu->menuview,_("Find Toolbar"), _("Show/Hide Application Find Toolbar"), 0, 0,TRUE);
  g_signal_connect(G_OBJECT(main_window.menu->viewfindtoolbar), "activate", G_CALLBACK(tog_findtoolbar),NULL);
  /* separator */
  create_separator_item(&main_window.menu->sep6,main_window.menu->menuview);


  if (classbrowser_status()==0){
	create_check_menu_item(&main_window.menu->tog_class,main_window.menu->menuview,_("Show Side Panel"), _("Show/Hide Application Side Panel"), GDK_F8, 0,TRUE);
  }else {
	create_check_menu_item(&main_window.menu->tog_class,main_window.menu->menuview,_("Show Side Panel"), _("Show/Hide Application Side Panel"), GDK_F8, 0,FALSE);
  }
  g_signal_connect(G_OBJECT(main_window.menu->tog_class), "activate", G_CALLBACK(tog_classbrowser),NULL);

  create_check_menu_item(&main_window.menu->viewfullscreen,main_window.menu->menuview,_("Fullscreen"), _("Enable/Disable Fullscreen mode"), GDK_F11, 0,FALSE);
  g_signal_connect(G_OBJECT(main_window.menu->viewfullscreen), "activate", G_CALLBACK(tog_fullscreen),NULL);
  /* separator */
  create_separator_item(&main_window.menu->sep7,main_window.menu->menuview);

  create_stock_menu_item(&main_window.menu->zoomin,main_window.menu->menuview,GTK_STOCK_ZOOM_IN, _("Increases zoom in 10%"), GDK_plus, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->zoomin), "activate", G_CALLBACK(zoom_in),NULL);
  create_stock_menu_item(&main_window.menu->zoomout,main_window.menu->menuview,GTK_STOCK_ZOOM_OUT, _("Decreases zoom in 10%"), GDK_minus, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->zoomout), "activate", G_CALLBACK(zoom_out),NULL);
  create_stock_menu_item(&main_window.menu->zoom100,main_window.menu->menuview,GTK_STOCK_ZOOM_100, _("Restores normal zoom level"), GDK_0, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->zoom100), "activate", G_CALLBACK(zoom_100),NULL);
  /* separator */
  create_separator_item(&main_window.menu->sep8,main_window.menu->menuview);

 /* toolbar icon size menu*/
  main_window.menu->Ticonsize = gtk_menu_item_new_with_mnemonic(_("_Toolbar Icon Size"));
  gtk_container_add (GTK_CONTAINER (main_window.menu->menuview), main_window.menu->Ticonsize);
  main_window.menu->Ticonsizemenu = gtk_menu_new();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (main_window.menu->Ticonsize), main_window.menu->Ticonsizemenu);

  /*set new radio button group*/
  main_window.menu->iconsizegroup=NULL;

  main_window.menu->sizebig=gtk_radio_menu_item_new_with_mnemonic (main_window.menu->iconsizegroup,_("Big Icons"));
  main_window.menu->iconsizegroup = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (main_window.menu->sizebig));
  gtk_menu_shell_append(GTK_MENU_SHELL(main_window.menu->Ticonsizemenu), main_window.menu->sizebig);
  g_signal_connect(G_OBJECT(main_window.menu->sizebig), "activate", G_CALLBACK(size_change), NULL);
  main_window.menu->sizesmall=gtk_radio_menu_item_new_with_mnemonic (main_window.menu->iconsizegroup,_("Small Icons"));
  main_window.menu->iconsizegroup = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (main_window.menu->sizesmall));
  gtk_menu_shell_append(GTK_MENU_SHELL(main_window.menu->Ticonsizemenu), main_window.menu->sizesmall);
  /* separator */
  create_separator_item(&main_window.menu->seppr,main_window.menu->menuview);

  create_mnemonic_menu_item(&main_window.menu->preview ,main_window.menu->menuview,_("_Show Preview"), _("Preview the Document"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->preview), "activate", G_CALLBACK(showpreview), NULL);
  /* separator */
  create_separator_item(&main_window.menu->sepd,main_window.menu->menuview);

  create_stock_menu_item(&main_window.menu->preferences,main_window.menu->menuview,GTK_STOCK_PREFERENCES, _("Application Config"), GDK_F5, 0);
  g_signal_connect(G_OBJECT(main_window.menu->preferences), "activate", G_CALLBACK(on_preferences1_activate), NULL);
}

/*
* create_code_menu
* create code menu widgets and fill code menu
*/
static void fill_menu_code (void){
  create_mnemonic_menu_item(&main_window.menu->syntax,main_window.menu->menucode,_("_Syntax check"), _("Check the syntax using the PHP command line binary"), GDK_F9, 0);
  g_signal_connect(G_OBJECT(main_window.menu->syntax), "activate", G_CALLBACK(syntax_check), NULL);
  create_mnemonic_menu_item(&main_window.menu->clearsyntax,main_window.menu->menucode,_("_Clear Syntax check"), _("Remove the syntax check window"), GDK_F9, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->clearsyntax), "activate", G_CALLBACK(syntax_check_clear), NULL);
  /* separator */
  create_separator_item(&main_window.menu->sep5,main_window.menu->menucode);

  create_mnemonic_menu_item(&main_window.menu->record,main_window.menu->menucode,_("_Record keyboard macro start/stop"), _("Record keyboard actions"), GDK_k, GDK_MOD1_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->record), "activate", G_CALLBACK(keyboard_macro_startstop), NULL);
  create_mnemonic_menu_item(&main_window.menu->playback,main_window.menu->menucode,_("_Playback keyboard macro"), _("Playback the stored keyboard macro"), GDK_k, GDK_CONTROL_MASK);
  g_signal_connect(G_OBJECT(main_window.menu->playback), "activate", G_CALLBACK(keyboard_macro_playback), NULL);

  main_window.menu->force = gtk_menu_item_new_with_mnemonic(_("_Force"));
  gtk_container_add (GTK_CONTAINER (main_window.menu->menucode), main_window.menu->force);
  main_window.menu->menuforce = gtk_menu_new();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (main_window.menu->force), main_window.menu->menuforce);

  create_mnemonic_menu_item(&main_window.menu->forcephp,main_window.menu->menuforce,_("_PHP/HTML/XML"), _("Force syntax highlighting to PHP/HTML/XML mode"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->forcephp), "activate", G_CALLBACK(force_php), NULL);
  create_mnemonic_menu_item(&main_window.menu->forcecss,main_window.menu->menuforce,_("_CSS"), _("Force syntax highlighting to CSS mode"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->forcecss), "activate", G_CALLBACK(force_css), NULL);
  create_mnemonic_menu_item(&main_window.menu->forcecxx,main_window.menu->menuforce,_("C/C_++"), _("Force syntax highlighting to C/C++ mode"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->forcecxx), "activate", G_CALLBACK(force_cxx), NULL);
  create_mnemonic_menu_item(&main_window.menu->forcesql,main_window.menu->menuforce,_("_SQL"), _("Force syntax highlighting to SQL mode"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->forcesql), "activate", G_CALLBACK(force_sql), NULL);
  create_mnemonic_menu_item(&main_window.menu->forceperl,main_window.menu->menuforce,_("_Perl"), _("Force syntax highlighting to Perl mode"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->forceperl), "activate", G_CALLBACK(force_perl), NULL);
  create_mnemonic_menu_item(&main_window.menu->forcepython,main_window.menu->menuforce,_("P_ython"), _("Force syntax highlighting to Python mode"), 0, 0);
  g_signal_connect(G_OBJECT(main_window.menu->forcepython), "activate", G_CALLBACK(force_python), NULL);
}
/*
 * main_window_create_menu
 * create application menu, menu should be free when no longer needed with g_slice_free
*/
void main_window_create_menu(void){
  main_window.menu=g_slice_new(Mainmenu);
  main_window.menu->accel_group=NULL;
  /*create menu bar*/
  main_window.menu->menubar = gtk_menu_bar_new();
  /* create menui accel gruop */
  main_window.menu->accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(main_window.window), main_window.menu->accel_group);
  /*create menus*/
  /*Menu file*/
  create_menu(&main_window.menu->menunew, &main_window.menu->file, _("_File"),main_window.menu->menubar);
  fill_menu_file();
  /*Menu edit*/
  create_menu(&main_window.menu->menuedit, &main_window.menu->edit, _("_Edit"), main_window.menu->menubar);
  fill_menu_edit();
  /* view menu */
  create_menu(&main_window.menu->menuview, &main_window.menu->view, _("_View"),main_window.menu->menubar);
  fill_menu_view();
  /*menu code*/
  create_menu(&main_window.menu->menucode, &main_window.menu->code, _("_Code"),main_window.menu->menubar);
  fill_menu_code();
  /*plugin menu*/
  create_menu(&main_window.menu->menuplugin, &main_window.menu->plugin, _("_Plugin"),main_window.menu->menubar);
  int i;
  for (i=0;i<NUM_PLUGINS_MAX;i++){
  main_window.menu->plugins[i]= gtk_menu_item_new_with_mnemonic(_("_Plugin"));
  g_signal_connect(G_OBJECT(main_window.menu->plugins[i]), "activate", G_CALLBACK(run_plugin), (gpointer)i);
  gtk_menu_shell_append(GTK_MENU_SHELL(main_window.menu->menuplugin), main_window.menu->plugins[i]);
  if (i<10)
 gtk_widget_add_accelerator(main_window.menu->plugins[i], "activate", main_window.menu->accel_group, parse_shortcut(i), GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
  }
  /* help menu */
  create_menu(&main_window.menu->menuhelp, &main_window.menu->help, _("_Help"),main_window.menu->menubar);
  fill_help_menu();
  /* add menu bar to main window */
  gtk_box_pack_start (GTK_BOX (main_window.prinbox), main_window.menu->menubar, FALSE, FALSE, 0);
  gtk_widget_show_all (main_window.menu->menubar);
}
