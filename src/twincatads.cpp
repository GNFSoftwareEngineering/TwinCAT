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
//            Name: TwinCATADS.cpp
//
//     Description: TwinCAT ADS class definition
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: twincatads.cpp %
//        %version: 50 %
//          %state: %
//         %cvtype: c++ %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#include "StdAfx.h"
#include "TwinCATADS.h"
#include "DriveStatus.h"
#include "IOAnalog.h"
#include "IODiscrete.h"
#include "VersionInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class ITwinCATADS
{
public:
  virtual ~ITwinCATADS () = default;

  virtual void Create (WORD portNumber) = 0;

  void SetVariable (CString const & symbolName, size_t cbLength, void const * pData);
  void RegisterNotification (CString const & symbolName, size_t cbLength, void * pNoteFunc, void * hUser);

  static HMODULE GetModuleHandle (void) { return m_hModule; }

protected:
  explicit ITwinCATADS (void);

  using CAdsDllVersion = std::vector <DWORD>;
  using CProcAds       = std::tuple <LPVOID *, CString>;
  using CAdsApi        = std::vector <CProcAds>;

  void Create (CString        const & adsDllFilename,
               CAdsDllVersion const & adsDllVersion,
               CAdsApi        const & adsApi,
               WORD                   portNumber);
  void Destroy (void);

  virtual long SyncWriteReq (AmsAddr       & amsAddr,
                             unsigned long   indexGroup,
                             unsigned long   indexOffset,
                             unsigned long   length,
                             void          * pData) = 0;
  virtual long SyncReadWriteReq (AmsAddr       & amsAddr,
                                 unsigned long   cbReadLength,
                                 void          * pReadData,
                                 unsigned long   cbWriteLength,
                                 void          * pWriteData) = 0;
  virtual long SyncAddDeviceNotificationReq (AmsAddr               & amsAddr,
                                             unsigned long           indexOffset,
                                             AdsNotificationAttrib * adsNotificationAttrib,
                                             void                  * pNoteFunc,
                                             unsigned long           hUser,
                                             unsigned long         * pNotification) = 0;
  virtual long SyncDelDeviceNotificationReq (AmsAddr       & amsAddr,
                                             unsigned long   hNotification) = 0;
  virtual long PortOpen (void) = 0;
  virtual void PortClose (void) = 0;
  virtual long GetLocalAddress (AmsAddr & amsAddr) = 0;
  virtual long GetDllVersion (void) = 0;

private:
  using CMapStringToHandle = std::map <CString, ULONG>;

  void UnRegisterNotification (void);

  ULONG GetHandle (CString const & symbolName);

  static void LoadLibrary (CString const & adsDllFilename, CAdsDllVersion const & version);
  static void FreeLibrary (void);

  static void GetProcAddress (CProcAds const & procAds);

  static CString GetADSErrorMessage (long error);

  static std::atomic_int32_t m_refCount;
  static HMODULE             m_hModule;
  static std::mutex          m_apiGate;

  static std::map <long, CString> const m_adsErrorMessage;

  AmsAddr m_amsAddr;
  CMapStringToHandle m_mapHandle;
  std::vector <std::tuple <ULONG, ULONG>> m_hNotification;

  template <typename _Fn> auto CallAPI (_Fn _Fx)
    {
      std::unique_lock <std::mutex> l_apiGate { m_apiGate };

      return _Fx (m_amsAddr);
    }

public:
  ITwinCATADS (ITwinCATADS const &) = delete;
  ITwinCATADS & operator = (ITwinCATADS const &) = delete;
};

std::atomic_int32_t ITwinCATADS::m_refCount (0);
HMODULE             ITwinCATADS::m_hModule  (nullptr);
std::mutex          ITwinCATADS::m_apiGate;

std::map <long, CString> const ITwinCATADS::m_adsErrorMessage
{
  { 0x00000001,                         _T ("internal error")                                                                          },
  { 0x00000002,                         _T ("no RTime")                                                                                },
  { 0x00000003,                         _T ("allocation locked memory error")                                                          },
  { 0x00000004,                         _T ("insert mailbox error")                                                                    },
  { 0x00000005,                         _T ("wrong receive HMSG")                                                                      },
  { 0x00000006,                         _T ("target port not found")                                                                   },
  { 0x00000007,                         _T ("target machine not found")                                                                },
  { 0x00000008,                         _T ("unknown command ID")                                                                      },
  { 0x00000009,                         _T ("bad task ID")                                                                             },
  { 0x0000000A,                         _T ("no IO")                                                                                   },
  { 0x0000000B,                         _T ("unknown ADS command")                                                                     },
  { 0x0000000C,                         _T ("Win32 error")                                                                             },
  { 0x0000000D,                         _T ("port not connected")                                                                      },
  { 0x0000000E,                         _T ("invalid ADS length")                                                                      },
  { 0x0000000F,                         _T ("invalid ADS Net ID")                                                                      },
  { 0x00000010,                         _T ("low installation level")                                                                  },
  { 0x00000011,                         _T ("no debug available")                                                                      },
  { 0x00000012,                         _T ("port disabled")                                                                           },
  { 0x00000013,                         _T ("port already connected")                                                                  },
  { 0x00000014,                         _T ("ADS Sync Win32 error")                                                                    },
  { 0x00000015,                         _T ("ADS Sync timeout")                                                                        },
  { 0x00000016,                         _T ("ADS Sync AMS error")                                                                      },
  { 0x00000017,                         _T ("ADS Sync no index map")                                                                   },
  { 0x00000018,                         _T ("invalid ADS port")                                                                        },
  { 0x00000019,                         _T ("no memory")                                                                               },
  { 0x0000001A,                         _T ("TCP send error")                                                                          },
  { 0x0000001B,                         _T ("host unreachable")                                                                        },
  { 0x0000001C,                         _T ("invalid AMS fragment")                                                                    },
  { 0x00000500,                         _T ("no locked memory can be allocated")                                                       },
  { 0x00000501,                         _T ("the size of the router memory could not be changed")                                      },
  { 0x00000502,                         _T ("the mailbox is full; the current sent message was rejected")                              },
  { 0x00000503,                         _T ("the mailbox is full; the sent message will not be displayed in the debug monitor")        },
  { 0x00000504,                         _T ("unknown port type")                                                                       },
  { 0x00000505,                         _T ("router is not initialized")                                                               },
  { 0x00000506,                         _T ("the desired port number is already assigned")                                             },
  { 0x00000507,                         _T ("port not registered")                                                                     },
  { 0x00000508,                         _T ("the maximum number of ports reached")                                                     },
  { 0x00000509,                         _T ("invalid port")                                                                            },
  { 0x0000050A,                         _T ("TwinCAT router not active")                                                               },
  { ADSERR_DEVICE_ERROR,                _T ("error class <device error>")                                                              },
  { ADSERR_DEVICE_SRVNOTSUPP,           _T ("service is not supported by server")                                                      },
  { ADSERR_DEVICE_INVALIDGRP,           _T ("invalid index group")                                                                     },
  { ADSERR_DEVICE_INVALIDOFFSET,        _T ("invalid index offset")                                                                    },
  { ADSERR_DEVICE_INVALIDACCESS,        _T ("reading/writing not permitted")                                                           },
  { ADSERR_DEVICE_INVALIDSIZE,          _T ("parameter size not correct")                                                              },
  { ADSERR_DEVICE_INVALIDDATA,          _T ("invalid parameter value(s)")                                                              },
  { ADSERR_DEVICE_NOTREADY,             _T ("device is not in a ready state")                                                          },
  { ADSERR_DEVICE_BUSY,                 _T ("device is busy")                                                                          },
  { ADSERR_DEVICE_INVALIDCONTEXT,       _T ("invalid context (must be in Windows)")                                                    },
  { ADSERR_DEVICE_NOMEMORY,             _T ("out of memory")                                                                           },
  { ADSERR_DEVICE_INVALIDPARM,          _T ("invalid parameter value(s)")                                                              },
  { ADSERR_DEVICE_NOTFOUND,             _T ("file(s) not found")                                                                       },
  { ADSERR_DEVICE_SYNTAX,               _T ("syntax error in command or file")                                                         },
  { ADSERR_DEVICE_INCOMPATIBLE,         _T ("objects do not match")                                                                    },
  { ADSERR_DEVICE_EXISTS,               _T ("object already exists")                                                                   },
  { ADSERR_DEVICE_SYMBOLNOTFOUND,       _T ("symbol not found")                                                                        },
  { ADSERR_DEVICE_SYMBOLVERSIONINVALID, _T ("symbol version invalid")                                                                  },
  { ADSERR_DEVICE_INVALIDSTATE,         _T ("server is in invalid state")                                                              },
  { ADSERR_DEVICE_TRANSMODENOTSUPP,     _T ("AdsTransMode not supported")                                                              },
  { ADSERR_DEVICE_NOTIFYHNDINVALID,     _T ("notification handle is invalid")                                                          },
  { ADSERR_DEVICE_CLIENTUNKNOWN,        _T ("notification client not registered")                                                      },
  { ADSERR_DEVICE_NOMOREHDLS,           _T ("no more notification handles")                                                            },
  { ADSERR_DEVICE_INVALIDWATCHSIZE,     _T ("size for watch too big")                                                                  },
  { ADSERR_DEVICE_NOTINIT,              _T ("device not initialized")                                                                  },
  { ADSERR_DEVICE_TIMEOUT,              _T ("device has a timeout")                                                                    },
  { ADSERR_DEVICE_NOINTERFACE,          _T ("query interface failed")                                                                  },
  { ADSERR_DEVICE_INVALIDINTERFACE,     _T ("wrong interface required")                                                                },
  { ADSERR_DEVICE_INVALIDCLSID,         _T ("class identifier is invalid")                                                             },
  { ADSERR_DEVICE_INVALIDOBJID,         _T ("object identifier is invalid")                                                            },
  { ADSERR_DEVICE_PENDING,              _T ("request is pending")                                                                      },
  { ADSERR_DEVICE_ABORTED,              _T ("request is aborted")                                                                      },
  { ADSERR_DEVICE_WARNING,              _T ("signal warning")                                                                          },
  { ADSERR_DEVICE_INVALIDARRAYIDX,      _T ("invalid array index")                                                                     },
  { ADSERR_DEVICE_SYMBOLNOTACTIVE,      _T ("symbol not active")                                                                       },
  { ADSERR_DEVICE_ACCESSDENIED,         _T ("access denied")                                                                           },
  { ADSERR_DEVICE_LICENSENOTFOUND,      _T ("missing license")                                                                         },
  { ADSERR_DEVICE_LICENSEEXPIRED,       _T ("license expired")                                                                         },
  { ADSERR_DEVICE_LICENSEEXCEEDED,      _T ("license exceeded")                                                                        },
  { ADSERR_DEVICE_LICENSEINVALID,       _T ("license invalid")                                                                         },
  { ADSERR_DEVICE_LICENSESYSTEMID,      _T ("license invalid system identifier")                                                       },
  { ADSERR_DEVICE_LICENSENOTIMELIMIT,   _T ("license not time limited")                                                                },
  { ADSERR_DEVICE_LICENSEFUTUREISSUE,   _T ("license issue time in the future")                                                        },
  { ADSERR_DEVICE_LICENSETIMETOLONG,    _T ("license time period to long")                                                             },
  { ADSERR_DEVICE_EXCEPTION,            _T ("exception occured during system start")                                                   },
  { ADSERR_DEVICE_LICENSEDUPLICATED,    _T ("license file read twice")                                                                 },
  { ADSERR_DEVICE_SIGNATUREINVALID,     _T ("invalid signature")                                                                       },
  { ADSERR_DEVICE_CERTIFICATEINVALID,   _T ("public key certificate")                                                                  },
  { ADSERR_CLIENT_ERROR,                _T ("error class <client error>")                                                              },
  { ADSERR_CLIENT_INVALIDPARM,          _T ("invalid parameter at service")                                                            },
  { ADSERR_CLIENT_LISTEMPTY,            _T ("polling list is empty")                                                                   },
  { ADSERR_CLIENT_VARUSED,              _T ("var connection already in use")                                                           },
  { ADSERR_CLIENT_DUPLINVOKEID,         _T ("invoke identifier in use")                                                                },
  { ADSERR_CLIENT_SYNCTIMEOUT,          _T ("timeout elapsed")                                                                         },
  { ADSERR_CLIENT_W32ERROR,             _T ("error in Win32 subsystem")                                                                },
  { ADSERR_CLIENT_TIMEOUTINVALID,       _T ("invalid client timeout value")                                                            },
  { ADSERR_CLIENT_PORTNOTOPEN,          _T ("ADS port not opened")                                                                     },
  { ADSERR_CLIENT_NOAMSADDR,            _T ("internal error in ADS sync")                                                              },
  { ADSERR_CLIENT_SYNCINTERNAL,         _T ("hash table overflow")                                                                     },
  { ADSERR_CLIENT_ADDHASH,              _T ("key not found in hash")                                                                   },
  { ADSERR_CLIENT_REMOVEHASH,           _T ("no more symbols in cache")                                                                },
  { ADSERR_CLIENT_NOMORESYM,            _T ("invalid response received")                                                               },
  { ADSERR_CLIENT_SYNCRESINVALID,       _T ("invalid response received")                                                               },
  { ADSERR_CLIENT_SYNCPORTLOCKED,	      _T ("sync port is locked")                                                                     },
  { 0x00001000,                         _T ("internal fatal error in the TwinCAT realtime system")                                     },
  { 0x00001001,                         _T ("timer value not vaild")                                                                   },
  { 0x00001002,                         _T ("task pointer has the invalid value zero")                                                 },
  { 0x00001003,                         _T ("task stack pointer has the invalid value zero")                                           },
  { 0x00001004,                         _T ("the demand task priority is already assigned")                                            },
  { 0x00001005,                         _T ("no more free task control blocks available; maximum number of task control blocks is 64") },
  { 0x00001006,                         _T ("no more free semaphores available; maximum number of semaphores is 64")                   },
  { 0x00001007,                         _T ("no more free queues available; maximum number of free queues is 64")                      },
  { 0x0000100D,                         _T ("an external synchronization interrupt is already applied")                                },
  { 0x0000100E,                         _T ("no external synchronization interrupt applied")                                           },
  { 0x0000100F,                         _T ("the application of the external synchronization interrupt failed")                        },
  { 0x00001010,                         _T ("call of a service function in the wrong context")                                         },
  { 0x00001017,                         _T ("Intel VT-x extension is not supported")                                                   },
  { 0x00001018,                         _T ("Intel VT-x extension is not enabled in system BIOS")                                      },
  { 0x00001019,                         _T ("missing function in Intel VT-x extension")                                                },
  { 0x0000101A,                         _T ("enabling Intel VT-x failed")                                                              }
};

