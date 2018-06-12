/* GStreamer
 *
 * Copyright (C) 2014-2015 Stephan Hesse <stephan@emliri.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstplayer.h"
#include "gstplayer-signal-adapter.h"

#include <gst/gst.h>

GST_DEBUG_CATEGORY_STATIC (gst_player_debug);
#define GST_CAT_DEFAULT gst_player_debug

enum
{
  SIGNAL_URI_LOADED,
  SIGNAL_POSITION_UPDATED,
  SIGNAL_DURATION_CHANGED,
  SIGNAL_STATE_CHANGED,
  SIGNAL_BUFFERING,
  SIGNAL_END_OF_STREAM,
  SIGNAL_ERROR,
  SIGNAL_WARNING,
  SIGNAL_VIDEO_DIMENSIONS_CHANGED,
  SIGNAL_MEDIA_INFO_UPDATED,
  SIGNAL_VOLUME_CHANGED,
  SIGNAL_MUTE_CHANGED,
  SIGNAL_SEEK_DONE,
  SIGNAL_LAST
};

struct _GstPlayerSignalAdapter
{
  GstObject parent;
  GstPlayer *player;
};

struct _GstPlayerSignalAdapterClass
{
  GstObjectClass parent_class;
};

#define parent_class gst_player_signal_adapter_parent_class
G_DEFINE_TYPE (GstPlayerSignalAdapter, gst_player_signal_adapter, GST_TYPE_OBJECT);

static guint signals[SIGNAL_LAST] = { 0, };

void gst_player_signal_adapter_dispose(GstPlayerSignalAdapter* adapter) {

}

void gst_player_signal_adapter_finalize(GstPlayerSignalAdapter* adapter) {
  
}

static void
gst_player_signal_adapter_class_init (GstPlayerSignalAdapterClass * klass)
{
  /*
  gobject_class->set_property = gst_player_set_property;
  gobject_class->get_property = gst_player_get_property;
  */
  gobject_class->dispose = gst_player_signal_adapter_dispose;
  gobject_class->finalize = gst_player_signal_adapter_finalize;
  // gobject_class->constructed = gst_player_constructed;

  signals[SIGNAL_URI_LOADED] =
      g_signal_new ("uri-loaded", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 1, G_TYPE_STRING);

  signals[SIGNAL_POSITION_UPDATED] =
      g_signal_new ("position-updated", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 1, GST_TYPE_CLOCK_TIME);

  signals[SIGNAL_DURATION_CHANGED] =
      g_signal_new ("duration-changed", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 1, GST_TYPE_CLOCK_TIME);

  signals[SIGNAL_STATE_CHANGED] =
      g_signal_new ("state-changed", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 1, GST_TYPE_PLAYER_STATE);

  signals[SIGNAL_BUFFERING] =
      g_signal_new ("buffering", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 1, G_TYPE_INT);

  signals[SIGNAL_END_OF_STREAM] =
      g_signal_new ("end-of-stream", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 0, G_TYPE_INVALID);

  signals[SIGNAL_ERROR] =
      g_signal_new ("error", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 1, G_TYPE_ERROR);

  signals[SIGNAL_VIDEO_DIMENSIONS_CHANGED] =
      g_signal_new ("video-dimensions-changed", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);

  signals[SIGNAL_MEDIA_INFO_UPDATED] =
      g_signal_new ("media-info-updated", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 1, GST_TYPE_PLAYER_MEDIA_INFO);

  signals[SIGNAL_VOLUME_CHANGED] =
      g_signal_new ("volume-changed", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 0, G_TYPE_INVALID);

  signals[SIGNAL_MUTE_CHANGED] =
      g_signal_new ("mute-changed", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 0, G_TYPE_INVALID);

  signals[SIGNAL_WARNING] =
      g_signal_new ("warning", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 1, G_TYPE_ERROR);

  signals[SIGNAL_SEEK_DONE] =
      g_signal_new ("seek-done", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS, 0, NULL,
      NULL, NULL, G_TYPE_NONE, 1, GST_TYPE_CLOCK_TIME);
}

static void
eos_dispatch (gpointer user_data)
{
  GstPlayer *player = user_data;

  if (player->inhibit_sigs)
    return;

  g_signal_emit (player, signals[SIGNAL_END_OF_STREAM], 0);
}

typedef struct
{
  GstPlayer *player;
  gint percent;
} BufferingSignalData;

static void
buffering_dispatch (gpointer user_data)
{
  BufferingSignalData *data = user_data;

  if (data->player->inhibit_sigs)
    return;

  if (data->player->target_state >= GST_STATE_PAUSED) {
    g_signal_emit (data->player, signals[SIGNAL_BUFFERING], 0, data->percent);
  }
}

static void
buffering_signal_data_free (BufferingSignalData * data)
{
  g_object_unref (data->player);
  g_free (data);
}

typedef struct
{
  GstPlayer *player;
  gint width, height;
} VideoDimensionsChangedSignalData;

static void
video_dimensions_changed_dispatch (gpointer user_data)
{
  VideoDimensionsChangedSignalData *data = user_data;

  if (data->player->inhibit_sigs)
    return;

  if (data->player->target_state >= GST_STATE_PAUSED) {
    g_signal_emit (data->player, signals[SIGNAL_VIDEO_DIMENSIONS_CHANGED], 0,
        data->width, data->height);
  }
}

static void
video_dimensions_changed_signal_data_free (VideoDimensionsChangedSignalData *
    data)
{
  g_object_unref (data->player);
  g_free (data);
}

