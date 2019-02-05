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
//            Name: TwinCATIO.cpp
//
//     Description: TwinCAT I/O Base class definition
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: twincatio.cpp %
//        %version: 19 %
//          %state: %
//         %cvtype: c++ %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#include "StdAfx.h"
#include "TwinCATIO.h"
#include "TwinCATADS.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class CTwinCATIO::CFilteredADChannel final
{
public:
  explicit CFilteredADChannel (BYTE & control, const short & data)
    : m_control (control)
    , m_data (data)
    , m_adFilter ((((0 - MIN_AD_COUNTS) << PRECISION)) >> KLOG2)
    { ; }
  virtual ~CFilteredADChannel () = default;

  inline void Sample (void)
    { m_control = static_cast <BYTE> (0x00); }

  // Simple first order lag filter
  //
  // l_f1 = (l_r + (K - 1) * l_f0) / K

  inline void Update (void)
    { m_adFilter = static_cast <DWORD> ((((m_data - MIN_AD_COUNTS) << PRECISION) + ((1 << KLOG2) - 1) * m_adFilter) >> KLOG2); }

  // Round result (add 1/2 and truncate)

  inline short GetValue (void) const
    { return static_cast <short> (((m_adFilter + (1 << (PRECISION - 1))) >> PRECISION) + MIN_AD_COUNTS); }

private:
  BYTE & m_control;     // reference to raw analog control output (zero returns process value)
  const short & m_data; // reference to raw analog data input [-32768, 32767]
  DWORD m_adFilter;     // fixed-point filtered value

  enum { KLOG2         = 3 };
  enum { MIN_AD_COUNTS = SHRT_MIN };
  enum { PRECISION     = std::numeric_limits <DWORD>::digits - std::numeric_limits <WORD>::digits - KLOG2 };

public:
  // copy construction and assignment not allowed for this class

  CFilteredADChannel (const CFilteredADChannel &) = delete;
  CFilteredADChannel & operator = (const CFilteredADChannel &) = delete;
};

CString const CTwinCATIO::TCATIODRV_LIBRARY (_T ("TCatIoDrv.dll"));

LONG                               CTwinCATIO::m_refCount         (0);
HMODULE                            CTwinCATIO::m_hModule          (nullptr);
CTwinCATIO::ProcTCatIoOpen         CTwinCATIO::TCatIoOpen         (nullptr);
CTwinCATIO::ProcTCatIoClose        CTwinCATIO::TCatIoClose        (nullptr);
CTwinCATIO::ProcTCatIoInputUpdate  CTwinCATIO::TCatIoInputUpdate  (nullptr);
CTwinCATIO::ProcTCatIoOutputUpdate CTwinCATIO::TCatIoOutputUpdate (nullptr);
CTwinCATIO::ProcTCatIoGetInputPtr  CTwinCATIO::TCatIoGetInputPtr  (nullptr);
CTwinCATIO::ProcTCatIoGetOutputPtr CTwinCATIO::TCatIoGetOutputPtr (nullptr);

std::mutex CTwinCATIO::m_apiGate;

CTwinCATIO::CTwinCATIO (WORD                          analogPortNumber,
                        WORD                          discretePortNumber,
                        bool                          simulationMode,
                        std::shared_ptr <CTwinCATADS> twinCATADS)
  : m_analogPortNumber (analogPortNumber)
  , m_discretePortNumber (discretePortNumber)
  , m_simulationMode (simulationMode)
  , m_twinCATADS (twinCATADS)
{
  m_analogInputs.push_back (new SAnalogInputs);
  m_analogOutputs.push_back (new SAnalogOutputs);
  m_discreteInputs.push_back (new SDiscreteInputs);
  m_discreteOutputs.push_back (new SDiscreteOutputs);

  ::memset (m_analogOutputs[0], 0, sizeof (SAnalogOutputs));
  ::memset (m_discreteOutputs[0], 0, sizeof (SDiscreteOutputs));

  m_analogInputs.push_back (nullptr);
  m_analogOutputs.push_back (nullptr);
  m_discreteInputs.push_back (nullptr);
  m_discreteOutputs.push_back (nullptr);

  for (int l_i (0); static_cast <size_t> (l_i) < PDCLib::NUM_ELEMENTS (m_analogInputs[0]->m_analogInputData); ++l_i)
    {
      CFilteredADChannelPtr l_adChannel (new CFilteredADChannel (m_analogOutputs[0]->m_analogInputCtrl[l_i],
                                                                 m_analogInputs[0]->m_analogInputData[l_i]));

      m_adChannel.push_back (l_adChannel);
    }

  ::InterlockedIncrement (&m_refCount);
}

