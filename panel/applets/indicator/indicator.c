/*  Copyright (c) 2009 Mark Trompell <mark@foresightlinux.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>

#include <gtk/gtk.h>
#include <libindicator/indicator-object.h>

#include "indicator.h"

#include "indicator-config.h"

/* default settings */
#define DEFAULT_SETTING1 NULL
#define DEFAULT_SETTING2 1
#define DEFAULT_SETTING3 FALSE

typedef enum {
    PANEL_APPLET_ORIENT_LEFT,
    PANEL_APPLET_ORIENT_RIGHT,
    PANEL_APPLET_ORIENT_TOP,
    PANEL_APPLET_ORIENT_BOTTOM
} PanelAppletOrient;

// <indicator-applet-copy>
static gchar * indicator_order[] = {
	"libappmenu.so",
	"libapplication.so",
	"libsoundmenu.so",
	"libnetworkmenu.so",
	"libmessaging.so",
	"libdatetime.so",
	"libme.so",
	"libsession.so",
	NULL
};

static GtkPackDirection packdirection;
static PanelAppletOrient orient;

#define  MENU_DATA_INDICATOR_OBJECT  "indicator-object"
#define  MENU_DATA_INDICATOR_ENTRY   "indicator-entry"

#define  IO_DATA_ORDER_NUMBER        "indicator-order-number"

static gint
name2order (const gchar * name) {
	int i;

	for (i = 0; indicator_order[i] != NULL; i++) {
		if (g_strcmp0(name, indicator_order[i]) == 0) {
			return i;
		}
	}

	return -1;
}

typedef struct _incoming_position_t incoming_position_t;
struct _incoming_position_t {
	gint objposition;
	gint entryposition;
	gint menupos;
	gboolean found;
};

/* This function helps by determining where in the menu list
   this new entry should be placed.  It compares the objects
   that they're on, and then the individual entries.  Each
   is progressively more expensive. */
static void
place_in_menu (GtkWidget * widget, gpointer user_data)
{
	incoming_position_t * position = (incoming_position_t *)user_data;
	if (position->found) {
		/* We've already been placed, just finish the foreach */
		return;
	}

	IndicatorObject * io = INDICATOR_OBJECT(g_object_get_data(G_OBJECT(widget), MENU_DATA_INDICATOR_OBJECT));
	g_assert(io != NULL);

	gint objposition = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(io), IO_DATA_ORDER_NUMBER));
	/* We've already passed it, well, then this is where
	   we should be be.  Stop! */
	if (objposition > position->objposition) {
		position->found = TRUE;
		return;
	}

	/* The objects don't match yet, keep looking */
	if (objposition < position->objposition) {
		position->menupos++;
		return;
	}

	/* The objects are the same, let's start looking at entries. */
	IndicatorObjectEntry * entry = (IndicatorObjectEntry *)g_object_get_data(G_OBJECT(widget), MENU_DATA_INDICATOR_ENTRY);
	gint entryposition = indicator_object_get_location(io, entry);

	if (entryposition > position->entryposition) {
		position->found = TRUE;
		return;
	}

	if (entryposition < position->entryposition) {
		position->menupos++;
		return;
	}

	/* We've got the same object and the same entry.  Well,
	   let's just put it right here then. */
	position->found = TRUE;
	return;
}

static void
something_shown (GtkWidget * widget, gpointer user_data)
{
	GtkWidget * menuitem = GTK_WIDGET(user_data);
	gtk_widget_show(menuitem);
}

static void
something_hidden (GtkWidget * widget, gpointer user_data)
{
	GtkWidget * menuitem = GTK_WIDGET(user_data);
	gtk_widget_hide(menuitem);
}

static void
sensitive_cb (GObject * obj, GParamSpec * pspec, gpointer user_data)
{
	g_return_if_fail(GTK_IS_WIDGET(obj));
	g_return_if_fail(GTK_IS_WIDGET(user_data));

	gtk_widget_set_sensitive(GTK_WIDGET(user_data), gtk_widget_get_sensitive(GTK_WIDGET(obj)));
	return;
}

static gboolean
entry_scrolled (GtkWidget *menuitem, GdkEventScroll *event, gpointer data)
{
	IndicatorObject *io = g_object_get_data (G_OBJECT (menuitem), MENU_DATA_INDICATOR_OBJECT);
	IndicatorObjectEntry *entry = g_object_get_data (G_OBJECT (menuitem), MENU_DATA_INDICATOR_ENTRY);

	g_return_val_if_fail(INDICATOR_IS_OBJECT(io), FALSE);

	g_signal_emit_by_name (io, "scroll", 1, event->direction);
	g_signal_emit_by_name (io, "scroll-entry", entry, 1, event->direction);

	return FALSE;
}

