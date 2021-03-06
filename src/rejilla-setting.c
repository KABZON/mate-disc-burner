/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * rejilla
 * Copyright (C) Philippe Rouquier 2009 <bonfire-app@wanadoo.fr>
 * 
 * rejilla is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * rejilla is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <glib.h>
#include <gtk/gtk.h>

#include "rejilla-setting.h"

typedef struct _RejillaSettingPrivate RejillaSettingPrivate;
struct _RejillaSettingPrivate
{
	gint win_width;
	gint win_height;
	gint stock_file_chooser_percent;
	gint rejilla_file_chooser_percent;
	gint player_volume;
	gint display_layout;
	gint data_disc_column;
	gint data_disc_column_order;
	gint image_size_width;
	gint image_size_height;
	gint video_size_height;
	gint video_size_width;
	gint display_proportion;

	gchar *layout_audio;
	gchar *layout_video;
	gchar *layout_data;

	gchar **search_entry_history;

	guint win_maximized:1;
	guint show_preview:1;
	guint show_sidepane:1;
};

#define REJILLA_SETTING_PRIVATE(o)  (G_TYPE_INSTANCE_GET_PRIVATE ((o), REJILLA_TYPE_SETTING, RejillaSettingPrivate))

enum
{
	VALUE_CHANGED,

	LAST_SIGNAL
};


static guint setting_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (RejillaSetting, rejilla_setting, G_TYPE_OBJECT);

gboolean
rejilla_setting_get_value (RejillaSetting *setting,
                           RejillaSettingValue setting_value,
                           gpointer *value)
{
	RejillaSettingPrivate *priv;

	g_return_val_if_fail (REJILLA_IS_SETTING (setting), FALSE);
	g_return_val_if_fail (value != NULL, FALSE);

	priv = REJILLA_SETTING_PRIVATE (setting);

	switch (setting_value) {
		/** gint **/
		case REJILLA_SETTING_WIN_WIDTH:
			(*value) = GINT_TO_POINTER (priv->win_width);
			break;

		case REJILLA_SETTING_WIN_HEIGHT:
			(*value) = GINT_TO_POINTER (priv->win_height);
			break;

		case REJILLA_SETTING_STOCK_FILE_CHOOSER_PERCENT:
			(*value) = GINT_TO_POINTER (priv->stock_file_chooser_percent);
			break;
	
		case REJILLA_SETTING_REJILLA_FILE_CHOOSER_PERCENT:
			(*value) = GINT_TO_POINTER (priv->rejilla_file_chooser_percent);
			break;

		case REJILLA_SETTING_PLAYER_VOLUME:
			(*value) = GINT_TO_POINTER (priv->player_volume);
			break;

		case REJILLA_SETTING_DISPLAY_LAYOUT:
			(*value) = GINT_TO_POINTER (priv->display_layout);
			break;

		case REJILLA_SETTING_DATA_DISC_COLUMN:
			(*value) = GINT_TO_POINTER (priv->data_disc_column);
			break;

		case REJILLA_SETTING_DATA_DISC_COLUMN_ORDER:
			(*value) = GINT_TO_POINTER (priv->data_disc_column_order);
			break;

		case REJILLA_SETTING_IMAGE_SIZE_WIDTH:
			(*value) = GINT_TO_POINTER (priv->image_size_width);
			break;

		case REJILLA_SETTING_IMAGE_SIZE_HEIGHT:
			(*value) = GINT_TO_POINTER (priv->image_size_height);
			break;

		case REJILLA_SETTING_VIDEO_SIZE_HEIGHT:
			(*value) = GINT_TO_POINTER (priv->video_size_height);
			break;

		case REJILLA_SETTING_VIDEO_SIZE_WIDTH:
			(*value) = GINT_TO_POINTER (priv->video_size_width);
			break;

		case REJILLA_SETTING_DISPLAY_PROPORTION:
			(*value) = GINT_TO_POINTER (priv->display_proportion);
			break;

			/** gboolean **/
		case REJILLA_SETTING_WIN_MAXIMIZED:
			(*value) = GINT_TO_POINTER (priv->win_maximized);
			break;

		case REJILLA_SETTING_SHOW_PREVIEW:
			(*value) = GINT_TO_POINTER (priv->show_preview);
			break;

		case REJILLA_SETTING_SHOW_SIDEPANE:
			(*value) = GINT_TO_POINTER (priv->show_sidepane);
			break;

			/** gchar * **/
		case REJILLA_SETTING_DISPLAY_LAYOUT_AUDIO:
			(*value) = priv->layout_audio;
			break;

		case REJILLA_SETTING_DISPLAY_LAYOUT_VIDEO:
			(*value) = priv->layout_video;
			break;

		case REJILLA_SETTING_DISPLAY_LAYOUT_DATA:
			(*value) = priv->layout_data;
			break;

			/** gchar ** **/
		case REJILLA_SETTING_SEARCH_ENTRY_HISTORY:
			(*value) = g_strdupv (priv->search_entry_history);
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

gboolean
rejilla_setting_set_value (RejillaSetting *setting,
                           RejillaSettingValue setting_value,
                           gconstpointer value)
{
	RejillaSettingPrivate *priv;

	g_return_val_if_fail (REJILLA_IS_SETTING (setting), FALSE);

	priv = REJILLA_SETTING_PRIVATE (setting);

	switch (setting_value) {
		/** gint **/
		case REJILLA_SETTING_WIN_WIDTH:
			priv->win_width = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_WIN_HEIGHT:
			priv->win_height = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_STOCK_FILE_CHOOSER_PERCENT:
			priv->stock_file_chooser_percent = GPOINTER_TO_INT (value);
			break;
	
		case REJILLA_SETTING_REJILLA_FILE_CHOOSER_PERCENT:
			priv->rejilla_file_chooser_percent = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_PLAYER_VOLUME:
			priv->player_volume = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_DISPLAY_LAYOUT:
			priv->display_layout = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_DATA_DISC_COLUMN:
			priv->data_disc_column = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_DATA_DISC_COLUMN_ORDER:
			priv->data_disc_column_order = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_IMAGE_SIZE_WIDTH:
			priv->image_size_width = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_IMAGE_SIZE_HEIGHT:
			priv->image_size_height = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_VIDEO_SIZE_HEIGHT:
			priv->video_size_height = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_VIDEO_SIZE_WIDTH:
			priv->video_size_width = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_DISPLAY_PROPORTION:
			priv->display_proportion = GPOINTER_TO_INT (value);
			break;

			/** gboolean **/
		case REJILLA_SETTING_WIN_MAXIMIZED:
			priv->win_maximized = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_SHOW_PREVIEW:
			priv->show_preview = GPOINTER_TO_INT (value);
			break;

		case REJILLA_SETTING_SHOW_SIDEPANE:
			priv->show_sidepane = GPOINTER_TO_INT (value);
			break;

			/** gchar * **/
		case REJILLA_SETTING_DISPLAY_LAYOUT_AUDIO:
			priv->layout_audio = g_strdup (value);
			break;

		case REJILLA_SETTING_DISPLAY_LAYOUT_VIDEO:
			priv->layout_video = g_strdup (value);
			break;

		case REJILLA_SETTING_DISPLAY_LAYOUT_DATA:
			priv->layout_data = g_strdup (value);
			break;

			/** gchar ** **/
		case REJILLA_SETTING_SEARCH_ENTRY_HISTORY:
			if (priv->search_entry_history)
				g_strfreev (priv->search_entry_history);

			if (value)
				priv->search_entry_history = g_strdupv ((gchar **) value);
			else
				priv->search_entry_history = NULL;

			break;

		default:
			return FALSE;
	}

	return TRUE;
}

static gint
rejilla_setting_get_integer (GKeyFile *file,
                             const gchar *group_name,
                             const gchar *key)
{
	GError *error = NULL;
	gint int_value;

	int_value = g_key_file_get_integer (file, group_name, key, &error);
	if (!int_value && error) {
		g_error_free (error);
		return -1;
	}

	return int_value;
}

gboolean
rejilla_setting_load (RejillaSetting *setting)
{
	RejillaSettingPrivate *priv;
	GKeyFile *key_file;
	gboolean res;
	gchar *path;

	priv = REJILLA_SETTING_PRIVATE (setting);

	path = g_build_path (G_DIR_SEPARATOR_S,
	                     g_get_user_config_dir (),
	                     "rejilla",
	                     "application-settings",
	                     NULL);

	key_file = g_key_file_new ();
	res = g_key_file_load_from_file (key_file,
	                                 path,
	                                 G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,
	                                 NULL);
	g_free (path);
	if (!res)
		return res;

	priv->search_entry_history = g_key_file_get_string_list (key_file,
	                                                         "Filter",
	                                                         "history",
	                                                         NULL,
	                                                         NULL);

	priv->layout_audio = g_key_file_get_string (key_file,
	                                            "Display",
	                                            "layout-audio",
	                                            NULL);
	priv->layout_video = g_key_file_get_string (key_file,
	                                            "Display",
	                                            "layout-video",
	                                            NULL);
	priv->layout_data = g_key_file_get_string (key_file,
	                                           "Display",
	                                           "layout-data",
	                                           NULL);
	priv->show_sidepane = g_key_file_get_boolean (key_file,
	                                              "Display",
	                                              "show-sidepane",
	                                              NULL);
	priv->show_preview = g_key_file_get_boolean (key_file,
	                                             "Display",
	                                             "show-preview",
	                                             NULL);

	priv->win_width = g_key_file_get_integer (key_file,
	                                          "Display",
	                                          "main-window-width",
	                                          NULL);
	priv->win_height = g_key_file_get_integer (key_file,
	                                           "Display",
	                                           "main-window-height",
	                                           NULL);
	priv->win_maximized = g_key_file_get_boolean (key_file,
	                                             "Display",
	                                             "main-window-maximized",
	                                             NULL);
	priv->stock_file_chooser_percent = rejilla_setting_get_integer (key_file,
	                                                                "Display",
	                                                                "stock-file-chooser-percent");
	priv->rejilla_file_chooser_percent = rejilla_setting_get_integer (key_file,
	                                                                  "Display",
	                                                                  "rejilla-file-chooser-percent");
	priv->player_volume = g_key_file_get_integer (key_file,
	                                              "Player",
	                                              "player-volume",
	                                              NULL);
	priv->display_layout = g_key_file_get_integer (key_file,
	                                              "Display",
	                                              "layout",
	                                              NULL);
	priv->data_disc_column = g_key_file_get_integer (key_file,
	                                                 "Display",
	                                                 "data-disc-column",
	                                                 NULL);
	priv->data_disc_column_order = g_key_file_get_integer (key_file,
	                                                       "Display",
	                                                       "data-disc-column-order",
	                                                       NULL);
	priv->image_size_width = g_key_file_get_integer (key_file,
	                                                 "Player",
	                                                 "image-size-width",
	                                                 NULL);
	priv->image_size_height = g_key_file_get_integer (key_file,
	                                                  "Player",
	                                                  "image-size-height",
	                                                  NULL);
	priv->video_size_width = g_key_file_get_integer (key_file,
	                                                 "Player",
	                                                 "video-size-width",
	                                                 NULL);
	priv->video_size_height = g_key_file_get_integer (key_file,
	                                                  "Player",
	                                                  "video-size-height",
	                                                  NULL);
	priv->display_proportion = rejilla_setting_get_integer (key_file,
	                                                        "Display",
	                                                        "pane-position");
	g_key_file_free (key_file);

	return TRUE;
}

gboolean
rejilla_setting_save (RejillaSetting *setting)
{
	RejillaSettingPrivate *priv;
	gchar *contents = NULL;
	gsize content_size = 0;
	GKeyFile *key_file;
	gboolean res;
	gchar *path;

	priv = REJILLA_SETTING_PRIVATE (setting);

	path = g_build_path (G_DIR_SEPARATOR_S,
	                     g_get_user_config_dir (),
	                     "rejilla",
	                     "application-settings",
	                     NULL);

	key_file = g_key_file_new ();
	res = g_key_file_load_from_file (key_file,
	                                 path,
	                                 G_KEY_FILE_KEEP_COMMENTS|G_KEY_FILE_KEEP_TRANSLATIONS,
	                                 NULL);

	/* Don't worry if it does not work, it could be
	 * that there isn't any at the moment. */
	if (priv->search_entry_history)
		g_key_file_set_string_list (key_file,
					    "Filter",
					    "history",
					    (const gchar *const*) priv->search_entry_history,
					    g_strv_length (priv->search_entry_history));

	if (priv->layout_audio)
		g_key_file_set_string (key_file,
		                       "Display",
		                       "layout-audio",
		                       priv->layout_audio);

	if (priv->layout_video)
		g_key_file_set_string (key_file,
		                       "Display",
		                       "layout-video",
		                       priv->layout_video);

	if (priv->layout_data)
		g_key_file_set_string (key_file,
		                       "Display",
		                       "layout-data",
		                       priv->layout_data);

	g_key_file_set_boolean (key_file,
	                        "Display",
	                        "show-sidepane",
	                        priv->show_sidepane);
	g_key_file_set_boolean (key_file,
	                        "Display",
	                        "show-preview",
	                        priv->show_preview);

	g_key_file_set_integer (key_file,
	                        "Display",
	                        "main-window-width",
	                        priv->win_width);
	g_key_file_set_integer (key_file,
	                        "Display",
	                        "main-window-height",
	                        priv->win_height);
	g_key_file_set_boolean (key_file,
	                        "Display",
	                        "main-window-maximized",
	                        priv->win_maximized);
	g_key_file_set_integer (key_file,
	                        "Display",
	                        "stock-file-chooser-percent",
	                        priv->stock_file_chooser_percent);
	g_key_file_set_integer (key_file,
	                        "Display",
	                        "rejilla-file-chooser-percent",
	                        priv->rejilla_file_chooser_percent);
	g_key_file_set_integer (key_file,
	                        "Player",
	                        "player-volume",
	                        priv->player_volume);
	g_key_file_set_integer (key_file,
	                        "Display",
	                        "layout",
	                        priv->display_layout);
	g_key_file_set_integer (key_file,
	                        "Display",
	                        "data-disc-column",
	                        priv->data_disc_column);
	g_key_file_set_integer (key_file,
	                        "Display",
	                        "data-disc-column-order",
	                        priv->data_disc_column_order);
	g_key_file_set_integer (key_file,
	                        "Player",
	                        "image-size-width",
	                        priv->image_size_width);
	g_key_file_set_integer (key_file,
	                        "Player",
	                        "image-size-height",
	                        priv->image_size_height);
	g_key_file_set_integer (key_file,
	                        "Player",
	                        "video-size-width",
	                        priv->video_size_width);
	g_key_file_set_integer (key_file,
	                        "Player",
	                        "video-size-height",
	                        priv->video_size_height);
	g_key_file_set_integer (key_file,
	                        "Display",
	                        "pane-position",
	                        priv->display_proportion);

	contents = g_key_file_to_data (key_file, &content_size, NULL);
	g_file_set_contents (path, contents, content_size, NULL);
	g_free (contents);
	g_free (path);

	g_key_file_free (key_file);

	return TRUE;
}

static void
rejilla_setting_init (RejillaSetting *object)
{
	RejillaSettingPrivate *priv;

	priv = REJILLA_SETTING_PRIVATE (object);
	priv->win_width = -1;
	priv->win_height = -1;
	priv->stock_file_chooser_percent = -1;
	priv->rejilla_file_chooser_percent = -1;
	priv->player_volume = -1;
	priv->display_layout = -1;
	priv->data_disc_column = -1;
	priv->data_disc_column_order = -1;
	priv->image_size_width = -1;
	priv->image_size_height = -1;
	priv->video_size_height = -1;
	priv->video_size_width = -1;
	priv->display_proportion = -1;
}

static void
rejilla_setting_finalize (GObject *object)
{
	RejillaSettingPrivate *priv;

	priv = REJILLA_SETTING_PRIVATE (object);
	if (priv->layout_video) {
		g_free (priv->layout_video);
		priv->layout_video = NULL;
	}

	if (priv->layout_data) {
		g_free (priv->layout_data);
		priv->layout_data = NULL;
	}

	if (priv->layout_audio) {
		g_free (priv->layout_audio);
		priv->layout_audio = NULL;
	}

	if (priv->search_entry_history) {
		g_strfreev (priv->search_entry_history);
		priv->search_entry_history = NULL;
	}

	G_OBJECT_CLASS (rejilla_setting_parent_class)->finalize (object);
}

static void
rejilla_setting_value_changed (RejillaSetting *self, gint value)
{
	/* TODO: Add default signal handler implementation here */
}

static void
rejilla_setting_class_init (RejillaSettingClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (RejillaSettingPrivate));

	object_class->finalize = rejilla_setting_finalize;

	klass->value_changed = rejilla_setting_value_changed;

	setting_signals[VALUE_CHANGED] =
		g_signal_new ("value_changed",
		              G_OBJECT_CLASS_TYPE (klass),
		              G_SIGNAL_RUN_LAST,
		              G_STRUCT_OFFSET (RejillaSettingClass, value_changed),
		              NULL, NULL,
		              g_cclosure_marshal_VOID__INT,
		              G_TYPE_NONE, 1,
		              G_TYPE_INT);
}

static RejillaSetting *default_setting = NULL;

RejillaSetting *
rejilla_setting_get_default (void)
{
	if (!default_setting)
		default_setting = g_object_new (REJILLA_TYPE_SETTING, NULL);

	return default_setting;
}
