#pragma once

// NOTE : The following MIT license applies to this file ONLY and not to the SDK as a whole. Please review
// the SDK documentation for the description of the full license terms, which are also provided in the file
// "NDI License Agreement.pdf" within the SDK or online at http://ndi.link/ndisdk_license. Your use of any
// part of this SDK is acknowledgment that you agree to the SDK license terms. The full NDI SDK may be
// downloaded at http://ndi.video/
//
//***********************************************************************************************************
//
// Copyright (C) 2023-2026 Vizrt NDI AB. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files(the "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
// following conditions :
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
// EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//***********************************************************************************************************

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdbool.h>
#include <stdint.h>
#endif

#include "Processing.NDI.Lib.File.Exports.h"
#include "Processing.NDI.Lib.File.Types.h"
#if NDILIB_CPP_DEFAULT_CONSTRUCTORS
#include "Processing.NDI.Lib.File.EventString.h"
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS

// Structures and type definitions required by file playback.
// Forward declaration for the NDI source type used by the player.
// For the full definition, include Processing.NDI.Lib.h from the NDI SDK.
typedef struct NDIlib_source_t NDIlib_source_t;

// The reference to an instance of the file player.
struct NDIlib_file_player_instance_type;
typedef struct NDIlib_file_player_instance_type* NDIlib_file_player_instance_t;

enum
{
	NDIlib_file_player_event_path_max_size = 4096,
	NDIlib_file_player_event_message_max_size = 1024
};

// Events that can be polled from the player.
typedef enum NDIlib_file_player_event_type_e
{
	// No event was available before the timeout elapsed.
	NDIlib_file_player_event_type_none = 0,

	// Playback reached natural EOF.
	NDIlib_file_player_event_type_ended = 1,

	// Playback stopped because of an unrecoverable runtime error.
	NDIlib_file_player_event_type_error = 2,

	// A file was successfully opened for playback.
	NDIlib_file_player_event_type_file_opened = 3,

	// The first frame on screen was sent: the first main video frame, or the first audio packet of a file without
	// video. Fires at the start of each file and again after every seek and loop. When the file's main video cannot
	// be sent, the first audio or video packet sent fires it instead; when no main video frame is sent within 1 s of
	// file time (an empty video track, or a seek into an audio-only tail), the packet sent then fires it.
	NDIlib_file_player_event_type_first_frame = 4,

	// The current file playback session was closed by stop() or by starting another file.
	NDIlib_file_player_event_type_file_closed = 5,

	// Playback automatically looped back to the beginning of the file.
	NDIlib_file_player_event_type_looped = 6,

	NDIlib_file_player_event_type_max = 0x7fffffff
} NDIlib_file_player_event_type_e;

// Information returned by NDIlib_file_player_capture_event.
typedef struct NDIlib_file_player_event_t
{
	// The event that occurred.
	NDIlib_file_player_event_type_e type;

	// Path associated with file_opened/file_closed events. This buffer is owned by the event struct
	// itself and remains valid after later capture calls. Empty string when no path applies.
	char p_path[NDIlib_file_player_event_path_max_size];

	// Optional diagnostic text for error events. This buffer is owned by the event struct itself and
	// remains valid after later capture calls. Empty string when no message applies.
	char p_message[NDIlib_file_player_event_message_max_size];

	// The PTS of the packet that fired a first_frame event (see NDIlib_file_player_event_type_first_frame); otherwise 0.
	uint64_t first_pts_100ns;

	// The last sent audio/video packet PTS for file_closed/looped events; otherwise 0.
	uint64_t last_pts_100ns;

	// Original NDI source timecode from the per-frame sidecar for first_frame/file_closed/looped
	// events when known. This is distinct from MKV PTS and may be editorial/non-monotonic.
	bool has_source_timecode;
	int64_t source_timecode_100ns;

	// Structured source format for file_opened/first_frame/looped events when known.
	// Zero-initialized when no source format applies.
	NDIlib_file_source_format_t source_format;

#if NDILIB_CPP_DEFAULT_CONSTRUCTORS
	NDIlib_file_player_event_t(
		NDIlib_file_player_event_type_e type_ = NDIlib_file_player_event_type_none,
		const char* p_path_ = NULL,
		const char* p_message_ = NULL,
		uint64_t first_pts_100ns_ = 0,
		uint64_t last_pts_100ns_ = 0,
		const NDIlib_file_source_format_t* p_source_format_ = NULL,
		bool has_source_timecode_ = false,
		int64_t source_timecode_100ns_ = 0);
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS
} NDIlib_file_player_event_t;

// Metadata discovered by NDIlib_file_player_probe_file without starting playback.
// source_format carries the unified track layout (including future proxy support).
typedef struct NDIlib_file_player_file_info_t
{
	uint64_t duration_100ns;
	bool duration_is_final;

	uint32_t track_count;
	uint32_t video_track_count;
	uint32_t audio_track_count;
	uint32_t metadata_track_count;

	NDIlib_file_source_format_t source_format;

#if NDILIB_CPP_DEFAULT_CONSTRUCTORS
	NDIlib_file_player_file_info_t(void);
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS
} NDIlib_file_player_file_info_t;