ITwinCATADS::ITwinCATADS (void)
{
  ::memset (&m_amsAddr, 0, sizeof (m_amsAddr));

  ++m_refCount;
}

void
ITwinCATADS::SetVariable (CString const & symbolName, size_t cbLength, void const * pData)
{
  auto const l_hSymbol (GetHandle (symbolName));

  auto const l_error (CallAPI ([this, l_hSymbol, cbLength, pData] (auto & amsAddr)
                               {
                                 return SyncWriteReq (amsAddr, ADSIGRP_SYM_VALBYHND, l_hSymbol, cbLength, const_cast <void *> (pData));
                               }));

  if (l_error == ADSERR_NOERR)
    {
      return;
    }

  PDCLib::ThrowStringException (_T ("unable to write value to symbol %s; %s"), (LPCTSTR) symbolName, (LPCTSTR) GetADSErrorMessage (l_error));
}

void
ITwinCATADS::RegisterNotification (CString const & symbolName,
                                   size_t          cbLength,
                                   void          * pNoteFunc,
                                   void          * hUser)
{
  std::tuple <ULONG, ULONG> l_hSymbol;

  std::get <0> (l_hSymbol) = GetHandle (symbolName);

  AdsNotificationAttrib l_adsNotificationAttrib;

  l_adsNotificationAttrib.cbLength   = cbLength;             // total size of variable in bytes
  l_adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA; // notify on change
  l_adsNotificationAttrib.nMaxDelay  = 1000000;              // 100 milliseconds
  l_adsNotificationAttrib.nCycleTime =  500000;              //  50 milliseconds

  auto const l_error (CallAPI ([this, &l_hSymbol, &l_adsNotificationAttrib, pNoteFunc, hUser] (auto & amsAddr)
                               {
                                 return SyncAddDeviceNotificationReq (amsAddr,
                                                                      std::get <0> (l_hSymbol),
                                                                      &l_adsNotificationAttrib,
                                                                      pNoteFunc,
                                                                      reinterpret_cast <unsigned long> (hUser),
                                                                      &std::get <1> (l_hSymbol));
                               }));

  if (l_error == ADSERR_NOERR)
    {
      m_hNotification.push_back (l_hSymbol);

      return;
    }

  PDCLib::ThrowStringException (_T ("unable to register notification for symbol %s; %s"), (LPCTSTR) symbolName, (LPCTSTR) GetADSErrorMessage (l_error));
}

void
ITwinCATADS::UnRegisterNotification (void)
{
  CallAPI ([this] (auto & amsAddr)
           {
             for (auto&& l_hNotification : m_hNotification)
               {
                 VERIFY (SyncDelDeviceNotificationReq (amsAddr, std::get <1> (l_hNotification)) == ADSERR_NOERR);

                 VERIFY (SyncWriteReq (amsAddr, ADSIGRP_SYM_RELEASEHND, std::get <0> (l_hNotification), 0, nullptr) == ADSERR_NOERR);
               }
           });

  m_hNotification.clear ();
}

ULONG
ITwinCATADS::GetHandle (CString const & symbolName)
{
  auto const l_pos (m_mapHandle.lower_bound (symbolName));

  if ((l_pos == m_mapHandle.end ()) || m_mapHandle.key_comp () (symbolName, std::get <0> (*l_pos)))
    {
      std::vector <char> l_symbolName;

      PDCLib::StringToVector (symbolName, l_symbolName);

      ULONG l_hSymbol (0);

      auto const l_error (CallAPI ([this, &l_hSymbol, &l_symbolName] (auto & amsAddr)
                          {
                            return SyncReadWriteReq (amsAddr,
                                                     sizeof (l_hSymbol),
                                                     &l_hSymbol,
                                                     static_cast <unsigned long> (l_symbolName.size () * sizeof (l_symbolName[0])),
                                                     &l_symbolName[0]);
                          }));


      if (l_error == ADSERR_NOERR)
        {
          m_mapHandle.insert (l_pos, CMapStringToHandle::value_type (symbolName, l_hSymbol));

          return l_hSymbol;
        }

      PDCLib::ThrowStringException (_T ("unable to acquire handle for symbol %s; %s"), (LPCTSTR) symbolName, (LPCTSTR) GetADSErrorMessage (l_error));
    }

  return std::get <1> (*l_pos);
}

void
ITwinCATADS::Create (CString        const & adsDllFilename,
                     CAdsDllVersion const & adsDllVersion,
                     CAdsApi        const & adsApi,
                     WORD                   portNumber)
{
  if (m_hModule)
    {
      CallAPI ([this, portNumber] (auto & amsAddr)
               {
                 auto const l_error (GetLocalAddress (amsAddr));

                 if (l_error == ADSERR_NOERR)
                   {
                     amsAddr.port = portNumber;
                      
                     return;
                   }

                 PDCLib::ThrowStringException (_T ("unable to get TwinCAT ADS local address; %s"), (LPCTSTR) GetADSErrorMessage (l_error));
               });
                      
      return;
    }

  try
    {
      LoadLibrary (adsDllFilename, adsDllVersion);

      for (auto&& l_adsApi : adsApi)
        {
          GetProcAddress (l_adsApi);
        }
    }
  catch (CString const & errorMessage)
    {
      FreeLibrary ();

      PDCLib::ThrowStringException (_T ("unable to load library: %s; %s"), (LPCTSTR) adsDllFilename, (LPCTSTR) errorMessage);
    }

  CallAPI ([this, portNumber] (auto & amsAddr)
           {
             auto const l_dllVersion (GetDllVersion ());

             auto const l_adsVersion (reinterpret_cast <AdsVersion const *> (&l_dllVersion));

             PDCLib::Trace (_T ("TwinCAT ADS Version  : %ld"), static_cast <int> (l_adsVersion->version));
             PDCLib::Trace (_T ("TwinCAT ADS Revision : %ld"), static_cast <int> (l_adsVersion->revision));
             PDCLib::Trace (_T ("TwinCAT ADS Build    : %ld"), static_cast <int> (l_adsVersion->build));

             auto const l_port (PortOpen ());

             PDCLib::Trace (_T ("TwinCAT ADS Port     : %ld"), l_port);

             auto const l_error (GetLocalAddress (amsAddr));

             if (l_error == ADSERR_NOERR)
               {
                 amsAddr.port = portNumber;

                 return;
               }

              PortClose ();

              FreeLibrary ();

              PDCLib::ThrowStringException (_T ("unable to get TwinCAT ADS local address; %s"), (LPCTSTR) GetADSErrorMessage (l_error));
           });
}

void
ITwinCATADS::Destroy (void)
{
  UnRegisterNotification ();

  if (--m_refCount == 0)
    {
      if (m_hModule)
        {
          CallAPI ([this] (auto &) { PortClose (); });
        }

      FreeLibrary ();
    }
}

void
ITwinCATADS::LoadLibrary (CString const & adsDllFilename, CAdsDllVersion const & adsDllVersion)
{
  if ((m_hModule = ::AfxLoadLibrary (adsDllFilename)) == nullptr)
    {
      PDCLib::ThrowStringException (_T ("%s"), (LPCTSTR) PDCLib::GetErrorMessage (::GetLastError ()));
    }

  std::vector <DWORD> l_adsDllVersion;

  if (PDCLib::CVersionInfo l_versionInfo; l_versionInfo.Create (m_hModule) && l_versionInfo.GetFileVersion (l_adsDllVersion))
    {
      if (l_adsDllVersion.size () == adsDllVersion.size ())
        {
          if (std::lexicographical_compare (begin (l_adsDllVersion), end (l_adsDllVersion), begin (adsDllVersion), end (adsDllVersion)))
            {
              PDCLib::ThrowStringException (_T ("minimum version requirement not satisfied"));
            }

          return;
        }

      PDCLib::ThrowStringException (_T ("invalid version information"));
    }

  PDCLib::ThrowStringException (_T ("unable to load version information"));
}

void
ITwinCATADS::FreeLibrary (void)
{
  if (m_hModule)
    {
      VERIFY (::AfxFreeLibrary (m_hModule));

      m_hModule = nullptr;
    }
}

void
ITwinCATADS::GetProcAddress (CProcAds const & procAds)
{
  std::vector <char> l_procName;

  PDCLib::StringToVector (std::get <1> (procAds), l_procName);

  if ((*std::get <0> (procAds) = ::GetProcAddress (m_hModule, &l_procName[0])) == nullptr)
    {
      PDCLib::ThrowStringException (_T ("unable to get procedure address for %s: %s"), (LPCTSTR) CString (std::get <1> (procAds)), (LPCTSTR) PDCLib::GetErrorMessage (::GetLastError ()));
    }
}

CString
ITwinCATADS::GetADSErrorMessage (long error)
{
  if (auto const l_adsErrorMessage (m_adsErrorMessage.find (error)); l_adsErrorMessage == m_adsErrorMessage.end ())
    {
      if (error < WSABASEERR)
        {
          return PDCLib::StringWithFormat (_T ("unrecognized TwinCAT ADS error code (%ld)"), error);
        }

      return PDCLib::StringWithFormat (_T ("%s (%ld)"), (LPCTSTR) PDCLib::GetErrorMessage (error), error);
    }
  else
    {
      return PDCLib::StringWithFormat (_T ("%s (%ld)"), (LPCTSTR) std::get <1> (*l_adsErrorMessage), error);
    }
}

