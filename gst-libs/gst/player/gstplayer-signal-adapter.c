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

enum
{
  PROP_0,
  PROP_MESSAGE_BUS,
  PROP_LAST
};

struct _GstPlayerSignalAdapter
{
  GstObject parent;
  GstBus *bus;
  GSource *bus_event_source;
};

struct _GstPlayerSignalAdapterClass
{
  GstObjectClass parent_class;
};

#define parent_class gst_player_signal_adapter_parent_class
G_DEFINE_TYPE (GstPlayerSignalAdapter, gst_player_signal_adapter, GST_TYPE_OBJECT);

static guint signals[SIGNAL_LAST] = { 0, };

static GParamSpec *param_specs[PROP_LAST] = { NULL, };

static void
gst_player_signal_adapter_emit(GstPlayerMessage message_type)
{
  switch (message_type) {
  case GST_PLAYER_MESSAGE_URI_LOADED:

    break;
  case GST_PLAYER_MESSAGE_POSITION_UPDATED:

    break;
  case GST_PLAYER_MESSAGE_DURATION_CHANGED:

    break;
  case GST_PLAYER_MESSAGE_STATE_CHANGED:

    break;
  case GST_PLAYER_MESSAGE_BUFFERING:

    break;
  case GST_PLAYER_MESSAGE_END_OF_STREAM:
    break;
  case GST_PLAYER_MESSAGE_ERROR:

    break;
  case GST_PLAYER_MESSAGE_WARNING:

    break;
  case GST_PLAYER_MESSAGE_VIDEO_DIMENSIONS_CHANGED:

    break;
  case GST_PLAYER_MESSAGE_MEDIA_INFO_UPDATED:

    break;
  case GST_PLAYER_MESSAGE_VOLUME_CHANGED:

    break;
  case GST_PLAYER_MESSAGE_MUTE_CHANGED:

    break;
  case GST_PLAYER_MESSAGE_SEEK_DONE:

    break;
  default:
    // Q: handle this some way?
    break;
  }
}

/**
 * To integrate with other non-Glib event-loops.
 *
 * Plain blocking poll with optional timeout. If timeout is 0 a message has to be on the bus immediately..
 * If timeout is GST_CLOCK_TIME_NONE, this function will block forever until a message was posted on the bus.
 *
 * Should not be used in combination with a main-context passed in the constructor unless intended. In this case,
 * both the bus-watching event-sources callbacks and this function will compete for pop'ing off messages from the bus.
 *
 * If there is no concurrent consumer, this method when called repeatedly will consume and process on message
 * from the bus on each call. That is, it will emit a signal (synchroneously) from within the call (_emit)
 * and then unref the message obtained.
 *
 * Returns TRUE when a message was consumed, FALSE when none was obtained.
 */
GST_PLAYER_API
gboolean gst_player_signal_adapter_poll_one (GstPlayerSignalAdapter * self, GstClockTime max_timeout)
{
  g_warn_if_fail(!self->bus_event_source);

  GstMessage *msg = gst_bus_timed_pop(self->bus, max_timeout);
  if (!msg) {
    return FALSE;
  }
  gst_player_signal_adapter_emit(msg);
  gst_message_unref(msg);
  return TRUE;
}

static gboolean
gst_player_signal_adapter_on_message(gpointer user_data)
{
  GstPlayerSignalAdapter *self = GST_PLAYER_SIGNAL_ADAPTER(user_data);
  GstMessage *next_message = gst_bus_peek(self->bus);

  GstStructure *s = gst_message_get_structure (next_message);

  g_return_val_if_fail(
    g_str_equal(
      gst_structure_get_name(s), "gst-player-message-data"), FALSE);

  GstPlayerMessage message_type;
  gst_structure_get_enum(s, "player-message", GST_TYPE_PLAYER_MESSAGE, &message_type);

  gst_player_signal_adapter_emit(message_type);

  return TRUE;
}

GST_PLAYER_API
GstPlayerSignalAdapter *  gst_player_signal_adapter_new(GstBus* bus, GMainContext* context) {

  GstPlayerSignalAdapter *self = g_object_new (GST_TYPE_PLAYER_SIGNAL_ADAPTER,
    "message-bus", bus,
    NULL);

  if (context) {
    GSource* bus_event_source = gst_bus_create_watch(bus);
    g_source_attach(bus_event_source, context);
    g_source_set_callback (bus_event_source,
      G_SOURCE_FUNC(gst_player_signal_adapter_on_message),
      self,
      NULL);

    self->bus_event_source = bus_event_source;
  }

  return self;
}

static void
gst_player_signal_adapter_init (GstPlayerSignalAdapter* object)
{
}

static void
gst_player_signal_adapter_dispose(GObject* object) {
  GstPlayerSignalAdapter *self = GST_PLAYER_SIGNAL_ADAPTER(object);

  if (self->bus_event_source) {
    g_source_destroy (self->bus_event_source);
    g_source_unref (self->bus_event_source);
  }

  gst_object_unref(self->bus);

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static void
gst_player_signal_adapter_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstPlayerSignalAdapter *self = GST_PLAYER_SIGNAL_ADAPTER(object);

  switch (prop_id) {
    case PROP_MESSAGE_BUS: {
      GstBus *bus = GST_BUS(g_value_get_object (value));
      self->bus = gst_object_ref(bus);
      break;
    }
  }
}

static void
gst_player_signal_adapter_class_init (GstPlayerSignalAdapterClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;

  gobject_class->set_property = gst_player_signal_adapter_set_property;
  gobject_class->dispose = gst_player_signal_adapter_dispose;

  param_specs[PROP_MESSAGE_BUS] =
    g_param_spec_object ("message-bus", "Message Bus",
    "Bus to consume player messages from", GST_TYPE_BUS,
    G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS);

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

#if 0

static void
eos_dispatch (gpointer user_data)
{
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

#endif
