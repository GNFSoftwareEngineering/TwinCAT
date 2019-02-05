#if !defined (ADSAPI_H)
#define ADSAPI_H

// ============================================================================
//
//                              CONFIDENTIAL
//
//        GENOMICS INSTITUTE OF THE NOVARTIS RESEARCH FOUNDATION (GNF)
//
//  This is an unpublished work of authorship, which contains trade secrets,
//  created in 2002.  GNF owns all rights to this work and intends to maintain
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
//            Name: AdsApi.h
//
//     Description: TwinCAT ADS API declaration
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: adsapi.h %
//        %version: 3 %
//          %state: %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#include "AdsDef.h"

#if _MSC_VER > 1000
#pragma once
#endif

// TwinCAT 2 ADS Interface (legacy support for backwards compatibility)

using PAdsNotificationFuncTC2                = void (*) (AmsAddr                 * pAddr,
													                               AdsNotificationHeader   * pNotification,
													                               unsigned long             hUser);

using ProcAdsGetDllVersionTC2                = long (*) (void);
using ProcAdsPortOpenTC2                     = long (*) (void);
using ProcAdsPortCloseTC2                    = long (*) (void);
using ProcAdsGetLocalAddressTC2              = long (*) (AmsAddr                 * pAddr);
using ProcAdsSyncWriteReqTC2                 = long (*) (AmsAddr                 * pServerAddr,
                                                         unsigned long             indexGroup,
                                                         unsigned long             indexOffset,
                                                         unsigned long             length,
                                                         void                    * pData);
using ProcAdsSyncReadWriteReqTC2             = long (*) (AmsAddr                 * pAddr,
                                                         unsigned long             indexGroup,
                                                         unsigned long             indexOffset,
                                                         unsigned long             cbReadLength,
                                                         void                    * pReadData,
                                                         unsigned long             cbWriteLength,
                                                         void                    * pWriteData);
using ProcAdsSyncAddDeviceNotificationReqTC2 = long (*) (AmsAddr                 * pAddr,
                                                         unsigned long             indexGroup,
                                                         unsigned long             indexOffset,
                                                         AdsNotificationAttrib   * pNoteAttrib,
                                                         PAdsNotificationFuncTC2   pNoteFunc,
                                                         unsigned long             hUser,
                                                         unsigned long           * pNotification);
using ProcAdsSyncDelDeviceNotificationReqTC2 = long (*) (AmsAddr                 * pAddr,
                                                         unsigned long             hNotification);

// TwinCAT 3 ADS Interface

using PAdsNotificationFuncTC3                = PAdsNotificationFuncEx;

using ProcAdsGetDllVersionTC3                = long (__stdcall *) (void);
using ProcAdsPortOpenTC3                     = long (__stdcall *) (void);
using ProcAdsPortCloseTC3                    = long (__stdcall *) (void);
using ProcAdsGetLocalAddressTC3              = long (__stdcall *) (AmsAddr                 * pAddr);
using ProcAdsSyncWriteReqTC3                 = long (__stdcall *) (AmsAddr                 * pServerAddr,
                                                                   unsigned long             indexGroup,
                                                                   unsigned long             indexOffset,
                                                                   unsigned long             length,
                                                                   void                    * pData);
using ProcAdsSyncReadWriteReqTC3             = long (__stdcall *) (AmsAddr                 * pAddr,
                                                                   unsigned long             indexGroup,
                                                                   unsigned long             indexOffset,
                                                                   unsigned long             cbReadLength,
                                                                   void                    * pReadData,
                                                                   unsigned long             cbWriteLength,
                                                                   void                    * pWriteData);
using ProcAdsSyncAddDeviceNotificationReqTC3 = long (__stdcall *) (AmsAddr                 * pAddr,
                                                                   unsigned long             indexGroup,
                                                                   unsigned long             indexOffset,
                                                                   AdsNotificationAttrib   * pNoteAttrib,
                                                                   PAdsNotificationFuncTC3   pNoteFunc,
                                                                   unsigned long             hUser,
                                                                   unsigned long           * pNotification);
using ProcAdsSyncDelDeviceNotificationReqTC3 = long (__stdcall *) (AmsAddr                 * pAddr,
                                                                   unsigned long             hNotification);

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
//  10/31/2005  MCC     initial revision
//  01/09/2015  MCC     templatized number of elements macro
//  06/06/2018  MCC     implemented support for TwinCAT 3 ADS interface
//
// ============================================================================

#endif