class CTwinCATADS2 final : public ITwinCATADS
{
public:
  explicit CTwinCATADS2 (void) = default;
  virtual ~CTwinCATADS2 () { Destroy (); }

  virtual void Create (WORD portNumber) override final
    {
      ITwinCATADS::Create (ADSDLL_LIBRARY, ADSDLL_VERSION, m_adsApi, portNumber);
    }

private:
  virtual long SyncWriteReq (AmsAddr       & amsAddr,
                             unsigned long   indexGroup,
                             unsigned long   indexOffset,
                             unsigned long   length,
                             void          * pData) override final
    {
      return AdsSyncWriteReq (&amsAddr, indexGroup, indexOffset, length, pData);
    }
  virtual long SyncReadWriteReq (AmsAddr       & amsAddr,
                                 unsigned long   cbReadLength,
                                 void          * pReadData,
                                 unsigned long   cbWriteLength,
                                 void          * pWriteData) override final
    {
      return AdsSyncReadWriteReq (&amsAddr, ADSIGRP_SYM_HNDBYNAME, 0, cbReadLength, pReadData, cbWriteLength, pWriteData);
    }
  virtual long SyncAddDeviceNotificationReq (AmsAddr               & amsAddr,
                                             unsigned long           indexOffset,
                                             AdsNotificationAttrib * adsNotificationAttrib,
                                             void                  * pNoteFunc,
                                             unsigned long           hUser,
                                             unsigned long         * pNotification) override final
    {
      return AdsSyncAddDeviceNotificationReq (&amsAddr,
                                              ADSIGRP_SYM_VALBYHND,
                                              indexOffset,
                                              adsNotificationAttrib,
                                              reinterpret_cast <PAdsNotificationFuncTC2> (pNoteFunc),
                                              hUser,
                                              pNotification);
    }
  virtual long SyncDelDeviceNotificationReq (AmsAddr       & amsAddr,
                                             unsigned long   hNotification) override final
    {
      return AdsSyncDelDeviceNotificationReq (&amsAddr, hNotification);
    }

  virtual long PortOpen (void) override final { return AdsPortOpen (); }
  virtual void PortClose (void) override final { AdsPortClose (); }
  virtual long GetLocalAddress (AmsAddr & amsAddr) override final { return AdsGetLocalAddress (&amsAddr); }
  virtual long GetDllVersion (void) override final { return AdsGetDllVersion (); }

  static CString                                const ADSDLL_LIBRARY;
  static CAdsDllVersion                         const ADSDLL_VERSION;

  static ProcAdsGetDllVersionTC2                      AdsGetDllVersion;
  static ProcAdsPortOpenTC2                           AdsPortOpen;
  static ProcAdsPortCloseTC2                          AdsPortClose;
  static ProcAdsGetLocalAddressTC2                    AdsGetLocalAddress;
  static ProcAdsSyncWriteReqTC2                       AdsSyncWriteReq;
  static ProcAdsSyncReadWriteReqTC2                   AdsSyncReadWriteReq;
  static ProcAdsSyncAddDeviceNotificationReqTC2       AdsSyncAddDeviceNotificationReq;
  static ProcAdsSyncDelDeviceNotificationReqTC2       AdsSyncDelDeviceNotificationReq;

  static CAdsApi                                const m_adsApi;

public:
  CTwinCATADS2 (CTwinCATADS2 const &) = delete;
  CTwinCATADS2 & operator = (CTwinCATADS2 const &) = delete;
};

CString                                const CTwinCATADS2::ADSDLL_LIBRARY                  (_T ("AdsDll.dll"));
CTwinCATADS2::CAdsDllVersion           const CTwinCATADS2::ADSDLL_VERSION                  { 2, 11, 0, 3 };

ProcAdsGetDllVersionTC2                      CTwinCATADS2::AdsGetDllVersion                (nullptr);
ProcAdsPortOpenTC2                           CTwinCATADS2::AdsPortOpen                     (nullptr);
ProcAdsPortCloseTC2                          CTwinCATADS2::AdsPortClose                    (nullptr);
ProcAdsGetLocalAddressTC2                    CTwinCATADS2::AdsGetLocalAddress              (nullptr);
ProcAdsSyncWriteReqTC2                       CTwinCATADS2::AdsSyncWriteReq                 (nullptr);
ProcAdsSyncReadWriteReqTC2                   CTwinCATADS2::AdsSyncReadWriteReq             (nullptr);
ProcAdsSyncAddDeviceNotificationReqTC2       CTwinCATADS2::AdsSyncAddDeviceNotificationReq (nullptr);
ProcAdsSyncDelDeviceNotificationReqTC2       CTwinCATADS2::AdsSyncDelDeviceNotificationReq (nullptr);

CTwinCATADS2::CAdsApi                  const CTwinCATADS2::m_adsApi
{
  CProcAds { reinterpret_cast <LPVOID *> (&AdsGetDllVersion),                _T ("AdsGetDllVersion")                },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsPortOpen),                     _T ("AdsPortOpen")                     },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsPortClose),                    _T ("AdsPortClose")                    },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsGetLocalAddress),              _T ("AdsGetLocalAddress")              },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsSyncWriteReq),                 _T ("AdsSyncWriteReq")                 },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsSyncReadWriteReq),             _T ("AdsSyncReadWriteReq")             },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsSyncAddDeviceNotificationReq), _T ("AdsSyncAddDeviceNotificationReq") },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsSyncDelDeviceNotificationReq), _T ("AdsSyncDelDeviceNotificationReq") }
};

class CTwinCATADS3 final : public ITwinCATADS
{
public:
  explicit CTwinCATADS3 (void) = default;
  virtual ~CTwinCATADS3 () { Destroy (); }

  virtual void Create (WORD portNumber) override final
    {
      ITwinCATADS::Create (ADSDLL_LIBRARY, ADSDLL_VERSION, m_adsApi, portNumber);
    }

private:
  virtual long SyncWriteReq (AmsAddr       & amsAddr,
                             unsigned long   indexGroup,
                             unsigned long   indexOffset,
                             unsigned long   length,
                             void          * pData) override final
    {
      return AdsSyncWriteReq (&amsAddr, indexGroup, indexOffset, length, pData);
    }
  virtual long SyncReadWriteReq (AmsAddr       & amsAddr,
                                 unsigned long   cbReadLength,
                                 void          * pReadData,
                                 unsigned long   cbWriteLength,
                                 void          * pWriteData) override final
    {
      return AdsSyncReadWriteReq (&amsAddr, ADSIGRP_SYM_HNDBYNAME, 0, cbReadLength, pReadData, cbWriteLength, pWriteData);
    }
  virtual long SyncAddDeviceNotificationReq (AmsAddr               & amsAddr,
                                             unsigned long           indexOffset,
                                             AdsNotificationAttrib * adsNotificationAttrib,
                                             void                  * pNoteFunc,
                                             unsigned long           hUser,
                                             unsigned long         * pNotification) override final
    {
      return AdsSyncAddDeviceNotificationReq (&amsAddr,
                                              ADSIGRP_SYM_VALBYHND,
                                              indexOffset,
                                              adsNotificationAttrib,
                                              reinterpret_cast <PAdsNotificationFuncTC3> (pNoteFunc),
                                              hUser,
                                              pNotification);
    }
  virtual long SyncDelDeviceNotificationReq (AmsAddr       & amsAddr,
                                             unsigned long   hNotification) override final
    {
      return AdsSyncDelDeviceNotificationReq (&amsAddr, hNotification);
    }

  virtual long PortOpen (void) override final { return AdsPortOpen (); }
  virtual void PortClose (void) override final { AdsPortClose (); }
  virtual long GetLocalAddress (AmsAddr & amsAddr) override final { return AdsGetLocalAddress (&amsAddr); }
  virtual long GetDllVersion (void) override final { return AdsGetDllVersion (); }

  static CString                                const ADSDLL_LIBRARY;
  static CAdsDllVersion                         const ADSDLL_VERSION;

  static ProcAdsGetDllVersionTC3                      AdsGetDllVersion;
  static ProcAdsPortOpenTC3                           AdsPortOpen;
  static ProcAdsPortCloseTC3                          AdsPortClose;
  static ProcAdsGetLocalAddressTC3                    AdsGetLocalAddress;
  static ProcAdsSyncWriteReqTC3                       AdsSyncWriteReq;
  static ProcAdsSyncReadWriteReqTC3                   AdsSyncReadWriteReq;
  static ProcAdsSyncAddDeviceNotificationReqTC3       AdsSyncAddDeviceNotificationReq;
  static ProcAdsSyncDelDeviceNotificationReqTC3       AdsSyncDelDeviceNotificationReq;

  static CAdsApi                                const m_adsApi;

public:
  CTwinCATADS3 (CTwinCATADS3 const &) = delete;
  CTwinCATADS3 & operator = (CTwinCATADS3 const &) = delete;
};

CString                                const CTwinCATADS3::ADSDLL_LIBRARY                  (_T ("TcAdsDll.dll"));
CTwinCATADS3::CAdsDllVersion           const CTwinCATADS3::ADSDLL_VERSION                  { 2, 11, 0, 41 };

ProcAdsGetDllVersionTC3                      CTwinCATADS3::AdsGetDllVersion                (nullptr);
ProcAdsPortOpenTC3                           CTwinCATADS3::AdsPortOpen                     (nullptr);
ProcAdsPortCloseTC3                          CTwinCATADS3::AdsPortClose                    (nullptr);
ProcAdsGetLocalAddressTC3                    CTwinCATADS3::AdsGetLocalAddress              (nullptr);
ProcAdsSyncWriteReqTC3                       CTwinCATADS3::AdsSyncWriteReq                 (nullptr);
ProcAdsSyncReadWriteReqTC3                   CTwinCATADS3::AdsSyncReadWriteReq             (nullptr);
ProcAdsSyncAddDeviceNotificationReqTC3       CTwinCATADS3::AdsSyncAddDeviceNotificationReq (nullptr);
ProcAdsSyncDelDeviceNotificationReqTC3       CTwinCATADS3::AdsSyncDelDeviceNotificationReq (nullptr);

CTwinCATADS3::CAdsApi                  const CTwinCATADS3::m_adsApi
{
  CProcAds { reinterpret_cast <LPVOID *> (&AdsGetDllVersion),                _T ("_AdsGetDllVersion@0")                 },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsPortOpen),                     _T ("_AdsPortOpen@0")                      },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsPortClose),                    _T ("_AdsPortClose@0")                     },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsGetLocalAddress),              _T ("_AdsGetLocalAddress@4")               },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsSyncWriteReq),                 _T ("_AdsSyncWriteReq@20")                 },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsSyncReadWriteReq),             _T ("_AdsSyncReadWriteReq@28")             },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsSyncAddDeviceNotificationReq), _T ("_AdsSyncAddDeviceNotificationReq@28") },
  CProcAds { reinterpret_cast <LPVOID *> (&AdsSyncDelDeviceNotificationReq), _T ("_AdsSyncDelDeviceNotificationReq@8")  }
};

CString                   const CTwinCATADS::VAR_ACCELERATION      (_T ("Acceleration"));
CString                   const CTwinCATADS::VAR_DECELERATION      (_T ("Deceleration"));
CString                   const CTwinCATADS::VAR_JERK              (_T ("Jerk"));
CString                   const CTwinCATADS::VAR_POSITION          (_T ("Position"));
CString                   const CTwinCATADS::VAR_VELOCITY          (_T ("Velocity"));
CString                   const CTwinCATADS::VAR_DIRECTION         (_T ("Direction"));
CString                   const CTwinCATADS::VAR_BEGINMOTION       (_T ("BeginMotion"));
CString                   const CTwinCATADS::VAR_STOPMOTION        (_T ("StopMotion"));
CString                   const CTwinCATADS::VAR_MOTIONCOMPLETE    (_T ("MotionComplete"));
CString                   const CTwinCATADS::VAR_MOTIONSTOPPED     (_T ("MotionStopped"));
CString                   const CTwinCATADS::VAR_MOTIONFAULTED     (_T ("MotionFaulted"));
CString                   const CTwinCATADS::VAR_RUNPROGRAM        (_T ("RunProgram"));
CString                   const CTwinCATADS::VAR_STOPPROGRAM       (_T ("StopProgram"));
CString                   const CTwinCATADS::VAR_PROGRAMCOMPLETE   (_T ("ProgramComplete"));
CString                   const CTwinCATADS::VAR_FAULTCODE         (_T ("FaultCode"));
CString                   const CTwinCATADS::VAR_PROGRAMSTATUS     (_T ("ProgramStatus"));
CString                   const CTwinCATADS::VAR_ACTUALPOSITION    (_T ("ActualPosition"));
CString                   const CTwinCATADS::VAR_ACTUALVELOCITY    (_T ("ActualVelocity"));
CString                   const CTwinCATADS::VAR_ANALOGINPUTS      (_T ("IOAnalogTask.Inputs.AnalogInputs"));
CString                   const CTwinCATADS::VAR_ANALOGOUTPUTS     (_T ("IOAnalogTask.Outputs.AnalogOutputs"));
CString                   const CTwinCATADS::VAR_DISCRETEINPUTS    (_T ("IODiscreteTask.Inputs.DiscreteInputs"));
CString                   const CTwinCATADS::VAR_DISCRETEOUTPUTS   (_T ("IODiscreteTask.Outputs.DiscreteOutputs"));

CTwinCATADS::MC_Bool      const CTwinCATADS::MC_False              (0x00);
CTwinCATADS::MC_Bool      const CTwinCATADS::MC_True               (0x01);

CTwinCATADS::MC_Direction const CTwinCATADS::MC_None               (0);
CTwinCATADS::MC_Direction const CTwinCATADS::MC_Positive           (1);
CTwinCATADS::MC_Direction const CTwinCATADS::MC_Shortest           (2);
CTwinCATADS::MC_Direction const CTwinCATADS::MC_Negative           (3);
CTwinCATADS::MC_Direction const CTwinCATADS::MC_Current            (4);

DWORD                     const CTwinCATADS::DRIVE_STATUS_OK       (::DRIVE_STATUS_OK);

DWORD                     const CTwinCATADS::PROGRAM_NO_FAULT      (::DRIVE_STATUS_OK);
DWORD                     const CTwinCATADS::PROGRAM_STOPPED_FAULT (0x00EC);

CTwinCATADS::MC_UDInt     const CTwinCATADS::AXIS_NO_FAULT         (0x00000000);
CTwinCATADS::MC_UDInt     const CTwinCATADS::AXIS_STOPPED_FAULT    (0x00004B00);