static void
entry_added (IndicatorObject * io, IndicatorObjectEntry * entry, GtkWidget * menubar)
{
	g_debug("Signal: Entry Added");
	gboolean something_visible = FALSE;
	gboolean something_sensitive = FALSE;

	GtkWidget * menuitem = gtk_menu_item_new();
	GtkWidget * box = (packdirection == GTK_PACK_DIRECTION_LTR) ?
			gtk_hbox_new(FALSE, 3) : gtk_vbox_new(FALSE, 3);

	g_object_set_data (G_OBJECT (menuitem), "indicator", io);
	g_object_set_data (G_OBJECT (menuitem), "box", box);

	if (entry->image != NULL) {
		gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(entry->image), FALSE, FALSE, 0);
		if (gtk_widget_get_visible(GTK_WIDGET(entry->image))) {
			something_visible = TRUE;
		}

		if (gtk_widget_get_sensitive(GTK_WIDGET(entry->image))) {
			something_sensitive = TRUE;
		}

		g_signal_connect(G_OBJECT(entry->image), "show", G_CALLBACK(something_shown), menuitem);
		g_signal_connect(G_OBJECT(entry->image), "hide", G_CALLBACK(something_hidden), menuitem);

		g_signal_connect(G_OBJECT(entry->image), "notify::sensitive", G_CALLBACK(sensitive_cb), menuitem);
	}
	if (entry->label != NULL) {
		switch(packdirection) {
			case GTK_PACK_DIRECTION_LTR:
				gtk_label_set_angle(GTK_LABEL(entry->label), 0.0);
				break;
			case GTK_PACK_DIRECTION_TTB:
				gtk_label_set_angle(GTK_LABEL(entry->label),
						(orient == PANEL_APPLET_ORIENT_LEFT) ? 
						270.0 : 90.0);
				break;
			default:
				break;
		}		
		gtk_box_pack_start(GTK_BOX(box), GTK_WIDGET(entry->label), FALSE, FALSE, 0);

		if (gtk_widget_get_visible(GTK_WIDGET(entry->label))) {
			something_visible = TRUE;
		}

		if (gtk_widget_get_sensitive(GTK_WIDGET(entry->label))) {
			something_sensitive = TRUE;
		}

		g_signal_connect(G_OBJECT(entry->label), "show", G_CALLBACK(something_shown), menuitem);
		g_signal_connect(G_OBJECT(entry->label), "hide", G_CALLBACK(something_hidden), menuitem);

		g_signal_connect(G_OBJECT(entry->label), "notify::sensitive", G_CALLBACK(sensitive_cb), menuitem);
	}
	gtk_container_add(GTK_CONTAINER(menuitem), box);
	gtk_widget_show(box);

	if (entry->menu != NULL) {
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), GTK_WIDGET(entry->menu));
	}

	incoming_position_t position;
	position.objposition = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(io), IO_DATA_ORDER_NUMBER));
	position.entryposition = indicator_object_get_location(io, entry);
	position.menupos = 0;
	position.found = FALSE;

	gtk_container_foreach(GTK_CONTAINER(menubar), place_in_menu, &position);

	gtk_menu_shell_insert(GTK_MENU_SHELL(menubar), menuitem, position.menupos);

	if (something_visible) {
		gtk_widget_show(menuitem);
	}
	gtk_widget_set_sensitive(menuitem, something_sensitive);

	g_object_set_data(G_OBJECT(menuitem), MENU_DATA_INDICATOR_ENTRY,  entry);
	g_object_set_data(G_OBJECT(menuitem), MENU_DATA_INDICATOR_OBJECT, io);
	g_signal_connect(G_OBJECT (menuitem), "scroll-event", G_CALLBACK(entry_scrolled), NULL);

	return;
}

static void
entry_removed_cb (GtkWidget * widget, gpointer userdata)
{
	gpointer data = g_object_get_data(G_OBJECT(widget), MENU_DATA_INDICATOR_ENTRY);

	if (data != userdata) {
		return;
	}

	IndicatorObjectEntry * entry = (IndicatorObjectEntry *)data;
	if (entry->label != NULL) {
		g_signal_handlers_disconnect_by_func(G_OBJECT(entry->label), G_CALLBACK(something_shown), widget);
		g_signal_handlers_disconnect_by_func(G_OBJECT(entry->label), G_CALLBACK(something_hidden), widget);
		g_signal_handlers_disconnect_by_func(G_OBJECT(entry->label), G_CALLBACK(sensitive_cb), widget);
	}
	if (entry->image != NULL) {
		g_signal_handlers_disconnect_by_func(G_OBJECT(entry->image), G_CALLBACK(something_shown), widget);
		g_signal_handlers_disconnect_by_func(G_OBJECT(entry->image), G_CALLBACK(something_hidden), widget);
		g_signal_handlers_disconnect_by_func(G_OBJECT(entry->image), G_CALLBACK(sensitive_cb), widget);
	}

	gtk_widget_destroy(widget);
	return;
}

static void
entry_removed (IndicatorObject * io G_GNUC_UNUSED, IndicatorObjectEntry * entry,
               gpointer user_data)
{
	g_debug("Signal: Entry Removed");

	gtk_container_foreach(GTK_CONTAINER(user_data), entry_removed_cb, entry);

	return;
}

static void
entry_moved_find_cb (GtkWidget * widget, gpointer userdata)
{
	gpointer * array = (gpointer *)userdata;
	if (array[1] != NULL) {
		return;
	}

	gpointer data = g_object_get_data(G_OBJECT(widget), MENU_DATA_INDICATOR_ENTRY);

	if (data != array[0]) {
		return;
	}

	array[1] = widget;
	return;
}

/* Gets called when an entry for an object was moved. */
static void
entry_moved (IndicatorObject * io, IndicatorObjectEntry * entry,
             gint old G_GNUC_UNUSED, gint new G_GNUC_UNUSED, gpointer user_data)
{
	GtkWidget * menubar = GTK_WIDGET(user_data);

	gpointer array[2];
	array[0] = entry;
	array[1] = NULL;

	gtk_container_foreach(GTK_CONTAINER(menubar), entry_moved_find_cb, array);
	if (array[1] == NULL) {
		g_warning("Moving an entry that isn't in our menus.");
		return;
	}

	GtkWidget * mi = GTK_WIDGET(array[1]);
	g_object_ref(G_OBJECT(mi));
	gtk_container_remove(GTK_CONTAINER(menubar), mi);

	incoming_position_t position;
	position.objposition = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(io), IO_DATA_ORDER_NUMBER));
	position.entryposition = indicator_object_get_location(io, entry);
	position.menupos = 0;
	position.found = FALSE;

	gtk_container_foreach(GTK_CONTAINER(menubar), place_in_menu, &position);

	gtk_menu_shell_insert(GTK_MENU_SHELL(menubar), mi, position.menupos);

	g_object_unref(G_OBJECT(mi));

	return;
}

static gboolean
load_module (const gchar * name, GtkWidget * menubar)
{
	g_debug("Looking at Module: %s", name);
	g_return_val_if_fail(name != NULL, FALSE);

	if (!g_str_has_suffix(name, G_MODULE_SUFFIX)) {
		return FALSE;
	}

	g_debug("Loading Module: %s", name);

	/* Build the object for the module */
	gchar * fullpath = g_build_filename(INDICATOR_DIR, name, NULL);
	IndicatorObject * io = indicator_object_new_from_file(fullpath);
	g_free(fullpath);
	g_return_val_if_fail(io != NULL, FALSE);

	/* Attach the 'name' to the object */
	g_object_set_data(G_OBJECT(io), IO_DATA_ORDER_NUMBER, GINT_TO_POINTER(name2order(name)));

	/* Connect to its signals */
	g_signal_connect(G_OBJECT(io), INDICATOR_OBJECT_SIGNAL_ENTRY_ADDED,   G_CALLBACK(entry_added),    menubar);
	g_signal_connect(G_OBJECT(io), INDICATOR_OBJECT_SIGNAL_ENTRY_REMOVED, G_CALLBACK(entry_removed),  menubar);
	g_signal_connect(G_OBJECT(io), INDICATOR_OBJECT_SIGNAL_ENTRY_MOVED,   G_CALLBACK(entry_moved),    menubar);

	/* Work on the entries */
	GList * entries = indicator_object_get_entries(io);
	GList * entry = NULL;

	for (entry = entries; entry != NULL; entry = g_list_next(entry)) {
		IndicatorObjectEntry * entrydata = (IndicatorObjectEntry *)entry->data;
		entry_added(io, entrydata, menubar);
	}

	g_list_free(entries);

	return TRUE;
}
// </indicator-applet-copy>


/* prototypes */
static gboolean
on_menu_press (GtkWidget *widget, GdkEventButton *event, IndicatorPlugin *indicator);