CTwinCATIO::~CTwinCATIO ()
{
  if (::InterlockedDecrement (&m_refCount) == 0)
    {
      std::unique_lock <std::mutex> l_apiGate { m_apiGate };

      if (m_hModule)
        {
          TCatIoClose ();

          VERIFY (::AfxFreeLibrary (m_hModule));

          m_hModule = nullptr;
        }
    }

  delete m_analogInputs[0];
  delete m_analogOutputs[0];
  delete m_discreteInputs[0];
  delete m_discreteOutputs[0];
}

bool
CTwinCATIO::Create (void)
{
  return m_simulationMode ||
         (Open () &&
          GetInputPtr (m_analogPortNumber, m_analogInputs[1]) &&
          GetInputPtr (m_discretePortNumber, m_discreteInputs[1]) &&
          GetOutputPtr (m_analogPortNumber, m_analogOutputs[1]) &&
          GetOutputPtr (m_discretePortNumber, m_discreteOutputs[1]));
}

void
CTwinCATIO::UpdateInputs (void)
{
  std::unique_lock <std::mutex> l_apiGate { m_apiGate };

  if (m_hModule)
    {
      if (m_analogInputs[1])
        {
          TCatIoInputUpdate (m_analogPortNumber);

          ::memcpy_s (const_cast <SAnalogInputs *> (m_analogInputs[0]), sizeof (SAnalogInputs), m_analogInputs[1], sizeof (SAnalogInputs));
        }

      if (m_discreteInputs[1])
        {
          TCatIoInputUpdate (m_discretePortNumber);

          ::memcpy_s (const_cast <SDiscreteInputs *> (m_discreteInputs[0]), sizeof (SDiscreteInputs), m_discreteInputs[1], sizeof (SDiscreteInputs));
        }
    }
  else if (m_twinCATADS)
    {
      m_twinCATADS->UpdateInputs (const_cast <SAnalogInputs *> (m_analogInputs[0]), const_cast <SDiscreteInputs *> (m_discreteInputs[0]));
    }

  for (auto&& l_adChannel : m_adChannel)
    {
      l_adChannel->Update ();
    }
}

void
CTwinCATIO::UpdateOutputs (void)
{
  for (auto&& l_adChannel : m_adChannel)
    {
      l_adChannel->Sample ();
    }

  std::unique_lock <std::mutex> l_apiGate { m_apiGate };

  if (m_hModule)
    {
      if (m_analogOutputs[1])
        {
          ::memcpy_s (m_analogOutputs[1], sizeof (SAnalogOutputs), m_analogOutputs[0], sizeof (SAnalogOutputs));

          TCatIoOutputUpdate (m_analogPortNumber);
        }

      if (m_discreteOutputs[1])
        {
          ::memcpy_s (m_discreteOutputs[1], sizeof (SDiscreteOutputs), m_discreteOutputs[0], sizeof (SDiscreteOutputs));

          TCatIoOutputUpdate (m_discretePortNumber);
        }
    }
  else if (m_twinCATADS)
    {
      m_twinCATADS->UpdateOutputs (m_analogOutputs[0], m_discreteOutputs[0]);
    }
}

bool
CTwinCATIO::IsModulePresent (void) const
{
  return m_hModule != nullptr;
}

bool
CTwinCATIO::IsInputBitClr (int index) const
{
  return IsInputBitClr (index / 8, 0x01 << (index % 8));
}

bool
CTwinCATIO::IsInputBitSet (int index) const
{
  return IsInputBitSet (index / 8, 0x01 << (index % 8));
}

void
CTwinCATIO::ClrOutputBit (int index)
{
  ClrOutputBits (index / 16, 0x0001 << (index % 16));
}

void
CTwinCATIO::SetOutputBit (int index)
{
  SetOutputBits (index / 16, 0x0001 << (index % 16));
}

void
CTwinCATIO::TglOutputBit (int index)
{
  TglOutputBits (index / 16, 0x0001 << (index % 16));
}

bool
CTwinCATIO::IsOutputBitClr (int index) const
{
  return IsOutputBitClr (index / 16, 0x0001 << (index % 16));
}

bool
CTwinCATIO::IsOutputBitSet (int index) const
{
  return IsOutputBitSet (index / 16, 0x0001 << (index % 16));
}

bool
CTwinCATIO::IsInputBitClr (int group, BYTE mask) const
{
  return ::IsBitClr (m_discreteInputs[0]->m_discreteInput[group], mask);
}

bool
CTwinCATIO::IsInputBitSet (int group, BYTE mask) const
{
  return ::IsBitSet (m_discreteInputs[0]->m_discreteInput[group], mask);
}

void
CTwinCATIO::ClrOutputBits (int group, WORD mask)
{
  ::ClrBits (m_discreteOutputs[0]->m_discreteOutput[group], mask);
}

void
CTwinCATIO::SetOutputBits (int group, WORD mask)
{
  ::SetBits (m_discreteOutputs[0]->m_discreteOutput[group], mask);
}

void
CTwinCATIO::TglOutputBits (int group, WORD mask)
{
  ::TglBits (m_discreteOutputs[0]->m_discreteOutput[group], mask);
}

bool
CTwinCATIO::IsOutputBitClr (int group, WORD mask) const
{
  return ::IsBitClr (m_discreteOutputs[0]->m_discreteOutput[group], mask);
}

bool
CTwinCATIO::IsOutputBitSet (int group, WORD mask) const
{
  return ::IsBitSet (m_discreteOutputs[0]->m_discreteOutput[group], mask);
}

void
CTwinCATIO::SetAnalogOutput (int channel, short value)
{
  m_analogOutputs[0]->m_analogOutputCtrl[channel] = static_cast <BYTE> (0x00);
  m_analogOutputs[0]->m_analogOutputData[channel] = value;
}

short
CTwinCATIO::GetAnalogInput (int channel) const
{
  return m_adChannel[channel]->GetValue ();
}

void
CTwinCATIO::SetPWMDutyCycle (int channel, double value)
{
  m_analogOutputs[0]->m_analogOutputCtrl[channel] = static_cast <BYTE> (0x00);
  m_analogOutputs[0]->m_analogOutputData[channel] = static_cast <short> (std::max (std::min (value, 100.0), 0.0) * 327.67 + 0.5);
}

bool
CTwinCATIO::GetSimulationMode (void) const
{
  return m_simulationMode;
}

CString
CTwinCATIO::GetErrorMessage (void) const
{
  return m_errorMessage;
}

WORD
CTwinCATIO::GetAnalogPortNumber (void) const
{
  return m_analogPortNumber;
}

WORD
CTwinCATIO::GetDiscretePortNumber (void) const
{
  return m_discretePortNumber;
}