std::array <CString, 268> const CTwinCATADS::m_programStatusMessage
{
  /* 0x0000 */ _T ("Drive initialization is incomplete"),
  /* 0x0001 */ _T ("Drive initialization is running"),
  /* 0x0002 */ _T ("Drive initialization is complete, drives OK"),
  /* 0x0003 */ _T ("Axis 1 has faulted"),
  /* 0x0004 */ _T ("Axis 2 has faulted"),
  /* 0x0005 */ _T ("Axis 3 has faulted"),
  /* 0x0006 */ _T ("Axis 4 has faulted"),
  /* 0x0007 */ _T ("Axis 5 has faulted"),
  /* 0x0008 */ _T ("Axis 6 has faulted"),
  /* 0x0009 */ _T ("Axis 7 has faulted"),
  /* 0x000A */ _T ("Axis 8 has faulted"),
  /* 0x000B */ _T ("E-Stop occurred; faulted"),
  /* 0x000C */ _T ("No air pressure; faulted"),
  /* 0x000D */ _T ("No vacuum; faulted"),
  /* 0x000E */ _T ("No control power; faulted"),
  /* 0x000F */ _T ("Initialization Program Timed Out"),
  /* 0x0010 */ _T ("Axis 9 has faulted"),
  /* 0x0011 */ _T ("Axis 10 has faulted"),
  /* 0x0012 */ _T ("Axis 11 has faulted"),
  /* 0x0013 */ _T ("Axis 12 has faulted"),
  /* 0x0014 */ _T ("Axis 13 has faulted"),
  /* 0x0015 */ _T ("Axis 14 has faulted"),
  /* 0x0016 */ _T ("Axis 15 has faulted"),
  /* 0x0017 */ _T ("Axis 16 has faulted"),
  /* 0x0018 */ _T ("Axis 17 has faulted"),
  /* 0x0019 */ _T ("Axis 18 has faulted"),
  /* 0x001A */ _T ("Axis 19 has faulted"),
  /* 0x001B */ _T ("Axis 20 has faulted"),
  /* 0x001C */ _T ("Axis 21 has faulted"),
  /* 0x001D */ _T ("Axis 22 has faulted"),
  /* 0x001E */ _T ("Axis 23 has faulted"),
  /* 0x001F */ _T ("Axis 24 has faulted"),
  /* 0x0020 */ _T ("Axis 25 has faulted"),
  /* 0x0021 */ _T ("Axis 26 has faulted"),
  /* 0x0022 */ _T ("Axis 27 has faulted"),
  /* 0x0023 */ _T ("Axis 28 has faulted"),
  /* 0x0024 */ _T ("Axis 29 has faulted"),
  /* 0x0025 */ _T ("Axis 30 has faulted"),
  /* 0x0026 */ _T ("Axis 31 has faulted"),
  /* 0x0027 */ _T ("Axis 32 has faulted"),
  /* 0x0028 */ _T ("Axis 33 has faulted"),
  /* 0x0029 */ _T ("Axis 34 has faulted"),
  /* 0x002A */ _T ("Axis 35 has faulted"),
  /* 0x002B */ _T ("Axis 36 has faulted"),
  /* 0x002C */ _T ("Axis 37 has faulted"),
  /* 0x002D */ _T ("Axis 38 has faulted"),
  /* 0x002E */ _T ("Axis 39 has faulted"),
  /* 0x002F */ _T ("Axis 40 has faulted"),
  /* 0x0030 */ _T ("SERCOS Fault: Unable to reset ring"),
  /* 0x0031 */ _T ("SERCOS Fault: Incorrect phase"),
  /* 0x0032 */ _T ("Ethercat fault: Unable to reset"),
  /* 0x0033 */ _T ("Unable to catch rotor"),
  /* 0x0034 */ _T ("Vacuum Pump overloaded"),
  /* 0x0035 */ _T ("Brake resistor overloaded"),
  /* 0x0036 */ _T ("Rotor fan overloaded"),
  /* 0x0037 */ _T ("Controller BK9000(Box1) has faulted or power reset not completed"),
  /* 0x0038 */ _T ("Controller BK9000(Box2) has faulted or power reset not completed"),
  /* 0x0039 */ _T ("Controller BK9000(Box3) has faulted or power reset not completed"),
  /* 0x003A */ _T ("Controller BK9000(Box4) has faulted or power reset not completed"),
  /* 0x003B */ _T ("Controller BK9000(Box5) has faulted or power reset not completed"),
  /* 0x003C */ _T ("Fieldbus IL2301-B900(Box1) has faulted or power reset not completed"),
  /* 0x003D */ _T ("Fieldbus IL2301-B900(Box2) has faulted or power reset not completed"),
  /* 0x003E */ _T ("Fieldbus IL2301-B900(Box3) has faulted or power reset not completed"),
  /* 0x003F */ _T ("Fieldbus IL2301-B900(Box4) has faulted or power reset not completed"),
  /* 0x0040 */ _T ("Fieldbus IL2301-B900(Box5) has faulted or power reset not completed"),
  /* 0x0041 */ _T ("Spill sensor 1 detected"),
  /* 0x0042 */ _T ("Spill sensor 2 detected"),
  /* 0x0043 */ _T ("Spill sensor 3 detected"),
  /* 0x0044 */ _T ("Spill sensor 4 detected"),
  /* 0x0045 */ _T ("Recirculating Pump has faulted"),
  /* 0x0046 */ _T ("Stirrer Pump has faulted"),
  /* 0x0047 */ _T ("X-Axis has faulted"),
  /* 0x0048 */ _T ("X-Axis 1 has faulted"),
  /* 0x0049 */ _T ("X-Axis 2 has faulted"),
  /* 0x004A */ _T ("Y-Axis has faulted"),
  /* 0x004B */ _T ("Y-Axis 1 has faulted"),
  /* 0x004C */ _T ("Y-Axis 2 has faulted"),
  /* 0x004D */ _T ("Z-Axis has faulted"),
  /* 0x004E */ _T ("Z-Axis 1 has faulted"),
  /* 0x004F */ _T ("Z-Axis 2 has faulted"),
  /* 0x0050 */ _T ("W-Axis has faulted"),
  /* 0x0051 */ _T ("W-Axis 1 has faulted"),
  /* 0x0052 */ _T ("W-Axis 2 has faulted"),
  /* 0x0053 */ _T ("Transfer Pump has faulted"),
  /* 0x0054 */ _T ("Conical Transfer Pump has faulted"),
  /* 0x0055 */ _T ("Reagent Pump has faulted"),
  /* 0x0056 */ _T ("Reagent Pump 1 has faulted"),
  /* 0x0057 */ _T ("Reagent Pump 2 has faulted"),
  /* 0x0058 */ _T ("Reagent Pump 3 has faulted"),
  /* 0x0059 */ _T ("Reagent Pump 4 has faulted"),
  /* 0x005A */ _T ("Reagent Removal Pump has faulted"),
  /* 0x005B */ _T ("Wash Pump has faulted"),
  /* 0x005C */ _T ("Wash Pump 1 has faulted"),
  /* 0x005D */ _T ("Wash Pump 2 has faulted"),
  /* 0x005E */ _T ("Wash Pump 3 has faulted"),
  /* 0x005F */ _T ("Wash Pump 4 has faulted"),
  /* 0x0060 */ _T ("Waste Pump has faulted"),
  /* 0x0061 */ _T ("Waste Pump 1 has faulted"),
  /* 0x0062 */ _T ("Waste Pump 2 has faulted"),
  /* 0x0063 */ _T ("Waste Pump 3 has faulted"),
  /* 0x0064 */ _T ("Waste Pump 4 has faulted"),
  /* 0x0065 */ _T ("Valve has faulted"),
  /* 0x0066 */ _T ("Valve 1 has faulted"),
  /* 0x0067 */ _T ("Valve 2 has faulted"),
  /* 0x0068 */ _T ("Valve 3 has faulted"),
  /* 0x0069 */ _T ("Valve 4 has faulted"),
  /* 0x006A */ _T ("Tip Pump 1 has faulted"),
  /* 0x006B */ _T ("Tip Pump 2 has faulted"),
  /* 0x006C */ _T ("Tip Pump 3 has faulted"),
  /* 0x006D */ _T ("Tip Pump 4 has faulted"),
  /* 0x006E */ _T ("Sample Well Cleaning Pump 1 has faulted"),
  /* 0x006F */ _T ("Sample Well Cleaning Pump 2 has faulted"),
  /* 0x0070 */ _T ("Sample Well Cleaning Pump 3 has faulted"),
  /* 0x0071 */ _T ("Sample Well Cleaning Pump 4 has faulted"),
  /* 0x0072 */ _T ("Cleaning Reagent Pump has faulted"),
  /* 0x0073 */ _T ("Aspirate Pump has faulted"),
  /* 0x0074 */ _T ("Bottle Wash Pump has faulted"),
  /* 0x0075 */ _T ("Fraction Collector X-Axis has faulted"),
  /* 0x0076 */ _T ("Fraction Collector Z-Axis has faulted"),
  /* 0x0077 */ _T ("Fraction Collector Reagent Pump has faulted"),
  /* 0x0078 */ _T ("Fraction Collector Wash Pump has faulted"),
  /* 0x0079 */ _T ("Fraction Collector Waste Pump has faulted"),
  /* 0x007A */ _T ("Loading Dock X-Axis has faulted"),
  /* 0x007B */ _T ("Loading Dock Y-Axis has faulted"),
  /* 0x007C */ _T ("Loading Dock Z-Axis has faulted"),
  /* 0x007D */ _T ("Loading Dock Aspirate Pump has faulted"),
  /* 0x007E */ _T ("Loading Dock Transfer HSC Pump has faulted"),
  /* 0x007F */ _T ("Loading Dock Transfer FC Pump has faulted"),
  /* 0x0080 */ _T ("Loading Dock Buffer Pump has faulted"),
  /* 0x0081 */ _T ("Loading Dock Wash Pump 1 has faulted"),
  /* 0x0082 */ _T ("Loading Dock Wash Pump 2 has faulted"),
  /* 0x0083 */ _T ("Loading Dock Wash Pump 3 has faulted"),
  /* 0x0084 */ _T ("Loading Dock Waste Pump has faulted"),
  /* 0x0085 */ _T ("Transfer HSC Pump has faulted"),
  /* 0x0086 */ _T ("Transfer FC Pump has faulted"),
  /* 0x0087 */ _T ("Buffer Pump has faulted"),
  /* 0x0088 */ _T ("Rotor Axis has faulted"),
  /* 0x0089 */ _T ("High Speed Centrifuge E-Stop has been depressed"),
  /* 0x008A */ _T ("High Speed Centrifuge Control Power is not present"),
  /* 0x008B */ _T ("High Speed Centrifuge Rotor Axis has faulted"),
  /* 0x008C */ _T ("High Speed Centrifuge X-Axis has faulted"),
  /* 0x008D */ _T ("High Speed Centrifuge Sonicator Axis has faulted"),
  /* 0x008E */ _T ("High Speed Centrifuge Aspirator Axis has faulted"),
  /* 0x008F */ _T ("High Speed Centrifuge Transfer FC Pump has faulted"),
  /* 0x0090 */ _T ("High Speed Centrifuge Buffer Pump has faulted"),
  /* 0x0091 */ _T ("High Speed Centrifuge Wash Pump 1 has faulted"),
  /* 0x0092 */ _T ("High Speed Centrifuge Wash Pump 2 has faulted"),
  /* 0x0093 */ _T ("High Speed Centrifuge Wash Pump 3 has faulted"),
  /* 0x0094 */ _T ("High Speed Centrifuge Waste Pump has faulted"),
  /* 0x0095 */ _T ("Waste Bottle is full"),
  /* 0x0096 */ _T ("Tip 1's Pump has faulted"),
  /* 0x0097 */ _T ("Tip 2's Pump has faulted"),
  /* 0x0098 */ _T ("Tip 3's Pump has faulted"),
  /* 0x0099 */ _T ("Tip 4's Pump has faulted"),
  /* 0x009A */ _T ("Tip 5's Pump has faulted"),
  /* 0x009B */ _T ("Tip 6's Pump has faulted"),
  /* 0x009C */ _T ("Tip 7's Pump has faulted"),
  /* 0x009D */ _T ("Tip 8's Pump has faulted"),
  /* 0x009E */ _T ("Reagent Pump 5 has faulted"),
  /* 0x009F */ _T ("Reagent Pump 6 has faulted"),
  /* 0x00A0 */ _T ("Reagent Pump 7 has faulted"),
  /* 0x00A1 */ _T ("Reagent Pump 8 has faulted"),
  /* 0x00A2 */ _T ("Tip Pump 5 has faulted"),
  /* 0x00A3 */ _T ("Tip Pump 6 has faulted"),
  /* 0x00A4 */ _T ("Tip Pump 7 has faulted"),
  /* 0x00A5 */ _T ("Tip Pump 8 has faulted"),
  /* 0x00A6 */ _T ("Controller BK9000 has faulted or power reset not completed"),
  /* 0x00A7 */ _T ("Fieldbus IL2301-B900 has faulted or power reset not completed"),
  /* 0x00A8 */ _T ("Prime Waste Pump has faulted"),
  /* 0x00A9 */ _T ("Quick Cleaning Reagent Pump has faulted"),
  /* 0x00AA */ _T ("Axis W Fault: Drive unable to reset"),
  /* 0x00AB */ _T ("Axis W1 Fault: Drive unable to reset"),
  /* 0x00AC */ _T ("Axis W2 Fault: Drive unable to reset"),
  /* 0x00AD */ _T ("Axis X Fault: Drive unable to reset"),
  /* 0x00AE */ _T ("Axis X1 Fault: Drive unable to reset"),
  /* 0x00AF */ _T ("Axis X2 Fault: Drive unable to reset"),
  /* 0x00B0 */ _T ("Axis Y Fault: Drive unable to reset"),
  /* 0x00B1 */ _T ("Axis Y1 Fault: Drive unable to reset"),
  /* 0x00B2 */ _T ("Axis Y2 Fault: Drive unable to reset"),
  /* 0x00B3 */ _T ("Axis Z Fault: Drive unable to reset"),
  /* 0x00B4 */ _T ("Axis Z1 Fault: Drive unable to reset"),
  /* 0x00B5 */ _T ("Axis Z2 Fault: Drive unable to reset"),
  /* 0x00B6 */ _T ("Axis W Fault: TwinCAT unable to reset"),
  /* 0x00B7 */ _T ("Axis W1 Fault: TwinCAT unable to reset"),
  /* 0x00B8 */ _T ("Axis W2 Fault: TwinCAT unable to reset"),
  /* 0x00B9 */ _T ("Axis X Fault: TwinCAT unable to reset"),
  /* 0x00BA */ _T ("Axis X1 Fault: TwinCAT unable to reset"),
  /* 0x00BB */ _T ("Axis X2 Fault: TwinCAT unable to reset"),
  /* 0x00BC */ _T ("Axis Y Fault: TwinCAT unable to reset"),
  /* 0x00BD */ _T ("Axis Y1 Fault: TwinCAT unable to reset"),
  /* 0x00BE */ _T ("Axis Y2 Fault: TwinCAT unable to reset"),
  /* 0x00BF */ _T ("Axis Z Fault: TwinCAT unable to reset"),
  /* 0x00C0 */ _T ("Axis Z1 Fault: TwinCAT unable to reset"),
  /* 0x00C1 */ _T ("Axis Z2 Fault: TwinCAT unable to reset"),
  /* 0x00C2 */ _T ("Ethercat Fault: Bus communication disrupted"),
  /* 0x00C3 */ _T ("Tiphead A is not in the up position: check air supply or limit switch"),
  /* 0x00C4 */ _T ("Tiphead B is not in the up position: check air supply or limit switch"),
  /* 0x00C5 */ _T ("Tiphead C is not in the up position: check air supply or limit switch"),
  /* 0x00C6 */ _T ("Gripper is not in the up position: check air supply or limit switch"),
  /* 0x00C7 */ _T ("Pipettor is in the unload position and cannot home"),
  /* 0x00C8 */ _T ("AllowBucketLift in incompatible state"),
  /* 0x00C9 */ _T ("Syringe #1 Fault: TwinCAT unable to reset"),
  /* 0x00CA */ _T ("Suspension Axis Fault: TwinCAT unable to reset"),
  /* 0x00CB */ _T ("Syringe #2 Fault: TwinCAT unable to reset"),
  /* 0x00CC */ _T ("Syringe #3 Fault: TwinCAT unable to reset"),
  /* 0x00CD */ _T ("Syringe #4 Fault: TwinCAT unable to reset"),
  /* 0x00CE */ _T ("Syringe #5 Fault: TwinCAT unable to reset"),
  /* 0x00CF */ _T ("Syringe #6 Fault: TwinCAT unable to reset"),
  /* 0x00D0 */ _T ("Syringe #7 Fault: TwinCAT unable to reset"),
  /* 0x00D1 */ _T ("Syringe #8 Fault: TwinCAT unable to reset"),
  /* 0x00D2 */ _T ("Rotary Valve Fault: TwinCAT unable to reset"),
  /* 0x00D3 */ _T ("Syringe #1 Fault: Drive unable to reset"),
  /* 0x00D4 */ _T ("Suspension Axis Fault: Drive unable to reset"),
  /* 0x00D5 */ _T ("Syringe #2 Fault: Drive unable to reset"),
  /* 0x00D6 */ _T ("Syringe #3 Fault: Drive unable to reset"),
  /* 0x00D7 */ _T ("Syringe #4 Fault: Drive unable to reset"),
  /* 0x00D8 */ _T ("Syringe #5 Fault: Drive unable to reset"),
  /* 0x00D9 */ _T ("Syringe #6 Fault: Drive unable to reset"),
  /* 0x00DA */ _T ("Syringe #7 Fault: Drive unable to reset"),
  /* 0x00DB */ _T ("Syringe #8 Fault: Drive unable to reset"),
  /* 0x00DC */ _T ("Rotary Valve Fault: Drive unable to reset"),
  /* 0x00DD */ _T ("Syringe #1 has faulted"),
  /* 0x00DE */ _T ("Suspension Axis has faulted"),
  /* 0x00DF */ _T ("Syringe #2 has faulted"),
  /* 0x00E0 */ _T ("Syringe #3 has faulted"),
  /* 0x00E1 */ _T ("Syringe #4 has faulted"),
  /* 0x00E2 */ _T ("Syringe #5 has faulted"),
  /* 0x00E3 */ _T ("Syringe #6 has faulted"),
  /* 0x00E4 */ _T ("Syringe #7 has faulted"),
  /* 0x00E5 */ _T ("Syringe #8 has faulted"),
  /* 0x00E6 */ _T ("Rotary Valve has faulted"),
  /* 0x00E7 */ _T ("Aspirate Program Timed Out"),
  /* 0x00E8 */ _T ("Aspirate Program: Move to the X1 Well Position Faulted"),
  /* 0x00E9 */ _T ("Aspirate Program: Move to ZAspiratePosition Faulted"),
  /* 0x00EA */ _T ("Aspirate Program: Move to ZCleanWellPosition Faulted"),
  /* 0x00EB */ _T ("Aspirate Program: Move to ZExitPosition Faulted"),
  /* 0x00EC */ _T ("User abort fault"),
  /* 0x00ED */ _T ("Deck Spill Sensor Detected"),
  /* 0x00EE */ _T ("Pump Spill Sensor Detected"),
  /* 0x00EF */ _T ("Reset Bus Coupler Failed"),
  /* 0x00F0 */ _T ("Reset Drive(s) Failed"),
  /* 0x00F1 */ _T ("Unable to Reset Tip Pump"),
  /* 0x00F2 */ _T ("Unable to Reset Tip Pump #1"),
  /* 0x00F3 */ _T ("Unable to Reset Tip Pump #2"),
  /* 0x00F4 */ _T ("Unable to Reset Tip Pump #3"),
  /* 0x00F5 */ _T ("Unable to Reset Tip Pump #4"),
  /* 0x00F6 */ _T ("Unable to Reset Bath Wash Pump"),
  /* 0x00F7 */ _T ("Unable to Reset Bath Waste Pump"),
  /* 0x00F8 */ _T ("Bath Wash Pump Faulted"),
  /* 0x00F9 */ _T ("Bath Waste Pump Faulted"),
  /* 0x00FA */ _T ("Tip Head #1 is not in the up position: check air supply or limit switch"),
  /* 0x00FB */ _T ("Tip Head #2 is not in the up position: check air supply or limit switch"),
  /* 0x00FC */ _T ("Tip Head #3 is not in the up position: check air supply or limit switch"),
  /* 0x00FD */ _T ("Tip Head #4 is not in the up position: check air supply or limit switch"),
  /* 0x00FE */ _T ("Tip Head(s) not in position. Check air supply or limit switch(es)"),
  /* 0x00FF */ _T ("CANOpen Faulted"),
  /* 0x0100 */ _T ("CANOpen Reset failed"),
  /* 0x0101 */ _T ("FB Reset Error"),
  /* 0x0102 */ _T ("FB Error"),
  /* 0x0103 */ _T ("Ethercat Fault : Enable to reset main bus coupler"),
  /* 0x0104 */ _T ("Ethercat Fault : Enable to reset drive"),
  /* 0x0105 */ _T ("Ethercat Fault : Enable to reset stacker bus coupler"),
  /* 0x0106 */ _T ("Ethercat Fault : Enable to reset local delidder bus coupler"),
  /* 0x0107 */ _T ("Ethercat Fault : Loss communication to main bus coupler"),
  /* 0x0108 */ _T ("Ethercat Fault : Loss communication to drive"),
  /* 0x0109 */ _T ("Ethercat Fault : Loss communication to stacker bus coupler"),
  /* 0x010A */ _T ("Ethercat Fault : Loss communication to local delidder bus coupler"),
  /* 0x010B */ _T ("Delidder is not in the up position : check air supply or limit switch")
};

std::map <DWORD, CString> const CTwinCATADS::m_adsErrorMessage
{
  { 0x4221, _T ("requested set velocity is not allowed") },
  { 0x422F, _T ("acceleration less than or equal to 0.0 is not allowed") },
  { 0x4230, _T ("absolute deceleration value less than or equal to 0.0 is not allowed") },
  { 0x4231, _T ("set velocity less than or equal to 0.0 is not allowed") },
  { 0x4260, _T ("controller not enabled") },
  { 0x4263, _T ("motion command issued when current command was not completed") },
  { 0x4357, _T ("limit switch triggered (negative direction)") },
  { 0x4358, _T ("limit switch triggered (positive direction)") },
  { 0x4359, _T ("set velocity not allowed") },
  { 0x4650, _T ("drive not ready for operation/drive hardware failed") },
  { 0x4B00, _T ("user abort fault") },
  { 0x4B09, _T ("TwinCAT axis is not ready or enabled") },
  { 0x4C00, _T ("TwinCAT issued MC_Stop, limit switch possibilly triggered") }
};

class CTwinCATADS::CSimAxis final
{
public:
  explicit CSimAxis (CTwinCATADS & twinCATADS, int axis);
  virtual ~CSimAxis () = default;

  void Run (void);

private:
  CTwinCATADS & m_twinCATADS;
  int const m_axis;
  bool m_beginMotion;
  double m_p;
  double m_v;
  double m_t0;
  double m_t1;
  double m_t2;
  double m_t3;

  inline static double Distance (double a, double t) { return a * t * t / 2.0; }

  static double const MS_PER_SECOND;

public:
  // copy construction and assignment not allowed for this class

  CSimAxis (const CSimAxis &) = delete;
  CSimAxis & operator = (const CSimAxis &) = delete;
};

double const CTwinCATADS::CSimAxis::MS_PER_SECOND (1000.0);

CTwinCATADS::CSimAxis::CSimAxis (CTwinCATADS & twinCATADS, int axis)
  : m_twinCATADS (twinCATADS)
  , m_axis (axis)
  , m_beginMotion (false)
  , m_p (0.0)
  , m_v (0.0)
  , m_t0 (0.0)
  , m_t1 (0.0)
  , m_t2 (0.0)
  , m_t3 (0.0)
{
  Run ();
}

void
CTwinCATADS::CSimAxis::Run (void)
{
  if (m_twinCATADS.m_beginMotion[0][m_axis] || m_twinCATADS.m_stopMotion[0][m_axis])
    {
      double const l_acceleration (m_twinCATADS.m_acceleration[m_axis]);
      double const l_deceleration (m_twinCATADS.m_deceleration[m_axis]);
      double const l_position (m_twinCATADS.m_position[m_axis]);
      double const l_velocity (m_twinCATADS.m_velocity[m_axis]);
      double const l_actualPosition (m_twinCATADS.m_actualPosition[1][m_axis]);
      DWORD  const l_direction (m_twinCATADS.m_direction[m_axis]);

      if (m_beginMotion)
        {
          double const l_t (static_cast <double> (PDCLib::GetTickCount ()) / MS_PER_SECOND);

          double l_p (0.0);
          double l_v (0.0);

          if (l_t < m_t3)
            {
              l_p += Distance (l_acceleration, std::min (l_t, m_t1) - m_t0);

              if (l_t < m_t1)
                {
                  // acceleration phase of trajectory...

                  l_v = l_acceleration * (l_t - m_t0);

                  if (m_twinCATADS.m_stopMotion[0][m_axis])
                    {
                      m_t1 = l_t;

                      m_t2 = l_t;

                      m_t3 = l_t + l_v / l_deceleration;
                    }
                }
              else
                {
                  l_p += m_v * (std::min (l_t, m_t2) - m_t1);

                  if (l_t < m_t2)
                    {
                      // target velocity phase of trajectory...

                      l_v  = m_v;

                      if (m_twinCATADS.m_stopMotion[0][m_axis])
                        {
                          m_t2 = l_t;

                          m_t3 = l_t + l_v / l_deceleration;
                        }
                    }
                  else
                    {
                      // deceleration phase of trajectory...

                      l_p += Distance (l_deceleration, m_t3 - m_t2);

                      l_p -= Distance (l_deceleration, m_t3 - l_t);

                      l_v  = l_deceleration * (m_t3 - l_t);
                    }
                }
            }
          else
            {
              l_p += Distance (l_acceleration, m_t1 - m_t0);

              l_p += m_v * (m_t2 - m_t1);

              l_p += Distance (l_deceleration, m_t3 - m_t2);

              if (m_twinCATADS.m_stopMotion[0][m_axis])
                {
                  m_twinCATADS.m_motionStopped[1][m_axis] = CTwinCATADS::MC_True;

                  m_twinCATADS.m_faultCode[1][m_axis] = CTwinCATADS::AXIS_STOPPED_FAULT;

                  PDCLib::Trace (_T ("stopped simulation axis (%ld)"), m_axis);
                }
              else
                {
                  m_twinCATADS.m_faultCode[1][m_axis] = CTwinCATADS::AXIS_NO_FAULT;

                  PDCLib::Trace (_T ("completed simulation axis (%ld)"), m_axis);
                }

              m_twinCATADS.m_motionComplete[1][m_axis] = MC_True;
            }

          if (l_direction)
            {
              m_twinCATADS.m_actualPosition[1][m_axis] = (l_direction == CTwinCATADS::MC_Positive) ? (m_p + l_p) : (m_p - l_p);
            }
          else
            {
              m_twinCATADS.m_actualPosition[1][m_axis] = (l_position > m_p) ? (m_p + l_p) : (m_p - l_p);
            }

          m_twinCATADS.m_actualVelocity[1][m_axis] = l_v;

          PDCLib::Trace (_T ("simulation (%ld) axis actual position: %.3f"), m_axis, m_twinCATADS.m_actualPosition[1][m_axis]);
          PDCLib::Trace (_T ("simulation (%ld) axis actual velocity: %.3f"), m_axis, m_twinCATADS.m_actualVelocity[1][m_axis]);
        }
      else if ((l_acceleration > 0.0) && (l_deceleration > 0.0) && (l_velocity > 0.0))
        {
          if (l_direction)
            {
              m_v = l_velocity;

              m_t0 = static_cast <double> (PDCLib::GetTickCount ()) / MS_PER_SECOND;

              m_t1 = m_t0 + (m_v / l_acceleration);

              m_t2 = std::numeric_limits <double>::infinity ();

              m_t3 = m_t2;

              m_p = l_actualPosition;

              m_beginMotion = true;

              PDCLib::Trace (_T ("started simulation axis (%ld)"), m_axis);
              PDCLib::Trace (_T ("target velocity    : %.3f"), m_v);
              PDCLib::Trace (_T ("acceleration begin : %.3f"), m_t0);
              PDCLib::Trace (_T ("acceleration end   : %.3f"), m_t1);
            }
          else
            {
              double const l_c (((1.0 / l_acceleration) + (1.0 / l_deceleration)) / 2.0);

              double const l_d (std::abs (l_position - l_actualPosition));

              m_v = std::min (std::sqrt (l_d / l_c), l_velocity);

              m_t0 = static_cast <double> (PDCLib::GetTickCount ()) / MS_PER_SECOND;

              m_t1 = m_t0 + (m_v / l_acceleration);

              m_t2 = m_t1 + ((m_v > 0.0) ? ((l_d - m_v * m_v * l_c) / m_v) : 0.0);

              m_t3 = m_t2 + (m_v / l_deceleration);

              m_p = l_actualPosition;

              m_beginMotion = true;

              PDCLib::Trace (_T ("started simulation axis (%ld)"), m_axis);
              PDCLib::Trace (_T ("actual position    : %.3f"), l_actualPosition);
              PDCLib::Trace (_T ("target position    : %.3f"), l_position);
              PDCLib::Trace (_T ("target velocity    : %.3f"), m_v);
              PDCLib::Trace (_T ("acceleration begin : %.3f"), m_t0);
              PDCLib::Trace (_T ("acceleration end   : %.3f"), m_t1);
              PDCLib::Trace (_T ("deceleration begin : %.3f"), m_t2);
              PDCLib::Trace (_T ("deceleration end   : %.3f"), m_t3);
            }
        }
      else
        {
          m_twinCATADS.m_motionComplete[1][m_axis] = CTwinCATADS::MC_True;
        }
    }
  else
    {
      m_twinCATADS.m_motionStopped[1][m_axis]  = CTwinCATADS::MC_False;
      m_twinCATADS.m_motionComplete[1][m_axis] = CTwinCATADS::MC_False;

      m_beginMotion = false;
    }
}

class CTwinCATADS::CSimProg final
{
public:
  explicit CSimProg (CTwinCATADS & twinCATADS, int prog);
  virtual ~CSimProg () = default;

  void Run (void);

private:
  CTwinCATADS & m_twinCATADS;
  int const m_prog;
  bool m_runProgram;
  ULONGLONG m_t;

  static ULONGLONG const RUN_PROGRAM_DELAY;

public:
  // copy construction and assignment not allowed for this class

  CSimProg (const CSimProg &) = delete;
  CSimProg & operator = (const CSimProg &) = delete;
};

ULONGLONG const CTwinCATADS::CSimProg::RUN_PROGRAM_DELAY (5000);

CTwinCATADS::CSimProg::CSimProg (CTwinCATADS & twinCATADS, int prog)
  : m_twinCATADS (twinCATADS)
  , m_prog (prog)
  , m_runProgram (false)
  , m_t (0)
{
  Run ();
}

void
CTwinCATADS::CSimProg::Run (void)
{
  if (m_twinCATADS.m_runProgram[0][m_prog])
    {
      if (m_runProgram)
        {
          if (!m_twinCATADS.m_runProgram[0][m_prog])
            {
              m_twinCATADS.m_programStatus[1][m_prog]   = CTwinCATADS::PROGRAM_STOPPED_FAULT;

              m_twinCATADS.m_programComplete[1][m_prog] = CTwinCATADS::MC_True;

              PDCLib::Trace (_T ("stopped simulation program (%ld)"), m_prog);
            }
          else if ((PDCLib::GetTickCount () - m_t) > RUN_PROGRAM_DELAY)
            {
              m_twinCATADS.m_programStatus[1][m_prog]   = CTwinCATADS::PROGRAM_NO_FAULT;

              m_twinCATADS.m_programComplete[1][m_prog] = CTwinCATADS::MC_True;

              PDCLib::Trace (_T ("completed simulation program (%ld)"), m_prog);
            }
        }
      else
        {
          m_t = PDCLib::GetTickCount ();

          m_runProgram = true;

          PDCLib::Trace (_T ("started simulation program (%ld)"), m_prog);
        }
    }
  else
    {
      m_twinCATADS.m_programComplete[1][m_prog] = CTwinCATADS::MC_False;

      m_runProgram = false;
    }
}

