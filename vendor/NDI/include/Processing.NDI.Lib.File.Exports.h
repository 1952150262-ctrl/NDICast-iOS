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
#include <cstddef>
#endif

#ifdef PROCESSINGNDILIBFILE_STATIC
#ifdef __cplusplus
#define PROCESSINGNDILIBFILE_API extern "C"
#else // __cplusplus
#define PROCESSINGNDILIBFILE_API
#endif // __cplusplus
#else  // PROCESSINGNDILIBFILE_STATIC
#ifdef _WIN32
#ifdef PROCESSINGNDILIBFILE_EXPORTS
#ifdef __cplusplus
#define PROCESSINGNDILIBFILE_API extern "C" __declspec(dllexport)
#else // __cplusplus
#define PROCESSINGNDILIBFILE_API __declspec(dllexport)
#endif // __cplusplus
#else  // PROCESSINGNDILIBFILE_EXPORTS
#ifdef __cplusplus
#define PROCESSINGNDILIBFILE_API extern "C" __declspec(dllimport)
#else // __cplusplus
#define PROCESSINGNDILIBFILE_API __declspec(dllimport)
#endif // __cplusplus
#endif // PROCESSINGNDILIBFILE_EXPORTS
#else  // _WIN32
#ifdef __cplusplus
#define PROCESSINGNDILIBFILE_API extern "C" __attribute((visibility("default")))
#else // __cplusplus
#define PROCESSINGNDILIBFILE_API __attribute((visibility("default")))
#endif // __cplusplus
#endif // _WIN32
#endif // PROCESSINGNDILIBFILE_STATIC

#ifndef NDILIB_CPP_DEFAULT_CONSTRUCTORS
#ifdef __cplusplus
#define NDILIB_CPP_DEFAULT_CONSTRUCTORS 1
#else // __cplusplus
#define NDILIB_CPP_DEFAULT_CONSTRUCTORS 0
#endif // __cplusplus
#endif // NDILIB_CPP_DEFAULT_CONSTRUCTORS

#ifndef NDILIB_CPP_DEFAULT_VALUE
#ifdef __cplusplus
#define NDILIB_CPP_DEFAULT_VALUE(a) = (a)
#else // __cplusplus
#define NDILIB_CPP_DEFAULT_VALUE(a)
#endif // __cplusplus
#endif // NDILIB_CPP_DEFAULT_VALUE
