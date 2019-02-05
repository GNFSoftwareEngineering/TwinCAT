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
//            Name: TwinCAT.cpp
//
//     Description: TwinCAT DLL Application Definition
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: twincat.cpp %
//        %version: 6 %
//          %state: %
//         %cvtype: c++ %
//     %derived_by: mgustafs %
//  %date_modified: %
//
// ============================================================================

#include "StdAfx.h"
#include "TwinCAT.h"
#include "HaspAdapter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using PDCLib::CHaspAdapter;

namespace
{
  CTwinCAT g_twinCAT;
}

BEGIN_MESSAGE_MAP (CTwinCAT, CWinApp)
END_MESSAGE_MAP ()

// Making changes

CTwinCAT::CTwinCAT (void)
{
}

CTwinCAT::~CTwinCAT ()
{
}

BOOL
CTwinCAT::InitInstance (void)
{
  if (CWinApp::InitInstance ())
    {
      try
        {
          PDCLib::TESTHR (PDCLib::CoInitialize ());

          if (CHaspAdapter::HasFeature (CHaspAdapter::EHaspFeature::haspBeckhoff))
            {
              return TRUE;
            }
          else
            {
              PDCLib::MessageBoxStop (_T ("HASP feature not found."));
            }
        }
      catch (_com_error const & e)
        {
          PDCLib::MessageBoxStop (_T ("Unable to initialize application: %s"), (LPCTSTR) PDCLib::GetErrorMessage (e));
        }
      catch (CString const & errorMessage)
        {
          PDCLib::MessageBoxStop (_T ("Unable to initialize application: %s"), (LPCTSTR) errorMessage);
        }
    }
  else
    {
      PDCLib::MessageBoxStop (_T ("Unable to initialize application instance."));
    }

  return FALSE;
}

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
//  09/19/2013  MCC     added explicit HASP feature check
//  09/05/2014  MEG     fixed InitInstance to handle CoInitialize behavior
//  09/09/2014  MCC     corrected problem with CoInitialize workaround
//  01/22/2015  MCC     added stacker feature
//  04/13/2017  MEG     added cast to CString for resolving conversion warning
//
// ============================================================================