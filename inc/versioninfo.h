#if !defined (VERSIONINFO_H)
#define VERSIONINFO_H

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
//            Name: VersionInfo.h
//
//     Description: Version Information class declaration
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: versioninfo.h %
//        %version: 4 %
//          %state: working %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: Monday, October 07, 2002 1:00:00 PM %
//
// ============================================================================

#if _MSC_VER > 1000
#pragma once
#endif

namespace PDCLib
{
#if defined (_PDCLIB_EXPORT)
class __declspec (dllexport) CVersionInfo final
#else
class __declspec (dllimport) CVersionInfo final
#endif
{
public:
  explicit CVersionInfo (void);
  virtual ~CVersionInfo (void);

  bool Create (CString const & fileName);
  bool Create (HMODULE hModule);

  bool GetCompanyName (CString & companyName, WORD codePage = CP_UNICODE) const;
  bool GetFileDescription (CString & fileDescription, WORD codePage = CP_UNICODE) const;
  bool GetFileVersion (CString & fileVersion, WORD codePage = CP_UNICODE) const;
  bool GetInternalName (CString & internalName, WORD codePage = CP_UNICODE) const;
  bool GetLegalCopyright (CString & legalCopyright, WORD codePage = CP_UNICODE) const;
  bool GetOriginalFilename (CString & originalFilename, WORD codePage = CP_UNICODE) const;
  bool GetProductName (CString & productName, WORD codePage = CP_UNICODE) const;
  bool GetProductVersion (CString & productVersion, WORD codePage = CP_UNICODE) const;
  bool GetComments (CString & comments, WORD codePage = CP_UNICODE) const;
  bool GetLegalTrademarks (CString & legalTrademarks, WORD codePage = CP_UNICODE) const;
  bool GetPrivateBuild (CString & privateBuild, WORD codePage = CP_UNICODE) const;
  bool GetSpecialBuild (CString & specialBuild, WORD codePage = CP_UNICODE) const;

  bool GetFileVersion (std::vector <DWORD> & fileVersion, WORD codePage = CP_UNICODE) const;

  static WORD const CP_ANSI;
  static WORD const CP_UNICODE;

private:
  std::vector <BYTE> m_versionInfo;

  bool Attach (CString const & fileName);
  bool FindResource (CString & resourceValue, WORD codePage, CString const & resourceName) const;

public:
  CVersionInfo (CVersionInfo const &) = delete;
  CVersionInfo & operator = (CVersionInfo const &) = delete;
};
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
//  08/06/2002  MCC     initial revision
//  07/19/2005  MCC     replaced all double-underscores
//  12/04/2006  MCC     relocated class into PDCLib DLL
//  08/08/2018  MCC     implemented more robust TwinCAT 3 ADS detection
//
// ============================================================================

#endif
