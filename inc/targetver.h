#if !defined (TARGETVER_H)
#define TARGETVER_H

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
//            Name: TargetVer.h
//
//     Description: Windows Target Version definitions
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: targetver.h %
//        %version: 3 %
//          %state: %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef WINVER
#undef WINVER
#endif

#define WINVER         0x0601 // Windows 7

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT   0x0601 // Windows 7

#ifdef _WIN32_WINDOWS
#undef _WIN32_WINDOWS
#endif

#define _WIN32_WINDOWS 0x0601 // Windows 7

#ifdef _WIN32_IE
#undef _WIN32_IE
#endif

#define _WIN32_IE      0x0800

#pragma comment (linker, \
                 "\"/manifestdependency:type='win32' " \
                 "name='Microsoft.Windows.Common-Controls' " \
                 "version='6.0.0.0' " \
                 "processorArchitecture='*' " \
                 "publicKeyToken='6595b64144ccf1df' " \
                 "language='*'\"")

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
//  11/23/2009  MCC     initial revision
//  07/29/2010  MCC     implemented support for sorting plate definitions
//  04/07/2014  MCC     retargeted for Windows 7
//
// ============================================================================

#endif