CTwinCATADS::CTwinCATADS (int  controllerId,
                          int  numAxes,
                          int  numPrograms,
                          bool simulationMode)
  : m_controllerId (PDCLib::StringWithFormat (_T ("Controller%ld."), controllerId))
  , m_acceleration (numAxes, 0.0)
  , m_deceleration (numAxes, 0.0)
  , m_jerk (numAxes, 0.0)
  , m_position (numAxes, 0.0)
  , m_velocity (numAxes, 0.0)
  , m_direction (numAxes, MC_None)
  , m_stopProgram (numPrograms, MC_False)
  , m_analogInputs (XShim <SAnalogInputs>::size)
  , m_discreteInputs (XShim <SDiscreteInputs>::size)
{
  ASSERT (controllerId >= 0);
  ASSERT (numAxes >= 0);
  ASSERT (numPrograms >= 0);

  AllocInputs (m_beginMotion, numAxes, MC_False, MC_True);
  AllocInputs (m_stopMotion, numAxes, MC_False, MC_True);
  AllocInputs (m_motionComplete, numAxes);
  AllocInputs (m_motionStopped, numAxes);
  AllocInputs (m_motionFaulted, numAxes);
  AllocInputs (m_runProgram, numPrograms, MC_False, MC_True);
  AllocInputs (m_programComplete, numPrograms);
  AllocInputs (m_faultCode, numAxes);
  AllocInputs (m_programStatus, numPrograms);
  AllocInputs (m_actualPosition, numAxes);
  AllocInputs (m_actualVelocity, numAxes);

  if (simulationMode)
    {
      for (int l_axis (0); l_axis < numAxes; ++l_axis)
        {
          CSimAxisPtr l_simAxis (new CSimAxis (*this, l_axis));

          m_simAxis.push_back (l_simAxis);
        }

      for (int l_prog (0); l_prog < numPrograms; ++l_prog)
        {
          CSimProgPtr l_simProg (new CSimProg (*this, l_prog));

          m_simProg.push_back (l_simProg);
        }
    }
}

CTwinCATADS::~CTwinCATADS ()
{
}

bool
CTwinCATADS::Create (void)
{
  return Create_ ();
}

bool
CTwinCATADS::Create (WORD analogPortNumber, WORD discretePortNumber)
{
  if (analogPortNumber == discretePortNumber)
    {
      m_errorMessage = _T ("analog and discrete port numbers must be unique");
    }
  else if ((analogPortNumber < AMSPORT_R0_IO) || (discretePortNumber < AMSPORT_R0_IO))
    {
      m_errorMessage = _T ("analog or discrete port number is out of range");
    }
  else if ((analogPortNumber < AMSPORT_R0_SPS) && (discretePortNumber < AMSPORT_R0_SPS))
    {
      return Create_ (analogPortNumber, discretePortNumber);
    }
  else
    {
      m_errorMessage = _T ("analog or discrete port number is out of range");
    }

  return false;
}

void
CTwinCATADS::UpdateInputs (void)
{
  std::unique_lock <std::mutex> l_notificationGate { m_notificationGate };

  UpdateInputs (m_actualPosition);
  UpdateInputs (m_actualVelocity);
  UpdateInputs (m_motionComplete);
  UpdateInputs (m_motionStopped);
  UpdateInputs (m_motionFaulted);
  UpdateInputs (m_faultCode);
  UpdateInputs (m_programComplete);
  UpdateInputs (m_programStatus);
}

bool
CTwinCATADS::UpdateOutputs (void)
{
  if (UpdateOutputs_ (VAR_BEGINMOTION, m_beginMotion, m_motionComplete) &&
      UpdateOutputs_ (VAR_STOPMOTION, m_stopMotion, m_motionStopped) &&
      UpdateOutputs (VAR_RUNPROGRAM, m_runProgram, m_programComplete))
    {
      for (auto&& l_simAxis : m_simAxis)
        {
          l_simAxis->Run ();
        }

      for (auto&& l_simProg : m_simProg)
        {
          l_simProg->Run ();
        }

      return true;
    }

  return false;
}

bool
CTwinCATADS::SetAcceleration (int axis, double acceleration)
{
  return SetVariable_ (EADSInstance::PLC, VAR_ACCELERATION, m_acceleration, axis, acceleration);
}

bool
CTwinCATADS::SetDeceleration (int axis, double deceleration)
{
  return SetVariable_ (EADSInstance::PLC, VAR_DECELERATION, m_deceleration, axis, deceleration);
}

bool
CTwinCATADS::SetDirection (int axis, MC_Direction direction)
{
  return SetVariable_ (EADSInstance::PLC, VAR_DIRECTION, m_direction, axis, direction);
}

bool
CTwinCATADS::SetJerk (int axis, double jerk)
{
  return SetVariable_ (EADSInstance::PLC, VAR_JERK, m_jerk, axis, jerk);
}

bool
CTwinCATADS::SetPosition (int axis, double position)
{
  return SetVariable_ (EADSInstance::PLC, VAR_POSITION, m_position, axis, position);
}

bool
CTwinCATADS::SetVelocity (int axis, double velocity)
{
  return SetVariable_ (EADSInstance::PLC, VAR_VELOCITY, m_velocity, axis, velocity);
}

bool CTwinCATADS::SetJogMode (int axis, double slewSpeed)
{
  return SetDirection (axis, (slewSpeed < 0.0) ? MC_Negative : MC_Positive) && SetVelocity (axis, std::abs (slewSpeed));
}

void CTwinCATADS::BeginMotion (int axis)
{
  m_beginMotion[0][axis] = MC_True;
}

void
CTwinCATADS::StopMotion (int axis)
{
  m_stopMotion[0][axis]  = MC_True;
  m_beginMotion[0][axis] = MC_False;
}

bool
CTwinCATADS::IsMotionComplete (int axis) const
{
  return (m_stopMotion[0][axis] ||
          m_beginMotion[0][axis] ||
          m_motionStopped[0][axis] ||
          m_motionComplete[0][axis]) ? false : true;
}

bool
CTwinCATADS::IsMotionFaulted (int axis) const
{
  return (m_motionFaulted[0][axis] == MC_True) && (m_faultCode[0][axis] != AXIS_STOPPED_FAULT);
}

DWORD
CTwinCATADS::GetFaultCode (int axis) const
{
  return m_faultCode[0][axis];
}

double
CTwinCATADS::GetPosition (int axis) const
{
  return m_actualPosition[0][axis];
}

double
CTwinCATADS::GetVelocity (int axis) const
{
  return m_actualVelocity[0][axis];
}

bool
CTwinCATADS::SetVariable (const CString & identifier, int value)
{
  return SetVariable_ (EADSInstance::PLC, identifier, value);
}

bool
CTwinCATADS::SetVariable (const CString & identifier, BYTE value)
{
  return SetVariable_ (EADSInstance::PLC, identifier, value);
}

bool
CTwinCATADS::SetVariable (const CString & identifier, double value)
{
  return SetVariable_ (EADSInstance::PLC, identifier, value);
}

bool
CTwinCATADS::SetVariable (const CString & identifier, const std::vector <int> & value)
{
  return SetVariable_ (EADSInstance::PLC, identifier, value);
}

bool
CTwinCATADS::SetVariable (const CString & identifier, const std::vector <BYTE> & value)
{
  return SetVariable_ (EADSInstance::PLC, identifier, value);
}

bool
CTwinCATADS::SetVariable (const CString & identifier, const std::vector <double> & value)
{
  return SetVariable_ (EADSInstance::PLC, identifier, value);
}

bool
CTwinCATADS::RunProgram (int identifier, bool stopEnabled)
{
  if (!stopEnabled || SetVariable_ (EADSInstance::PLC, VAR_STOPPROGRAM, m_stopProgram, identifier, MC_False))
    {
      m_runProgram[0][identifier] = MC_True;

      return true;
    }

  return false;
}

bool
CTwinCATADS::StopProgram (int identifier)
{
  return SetVariable_ (EADSInstance::PLC, VAR_STOPPROGRAM, m_stopProgram, identifier, MC_True);
}

bool
CTwinCATADS::IsProgramComplete (int identifier) const
{
  return (m_runProgram[0][identifier] || m_programComplete[0][identifier]) ? false : true;
}

DWORD
CTwinCATADS::GetProgramStatus (int identifier) const
{
  return m_programStatus[0][identifier];
}

CString
CTwinCATADS::GetErrorMessage (void) const
{
  return m_errorMessage;
}

CString
CTwinCATADS::GetErrorMessage (DWORD programStatus)
{
  if (programStatus < m_programStatusMessage.size ())
    {
      return m_programStatusMessage[programStatus];
    }
  else
    {
      return PDCLib::StringWithFormat (_T ("unrecognized TwinCAT ADS program status: 0x%08X"), programStatus);
    }
}

CString
CTwinCATADS::GetADSErrorMessage (DWORD faultCode)
{
  if (auto const l_adsErrorMessage (m_adsErrorMessage.find (faultCode)); l_adsErrorMessage == m_adsErrorMessage.end ())
    {
      return PDCLib::StringWithFormat (_T ("unrecognized TwinCAT ADS fault code: 0x%08X"), faultCode);
    }
  else
    {
      return PDCLib::StringWithFormat (_T ("%s: 0x%08X"), (LPCTSTR) std::get <1> (*l_adsErrorMessage), std::get <0> (*l_adsErrorMessage));
    }
}

HMODULE
CTwinCATADS::GetModuleHandle (void)
{
  return ITwinCATADS::GetModuleHandle ();
}

void
CTwinCATADS::UpdateInputs (SAnalogInputs * analogInputs, SDiscreteInputs * discreteInputs)
{
  std::unique_lock <std::mutex> l_notificationGate { m_notificationGate };

  UpdateInputs (m_analogInputs, analogInputs);
  UpdateInputs (m_discreteInputs, discreteInputs);
}

void
CTwinCATADS::UpdateOutputs (SAnalogOutputs const * analogOutputs, SDiscreteOutputs const * discreteOutputs)
{
  SetVariable_ (EADSInstance::AIO, VAR_ANALOGOUTPUTS, m_analogOutputs, analogOutputs);
  SetVariable_ (EADSInstance::DIO, VAR_DISCRETEOUTPUTS, m_discreteOutputs, discreteOutputs);
}

