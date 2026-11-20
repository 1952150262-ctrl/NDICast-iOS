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

// Shared video frame format values mirroring the sender format shape.
typedef enum NDIlib_file_video_frame_format_e
{
	NDIlib_file_video_frame_format_interleaved = 0,
	NDIlib_file_video_frame_format_progressive = 1,
	NDIlib_file_video_frame_format_field_0 = 2,
	NDIlib_file_video_frame_format_field_1 = 3,
	NDIlib_file_video_frame_format_max = 0x7fffffff
} NDIlib_file_video_frame_format_e;

typedef struct NDIlib_file_video_track_format_t
{
	bool is_high_quality;
	int32_t xres;
	int32_t yres;
	int32_t frame_rate_n;
	int32_t frame_rate_d;
	uint32_t fourcc;

	// Scan type from the container (recorded files store it in the track header). Live recorder
	// events report the source's declared format. For a probe of a file that predates scan-type
	// storage the field keeps its zero-initialized value.
	NDIlib_file_video_frame_format_e frame_format_type;

	// Display aspect ratio of the full frame (e.g. 16/9 = 1.7778f). 0 = unknown: assume square
	// pixels (xres/yres). Populated from the container for probes and from the source for live
	// recorder events.
	float picture_aspect_ratio;

#if NDILIB_CPP_DEFAULT_CONSTRUCTORS
	NDIlib_file_video_track_format_t(void);
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS
} NDIlib_file_video_track_format_t;

typedef struct NDIlib_file_source_format_t
{
	// Number of populated entries in video_tracks. Slot 0 is the primary full-bandwidth stream.
	// Slot 1 is the proxy track when available; unknown tracks remain zero-initialized.
	uint32_t video_track_count;
	NDIlib_file_video_track_format_t video_tracks[2];
	int32_t audio_sample_rate;
	int32_t audio_no_channels;
	int32_t audio_no_samples;
	uint32_t audio_bit_depth;
	uint32_t audio_fourcc;

#if NDILIB_CPP_DEFAULT_CONSTRUCTORS
	NDIlib_file_source_format_t(void);
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS
} NDIlib_file_source_format_t;

#if NDILIB_CPP_DEFAULT_CONSTRUCTORS
inline NDIlib_file_video_track_format_t::NDIlib_file_video_track_format_t(void)
	: is_high_quality(false)
	, xres(0)
	, yres(0)
	, frame_rate_n(0)
	, frame_rate_d(0)
	, fourcc(0)
	, frame_format_type(NDIlib_file_video_frame_format_interleaved)
	, picture_aspect_ratio(0.0f)
{
}

inline NDIlib_file_source_format_t::NDIlib_file_source_format_t(void)
	: video_track_count(0)
	, audio_sample_rate(0)
	, audio_no_channels(0)
	, audio_no_samples(0)
	, audio_bit_depth(0)
	, audio_fourcc(0)
{
}
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS
