#if !defined (HASPADAPTER_H)
#define HASPADAPTER_H

// ============================================================================
//
//                              CONFIDENTIAL
//
//        GENOMICS INSTITUTE OF THE NOVARTIS RESEARCH FOUNDATION (GNF)
//
//  This is an unpublished work of authorship, which contains trade secrets,
//  created in 2008.  GNF owns all rights to this work and intends to maintain
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
//            Name: HaspAdapter.h
//
//     Description: Hasp Adapter class declaration
//
//          Author: Marc Gustafson
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: haspadapter.h %
//        %version: 17.1.5 %
//          %state: %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#if _MSC_VER > 1000
#pragma once
#endif

class Chasp;

namespace PDCLib
{
#if defined (_PDCLIB_EXPORT)
class __declspec (dllexport) CHaspAdapter
#else
class __declspec (dllimport) CHaspAdapter
#endif
{
public:
  enum class EHaspFeature
    {
      haspObsoleteFeature    = -1,
      haspDefaultFeature     = 0,
      haspPDCApp             = 1,
      haspRunTimeEnv         = 2,
      haspRADSAC             = 3,
      haspMethodEditor       = 4,
      haspEnVisionBridge     = 5,
      haspEvowareBridge      = 6,
      haspExplorerBridge     = 7,
      haspMinitrakBridge     = 8,
      haspSchedulerBridge    = 9,
      haspViluBridge         = 10,
      haspVWorksBridge       = 11,
      haspRTELib             = 12,
      haspTopologyEditor     = 13,
      haspGalil              = 14,
      haspBeckhoff           = 15,
      haspPDCLib             = 16,
      haspDeviceEditor       = 17,
      haspHC12Emu            = 18,
      haspMagellanBridge     = 19,
      haspBiomekBridge       = 20,
      haspBenchWorksBridge   = 21,
      haspUberBridge         = 22,
      haspSimulator          = 23,
      haspGNFCommon          = 24,
      haspEchoBridge         = 25,
      haspGNFDeveloper       = 26,
      haspCyanBridge         = 27,
      haspG2Incubator        = 28,
      haspPHERAstarBridge    = 29,
      haspSummitAgent        = 30,
      haspRemoteControl      = 31,
      haspSSStation          = 32,
      haspADTool             = 33,
      haspPlateDBViewer      = 34,
      haspSingleTipDispenser = 35,
      haspLumiReader         = 36,
      haspCeldyne            = 37,
      haspCeligoBridge       = 38,
      haspFFStation          = 39,
      haspGNFService         = 40,
      haspStacker            = 41,
      haspG3Incubator        = 42,
      haspCentralStation     = 43,
      haspSimulationMode     = 44,
	    haspDelidder           = 45,
      haspAspirateSingleTip  = 46,
      haspInvalidFeature     = 47
    };

  using CHaspFeatureSet = std::set <EHaspFeature>;

  explicit CHaspAdapter (void);
  virtual ~CHaspAdapter ();

  void GetAllFeatures (CHaspFeatureSet & features);
  void GetUsableFeatures (CHaspFeatureSet & features);

  static bool HasFeature (EHaspFeature feature);
  static void GetValidFeatures (CHaspFeatureSet & features);
  static CString GetFeature (EHaspFeature feature);

private:
  std::unique_ptr <Chasp> m_hasp;

  void GetFeatures (CHaspFeatureSet & features, bool usableOnly);
  void GetFeatureIDs (IXMLDOMElementPtr & element, CHaspFeatureSet & features, bool usableOnly);

  static CString GetNodeValue (IXMLDOMNodePtr & item, CString const & name);

public:
  // copy construction and assignment not allowed for this class

  CHaspAdapter (const CHaspAdapter &) = delete;
  CHaspAdapter & operator = (const CHaspAdapter &) = delete;
};

CArchive & operator>> (CArchive & ar, CHaspAdapter::EHaspFeature & rhs);
CArchive & operator<< (CArchive & ar, CHaspAdapter::EHaspFeature const & rhs);
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
//  07/16/2008  MEG     initial revision
//  05/07/2009  MCC     added template editor feature
//  07/23/2009  MCC     added Cyan bridge feature
//  03/23/2011  MCC     added new features
//  09/20/2011  MCC     added Summit Agent feature
//  01/05/2012  MCC     secured remote control interface
//  01/11/2012  MCC     implemented support for device specific feature IDs
//  03/23/2012  MCC     added support for SSStation device type
//  11/13/2012  MCC     added missing features
//  05/08/2013  MCC     added support for single tip dispenser device type
//  07/25/2013  MEG     added support for lumi reader device type
//  05/06/2014  MCC     added Celdyne feature
//  05/21/2014  MEG     added Celigo bridge feature
//  06/26/2014  MCC     added Flask Filling Station feature
//  07/17/2014  MCC     implemented support for executing shell commands
//  09/11/2014  MCC     implemented support for developer and service features
//  01/22/2015  MCC     added stacker feature
//  07/02/2015  MCC     added G3 incubator feature
//  10/30/2015  MCC     added central station feature
//  03/03/2016  MCC     added simulation mode feature
//  06/05/2017  TAN     added delidder feature
//  06/25/2018  MCC     changed aspirate duration to floating point
//
// ============================================================================

#endif