IndicatorPlugin *
indicator_new ()
{
  IndicatorPlugin   *indicator;
  GtkOrientation  orientation;
  gint indicators_loaded = 0;

  /* Hack! prevent the appmenu indicator from swallowing our own menubar */
  setenv("APPMENU_DISPLAY_BOTH", "1");

  /* allocate memory for the plugin structure */
  indicator = g_new (IndicatorPlugin, 1);

  indicator->container = gtk_plug_new(0);
  
  /* Init some theme/icon stuff */
  g_object_set (gtk_settings_get_default(), "gtk-theme-name", "Ambiance", NULL);
  g_object_set (gtk_settings_get_default(), "gtk-icon-theme-name", "ubuntu-mono-dark", NULL);
  gtk_icon_theme_append_search_path(gtk_icon_theme_get_default(),
                                  INDICATOR_ICONS_DIR);
  gtk_rc_parse_string (
    "widget \"indicator-applet*\" style \"panel\"\n"
    "style \"indicator-applet-style\"\n"
    "{\n"
    "    GtkMenuBar::shadow-type = none\n"
    "    GtkMenuBar::internal-padding = 0\n"
    "    GtkWidget::focus-line-width = 0\n"
    "    GtkWidget::focus-padding = 0\n"
    "}\n"
    "style \"indicator-applet-menubar-style\"\n"
    "{\n"
    "    GtkMenuBar::shadow-type = none\n"
    "    GtkMenuBar::internal-padding = 0\n"
    "    GtkWidget::focus-line-width = 0\n"
    "    GtkWidget::focus-padding = 0\n"
    "    GtkMenuItem::horizontal-padding = 0\n"
    "}\n"
    "style \"indicator-applet-menuitem-style\"\n"
    "{\n"
    "    GtkWidget::focus-line-width = 0\n"
    "    GtkWidget::focus-padding = 0\n"
    "    GtkMenuItem::horizontal-padding = 0\n"
    "}\n"
    "widget \"*.indicator-applet\" style \"indicator-applet-style\""
    "widget \"*.indicator-applet-menuitem\" style \"indicator-applet-menuitem-style\""
    "widget \"*.indicator-applet-menubar\" style \"indicator-applet-menubar-style\"");
  gtk_widget_set_name(GTK_WIDGET (indicator->container), "indicator-applet");
  /* create some panel widgets */
  
  /* Build menu */
  indicator->menu = gtk_menu_bar_new();
  GTK_WIDGET_SET_FLAGS (indicator->menu, GTK_WIDGET_FLAGS(indicator->menu) | GTK_CAN_FOCUS);
  gtk_widget_set_name(GTK_WIDGET (indicator->menu), "indicator-applet-menubar");
  g_signal_connect(indicator->menu, "button-press-event", G_CALLBACK(on_menu_press), NULL);
  //g_signal_connect_after(indicator->menu, "expose-event", G_CALLBACK(menu_on_expose), menu);
  gtk_container_set_border_width(GTK_CONTAINER(indicator->menu), 0);

  /* load 'em */
  if (!g_file_test(INDICATOR_DIR, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR)) {
    g_warning ("%s does not exist, cannot read any indicators", INDICATOR_DIR);
  } else {
    GDir        *dir;
    const gchar *name;

    dir = g_dir_open(INDICATOR_DIR, 0, NULL);
    while ((name = g_dir_read_name (dir)) != NULL) {
      if (g_strcmp0(name, "libappmenu.so") == 0) {
        continue;
      }

      if (load_module(name, indicator->menu)) {
        indicators_loaded++;
      } else {
        g_warning("Failed to load module %s", name);
      }
    }
    g_dir_close(dir);
  }

  if (indicators_loaded == 0) {
    /* A label to allow for click through */
    indicator->item = gtk_button_new();
    gtk_button_set_label(GTK_BUTTON(indicator->item), "No Indicators");
    gtk_widget_show(indicator->item);
    gtk_container_add (GTK_CONTAINER (indicator->container), indicator->item);
  } else {
    gtk_widget_show(indicator->menu);
    gtk_container_add (GTK_CONTAINER (indicator->container), indicator->menu);
  }
  return indicator;
}



void
indicator_free (IndicatorPlugin    *indicator)
{
  g_free(indicator);
}


#if 0
static gboolean
indicator_size_changed (XfcePanelPlugin *plugin,
                     gint             size,
                     IndicatorPlugin    *indicator)
{
  GtkOrientation orientation;

  /* get the orientation of the plugin */
  orientation = xfce_panel_plugin_get_orientation (plugin);

  /* set the widget size */
  if (orientation == GTK_ORIENTATION_HORIZONTAL)
    gtk_widget_set_size_request (GTK_WIDGET (plugin), -1, size);
  else
    gtk_widget_set_size_request (GTK_WIDGET (plugin), size, -1);

  /* we handled the orientation */
  return TRUE;
}
#endif


static gboolean
on_menu_press (GtkWidget *widget, GdkEventButton *event, IndicatorPlugin *indicator)
{
    if (indicator != NULL && event->button == 1) /* left click only */
    {
     /*   gtk_menu_popup (GTK_MENU(indicator->menu), NULL, NULL, NULL, NULL, 0,
                        event->time);*/
        return TRUE;
    }
    return FALSE ;
}
