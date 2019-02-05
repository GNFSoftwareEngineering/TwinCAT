#if !defined (STDAFX_H)
#define STDAFX_H

// ============================================================================
//
//                              CONFIDENTIAL
//
//        GENOMICS INSTITUTE OF THE NOVARTIS RESEARCH FOUNDATION (GNF)
//
//  This is an unpublished work of authorship, which contains trade secrets,
//  created in 2001.  GNF owns all rights to this work and intends to maintain
//  it in confidence to preserve its trade secret status.  GNF reserves the
//  right, under the copyright laws of the United States or those of any other
//  country that may have jurisdiction, to protect this work as an unpublished
//  work, in the event of an inadvertent or deliberate unauthorized publication.
//  GNF also reserves its rights under all copyright laws to protect this work
//  as a published work, when appropriate.  Those having access to this work
//  may not copy it, use it, modify it or disclose the information contained
//  in it without the written authorization of GNF.
//
// ============================================================================

// ============================================================================
//
//            Name: StdAfx.h
//
//     Description: MFC Pre-Compiled Header Support
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 11 %
//           %name: stdafx.h %
//        %version: 8 %
//          %state: %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#include "TargetVer.h"

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

#define _AFX_ALL_WARNINGS

#include <afxwin.h>            // MFC base windows support
#include <afxmt.h>             // MFC multithreading suport

#include <shlobj.h>            // Windows Shell API Support

#include <afxeditbrowsectrl.h> // MFC edit browse control support

#if defined (min)
#undef min                     // undefine macro version of min function
#endif

#if defined (max)
#undef max                     // undefine macro version of max function
#endif

#include <comdef.h>            // Native C++ compiler COM support - main definitions header
#include <msxml6.h>            // XML serialization support

#include <algorithm>           // STL algorithms (for min and max template functions)
#include <array>               // STL array support
#include <atomic>              // STL atomic support
#include <limits>              // STL limits (for numeric_limits)
#include <map>                 // STL map container class support
#include <memory>              // STL memory management
#include <mutex>               // STL mutex support
#include <set>                 // STL set container class support
#include <vector>              // STL vector container class support

#include <strsafe.h>           // Safer C library string routine replacements

#ifndef _SDL_BANNED_RECOMMENDED
#define _SDL_BANNED_RECOMMENDED
#endif

#include "Banned.h"            // Microsoft SDL banned APIs
#include "Bit.h"               // Bit Twiddling Function support
#include "Utility.h"           // Common utility support

// ============================================================================
//  R E V I S I O N    N O T E S
// ============================================================================
//
//  For each change to this file, record the following:
//
//   1. who made the change and when the change was made
//   2. why the change was made and the intended result
//
// ============================================================================
//
//  Date        Author  Description
// ----------------------------------------------------------------------------
//  09/24/2008  MCC     initial revision
//  12/01/2009  TAN     baselined Windows target version header file
//  06/09/2010  MCC     implemented support for PWM output
//  06/21/2010  MCC     added Microsoft SDL include files
//  09/19/2013  MCC     added explicit HASP feature check
//  12/17/2014  MCC     implemented critical sections with C++11 concurrency
//  01/09/2015  MCC     templatized number of elements macro
//  08/22/2018  MCC     corrected problem with TwinCAT ADS variable names
//
// ============================================================================

#endif