bool
CTwinCATADS::Create_ (WORD analogPortNumber, WORD discretePortNumber)
{
  if (m_simAxis.empty () && m_simProg.empty ())
    {
      if (Create <CTwinCATADS3> (AMSPORT_R0_PLC_TC3))
        {
          if (RegisterNotification (EADSInstance::PLC, VAR_ACTUALPOSITION, m_actualPosition, OnActualPositionTC3) &&
              RegisterNotification (EADSInstance::PLC, VAR_ACTUALVELOCITY, m_actualVelocity, OnActualVelocityTC3) &&
              RegisterNotification (EADSInstance::PLC, VAR_MOTIONCOMPLETE, m_motionComplete, OnMotionCompleteTC3) &&
              RegisterNotification (EADSInstance::PLC, VAR_MOTIONSTOPPED, m_motionStopped, OnMotionStoppedTC3) &&
              RegisterNotification (EADSInstance::PLC, VAR_MOTIONFAULTED, m_motionFaulted, OnMotionFaultedTC3) &&
              RegisterNotification (EADSInstance::PLC, VAR_FAULTCODE, m_faultCode, OnFaultCodeTC3) &&
              RegisterNotification (EADSInstance::PLC, VAR_PROGRAMCOMPLETE, m_programComplete, OnProgramCompleteTC3) &&
              RegisterNotification (EADSInstance::PLC, VAR_PROGRAMSTATUS, m_programStatus, OnProgramStatusTC3))
            {
              if ((analogPortNumber == 0) && (discretePortNumber == 0))
                {
                  return UpdateOutputs ();
                }
              else if (Create <CTwinCATADS3> (analogPortNumber) && Create <CTwinCATADS3> (discretePortNumber))
                {
                  return RegisterNotification (EADSInstance::AIO, VAR_ANALOGINPUTS, m_analogInputs, OnAnalogInputsTC3) &&
                         RegisterNotification (EADSInstance::DIO, VAR_DISCRETEINPUTS, m_discreteInputs, OnDiscreteInputsTC3) &&
                         UpdateOutputs ();
                }
            }
        }
      else if (Create <CTwinCATADS2> (AMSPORT_R0_PLC_RTS1))
        {
          if (RegisterNotification (EADSInstance::PLC, VAR_ACTUALPOSITION, m_actualPosition, OnActualPositionTC2) &&
              RegisterNotification (EADSInstance::PLC, VAR_ACTUALVELOCITY, m_actualVelocity, OnActualVelocityTC2) &&
              RegisterNotification (EADSInstance::PLC, VAR_MOTIONCOMPLETE, m_motionComplete, OnMotionCompleteTC2) &&
              RegisterNotification (EADSInstance::PLC, VAR_MOTIONSTOPPED, m_motionStopped, OnMotionStoppedTC2) &&
              RegisterNotification (EADSInstance::PLC, VAR_MOTIONFAULTED, m_motionFaulted, OnMotionFaultedTC2) &&
              RegisterNotification (EADSInstance::PLC, VAR_FAULTCODE, m_faultCode, OnFaultCodeTC2) &&
              RegisterNotification (EADSInstance::PLC, VAR_PROGRAMCOMPLETE, m_programComplete, OnProgramCompleteTC2) &&
              RegisterNotification (EADSInstance::PLC, VAR_PROGRAMSTATUS, m_programStatus, OnProgramStatusTC2))
            {
              if ((analogPortNumber == 0) && (discretePortNumber == 0))
                {
                  return UpdateOutputs ();
                }
              else if (Create <CTwinCATADS2> (analogPortNumber) && Create <CTwinCATADS2> (discretePortNumber))
                {
                  return RegisterNotification (EADSInstance::AIO, VAR_ANALOGINPUTS, m_analogInputs, OnAnalogInputsTC2) &&
                         RegisterNotification (EADSInstance::DIO, VAR_DISCRETEINPUTS, m_discreteInputs, OnDiscreteInputsTC2) &&
                         UpdateOutputs ();
                }
            }
        }

      return false;
    }

  return UpdateOutputs ();
}

template <typename T> bool
CTwinCATADS::Create (WORD portNumber)
{
  try
    {
      auto const l_twinCATADS (std::make_shared <T> ());

      l_twinCATADS->Create (portNumber);

      m_twinCATADS.emplace_back (l_twinCATADS);

      return true;
    }
  catch (CString const & errorMessage)
    {
      PDCLib::Trace (_T ("%s"), (LPCTSTR) errorMessage);

      m_errorMessage = errorMessage;
    }

  return false;
}

bool
CTwinCATADS::UpdateOutputs (CString                              const & identifier,
                            std::vector <std::vector <MC_Bool> >       & reqVariable)
{
  // check for pending requests...

  if (reqVariable[0] == reqVariable[1])
    {
      // no pending requests...

      return true;
    }
  else if (SetVariable_ (EADSInstance::PLC, identifier, reqVariable[0]))
    {
      // clear pending requests...

      reqVariable[1] = reqVariable[0];

      return true;
    }

  return false;
}

bool
CTwinCATADS::UpdateOutputs (CString                              const & identifier,
                            std::vector <std::vector <MC_Bool> >       & reqVariable,
                            std::vector <std::vector <MC_Bool> > const & ackVariable)
{
  if (!reqVariable.empty ())
    {
      for (int l_i (0); static_cast <size_t> (l_i) < reqVariable[0].size (); ++l_i)
        {
          // check for acknowledgment...

          if (reqVariable[0][l_i] && ackVariable[0][l_i])
            {
              // request acknowledged, clear request...

              reqVariable[0][l_i] = MC_False;
            }
        }

      return UpdateOutputs (identifier, reqVariable);
    }

  return true;
}

bool
CTwinCATADS::UpdateOutputs_ (CString                              const & identifier,
                             std::vector <std::vector <MC_Bool> >       & reqVariable,
                             std::vector <std::vector <MC_Bool> > const & ackVariable)
{
  if (!reqVariable.empty ())
    {
      for (int l_i (0); static_cast <size_t> (l_i) < reqVariable[0].size (); ++l_i)
        {
          if ((reqVariable[0][l_i] == MC_True) && (reqVariable[1][l_i] == MC_False))
            {
              // pending request, clear any prior error...

              m_motionFaulted[0][l_i] = m_motionFaulted[1][l_i] = MC_False;
            }
          else if (reqVariable[0][l_i] && (ackVariable[0][l_i] || IsMotionFaulted (l_i)))
            {
              // positive acknowledgment or error (not axis stopped), clear request...

              reqVariable[0][l_i] = MC_False;
            }
        }

      return UpdateOutputs (identifier, reqVariable);
    }

  return true;
}

template <typename T> void
CTwinCATADS::AllocInputs (         std::vector <std::vector <T> >       & buffer,
                          typename std::vector <T>::size_type             size,
                                   T                              const & initValue)
{
  AllocInputs (buffer, size, initValue, initValue);
}

template <typename T> void
CTwinCATADS::AllocInputs (         std::vector <std::vector <T> >       & buffer,
                          typename std::vector <T>::size_type             size,
                                   T                              const & initValue1,
                                   T                              const & initValue2)
{
  ASSERT (buffer.empty ());

  if (size > 0)
    {
      buffer.resize (2);

      buffer[0].resize (size, initValue1);
      buffer[1].resize (size, initValue2);
    }
}

template <typename T> void
CTwinCATADS::UpdateInputs (std::vector <std::vector <T> > & buffer)
{
  if (buffer.empty ())
    {
      return;
    }

  buffer[0] = buffer[1];
}

template <typename T, typename U> bool
CTwinCATADS::RegisterNotification (EADSInstance            adsInstance,
                                   CString         const & identifier,
                                   std::vector <T> const & variable,
                                   U                       pNoteFunc)
{
  if (!variable.empty ())
    {
      try
        {
          m_twinCATADS[static_cast <int> (adsInstance)]->RegisterNotification (GetSymbolName (adsInstance, identifier),
                                                                               variable.size () * sizeof (T),
                                                                               pNoteFunc,
                                                                               this);
        }
      catch (CString const & errorMessage)
        {
          m_errorMessage = errorMessage;

          return false;
        }
    }

  return true;
}

template <typename T> bool
CTwinCATADS::SetVariable_ (EADSInstance adsInstance, CString const & identifier, std::vector <T> & value, int index, T value_)
{
  value[index] = value_;

  return SetVariable_ (adsInstance, identifier, value);
}

template <typename T> bool
CTwinCATADS::SetVariable_ (EADSInstance adsInstance, CString const & identifier, T const & value)
{
  if (static_cast <size_t> (adsInstance) < m_twinCATADS.size ())
    {
      try
        {
          m_twinCATADS[static_cast <int> (adsInstance)]->SetVariable (GetSymbolName (adsInstance, identifier), sizeof (T), &value);
        }
      catch (CString const & errorMessage)
        {
          m_errorMessage = errorMessage;

          return false;
        }
    }

  return true;
}

template <typename T> bool
CTwinCATADS::SetVariable_ (EADSInstance adsInstance, CString const & identifier, std::vector <T> const & value)
{
  if (static_cast <size_t> (adsInstance) < m_twinCATADS.size ())
    {
      try
        {
          m_twinCATADS[static_cast <int> (adsInstance)]->SetVariable (GetSymbolName (adsInstance, identifier), value.size () * sizeof (T), &value[0]);
        }
      catch (CString const & errorMessage)
        {
          m_errorMessage = errorMessage;

          return false;
        }
    }

  return true;
}

template <typename T> void
CTwinCATADS::CopyVariable (AdsNotificationHeader * pNotification, std::vector <T> & variable)
{
  std::unique_lock <std::mutex> l_notificationGate { m_notificationGate };

  if (pNotification->cbSampleSize == (variable.size () * sizeof (T)))
    {
      auto const l_pData (reinterpret_cast <T const *> (ADSNOTIFICATION_PDATA (pNotification)));

      std::copy (l_pData, l_pData + variable.size (), variable.begin ());
    }
}

CString
CTwinCATADS::GetSymbolName (EADSInstance adsInstance, CString const & identifier) const
{
  if (adsInstance == EADSInstance::PLC)
    {
      return m_controllerId + identifier;
    }

  return identifier;
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
//  10/27/2005  MCC     initial revision
//  01/17/2007  MCC     baselined Beckhoff timer state machine class
//  02/16/2007  MCC     baselined G2 PinTool reset state machine
//  08/15/2007  MCC     modified to clear any prior fault on motion trigger
//  07/25/2008  MCC     implemented support for drive status error messages
//  08/07/2008  MCC     updated drive status error message table
//  09/16/2008  MCC     added string to vector helper subroutine
//  09/25/2008  MCC     baselined TwinCAT DLL project
//  09/30/2008  MCC     updated drive status error message table
//  02/12/2009  MCC     modified axis identification in error messages
//  02/20/2009  MCC     updated drive status error message table
//  06/21/2010  MCC     added Microsoft SDL include files
//  12/03/2010  MCC     updated drive status error message table
//  04/07/2011  MEG     updated drive status error message table for Raven
//  09/27/2011  MCC     modified motion faulted condition
//  10/17/2011  MCC     updated drive status error message table for ISMAC
//  11/03/2011  MCC     implemented support for soft reset
//  11/11/2011  MCC     modified to simulate position
//  05/09/2012  TAN     updated aspirate program error message table for G2Dispenser
//  07/19/2012  TAN     converted TwinCATADS hex fault code to error message
//  07/24/2012  TAN     added TwinCATADS fault code 0x4B09 and error message
//  03/29/2013  MCC     implemented stop program interface
//  04/02/2013  MCC     added error checking to stop program interface
//  06/06/2014  MCC     implemented support for TwinCAT ADS simulation mode
//  11/19/2014  MCC     corrected code analysis issues
//  12/17/2014  MCC     implemented critical sections with C++11 concurrency
//  01/09/2015  MCC     templatized number of elements macro
//  01/14/2015  MCC     corrected for-range syntax
//  01/14/2015  MCC     modified for-range to use universal references
//  03/12/2015  MCC     updated drive status error message table for FF-Station
//  04/13/2017  MEG     added cast to CString for resolving conversion warning
//  06/04/2018  MCC     implemented support for TwinCAT ADS I/O interface
//  06/06/2018  MCC     implemented support for TwinCAT 3 ADS interface
//  06/07/2018  MCC     optimized support for TwinCAT ADS I/O interface
//  06/07/2018  MCC     optimized support for TwinCAT ADS I/O interface
//  06/26/2018  TAN     modified to use port 851 for TwinCAT 3 ADS interface
//  08/07/2018  MCC     corrected problem with TwinCAT 3 ADS detection
//  08/08/2018  MCC     implemented more robust TwinCAT 3 ADS detection
//  08/14/2018  MCC     updated numeric conversion templates
//  08/16/2018  MCC     refactored TwinCAT 3 ADS interface
//  08/16/2018  MCC     refactored TwinCAT 3 ADS interface further
//  08/17/2018  MCC     implemented support for R0 access through TwinCAT ADS
//  08/17/2018  MCC     corrected problem with TwinCAT ADS simulation mode
//  08/20/2018  MCC     removed superfluous null pointer check
//  08/21/2018  MCC     corrected problem with TwinCAT ADS variable names
//  08/23/2018  MCC     corrected problem with TwinCAT ADS variable names again
//  08/23/2018  MCC     modified to log version of loaded TwinCAT module
//  08/24/2018  MCC     updated TwinCAT ADS error message map
//  08/24/2018  MCC     made TwinCAT ADS controller identifier constant
//  10/24/2018  MCC     updated TwinCAT ADS program status message table
//
// ============================================================================
