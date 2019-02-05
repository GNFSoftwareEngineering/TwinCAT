#if !defined (DRIVESTATUS_H)
#define DRIVESTATUS_H

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
//            Name: DriveStatus.h
//
//     Description: Drive Status declaration
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: drivestatus.h %
//        %version: 1 %
//          %state: %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#if _MSC_VER > 1000
#pragma once
#endif

// General Drive Status Values

const int DRIVE_STATUS_UNINITIALIZED         (0);  // Drive initialization is incomplete
const int DRIVE_STATUS_INITIALIZING          (1);  // Drive initialization is running
const int DRIVE_STATUS_OK                    (2);  // Drive initialization is complete, drives OK
const int DRIVE_STATUS_AXIS1_FAULTED         (3);  // Axis 1 drive has faulted
const int DRIVE_STATUS_AXIS2_FAULTED         (4);  // Axis 2 drive has faulted
const int DRIVE_STATUS_AXIS3_FAULTED         (5);  // Axis 3 drive has faulted
const int DRIVE_STATUS_AXIS4_FAULTED         (6);  // Axis 4 drive has faulted
const int DRIVE_STATUS_AXIS5_FAULTED         (7);  // Axis 5 drive has faulted
const int DRIVE_STATUS_AXIS6_FAULTED         (8);  // Axis 6 drive has faulted
const int DRIVE_STATUS_AXIS7_FAULTED         (9);  // Axis 7 drive has faulted
const int DRIVE_STATUS_AXIS8_FAULTED         (10); // Axis 8 drive has faulted
const int DRIVE_STATUS_ESTOP_FAULTED         (11); // E-Stop occurred; faulted
const int DRIVE_STATUS_AIR_PRESSURE_FAULTED  (12); // No air pressure; faulted
const int DRIVE_STATUS_VACUUM_FAULTED        (13); // No vacuum; faulted
const int DRIVE_STATUS_CONTROL_POWER_FAULTED (14); // No control power; faulted

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
//  02/16/2007  MCC     initial revision
//
// ============================================================================

#endif
