/*
 * Copyright (C) 2015 Focalcrest, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <xs/pal.h>

int g_nScreenWidth = 400;
int g_nScreenHeight = 800;


static gboolean on_expose(GtkWidget *widget, GdkEventExpose *event,
		gpointer data)
{
	cairo_t *cr = gdk_cairo_create(widget->window);
	xsGraphics *gc = xsGetSystemGc();

	cr = gdk_cairo_create(widget->window);

	//gdk_cairo_region(cr, event->region);
	cairo_set_source_surface(cr, XS_SURFACE(gc), 0, 0);
	cairo_paint(cr);

	cairo_destroy (cr);

	return FALSE;
}

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event,
		gpointer data)
{
	xsSysEvent evt = {0};
	evt.type = XS_EVT_KEY_DOWN;
	switch(event ->keyval)
	{
	case GDK_KEY_Left:
		evt.data.key.keyCode = XS_PAD_KEY_LEFT_ARROW;
		break;
	case GDK_KEY_Up:
		evt.data.key.keyCode = XS_PAD_KEY_UP_ARROW;
		break;
	case GDK_KEY_Right:
		evt.data.key.keyCode = XS_PAD_KEY_RIGHT_ARROW;
		break;
	case GDK_KEY_Down:
		evt.data.key.keyCode = XS_PAD_KEY_DOWN_ARROW;
		break;
	case GDK_KEY_Return:
		evt.data.key.keyCode = XS_PAD_KEY_ENTER;
		break;
	default:
		evt.data.key.keyCode = XS_PAD_KEY_UNDEFINED;
		break;

	}
	xsSysEventHandler(&evt);
	return TRUE;
}

static gboolean on_key_release(GtkWidget *widget, GdkEventKey *event,
		gpointer data)
{
	return TRUE;
}

static gboolean on_button_press(GtkWidget *widget,
		GdkEventButton *event, gpointer data)
{
	return TRUE;
}

static gboolean on_button_release(GtkWidget *widget,
		GdkEventButton *event, gpointer data)
{
	return TRUE;
}

static gboolean on_motion_notify(GtkWidget *widget,
		GdkEventMotion *event, gpointer data)
{
	return TRUE;
}

static void on_destroy(GtkWidget *widget, gpointer *data)
{
	gtk_main_quit();
}

int main(int argc, char *argv[])
{
	GtkWidget *window;
	char title[64] = {0};
	char appId[XS_MAX_PATH] = {0};
	xsStrCpy(appId, "file:////home/lewis/project/test/app.xpk");

	// parser arguments
	gtk_init(&argc, &argv);

	// load title and orientation from main app
	xsSysEvent evt = {0};

	evt.type = XS_EVT_LOAD;
	evt.data.app.uri = appId;
	if (xsSysEventHandler(&evt) == XS_EC_OK)
	{// get app properties
		if (evt.data.app.name != NULL)
			xsTcsCpy(title, evt.data.app.name);
		if (evt.data.app.orient != XS_APP_ORIENT_DEFAULT)
		{
			int tempWidth = g_nScreenWidth;
			// exchange width and height if required
			if ((evt.data.app.orient == XS_APP_ORIENT_LANDSCAPE && g_nScreenWidth < g_nScreenHeight) ||
				(evt.data.app.orient == XS_APP_ORIENT_PORTRAIT && g_nScreenWidth > g_nScreenHeight))
			{
				g_nScreenWidth = g_nScreenHeight;
				g_nScreenHeight = tempWidth;
			}
		}
	}
	else
	{
		return -1;
	}

	// create window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), title);
	gtk_window_set_default_size(GTK_WINDOW(window), g_nScreenWidth,
			g_nScreenHeight);
	gtk_widget_set_app_paintable(window, TRUE);

	// set border width
	gtk_container_border_width(GTK_CONTAINER (window), 10);

	// set event filter
	gtk_widget_set_events(window, GDK_EXPOSURE_MASK
			| GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK
			| GDK_BUTTON_PRESS_MASK	| GDK_BUTTON_RELEASE_MASK
			| GDK_BUTTON_MOTION_MASK
			| GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK
			| GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK
			| GDK_EXPOSURE_MASK | GDK_FOCUS_CHANGE_MASK);

	// set expose callback
	g_signal_connect(G_OBJECT(window), "expose-event",
			G_CALLBACK(on_expose), NULL);

	// set key callback
	g_signal_connect(G_OBJECT(window), "key-press-event",
			G_CALLBACK(on_key_press), NULL);
	g_signal_connect(G_OBJECT(window), "key-release-event",
			G_CALLBACK(on_key_release), NULL);

	// set mouse callback
	g_signal_connect(G_OBJECT(window), "button-press-event",
			G_CALLBACK(on_button_press), NULL);
	g_signal_connect(G_OBJECT(window), "button-release-event",
			G_CALLBACK(on_button_release), NULL);
	g_signal_connect(G_OBJECT(window), "motion-notify-event",
			G_CALLBACK(on_motion_notify), NULL);

	// set destroy callback
	g_signal_connect(GTK_OBJECT(window), "destroy",
			GTK_SIGNAL_FUNC(on_destroy),
			NULL);

	// show window
	gtk_widget_show(window);

	// initial graphics interface
	PalGiInit(window->window);

	// start application
	evt.type = XS_EVT_START;
	xsSysEventHandler(&evt);

	// enter main loop
	gtk_main();

	// destroy graphics interface
	PalGiUninit();

	return 0;
}

