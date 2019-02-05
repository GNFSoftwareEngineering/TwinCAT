#if !defined (IOANALOG_H)
#define IOANALOG_H

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
//            Name: IOAnalog.h
//
//     Description: TwinCAT Analog I/O Memory Mapping
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: ioanalog.h %
//        %version: 2 %
//          %state: %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#pragma pack (push, 1)

struct SAnalogInputs
{
  short m_analogInputData[50];
};

typedef SAnalogInputs const * SAnalogInputsPtr;

struct SAnalogOutputs
{
  BYTE m_analogInputCtrl[50];
  BYTE m_analogOutputCtrl[50];
  short m_analogOutputData[50];
};

typedef SAnalogOutputs * SAnalogOutputsPtr;

#pragma pack (pop)

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
//  08/11/2005  MCC     initial revision
//  10/25/2005  MCC     integrated Beckhoff TwinCAT I/O into base I/O class
//
// ============================================================================

#endif