typedef struct NDIlib_file_player_stats_t
{
	uint64_t video_frames_sent[2];
	uint64_t audio_frames_sent;
	uint64_t metadata_frames_sent;
	uint64_t playback_wallclock_100ns;

#if NDILIB_CPP_DEFAULT_CONSTRUCTORS
	NDIlib_file_player_stats_t(void);
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS
} NDIlib_file_player_stats_t;

// The creation structure that is used when you are creating a file player.
typedef struct NDIlib_file_player_create_t
{
	// UTF-8 NDI source name advertised by this player for the whole instance lifetime.
	const char* p_ndi_send_name;

	// Optional comma-separated NDI groups advertised by this player for the whole instance lifetime.
	const char* p_groups;

#if NDILIB_CPP_DEFAULT_CONSTRUCTORS
	NDIlib_file_player_create_t(const char* p_ndi_send_name_ = NULL, const char* p_groups_ = NULL);
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS
} NDIlib_file_player_create_t;

//**************************************************************************************************************************
// Probe an MKV file and return stream metadata without starting playback or creating an NDI sender.
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_probe_file(const char* p_file_path, NDIlib_file_player_file_info_t* p_out_info);

// Create a new file player instance and advertise its NDI sender. p_create_settings must not be NULL,
// and p_ndi_send_name must be a non-empty UTF-8 string. The source name and groups are fixed for the
// lifetime of the instance; use start()/stop() only to change the file being sent.
PROCESSINGNDILIBFILE_API
NDIlib_file_player_instance_t NDIlib_file_player_create(const NDIlib_file_player_create_t* p_create_settings);

// This will destroy an existing file player instance.
PROCESSINGNDILIBFILE_API
void NDIlib_file_player_destroy(NDIlib_file_player_instance_t p_instance);

// Open an MKV file and start sending it through the instance's long-lived NDI sender. p_file_path must
// be a non-empty UTF-8 string. If another file is already playing, it is stopped first and a file_closed
// event is queued before the new file is opened. The new file always starts playing (not paused) and
// uses the instance's looping setting.
// For GOP-compressed (H.264/HEVC) files, the instance's FIRST start may briefly wait (up to ~0.5 s)
// for a just-connected receiver to finish registering with the sender, so that receiver gets the
// file's opening keyframe instead of showing nothing until the next GOP. With no receivers connected
// there is no wait, and later start() calls on the same instance (e.g. playlist file changes) never wait.
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_start(NDIlib_file_player_instance_t p_instance, const char* p_file_path);

// Stop playback and release only the current file resources. The advertised NDI sender remains alive
// until destroy() so receivers stay connected and see no signal between files.
PROCESSINGNDILIBFILE_API
void NDIlib_file_player_stop(NDIlib_file_player_instance_t p_instance);

// This returns TRUE while playback is active. It becomes FALSE after stop or when playback reaches EOF.
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_is_running(NDIlib_file_player_instance_t p_instance);

// Capture the next queued player event, waiting up to timeout_in_ms. Returns the captured event type,
// or _none if no event is available before the timeout elapses.
// Note: New event types may be added in future versions. Applications must ignore any event type they do
// not recognize.
PROCESSINGNDILIBFILE_API
NDIlib_file_player_event_type_e NDIlib_file_player_capture_event(NDIlib_file_player_instance_t p_instance, NDIlib_file_player_event_t* p_out_event, uint32_t timeout_in_ms);

// Total number of player events dropped because the caller did not capture events quickly enough.
PROCESSINGNDILIBFILE_API
uint64_t NDIlib_file_player_get_dropped_event_count(NDIlib_file_player_instance_t p_instance);

// Get playback telemetry for the current or most recent file session.
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_get_stats(NDIlib_file_player_instance_t p_instance, NDIlib_file_player_stats_t* p_out_stats);

// Seek to the specified file-relative time. Where playback resumes depends on the video essence (see below).
// Time is expressed in 100ns units where 0 = the first playable frame in the file timeline.
//
// Seek granularity depends on the video essence. For GOP-compressed video (H.264/HEVC),
// playback resumes AT the keyframe at or before the requested time (keyframe-snapped, like
// the default seek of common media players): compressed passthrough cannot preroll to the
// exact frame without bursting undisplayable frames at every connected receiver. For
// intra-only video (SpeedHQ, uncompressed), every frame is a safe entry point: playback resumes
// AT the first frame at or after the requested time (a time between two frames lands on the later one).
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_seek_to_time(NDIlib_file_player_instance_t p_instance, uint64_t time_100ns);

// Seek using a frame index. This currently converts the requested frame index through the current
// video frame rate into a file-relative 100ns time before performing the seek, so it is approximate
// for non-integer frame rates and should not be treated as an exact frame-addressing API.
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_seek_to_frame(NDIlib_file_player_instance_t p_instance, uint64_t frame_index);

