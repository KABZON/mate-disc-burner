/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/*
 * Librejilla-burn
 * Copyright (C) Philippe Rouquier 2005-2009 <bonfire-app@wanadoo.fr>
 *
 * Librejilla-burn is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The Librejilla-burn authors hereby grant permission for non-GPL compatible
 * GStreamer plugins to be used and distributed together with GStreamer
 * and Librejilla-burn. This permission is above and beyond the permissions granted
 * by the GPL license by which Librejilla-burn is covered. If you modify this code
 * you may extend this exception to your version of the code, but you are not
 * obligated to do so. If you do not wish to do so, delete this exception
 * statement from your version.
 * 
 * Librejilla-burn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>

#include <glib.h>
#include <glib-object.h>
#include <glib/gi18n-lib.h>

#include <gtk/gtk.h>

#include "burn-basics.h"
#include "rejilla-tags.h"
#include "burn-image-format.h"
#include "rejilla-image-properties.h"
#include "rejilla-image-type-chooser.h"

typedef struct _RejillaImagePropertiesPrivate RejillaImagePropertiesPrivate;
struct _RejillaImagePropertiesPrivate
{
	RejillaSessionCfg *session;

	GtkWidget *format;
	GtkWidget *format_box;

	guint edited:1;
	guint is_video:1;
};

#define REJILLA_IMAGE_PROPERTIES_PRIVATE(o)  (G_TYPE_INSTANCE_GET_PRIVATE ((o), REJILLA_TYPE_IMAGE_PROPERTIES, RejillaImagePropertiesPrivate))

static GtkDialogClass* parent_class = NULL;

G_DEFINE_TYPE (RejillaImageProperties, rejilla_image_properties, GTK_TYPE_FILE_CHOOSER_DIALOG);

enum {
	PROP_0,
	PROP_SESSION
};

static RejillaImageFormat
rejilla_image_properties_get_format (RejillaImageProperties *self)
{
	RejillaImagePropertiesPrivate *priv;
	RejillaImageFormat format;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (self);

	if (priv->format == NULL)
		return REJILLA_IMAGE_FORMAT_NONE;

	rejilla_image_type_chooser_get_format (REJILLA_IMAGE_TYPE_CHOOSER (priv->format),
					       &format);

	return format;
}

static gchar *
rejilla_image_properties_get_path (RejillaImageProperties *self)
{
	RejillaImagePropertiesPrivate *priv;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (self);
	return gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (self));
}

static void
rejilla_image_properties_set_path (RejillaImageProperties *self,
				   const gchar *path)
{
	RejillaImagePropertiesPrivate *priv;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (self);
	if (path) {
		gchar *name;

		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (self), path);

		/* The problem here is that is the file name doesn't exist
		 * in the folder then it won't be displayed so we check that */
		name = g_path_get_basename (path);
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (self), name);
	    	g_free (name);
	}
	else
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (self),
						     g_get_home_dir ());
}

static void
rejilla_image_properties_format_changed_cb (RejillaImageTypeChooser *chooser,
					    RejillaImageProperties *self)
{
	RejillaImagePropertiesPrivate *priv;
	RejillaImageFormat format;
	gchar *image_path;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (self);

	/* make sure the extension is still valid */
	image_path = rejilla_image_properties_get_path (self);
	if (!image_path)
		return;

	format = rejilla_image_properties_get_format (self);

	/* Set the format now */
	rejilla_burn_session_set_image_output_format (REJILLA_BURN_SESSION (priv->session), format);

	/* make sure the format is valid and possibly update path */
	if (format == REJILLA_IMAGE_FORMAT_ANY
	||  format == REJILLA_IMAGE_FORMAT_NONE)
		format = rejilla_burn_session_get_output_format (REJILLA_BURN_SESSION (priv->session));

	if (!priv->edited) {
		/* not changed: get a new default path */
		g_free (image_path);
		image_path = rejilla_image_format_get_default_path (format);
	}
	else if (image_path) {
		gchar *tmp;

		tmp = image_path;
		image_path = rejilla_image_format_fix_path_extension (format, FALSE, image_path);
		g_free (tmp);
	}
	else {
		priv->edited = FALSE;
		image_path = rejilla_image_format_get_default_path (format);
	}

	rejilla_image_properties_set_path (self, image_path);

	/* This is specific to video projects */
	if (priv->is_video) {
		if (format == REJILLA_IMAGE_FORMAT_CUE) {
			gboolean res = TRUE;

			/* There should always be a priv->format in this case but who knows... */
			if (priv->format)
				res = rejilla_image_type_chooser_get_VCD_type (REJILLA_IMAGE_TYPE_CHOOSER (priv->format));

			if (res)
				rejilla_burn_session_tag_add_int (REJILLA_BURN_SESSION (priv->session),
				                                  REJILLA_VCD_TYPE,
				                                  REJILLA_SVCD);
			else
				rejilla_burn_session_tag_add_int (REJILLA_BURN_SESSION (priv->session),
				                                  REJILLA_VCD_TYPE,
				                                  REJILLA_VCD_V2);
		}
	}
}

static void
rejilla_image_properties_set_formats (RejillaImageProperties *self,
				      RejillaImageFormat formats,
				      RejillaImageFormat format)
{
	RejillaImagePropertiesPrivate *priv;
	guint num;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (self);

	/* have a look at the formats and see if it is worth to display a widget */
	if (formats == REJILLA_IMAGE_FORMAT_NONE) {
		if (priv->format) {
			gtk_widget_destroy (priv->format);
			priv->format = NULL;
		}

		return;
	}	

	if (!priv->format_box) {
		GtkWidget *box;
		GtkWidget *label;
		GtkWidget *dialog_box;

		box = gtk_hbox_new (FALSE, 6);
		gtk_container_set_border_width (GTK_CONTAINER (box), 4);

		dialog_box = gtk_dialog_get_content_area (GTK_DIALOG (self));
		gtk_box_pack_start (GTK_BOX (dialog_box),
				  box,
				  FALSE,
				  FALSE,
				  0);

		label = gtk_label_new (_("Disc image type:"));
		gtk_widget_show (label);
		gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);

		priv->format = rejilla_image_type_chooser_new ();
		gtk_widget_show (priv->format);
		gtk_box_pack_start (GTK_BOX (box),
				    priv->format,
				    TRUE,
				    TRUE,
				    0);
		g_signal_connect (priv->format,
				  "changed",
				  G_CALLBACK (rejilla_image_properties_format_changed_cb),
				  self);

		priv->format_box = box;
	}

	num = rejilla_image_type_chooser_set_formats (REJILLA_IMAGE_TYPE_CHOOSER (priv->format),
						      formats,
	                                              FALSE,
	                                              priv->is_video);

	if (priv->is_video && format == REJILLA_IMAGE_FORMAT_CUE) {
		/* see whether it's a SVCD or a VCD */
		rejilla_image_type_chooser_set_VCD_type (REJILLA_IMAGE_TYPE_CHOOSER (priv->format),
		                                         (rejilla_burn_session_tag_lookup_int (REJILLA_BURN_SESSION (priv->session), REJILLA_VCD_TYPE) == REJILLA_SVCD));
	}
	else
		rejilla_image_type_chooser_set_format (REJILLA_IMAGE_TYPE_CHOOSER (priv->format),
						       format);

	if (num < 2) {
		gtk_widget_destroy (priv->format_box);
		priv->format_box = NULL;
		priv->format = NULL;
	}
	else
		gtk_widget_show (priv->format_box);
}

static void
rejilla_image_properties_set_output_path (RejillaImageProperties *self,
					  RejillaImageFormat format,
					  const gchar *path)
{
	RejillaImagePropertiesPrivate *priv;
	RejillaImageFormat real_format;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (self);

	if (format == REJILLA_IMAGE_FORMAT_ANY
	||  format == REJILLA_IMAGE_FORMAT_NONE)
		real_format = rejilla_burn_session_get_output_format (REJILLA_BURN_SESSION (priv->session));
	else
		real_format = format;

	switch (real_format) {
	case REJILLA_IMAGE_FORMAT_BIN:
		rejilla_burn_session_set_image_output_full (REJILLA_BURN_SESSION (priv->session),
							    format,
							    path,
							    NULL);
		break;

	case REJILLA_IMAGE_FORMAT_CDRDAO:
	case REJILLA_IMAGE_FORMAT_CLONE:
	case REJILLA_IMAGE_FORMAT_CUE:
		rejilla_burn_session_set_image_output_full (REJILLA_BURN_SESSION (priv->session),
							    format,
							    NULL,
							    path);
		break;

	default:
		break;
	}
}

static void
rejilla_image_properties_response (GtkFileChooser *chooser,
				   gint response_id,
				   gpointer NULL_data)
{
	RejillaImagePropertiesPrivate *priv;
	RejillaImageFormat format;
	gchar *path;

	if (response_id != GTK_RESPONSE_OK)
		return;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (chooser);

	/* get and check format */
	format = rejilla_image_properties_get_format (REJILLA_IMAGE_PROPERTIES (chooser));

	/* see if the user has changed the path */
	path = rejilla_image_properties_get_path (REJILLA_IMAGE_PROPERTIES (chooser));
	rejilla_image_properties_set_output_path (REJILLA_IMAGE_PROPERTIES (chooser),
						  format,
						  path);
	g_free (path);

	if (priv->is_video) {
		if (format == REJILLA_IMAGE_FORMAT_CUE) {
			gboolean res = TRUE;

			/* There should always be a priv->format in this case but who knows... */
			if (priv->format)
				res = rejilla_image_type_chooser_get_VCD_type (REJILLA_IMAGE_TYPE_CHOOSER (priv->format));

			if (res)
				rejilla_burn_session_tag_add_int (REJILLA_BURN_SESSION (priv->session),
								  REJILLA_VCD_TYPE,
								  REJILLA_SVCD);
			else
				rejilla_burn_session_tag_add_int (REJILLA_BURN_SESSION (priv->session),
								  REJILLA_VCD_TYPE,
								  REJILLA_VCD_V2);
		}
	}
}

static gchar *
rejilla_image_properties_get_output_path (RejillaImageProperties *self)
{
	gchar *path = NULL;
	RejillaImageFormat format;
	RejillaImagePropertiesPrivate *priv;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (self);

	format = rejilla_burn_session_get_output_format (REJILLA_BURN_SESSION (priv->session));
	switch (format) {
	case REJILLA_IMAGE_FORMAT_BIN:
		rejilla_burn_session_get_output (REJILLA_BURN_SESSION (priv->session),
						 &path,
						 NULL);
		break;

	case REJILLA_IMAGE_FORMAT_CLONE:
	case REJILLA_IMAGE_FORMAT_CDRDAO:
	case REJILLA_IMAGE_FORMAT_CUE:
		rejilla_burn_session_get_output (REJILLA_BURN_SESSION (priv->session),
						 NULL,
						 &path);
		break;

	default:
		break;
	}

	return path;
}