typedef struct
{
  GstPlayer *player;
  gchar *uri;
} UriLoadedSignalData;

static void
uri_loaded_dispatch (gpointer user_data)
{
  UriLoadedSignalData *data = user_data;

  g_signal_emit (data->player, signals[SIGNAL_URI_LOADED], 0, data->uri);
}

static void
uri_loaded_signal_data_free (UriLoadedSignalData * data)
{
  g_object_unref (data->player);
  g_free (data->uri);
  g_free (data);
}

typedef struct
{
  GstPlayer *player;
  GstPlayerState state;
} StateChangedSignalData;

static void
state_changed_dispatch (gpointer user_data)
{
  StateChangedSignalData *data = user_data;

  if (data->player->inhibit_sigs && data->state != GST_PLAYER_STATE_STOPPED
      && data->state != GST_PLAYER_STATE_PAUSED)
    return;

  g_signal_emit (data->player, signals[SIGNAL_STATE_CHANGED], 0, data->state);
}

static void
state_changed_signal_data_free (StateChangedSignalData * data)
{
  g_object_unref (data->player);
  g_free (data);
}
typedef struct
{
  GstPlayer *player;
  GstClockTime position;
} PositionUpdatedSignalData;

static void
position_updated_dispatch (gpointer user_data)
{
  PositionUpdatedSignalData *data = user_data;

  if (data->player->inhibit_sigs)
    return;

  if (data->player->target_state >= GST_STATE_PAUSED) {
    g_signal_emit (data->player, signals[SIGNAL_POSITION_UPDATED], 0,
        data->position);
    g_object_notify_by_pspec (G_OBJECT (data->player),
        param_specs[PROP_POSITION]);
  }
}

static void
position_updated_signal_data_free (PositionUpdatedSignalData * data)
{
  g_object_unref (data->player);
  g_free (data);
}

typedef struct
{
  GstPlayer *player;
  GError *err;
} ErrorSignalData;

static void
error_dispatch (gpointer user_data)
{
  ErrorSignalData *data = user_data;

  if (data->player->inhibit_sigs)
    return;

  g_signal_emit (data->player, signals[SIGNAL_ERROR], 0, data->err);
}

static void
free_error_signal_data (ErrorSignalData * data)
{
  g_object_unref (data->player);
  g_clear_error (&data->err);
  g_free (data);
}

typedef struct
{
  GstPlayer *player;
  GError *err;
} WarningSignalData;

static void
warning_dispatch (gpointer user_data)
{
  WarningSignalData *data = user_data;

  if (data->player->inhibit_sigs)
    return;

  g_signal_emit (data->player, signals[SIGNAL_WARNING], 0, data->err);
}

static void
free_warning_signal_data (WarningSignalData * data)
{
  g_object_unref (data->player);
  g_clear_error (&data->err);
  g_free (data);
}

typedef struct
{
  GstPlayer *player;
  GstClockTime duration;
} DurationChangedSignalData;

static void
duration_changed_dispatch (gpointer user_data)
{
  DurationChangedSignalData *data = user_data;

  if (data->player->inhibit_sigs)
    return;

  if (data->player->target_state >= GST_STATE_PAUSED) {
    g_signal_emit (data->player, signals[SIGNAL_DURATION_CHANGED], 0,
        data->duration);
    g_object_notify_by_pspec (G_OBJECT (data->player),
        param_specs[PROP_DURATION]);
  }
}

static void
duration_changed_signal_data_free (DurationChangedSignalData * data)
{
  g_object_unref (data->player);
  g_free (data);
}

typedef struct
{
  GstPlayer *player;
  GstClockTime position;
} SeekDoneSignalData;

static void
seek_done_dispatch (gpointer user_data)
{
  SeekDoneSignalData *data = user_data;

  if (data->player->inhibit_sigs)
    return;

  g_signal_emit (data->player, signals[SIGNAL_SEEK_DONE], 0, data->position);
}

static void
seek_done_signal_data_free (SeekDoneSignalData * data)
{
  g_object_unref (data->player);
  g_free (data);
}

typedef struct
{
  GstPlayer *player;
  GstPlayerMediaInfo *info;
} MediaInfoUpdatedSignalData;

static void
media_info_updated_dispatch (gpointer user_data)
{
  MediaInfoUpdatedSignalData *data = user_data;

  if (data->player->inhibit_sigs)
    return;

  if (data->player->target_state >= GST_STATE_PAUSED) {
    g_signal_emit (data->player, signals[SIGNAL_MEDIA_INFO_UPDATED], 0,
        data->info);
  }
}

static void
media_info_updated_signal_data_free (MediaInfoUpdatedSignalData * data)
{
  g_object_unref (data->player);
  g_object_unref (data->info);
  g_free (data);
}

static void
volume_changed_dispatch (gpointer user_data)
{
  GstPlayer *player = user_data;

  if (player->inhibit_sigs)
    return;

  g_signal_emit (player, signals[SIGNAL_VOLUME_CHANGED], 0);
  g_object_notify_by_pspec (G_OBJECT (player), param_specs[PROP_VOLUME]);
}

static void
mute_changed_dispatch (gpointer user_data)
{
  GstPlayer *player = user_data;

  if (player->inhibit_sigs)
    return;

  g_signal_emit (player, signals[SIGNAL_MUTE_CHANGED], 0);
  g_object_notify_by_pspec (G_OBJECT (player), param_specs[PROP_MUTE]);
}
