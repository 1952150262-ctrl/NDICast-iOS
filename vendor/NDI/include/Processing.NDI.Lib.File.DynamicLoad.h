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

#include "Processing.NDI.Lib.File.Player.h"
#include "Processing.NDI.Lib.File.Recorder.h"

typedef struct NDIlib_file_v1
{
	bool (*file_player_probe_file)(const char* p_file_path, NDIlib_file_player_file_info_t* p_out_info);
	NDIlib_file_player_instance_t (*file_player_create)(const NDIlib_file_player_create_t* p_create_settings);
	void (*file_player_destroy)(NDIlib_file_player_instance_t p_instance);
	bool (*file_player_start)(NDIlib_file_player_instance_t p_instance, const char* p_file_path);
	void (*file_player_stop)(NDIlib_file_player_instance_t p_instance);
	bool (*file_player_is_running)(NDIlib_file_player_instance_t p_instance);
	NDIlib_file_player_event_type_e (*file_player_capture_event)(NDIlib_file_player_instance_t p_instance, NDIlib_file_player_event_t* p_out_event, uint32_t timeout_in_ms);
	uint64_t (*file_player_get_dropped_event_count)(NDIlib_file_player_instance_t p_instance);
	bool (*file_player_get_stats)(NDIlib_file_player_instance_t p_instance, NDIlib_file_player_stats_t* p_out_stats);
	bool (*file_player_seek_to_time)(NDIlib_file_player_instance_t p_instance, uint64_t time_100ns);
	bool (*file_player_seek_to_frame)(NDIlib_file_player_instance_t p_instance, uint64_t frame_index);
	void (*file_player_set_paused)(NDIlib_file_player_instance_t p_instance, bool paused);
	bool (*file_player_get_paused)(NDIlib_file_player_instance_t p_instance);
	void (*file_player_set_loop)(NDIlib_file_player_instance_t p_instance, bool loop);
	bool (*file_player_get_loop)(NDIlib_file_player_instance_t p_instance);
	bool (*file_player_get_current_duration)(NDIlib_file_player_instance_t p_instance, uint64_t* p_out_duration, bool* p_out_is_final);
	bool (*file_player_get_current_position)(NDIlib_file_player_instance_t p_instance, uint64_t* p_out_position);
	bool (*file_player_get_current_source_timecode)(NDIlib_file_player_instance_t p_instance, int64_t* p_out_timecode_100ns);
	bool (*file_player_get_source_format)(NDIlib_file_player_instance_t p_instance, NDIlib_file_source_format_t* p_out_format);
	const NDIlib_source_t* (*file_player_get_advertised_source)(NDIlib_file_player_instance_t p_instance);

	NDIlib_file_recorder_instance_t (*file_recorder_create)(const NDIlib_file_recorder_create_t* p_create_settings);
	void (*file_recorder_destroy)(NDIlib_file_recorder_instance_t p_instance);
	bool (*file_recorder_start)(NDIlib_file_recorder_instance_t p_instance, const char* p_output_prefix);
	NDIlib_file_recorder_event_type_e (*file_recorder_capture_event)(NDIlib_file_recorder_instance_t p_instance, NDIlib_file_recorder_event_t* p_out_event, uint32_t timeout_in_ms);
	uint64_t (*file_recorder_get_dropped_event_count)(NDIlib_file_recorder_instance_t p_instance);
	void (*file_recorder_stop)(NDIlib_file_recorder_instance_t p_instance);
	bool (*file_recorder_rollover)(NDIlib_file_recorder_instance_t p_instance, const char* p_new_output_prefix);
	bool (*file_recorder_inject_metadata)(NDIlib_file_recorder_instance_t p_instance, const NDIlib_metadata_frame_t* p_metadata);
	bool (*file_recorder_set_source)(NDIlib_file_recorder_instance_t p_instance, const NDIlib_file_recorder_source_t* p_source);
	bool (*file_recorder_get_source)(NDIlib_file_recorder_instance_t p_instance, NDIlib_file_recorder_source_t* p_out_source);
	bool (*file_recorder_is_recording)(NDIlib_file_recorder_instance_t p_instance);
	bool (*file_recorder_get_current_file)(NDIlib_file_recorder_instance_t p_instance, const char** p_out_path);
	bool (*file_recorder_get_stats)(NDIlib_file_recorder_instance_t p_instance, NDIlib_file_recorder_stats_t* p_out_stats);
	bool (*file_recorder_get_source_format)(NDIlib_file_recorder_instance_t p_instance, NDIlib_file_source_format_t* p_out_format);
	bool (*file_recorder_start_with_timecode)(NDIlib_file_recorder_instance_t p_instance, const char* p_output_prefix, int64_t start_timecode_100ns);
} NDIlib_file_v1;

PROCESSINGNDILIBFILE_API
const NDIlib_file_v1* NDIlib_file_v1_load(void);

typedef const NDIlib_file_v1* (*NDIlib_file_v1_load_func)(void);

// The file name of the File library on each platform. The SDK does not load the library for you: an
// application that loads it at run time opens it (LoadLibrary/GetProcAddress on Windows, dlopen/dlsym
// elsewhere), looks up NDIlib_file_v1_load and calls it to get the function table. Keep the library
// loaded for as long as the table is used, because the table points into it.
#ifdef _WIN32
#ifdef _WIN64
#define NDILIB_FILE_LIBRARY_NAME "Processing.NDI.Lib.File.x64.dll"
#else
#define NDILIB_FILE_LIBRARY_NAME "Processing.NDI.Lib.File.x86.dll"
#endif
#elif defined(__APPLE__)
#define NDILIB_FILE_LIBRARY_NAME "libndifile.dylib"
#else
#define NDILIB_FILE_LIBRARY_NAME "libndifile.so.6"
#endif
