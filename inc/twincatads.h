#if !defined (TWINCATADS_H)
#define TWINCATADS_H

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
//            Name: TwinCATADS.h
//
//     Description: TwinCAT ADS class declaration
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: twincatads.h %
//        %version: 34 %
//          %state: %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#include "AdsApi.h"

#if _MSC_VER > 1000
#pragma once
#endif

class ITwinCATADS;

struct SAnalogInputs;
struct SAnalogOutputs;
struct SDiscreteInputs;
struct SDiscreteOutputs;

#if defined (_TWINCAT_EXPORT)
class __declspec (dllexport) CTwinCATADS final
#else
class __declspec (dllimport) CTwinCATADS final
#endif
{
public:
  explicit CTwinCATADS (int  controllerId,
                        int  numAxes,
                        int  numPrograms,
                        bool simulationMode);
  virtual ~CTwinCATADS ();

  // Object Creation and Update Interface

  bool Create (void);
  bool Create (WORD analogPortNumber, WORD discretePortNumber);

  void UpdateInputs (void);
  bool UpdateOutputs (void);

  // Motion Control Interface

  using MC_Direction = ADS_INT16;

  static MC_Direction const MC_Positive;
  static MC_Direction const MC_Shortest;
  static MC_Direction const MC_Negative;

  bool SetAcceleration (int axis, double acceleration);
  bool SetDeceleration (int axis, double deceleration);
  bool SetDirection (int axis, MC_Direction direction);
  bool SetJerk (int axis, double jerk);
  bool SetPosition (int axis, double position);
  bool SetVelocity (int axis, double velocity);
  bool SetJogMode (int axis, double slewSpeed);
  void BeginMotion (int axis);
  void StopMotion (int axis);

  // General Axis Status Information

  bool IsMotionComplete (int axis) const;
  bool IsMotionFaulted (int axis) const;
  DWORD GetFaultCode (int axis) const;
  double GetPosition (int axis) const;
  double GetVelocity (int axis) const;

  static DWORD const DRIVE_STATUS_OK;

  // Program Execution Interface

  bool SetVariable (const CString & identifier, int value);
  bool SetVariable (const CString & identifier, BYTE value);
  bool SetVariable (const CString & identifier, double value);
  bool SetVariable (const CString & identifier, const std::vector <int> & value);
  bool SetVariable (const CString & identifier, const std::vector <BYTE> & value);
  bool SetVariable (const CString & identifier, const std::vector <double> & value);
  bool RunProgram (int identifier, bool stopEnabled);
  bool StopProgram (int identifier);
  bool IsProgramComplete (int identifier) const;
  DWORD GetProgramStatus (int identifier) const;

  static DWORD const PROGRAM_NO_FAULT;
  static DWORD const PROGRAM_STOPPED_FAULT;

  // Diagnostic Interface

  CString GetErrorMessage (void) const;
  static CString GetErrorMessage (DWORD programStatus);
  static CString GetADSErrorMessage (DWORD faultCode);

  static HMODULE GetModuleHandle (void);

#if defined (_TWINCAT_EXPORT)
  void UpdateInputs (SAnalogInputs * analogInputs, SDiscreteInputs * discreteInputs);

  void UpdateOutputs (SAnalogOutputs const * analogOutputs, SDiscreteOutputs const * discreteOutputs);
#endif

private:
  class CSimAxis;
  class CSimProg;

  using MC_Bool  = ADS_UINT8;
  using MC_Byte  = ADS_UINT8;
  using MC_Word  = ADS_UINT16;
  using MC_Short = ADS_INT16;
  using MC_LReal = ADS_REAL64;
  using MC_UDInt = ADS_UINT32;

  using CSimAxisPtr = std::shared_ptr <CSimAxis>;
  using CSimProgPtr = std::shared_ptr <CSimProg>;

  CString const m_controllerId;

  std::vector <MC_LReal> m_acceleration;
  std::vector <MC_LReal> m_deceleration;
  std::vector <MC_LReal> m_jerk;
  std::vector <MC_LReal> m_position;
  std::vector <MC_LReal> m_velocity;
  std::vector <MC_Direction> m_direction;
  std::vector <MC_Bool> m_stopProgram;
  std::vector <CSimAxisPtr> m_simAxis;
  std::vector <CSimProgPtr> m_simProg;
  std::vector <std::vector <MC_Bool> > m_beginMotion;
  std::vector <std::vector <MC_Bool> > m_stopMotion;
  std::vector <std::vector <MC_Bool> > m_motionComplete;
  std::vector <std::vector <MC_Bool> > m_motionStopped;
  std::vector <std::vector <MC_Bool> > m_motionFaulted;
  std::vector <std::vector <MC_Bool> > m_runProgram;
  std::vector <std::vector <MC_Bool> > m_programComplete;
  std::vector <std::vector <MC_UDInt> > m_faultCode;
  std::vector <std::vector <MC_UDInt> > m_programStatus;
  std::vector <std::vector <MC_LReal> > m_actualPosition;
  std::vector <std::vector <MC_LReal> > m_actualVelocity;
  std::vector <MC_Byte> m_analogInputs;
  std::vector <MC_Byte> m_discreteInputs;
  std::vector <MC_Byte> m_analogOutputs;
  std::vector <MC_Byte> m_discreteOutputs;

  std::vector <std::shared_ptr <ITwinCATADS>> m_twinCATADS;
  std::mutex m_notificationGate;
  mutable CString m_errorMessage;

  static CString const VAR_ACCELERATION;
  static CString const VAR_DECELERATION;
  static CString const VAR_JERK;
  static CString const VAR_POSITION;
  static CString const VAR_VELOCITY;
  static CString const VAR_DIRECTION;
  static CString const VAR_BEGINMOTION;
  static CString const VAR_STOPMOTION;
  static CString const VAR_MOTIONCOMPLETE;
  static CString const VAR_MOTIONSTOPPED;
  static CString const VAR_MOTIONFAULTED;
  static CString const VAR_RUNPROGRAM;
  static CString const VAR_STOPPROGRAM;
  static CString const VAR_PROGRAMCOMPLETE;
  static CString const VAR_FAULTCODE;
  static CString const VAR_PROGRAMSTATUS;
  static CString const VAR_ACTUALPOSITION;
  static CString const VAR_ACTUALVELOCITY;
  static CString const VAR_ANALOGINPUTS;
  static CString const VAR_ANALOGOUTPUTS;
  static CString const VAR_DISCRETEINPUTS;
  static CString const VAR_DISCRETEOUTPUTS;

  static MC_Bool const MC_False;
  static MC_Bool const MC_True;

  static MC_Direction const MC_None;
  static MC_Direction const MC_Current;

  static MC_UDInt const AXIS_NO_FAULT;
  static MC_UDInt const AXIS_STOPPED_FAULT;

  static std::array <CString, 268> const m_programStatusMessage;
  static std::map <DWORD, CString> const m_adsErrorMessage;

  enum class EADSInstance { PLC, AIO, DIO };

  bool Create_ (WORD analogPortNumber = 0, WORD discretePortNumber = 0);

  template <typename T> bool Create (WORD portNumber);

  bool UpdateOutputs (CString                              const & identifier,
                      std::vector <std::vector <MC_Bool> >       & reqVariable);
  bool UpdateOutputs (CString                              const & identifier,
                      std::vector <std::vector <MC_Bool> >       & reqVariable,
                      std::vector <std::vector <MC_Bool> > const & ackVariable);
  bool UpdateOutputs_ (CString                              const & identifier,
                       std::vector <std::vector <MC_Bool> >       & reqVariable,
                       std::vector <std::vector <MC_Bool> > const & ackVariable);

  template <typename T> void AllocInputs (         std::vector <std::vector <T> >       & buffer,
                                          typename std::vector <T>::size_type             size,
                                                   T                              const & initValue = T ());
  template <typename T> void AllocInputs (         std::vector <std::vector <T> >       & buffer,
                                          typename std::vector <T>::size_type             size,
                                                   T                              const & initValue1,
                                                   T                              const & initValue2);
  template <typename T> void UpdateInputs (std::vector <std::vector <T> > & buffer);
  template <typename T, typename U> void UpdateInputs (std::vector <T> const & src, U * dst)
    { XShim <U, T>::copy (src, dst); }

  template <typename T, typename U> bool RegisterNotification (EADSInstance            adsInstance,
                                                               CString         const & identifier,
                                                               std::vector <T> const & variable,
                                                               U                       pNoteFunc);
  template <typename T, typename U> bool RegisterNotification (EADSInstance                           adsInstance,
                                                               CString                        const & identifier,
                                                               std::vector <std::vector <T> > const & variable,
                                                               U                                      pNoteFunc)
    { return RegisterNotification (adsInstance, identifier, variable[0], pNoteFunc); }

  template <typename T> bool SetVariable_ (EADSInstance adsInstance, CString const & identifier, std::vector <T> & value, int index, T value_);
  template <typename T> bool SetVariable_ (EADSInstance adsInstance, CString const & identifier, T const & value);
  template <typename T> bool SetVariable_ (EADSInstance adsInstance, CString const & identifier, std::vector <T> const & value);
  template <typename T, typename U> bool SetVariable_ (EADSInstance adsInstance, CString const & identifier, std::vector <T> & dst, U const * src)
    { return !XShim <U, T>::assign (dst, src) || SetVariable_ (adsInstance, identifier, dst); }

  template <typename T> void CopyVariable (AdsNotificationHeader * pNotification, std::vector <T> & variable);

  template <typename T, typename U = MC_Byte> struct XShim
  {
    enum { size = sizeof (T) / sizeof (U) };

    inline static void copy (std::vector <U> const & src, T * dst) { std::copy (src.begin (), src.end (), begin (dst)); }
    inline static bool assign (std::vector <U> & dst, T const * src)
      {
        if (std::equal (dst.begin (), dst.end (), begin (src), end (src)))
          {
            return false;
          }

        dst.assign (begin (src), end (src));

        return true;
      }

  private:
    U m_data[size];

    inline static auto begin (T * t) { return reinterpret_cast <XShim <T, U> *> (t)->m_data; }
    inline static auto begin (T const * t) { return reinterpret_cast <XShim <T, U> const *> (t)->m_data; }

    inline static auto end (T const * t) { return begin (t) + size; }
  };

  CString GetSymbolName (EADSInstance adsInstance, CString const & identifier) const;

#define ADSNOTIFICATION1(handler, memberData) \
  friend static void handler##TC2 (AmsAddr *, AdsNotificationHeader * pNotification, unsigned long hUser) \
    { reinterpret_cast <CTwinCATADS *> (hUser)->CopyVariable (pNotification, reinterpret_cast <CTwinCATADS *> (hUser)->memberData); } \
  friend static void __stdcall handler##TC3 (AmsAddr *, AdsNotificationHeader * pNotification, unsigned long hUser) \
    { reinterpret_cast <CTwinCATADS *> (hUser)->CopyVariable (pNotification, reinterpret_cast <CTwinCATADS *> (hUser)->memberData); }
#define ADSNOTIFICATION2(handler, memberData) \
  friend static void handler##TC2 (AmsAddr *, AdsNotificationHeader * pNotification, unsigned long hUser) \
    { reinterpret_cast <CTwinCATADS *> (hUser)->CopyVariable (pNotification, reinterpret_cast <CTwinCATADS *> (hUser)->memberData[1]); } \
  friend static void __stdcall handler##TC3 (AmsAddr *, AdsNotificationHeader * pNotification, unsigned long hUser) \
    { reinterpret_cast <CTwinCATADS *> (hUser)->CopyVariable (pNotification, reinterpret_cast <CTwinCATADS *> (hUser)->memberData[1]); }

  ADSNOTIFICATION2 (OnActualPosition, m_actualPosition)
  ADSNOTIFICATION2 (OnActualVelocity, m_actualVelocity)
  ADSNOTIFICATION2 (OnMotionComplete, m_motionComplete)
  ADSNOTIFICATION2 (OnMotionStopped, m_motionStopped)
  ADSNOTIFICATION2 (OnMotionFaulted, m_motionFaulted)
  ADSNOTIFICATION2 (OnFaultCode, m_faultCode)
  ADSNOTIFICATION2 (OnProgramComplete, m_programComplete)
  ADSNOTIFICATION2 (OnProgramStatus, m_programStatus)
  ADSNOTIFICATION1 (OnAnalogInputs, m_analogInputs)
  ADSNOTIFICATION1 (OnDiscreteInputs, m_discreteInputs)

#undef ADSNOTIFICATION1
#undef ADSNOTIFICATION2

public:
  // copy construction and assignment not allowed for this class

  CTwinCATADS (CTwinCATADS const &) = delete;
  CTwinCATADS & operator = (CTwinCATADS const &) = delete;
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
//  10/27/2005  MCC     initial revision
//  11/08/2005  MCC     implemented TwinCAT ADS callback mechanism
//  11/08/2005  MCC     implemented support for jog mode command
//  11/10/2005  MCC     modified TwinCAT ADS template class public interface
//  11/11/2005  MCC     implemented TwinCAT ADS motion fault handling
//  02/06/2006  MEG     fixed behavior with begin motion after an abort
//  02/07/2006  MCC     got rid of anonymous namespace
//  04/27/2006  MCC     moved device type enumeration out of global namespace
//  06/02/2006  MCC     implemented support for automation extension
//  06/06/2006  MCC     modified to unconditionally open ADS port
//  09/27/2006  MCC     implemented support for additional TwinCAT ADS features
//  10/02/2006  MCC     modified for QAC++ compliance
//  10/09/2006  MCC     modified init program status to work in simulation mode
//  01/17/2007  MCC     baselined Beckhoff timer state machine class
//  02/16/2007  MCC     baselined G2 PinTool reset state machine
//  07/25/2008  MCC     implemented support for drive status error messages
//  09/24/2008  MCC     baselined TwinCAT DLL project
//  07/19/2012  TAN     converted TwinCATADS hex fault code to error message
//  03/29/2013  MCC     implemented stop program interface
//  04/02/2013  MCC     added error checking to stop program interface
//  06/06/2014  MCC     implemented support for TwinCAT ADS simulation mode
//  12/17/2014  MCC     implemented critical sections with C++11 concurrency
//  01/09/2015  MCC     templatized number of elements macro
//  06/04/2018  MCC     implemented support for TwinCAT ADS I/O interface
//  06/06/2018  MCC     implemented support for TwinCAT 3 ADS interface
//  06/07/2018  MCC     optimized support for TwinCAT ADS I/O interface
//  06/07/2018  MCC     optimized support for TwinCAT ADS I/O interface
//  08/09/2018  MCC     implemented more robust TwinCAT 3 ADS detection
//  08/16/2018  MCC     refactored TwinCAT 3 ADS interface further
//  08/17/2018  MCC     implemented support for R0 access through TwinCAT ADS
//  08/22/2018  MCC     corrected problem with TwinCAT ADS variable names
//  08/23/2018  MCC     modified to log version of loaded TwinCAT module
//  08/24/2018  MCC     made TwinCAT ADS controller identifier constant
//  10/24/2018  MCC     updated TwinCAT ADS program status message table
//
// ============================================================================

#endif
