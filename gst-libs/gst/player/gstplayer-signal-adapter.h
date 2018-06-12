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

#ifndef __GST_PLAYER_SIGNAL_ADAPTER_H__
#define __GST_PLAYER_SIGNAL_ADAPTER_H__

#include <gst/gst.h>
#include <gst/player/gstplayer-types.h>

G_BEGIN_DECLS

#define GST_TYPE_PLAYER_SIGNAL_ADAPTER             (gst_player_signal_adapter_get_type ())
#define GST_IS_PLAYER_SIGNAL_ADAPTER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_PLAYER_SIGNAL_ADAPTER))
#define GST_IS_PLAYER_SIGNAL_ADAPTER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_PLAYER_SIGNAL_ADAPTER))
#define GST_PLAYER_SIGNAL_ADAPTER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_PLAYER_SIGNAL_ADAPTER, GstPlayerSignalAdapterClass))
#define GST_PLAYER_SIGNAL_ADAPTER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_PLAYER_SIGNAL_ADAPTER, GstPlayerSignalAdapter))
#define GST_PLAYER_SIGNAL_ADAPTER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_PLAYER_SIGNAL_ADAPTER, GstPlayerSignalAdapterClass))
#define GST_PLAYER_SIGNAL_ADAPTER_CAST(obj)        ((GstPlayerSignalAdapter*)(obj))

GST_PLAYER_API
GType        gst_player_signal_adapter_get_type                      (void);

GST_PLAYER_API
GstPlayerSignalAdapter *  gst_player_signal_adapter_new                     (GstPlayer* player);

G_END_DECLS

#endif /* __GST_PLAYER_SIGNAL_ADAPTER_H__ */