// Pause or resume playback of the current file. While paused, the playback thread remains alive but
// does not send frames. Pause applies to the current file only: it has no effect when no file is
// open, and NDIlib_file_player_start always begins playing, resetting pause to FALSE. After the file
// ends, pause can still be set and read until stop() or the next start().
PROCESSINGNDILIBFILE_API
void NDIlib_file_player_set_paused(NDIlib_file_player_instance_t p_instance, bool paused);

// Get whether playback of the current file is paused. Returns FALSE when no file is open (before the
// first start() or after stop()).
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_get_paused(NDIlib_file_player_instance_t p_instance);

// Enable or disable automatic looping when playback reaches natural EOF. Looping is a setting of the
// instance, not of the current file: it may be set before NDIlib_file_player_start and is kept across
// start() and stop() calls, applying to every file the instance plays until it is changed.
// A caller that advances a playlist on the ended event should set looping to FALSE, preferably before start().
PROCESSINGNDILIBFILE_API
void NDIlib_file_player_set_loop(NDIlib_file_player_instance_t p_instance, bool loop);

// Get the looping setting of the instance (FALSE until enabled with NDIlib_file_player_set_loop).
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_get_loop(NDIlib_file_player_instance_t p_instance);

// Get the current best-known file-relative duration. p_out_is_final is TRUE when the duration is
// finalized and will no longer change. Duration is expressed in 100ns units where 0 = the first
// playable frame in the file timeline. While a seek scans the file for its target (the first seek
// on a file, or a seek past what was read so far), this returns at once with the duration known
// when the scan began: on a file still being recorded it can lag the end of the file until then.
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_get_current_duration(NDIlib_file_player_instance_t p_instance, uint64_t* p_out_duration, bool* p_out_is_final);

// Get the file-relative playback position of the frame on screen, in 100ns units: the main (first) video
// track's frame sent last (the audio packet sent last for a file without video). Audio, metadata, the proxy
// video and frames read but not sent yet never move it. Until such a frame was sent, the position of the
// packet read last. Position is expressed where 0 = the first playable frame in the file timeline.
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_get_current_position(NDIlib_file_player_instance_t p_instance, uint64_t* p_out_position);

// Get the original NDI source timecode of the frame on screen (the same frame as
// NDIlib_file_player_get_current_position), restored from its per-frame sidecar. When that frame carries
// none, or none was sent yet, the timecode of the packet of any kind sent last. This is distinct from MKV
// PTS and may be editorial/non-monotonic. Returns FALSE until playback has sent a packet with a preserved
// source timecode.
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_get_current_source_timecode(NDIlib_file_player_instance_t p_instance, int64_t* p_out_timecode_100ns);

// Get the current detected audio/video source format for the active file. Returns FALSE until a file with
// at least one audio or video track has been opened. Callers that need frame rate should read
// `video_tracks[0].frame_rate_n` / `frame_rate_d` from the returned source format.
PROCESSINGNDILIBFILE_API
bool NDIlib_file_player_get_source_format(NDIlib_file_player_instance_t p_instance, NDIlib_file_source_format_t* p_out_format);

// Get the NDI source advertised by this player. The returned pointer and its strings are owned by the
// player and remain valid until NDIlib_file_player_destroy is called.
PROCESSINGNDILIBFILE_API
const NDIlib_source_t* NDIlib_file_player_get_advertised_source(NDIlib_file_player_instance_t p_instance);

#if NDILIB_CPP_DEFAULT_CONSTRUCTORS
inline NDIlib_file_player_event_t::NDIlib_file_player_event_t(
	const NDIlib_file_player_event_type_e type_,
	const char* const p_path_,
	const char* const p_message_,
	const uint64_t first_pts_100ns_,
	const uint64_t last_pts_100ns_,
	const NDIlib_file_source_format_t* const p_source_format_,
	const bool has_source_timecode_,
	const int64_t source_timecode_100ns_)
	: type(type_)
	, first_pts_100ns(first_pts_100ns_)
	, last_pts_100ns(last_pts_100ns_)
	, has_source_timecode(has_source_timecode_)
	, source_timecode_100ns(source_timecode_100ns_)
	, source_format()
{
	NDIlib_file_copy_event_string(p_path, sizeof(p_path), p_path_);
	NDIlib_file_copy_event_string(p_message, sizeof(p_message), p_message_);
	if (p_source_format_ != NULL)
	{
		source_format = *p_source_format_;
	}
}

inline NDIlib_file_player_file_info_t::NDIlib_file_player_file_info_t(void)
	: duration_100ns(0)
	, duration_is_final(false)
	, track_count(0)
	, video_track_count(0)
	, audio_track_count(0)
	, metadata_track_count(0)
	, source_format()
{
}

inline NDIlib_file_player_stats_t::NDIlib_file_player_stats_t(void)
	: video_frames_sent()
	, audio_frames_sent(0)
	, metadata_frames_sent(0)
	, playback_wallclock_100ns(0)
{
}

inline NDIlib_file_player_create_t::NDIlib_file_player_create_t(const char* const p_ndi_send_name_, const char* const p_groups_)
	: p_ndi_send_name(p_ndi_send_name_)
	, p_groups(p_groups_)
{
}
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS
