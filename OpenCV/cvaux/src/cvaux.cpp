/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                        Intel License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000, Intel Corporation, all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of Intel Corporation may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/

#include "_cvaux.h"

#if 0
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#else
#include <stdio.h>
#endif

#undef __IPCV_H_

#define __CV_DEFINE_USER_LIST__

#undef IPCVAPI

#ifdef WIN32
#define IPCVAPI(type, name, arg)                                                                \
    static void hasnt_been_loaded_##name()                                                      \
    {                                                                                           \
        MessageBox( 0, "Function " #name " hasn't been loaded", "Error", MB_OK | MB_ICONERROR );\
        exit(0);                                                                                \
    }                                                                                           \
    name##_t name = (name##_t)hasnt_been_loaded_##name;

#else

#define IPCVAPI(type, name, arg)                                   \
    static void hasnt_been_loaded_##name()                         \
    {                                                              \
        printf( "Error: Function " #name " hasn't been loaded\n" );\
        exit(0);                                                   \
    }                                                              \
    name##_t name = (name##_t)hasnt_been_loaded_##name;
#endif

#undef __IPCV_H_
#include "_ipcv.h"

#undef IPCVAPI
#define GETAPI(addr,name) { (ipp_func_addr*)&addr, #name, 0 },
#define IPCVAPI(type,name,arg) GETAPI( name, name )

typedef void (CV_CDECL * ipp_func_addr) (void);

struct func_desc
{
    ipp_func_addr *addr;
    const char *name;
    int loaded;
};

static func_desc ipp_func_desc[] =
{
#undef __IPCV_H_
#include "_ipcv.h"
#undef __IPCV_H_
    {
    0, 0}
};

static int loaded = cvFillInternalFuncsTable( (void*)ipp_func_desc );

#undef IPCVAPI
#endif