bool
CTwinCATIO::Open (void)
{
  std::unique_lock <std::mutex> l_apiGate { m_apiGate };

  if (m_hModule == nullptr)
    {
      if ((m_hModule = ::AfxLoadLibrary (TCATIODRV_LIBRARY)) == nullptr)
        {
          auto const l_lastError (::GetLastError ());

          if (m_twinCATADS && (l_lastError == ERROR_MOD_NOT_FOUND))
            {
              return true;
            }
          else
            {
              m_errorMessage.Format (_T ("unable to load library %s: %s"), (LPCTSTR) TCATIODRV_LIBRARY, (LPCTSTR) PDCLib::GetErrorMessage (l_lastError));
            }
        }
      else if (GetProcAddress (TCatIoOpen, "_TCatIoOpen@0") &&
               GetProcAddress (TCatIoClose, "_TCatIoClose@0") &&
               GetProcAddress (TCatIoInputUpdate, "_TCatIoInputUpdate@4") &&
               GetProcAddress (TCatIoOutputUpdate, "_TCatIoOutputUpdate@4") &&
               GetProcAddress (TCatIoGetInputPtr, "_TCatIoGetInputPtr@12") &&
               GetProcAddress (TCatIoGetOutputPtr, "_TCatIoGetOutputPtr@12"))
        {
          if (TCatIoOpen () == -1)
            {
              m_errorMessage = _T ("unable to open TwinCAT I/O interface");

              VERIFY (::AfxFreeLibrary (m_hModule));

              m_hModule = nullptr;
            }
        }
      else
        {
          VERIFY (::AfxFreeLibrary (m_hModule));

          m_hModule = nullptr;
        }
    }

  return m_hModule != nullptr;
}

template <class T> bool
CTwinCATIO::GetInputPtr (WORD port, T const * & inputPtr)
{
  ASSERT (inputPtr == nullptr);

  std::unique_lock <std::mutex> l_apiGate { m_apiGate };

  if (m_hModule)
    {
      if (TCatIoGetInputPtr (port, const_cast <void **> (reinterpret_cast <const void **> (&inputPtr)), sizeof (T)) == 0)
        {
          return true;
        }
      else
        {
          m_errorMessage.Format (_T ("unable to get input pointer for port %ld"), port);
        }

      return false;
    }

  return true;
}

template <class T> bool
CTwinCATIO::GetOutputPtr (WORD port, T * & outputPtr)
{
  ASSERT (outputPtr == nullptr);

  std::unique_lock <std::mutex> l_apiGate { m_apiGate };

  if (m_hModule)
    {
      if (TCatIoGetOutputPtr (port, reinterpret_cast <void **> (&outputPtr), sizeof (T)) == 0)
        {
          return true;
        }
      else
        {
          m_errorMessage.Format (_T ("unable to get output pointer for port %ld"), port);
        }

      return false;
    }

  return true;
}

template <typename T> bool
CTwinCATIO::GetProcAddress (T & procAddress, LPCSTR procName) const
{
  if ((procAddress = reinterpret_cast <T> (::GetProcAddress (m_hModule, procName))) == nullptr)
    {
      m_errorMessage.Format (_T ("unable to get procedure address for %s: %s"), (LPCTSTR) CString (procName), (LPCTSTR) PDCLib::GetErrorMessage (::GetLastError ()));
    }
  else
    {
      return true;
    }

  return false;
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
//  08/11/2005  MCC     initial revision
//  08/11/2005  MCC     modified for QAC++ compliance
//  10/25/2005  MCC     integrated Beckhoff TwinCAT I/O into base I/O class
//  05/04/2006  MCC     corrected Beckhoff analog input control/scaling
//  01/17/2007  MCC     baselined Beckhoff timer state machine class
//  11/06/2007  MCC     increased raw input range on filter
//  09/25/2008  MCC     baselined TwinCAT DLL project
//  06/09/2010  MCC     implemented support for PWM output
//  06/10/2010  MCC     updated scaling of PWM output for actual part (IE2512)
//  06/21/2010  MCC     added Microsoft SDL include files
//  06/05/2014  MCC     implemented index based discrete I/O interface
//  06/09/2014  MCC     refactored TwinCAT I/O interface
//  12/17/2014  MCC     implemented critical sections with C++11 concurrency
//  01/09/2015  MCC     templatized number of elements macro
//  01/14/2015  MCC     corrected for-range syntax
//  01/14/2015  MCC     modified for-range to use universal references
//  04/13/2017  MEG     added cast to CString for resolving conversion warning
//  06/04/2018  MCC     implemented support for TwinCAT ADS I/O interface
//  08/17/2018  MCC     implemented support for R0 access through TwinCAT ADS
//
// ============================================================================
