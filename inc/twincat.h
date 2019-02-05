#if !defined (BECKHOFFAPP_H)
#define BECKHOFFAPP_H

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
//            Name: TwinCAT.h
//
//     Description: TwinCAT DLL Application Declaration
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: twincat.h %
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

#ifndef __AFXWIN_H__
  #error include 'stdafx.h' before including this file for PCH
#endif

class CTwinCAT : public CWinApp
{

public:
  explicit CTwinCAT (void);
  virtual ~CTwinCAT ();

  virtual BOOL InitInstance (void);

  DECLARE_MESSAGE_MAP ()

private:

  // copy construction and assignment not allowed for this class

  CTwinCAT (const CTwinCAT &);
  CTwinCAT & operator = (const CTwinCAT &);
};

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
//
// ============================================================================

#endif