static void
rejilla_image_properties_update (RejillaImageProperties *self)
{
	RejillaImagePropertiesPrivate *priv;
	RejillaTrackType *track_type;
	RejillaImageFormat formats;
	RejillaImageFormat format;
	gchar *path;
	guint num;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (self);

	priv->edited = rejilla_session_cfg_has_default_output_path (priv->session);

	track_type = rejilla_track_type_new ();

	rejilla_burn_session_get_input_type (REJILLA_BURN_SESSION (priv->session), track_type);
	if (rejilla_track_type_get_has_stream (track_type)
	&& REJILLA_STREAM_FORMAT_HAS_VIDEO (rejilla_track_type_get_stream_format (track_type)))
		priv->is_video = TRUE;
	else
		priv->is_video = FALSE;

	rejilla_track_type_free (track_type);

	/* set all information namely path and format */
	path = rejilla_image_properties_get_output_path (self);
	rejilla_image_properties_set_path (self, path);
	g_free (path);

	format = rejilla_burn_session_get_output_format (REJILLA_BURN_SESSION (priv->session));

	num = rejilla_burn_session_get_possible_output_formats (REJILLA_BURN_SESSION (priv->session), &formats);
	rejilla_image_properties_set_formats (self,
					      num > 0 ? formats:REJILLA_IMAGE_FORMAT_NONE,
					      format);
}

void
rejilla_image_properties_set_session (RejillaImageProperties *props,
				      RejillaSessionCfg *session)
{
	RejillaImagePropertiesPrivate *priv;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (props);

	priv->session = g_object_ref (session);
	rejilla_image_properties_update (REJILLA_IMAGE_PROPERTIES (props));
}

static void
rejilla_image_properties_set_property (GObject *object,
				       guint property_id,
				       const GValue *value,
				       GParamSpec *pspec)
{
	RejillaImagePropertiesPrivate *priv;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (object);

	switch (property_id) {
	case PROP_SESSION: /* Readable and only writable at creation time */
		rejilla_image_properties_set_session (REJILLA_IMAGE_PROPERTIES (object),
						      g_value_get_object (value));
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
rejilla_image_properties_get_property (GObject *object,
				       guint property_id,
				       GValue *value,
				       GParamSpec *pspec)
{
	RejillaImagePropertiesPrivate *priv;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (object);

	switch (property_id) {
	case PROP_SESSION:
		g_value_set_object (value, G_OBJECT (priv->session));
		break;

	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
	}
}

static void
rejilla_image_properties_init (RejillaImageProperties *object)
{
	RejillaImagePropertiesPrivate *priv;
	GtkWidget *box;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (object);

	gtk_window_set_title (GTK_WINDOW (object), _("Location for Image File"));
	box = gtk_dialog_get_content_area (GTK_DIALOG (object));
	gtk_container_set_border_width (GTK_CONTAINER (box), 10);

	g_signal_connect (object,
			  "response",
			  G_CALLBACK (rejilla_image_properties_response),
			  NULL);
}

static void
rejilla_image_properties_finalize (GObject *object)
{
	RejillaImagePropertiesPrivate *priv;

	priv = REJILLA_IMAGE_PROPERTIES_PRIVATE (object);

	if (priv->session) {
		g_object_unref (priv->session);
		priv->session = NULL;
	}

	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
rejilla_image_properties_class_init (RejillaImagePropertiesClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	parent_class = GTK_DIALOG_CLASS (g_type_class_peek_parent (klass));

	g_type_class_add_private (klass, sizeof (RejillaImagePropertiesPrivate));

	object_class->set_property = rejilla_image_properties_set_property;
	object_class->get_property = rejilla_image_properties_get_property;
	object_class->finalize = rejilla_image_properties_finalize;

	g_object_class_install_property (object_class,
					 PROP_SESSION,
					 g_param_spec_object ("session",
							      "The session",
							      "The session to work with",
							      REJILLA_TYPE_SESSION_CFG,
							      G_PARAM_READWRITE));
}

GtkWidget *
rejilla_image_properties_new (void)
{
	/* Reminder: because it is a GtkFileChooser we can set the session as
	 * a construct parameter because the GtkFileChooser interface won't be
	 * setup when we set the value for the session property. */
	return GTK_WIDGET (g_object_new (REJILLA_TYPE_IMAGE_PROPERTIES,
					 "action", GTK_FILE_CHOOSER_ACTION_SAVE,
					 "do-overwrite-confirmation", TRUE,
					 "local-only", TRUE,
					 NULL));
}
