#if !defined (TWINCATIO_H)
#define TWINCATIO_H

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
//            Name: TwinCATIO.h
//
//     Description: TwinCAT I/O Base class declaration
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: twincatio.h %
//        %version: 22 %
//          %state: %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#include "IOAnalog.h"
#include "IODiscrete.h"

#if _MSC_VER > 1000
#pragma once
#endif

class CTwinCATADS;

#if defined (_TWINCAT_EXPORT)
class __declspec (dllexport) CTwinCATIO final
#else
class __declspec (dllimport) CTwinCATIO final
#endif
{
public:
  explicit CTwinCATIO (WORD                          analogPortNumber,
                       WORD                          discretePortNumber,
                       bool                          simulationMode,
                       std::shared_ptr <CTwinCATADS> twinCATADS = nullptr);
  virtual ~CTwinCATIO ();

  bool Create (void);
  void UpdateInputs (void);
  void UpdateOutputs (void);

  bool IsModulePresent (void) const;

  bool IsInputBitClr (int index) const;
  bool IsInputBitSet (int index) const;

  void ClrOutputBit (int index);
  void SetOutputBit (int index);
  void TglOutputBit (int index);
  bool IsOutputBitClr (int index) const;
  bool IsOutputBitSet (int index) const;

  bool IsInputBitClr (int group, BYTE mask) const;  // deprecated (use index based interface above)
  bool IsInputBitSet (int group, BYTE mask) const;  // deprecated (use index based interface above)

  void ClrOutputBits (int group, WORD mask);        // deprecated (use index based interface above)
  void SetOutputBits (int group, WORD mask);        // deprecated (use index based interface above)
  void TglOutputBits (int group, WORD mask);        // deprecated (use index based interface above)
  bool IsOutputBitClr (int group, WORD mask) const; // deprecated (use index based interface above)
  bool IsOutputBitSet (int group, WORD mask) const; // deprecated (use index based interface above)

  void SetAnalogOutput (int channel, short value);
  short GetAnalogInput (int channel) const;

  void SetPWMDutyCycle (int channel, double value);

  bool GetSimulationMode (void) const;
  CString GetErrorMessage (void) const;

  WORD GetAnalogPortNumber (void) const;
  WORD GetDiscretePortNumber (void) const;

  inline static HMODULE GetModuleHandle (void) { return m_hModule; }

private:
  using ProcTCatIoOpen         = long (__stdcall *) (void);
  using ProcTCatIoClose        = long (__stdcall *) (void);
  using ProcTCatIoInputUpdate  = long (__stdcall *) (unsigned short port);
  using ProcTCatIoOutputUpdate = long (__stdcall *) (unsigned short port);
  using ProcTCatIoGetInputPtr  = long (__stdcall *) (unsigned short port, void** ppInput, int nSize);
  using ProcTCatIoGetOutputPtr = long (__stdcall *) (unsigned short port, void** ppOutput, int nSize);

  class CFilteredADChannel;

  using CFilteredADChannelPtr = std::shared_ptr <CFilteredADChannel>;

  static CString const TCATIODRV_LIBRARY;

  static LONG                   m_refCount;
  static HMODULE                m_hModule;
  static ProcTCatIoOpen         TCatIoOpen;
  static ProcTCatIoClose        TCatIoClose;
  static ProcTCatIoInputUpdate  TCatIoInputUpdate;
  static ProcTCatIoOutputUpdate TCatIoOutputUpdate;
  static ProcTCatIoGetInputPtr  TCatIoGetInputPtr;
  static ProcTCatIoGetOutputPtr TCatIoGetOutputPtr;

  static std::mutex m_apiGate;

  WORD const m_analogPortNumber;
  WORD const m_discretePortNumber;
  bool const m_simulationMode;
  std::shared_ptr <CTwinCATADS> m_twinCATADS;
  std::vector <SAnalogInputsPtr> m_analogInputs;
  std::vector <SAnalogOutputsPtr> m_analogOutputs;
  std::vector <SDiscreteInputsPtr> m_discreteInputs;
  std::vector <SDiscreteOutputsPtr> m_discreteOutputs;
  std::vector <CFilteredADChannelPtr> m_adChannel;
  mutable CString m_errorMessage;

  bool Open (void);

  template <class T> bool GetInputPtr (WORD port, T const * & inputPtr);
  template <class T> bool GetOutputPtr (WORD port, T * & outputPtr);

  template <typename T> bool GetProcAddress (T & procAddress, LPCSTR procName) const;

public:
  // copy construction and assignment not allowed for this class

  CTwinCATIO (const CTwinCATIO &) = delete;
  CTwinCATIO & operator = (const CTwinCATIO &) = delete;
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
//  08/11/2005  MCC     initial revision
//  10/25/2005  MCC     integrated Beckhoff TwinCAT I/O into base I/O class
//  10/28/2005  MCC     baselined Beckhoff TwinCAT ADS template class
//  11/09/2005  MCC     implemented support for jog mode command
//  03/09/2006  MCC     corrected problem with TwinCAT I/O template class
//  04/27/2006  MCC     moved device type enumeration out of global namespace
//  05/01/2006  MCC     corrected Beckhoff analog output control/scaling
//  05/04/2006  MCC     corrected Beckhoff analog input control/scaling
//  06/02/2006  MCC     implemented support for automation extension
//  10/24/2006  MCC     exposed TwinCAT I/O simulation mode to I/O subclasses
//  01/17/2007  MCC     baselined Beckhoff timer state machine class
//  03/12/2008  MCC     implemented support for Visual Studio 2008
//  09/24/2008  MCC     baselined TwinCAT DLL project
//  06/09/2010  MCC     implemented support for PWM output
//  06/05/2014  MCC     implemented index based discrete I/O interface
//  06/09/2014  MCC     refactored TwinCAT I/O interface
//  12/17/2014  MCC     implemented critical sections with C++11 concurrency
//  12/17/2014  MCC     replaced auto pointers with C++11 unique pointers
//  01/09/2015  MCC     templatized number of elements macro
//  06/04/2018  MCC     implemented support for TwinCAT ADS I/O interface
//  08/17/2018  MCC     implemented support for R0 access through TwinCAT ADS
//  08/23/2018  MCC     modified to log version of loaded TwinCAT module
//
// ============================================================================

#endif
