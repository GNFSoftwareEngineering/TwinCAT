#if !defined (UTILITY_H)
#define UTILITY_H

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
//            Name: Utility.h
//
//     Description: Utility declarations
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: utility.h %
//        %version: 38 %
//          %state: %
//         %cvtype: incl %
//     %derived_by: mconner %
//  %date_modified: %
//
// ============================================================================

#if _MSC_VER > 1000
#pragma once
#endif

namespace PDCLib
{
#if defined (_PDCLIB_EXPORT)
#define __DECLSPEC __declspec (dllexport)
#else
#define __DECLSPEC __declspec (dllimport)
#endif

#if (_MANAGED != 1) && (_M_CEE != 1)
using CMutex      = std::recursive_mutex;
using CLockGuard  = std::lock_guard <CMutex>;
using CUniqueLock = std::unique_lock <CMutex>;
#endif

extern CLSID const __DECLSPEC CLSID_DOMDocument60;
extern CLSID const __DECLSPEC CLSID_XMLSchemaCache60;

void __DECLSPEC DDV_FileName (CDataExchange * pDX, CString const & fileName, bool inhibitFileNotFound = true);;
void __DECLSPEC DDV_DirectoryName (CDataExchange * pDX, CString const & dirName);
void __DECLSPEC DDV_NonEmptyText (CDataExchange * pDX, CString const & text);
void __DECLSPEC DDV_HostName (CDataExchange * pDX, CString const & hostName);
void __DECLSPEC DDV_Regex (CDataExchange * pDX, CString const & text, CString const & regex);
void __DECLSPEC DDV_Password (CDataExchange * pDX, CString const & password);

void __DECLSPEC DDX_IPAddress (CDataExchange * pDX, int nIDC, CString & ipAddressVal);

CString __DECLSPEC GetErrorMessage (DWORD messageId);
CString __DECLSPEC GetErrorMessage (CException * e, bool autoDelete = false);
CString __DECLSPEC GetErrorMessage (_com_error const & e);

CString __DECLSPEC StringWithFormat (LPCTSTR format, ...);

void __DECLSPEC ThrowStringException (LPCTSTR format, ...);

void __DECLSPEC TESTHR (HRESULT x);

HRESULT __DECLSPEC CoInitialize (LPVOID pvReserved = nullptr);

void __DECLSPEC MessageBoxStop (LPCTSTR format, ...);
void __DECLSPEC MessageBoxOk (LPCTSTR format, ...);
bool __DECLSPEC MessageBoxOkCancel (LPCTSTR format, ...);
bool __DECLSPEC MessageBoxYesNo (LPCTSTR format, ...);

CString __DECLSPEC TerminatePath (const CString & pathName);

void __DECLSPEC InitHost (SOCKADDR_IN & host, const CString & hostAddress, UINT hostPort);
CString __DECLSPEC GetHostAddress (SOCKADDR_IN const & host);

CString __DECLSPEC GetAppDataFolder (void);
CString __DECLSPEC GetAppProgFolder (void);
CString __DECLSPEC GetTempFileName (void);
CString __DECLSPEC GetRegKey (void);
CString __DECLSPEC GetRegSection (void);

WORD __DECLSPEC GetCRC (void const * buf, size_t len, WORD crc = 0);

CTime __DECLSPEC GetFileWriteTime (HANDLE hFile);

int __DECLSPEC GenerateDump (PEXCEPTION_POINTERS pExceptionPointers);

void __DECLSPEC ReportEvent (WORD eventType, LPCTSTR format, ...);

IXMLDOMDocumentPtr __DECLSPEC LoadXML (WORD idResource, CString const & xmlSource, bool loadIndirect = true, HMODULE hModule = nullptr);

ULONGLONG __DECLSPEC GetTickCount (void);

CString __DECLSPEC ReadProfileEncrypted (CString const & entry, CString const & value);
bool __DECLSPEC WriteProfileEncrypted (CString const & entry, CString const & value);

template <typename T, size_t N> size_t NUM_ELEMENTS (T (&)[N]) { return N; }

LONGLONG __DECLSPEC GetFileSize (CString const & fileName);

#ifdef _DEBUG
void __DECLSPEC Trace (LPCTSTR format, ...);
#else
inline void Trace (LPCTSTR format, ...) { UNUSED_ALWAYS (format); }
#endif

template <typename T> void
StringToVector (CString const & cs, std::vector <T> & vt, TCHAR tc = _T ('\0'))
{
  vt.reserve (cs.GetLength () + 1);

  for (LPCTSTR l_p (cs); *l_p; ++l_p)
    {
      vt.push_back (static_cast <T> (*l_p));
    }

  vt.push_back (static_cast <T> (tc));
}

template <typename _Fn> void
Tokenize (CString const & buffer, CString const & delimiter, _Fn _Fx)
{
  std::vector <TCHAR> l_buffer;

  StringToVector (buffer, l_buffer);

  LPTSTR l_context (nullptr);

  for (LPCTSTR l_token (::_tcstok_s (&l_buffer[0], delimiter, &l_context));
       l_token;
       l_token = ::_tcstok_s (nullptr, delimiter, &l_context))
    {
      _Fx (CString (l_token).Trim ());
    }
}

inline void
Tokenize (CString const & buffer, std::vector <CString> & tokens, CString const & delimiter = _T (","))
{
  Tokenize (buffer, delimiter, [&tokens] (auto const & token) { tokens.emplace_back (token); });
}

template <typename _Type> class IntConverter
{
public:
  using Type = typename _Type;

  static inline auto Convert (LPCTSTR _String, LPTSTR * _EndPtr, int _Radix) { return ::_tcstoll (_String, _EndPtr, _Radix); }
};

template <typename _Type> class UIntConverter
{
public:
  using Type = typename _Type;

  static inline auto Convert (LPCTSTR _String, LPTSTR * _EndPtr, int _Radix) { return ::_tcstoull (_String, _EndPtr, _Radix); }
};

template <typename _Type> class FloatConverter
{
public:
  using Type = typename _Type;

  static inline auto Convert (LPCTSTR _String, LPTSTR * _EndPtr, int) { return ::_tcstod (_String, _EndPtr); }
};

using Int8Converter    = IntConverter   <char>;
using UInt8Converter   = UIntConverter  <unsigned char>;
using Int16Converter   = IntConverter   <short>;
using UInt16Converter  = UIntConverter  <unsigned short>;
using Int32Converter   = IntConverter   <long>;
using UInt32Converter  = UIntConverter  <unsigned long>;
using Int64Converter   = IntConverter   <long long>;
using UInt64Converter  = UIntConverter  <unsigned long long>;
using Float32Converter = FloatConverter <float>;
using Float64Converter = FloatConverter <double>;

template <typename _Converter> auto Convert (CString const & _Value, int _Radix = 10)
{
  LPTSTR _EndPtr (nullptr);

  auto const __Value (_Converter::Convert (CString (_Value).Trim (), &_EndPtr, _Radix));

  if (*_EndPtr)
    {
      ThrowStringException (_T ("invalid numeric value: %s"), (LPCTSTR) _Value);
    }
  else if ((__Value < std::numeric_limits <_Converter::Type>::lowest ()) || (__Value > std::numeric_limits <_Converter::Type>::max ()))
    {
      ThrowStringException (_T ("value is out of range: %s"), (LPCTSTR) _Value);
    }

  return static_cast <_Converter::Type> (__Value);
}

template <typename _Converter, typename _Type> void
Tokenize (CString const & buffer, std::vector <_Type> & tokens, CString const & delimiter = _T (","))
{
  Tokenize (buffer, delimiter, [&tokens] (auto token) { tokens.emplace_back (Convert <_Converter> (token)); });
}

#if (_MANAGED != 1) && (_M_CEE != 1)
class __DECLSPEC IWorkerThread
{
public:
  virtual ~IWorkerThread () = default;

  virtual bool OnStartup (void) = 0;
  virtual bool OnRun (void) = 0;
  virtual void OnShutdown (void) = 0;

protected:
  explicit IWorkerThread (void) = default;

public:
  IWorkerThread (IWorkerThread const &) = delete;
  IWorkerThread & operator = (IWorkerThread const &) = delete;
};

class __DECLSPEC CWorkerThread final
{
public:
  explicit CWorkerThread (IWorkerThread & workerThread, DWORD loopRate = 0, bool pumpMessages = false);
  virtual ~CWorkerThread ();

  bool Create (bool workerSuspended = false);
  void Resume (void);
  void Terminate (void);
  void SetPriority (int priority);

  int GetPriority (void) const;
  DWORD GetThreadId (void) const;
  bool IsCreated (void) const;
  bool IsTerminating (void) const;
  bool IsTerminated (void) const;

private:
  struct SWorkerThreadImpl;

  IWorkerThread & m_workerThread;
  DWORD const m_loopRate;
  bool const m_pumpMessages;
  std::unique_ptr <SWorkerThreadImpl> m_workerThreadImpl;

  void WorkerThread (void);

  static void WorkerThread_ (CWorkerThread * workerThread);

public:

  CWorkerThread (CWorkerThread const &) = delete;
  CWorkerThread & operator = (CWorkerThread const &) = delete;
};

using CWorkerThreadPtr = std::shared_ptr <PDCLib::CWorkerThread>;

void __DECLSPEC Sleep (DWORD duration);
#endif

class __DECLSPEC IIsRepeat : public CObject
{
  DECLARE_DYNAMIC (IIsRepeat)

public:
  explicit IIsRepeat (void) = default;
  virtual ~IIsRepeat () = default;

  virtual bool GetValue (void) const = 0;
  virtual void SetValue (bool value) = 0;

  IIsRepeat (const IIsRepeat &) = delete;
  IIsRepeat & operator = (const IIsRepeat &) = delete;
};

class __DECLSPEC CIsRepeat final : public IIsRepeat
{
  DECLARE_DYNAMIC (CIsRepeat)

public:
  explicit CIsRepeat (void) : m_value (false) { ; }
  virtual ~CIsRepeat () = default;

  virtual bool GetValue (void) const final { return m_value; }
  virtual void SetValue (bool value) final { m_value = value; }

private:
  bool m_value;

public:
  CIsRepeat (const CIsRepeat &) = delete;
  CIsRepeat & operator = (const CIsRepeat &) = delete;
};

class __DECLSPEC IIsMarker : public CObject
{
  DECLARE_DYNAMIC (IIsMarker)

public:
  explicit IIsMarker (void) = default;
  virtual ~IIsMarker () = default;

  virtual bool GetValue (void) const = 0;
  virtual void SetValue (bool value) = 0;

  IIsMarker (const IIsMarker &) = delete;
  IIsMarker & operator = (const IIsMarker &) = delete;
};

class __DECLSPEC CIsMarker final : public IIsMarker
{
  DECLARE_DYNAMIC (CIsMarker)

public:
  explicit CIsMarker (void) : m_value (false) { ; }
  virtual ~CIsMarker () = default;

  virtual bool GetValue (void) const final { return m_value; }
  virtual void SetValue (bool value) final { m_value = value; }

private:
  bool m_value;

public:
  CIsMarker (const CIsMarker &) = delete;
  CIsMarker & operator = (const CIsMarker &) = delete;
};

class __DECLSPEC CApplicationMutex final
{
public:
  explicit CApplicationMutex (CString const & mutexName, bool showWindow = true, UINT cursorMenu = 0);
  virtual ~CApplicationMutex () = default;

  bool InitInstanceHook (void) const;

  void WindowProcHook (CWnd * pWnd, UINT dwMessage, WPARAM wParam, LPARAM lParam, UINT nIDResource, CString const & tip, bool minimizeToTray = true) const;

  bool TrayAdd (CWnd * pWnd, UINT nIDResource, CString const & tip, bool minimizeToTray = true) const;
  bool TrayModify (CWnd * pWnd, UINT nIDResource, CString const & tip) const;
  bool TrayDelete (CWnd * pWnd, UINT nIDResource) const;
 
private:
  CString const m_mutexName;
  bool const m_showWindow;
  UINT const m_cursorMenu;
  UINT const m_wmContextSwitch;

  static UINT const WM_TASKBARCREATED;
  static UINT const WM_NOTIFYTRAYICON;

  static bool TrayMessage (CWnd * pWnd, DWORD dwMessage, UINT nIDResource, CString const & tip, bool minimizeToTray);
  static void ShowWindow (CWnd * pWnd);

public:

  // copy construction and assignment not allowed for this class

  CApplicationMutex (const CApplicationMutex &) = delete;
  CApplicationMutex & operator = (const CApplicationMutex &) = delete;
};

class __DECLSPEC CEditBrowseFileCtrl final : public CMFCEditBrowseCtrl
{
public:
  enum class EMode { PathName, FileName, FileTitle };

  explicit CEditBrowseFileCtrl (bool openFileDialog = true);
  explicit CEditBrowseFileCtrl (bool            openFileDialog,
                                CString const & defExt,
                                CString const & filter);
  explicit CEditBrowseFileCtrl (bool            openFileDialog,
                                EMode           mode,
                                CString const & defExt,
                                CString const & filter);
  virtual ~CEditBrowseFileCtrl ();

  inline void SetFileOpenDialog (bool openFileDialog) { m_openFileDialog = openFileDialog; }
  inline void SetMode (EMode mode) { m_mode = mode; }
  inline void SetDefExt (CString const & defExt) { m_defExt = defExt; }
  inline void SetFilter (CString const & filter) { m_filter = filter; }

protected:
  virtual void OnBrowse (void) final;

private:
  bool m_openFileDialog;
  EMode m_mode;
  CString m_defExt;
  CString m_filter;
  CString m_dirName;

  CString GetDefaultPath (void);

public:
  // copy construction and assignment not allowed for this class

  CEditBrowseFileCtrl (const CEditBrowseFileCtrl &) = delete;
  CEditBrowseFileCtrl & operator = (const CEditBrowseFileCtrl &) = delete;
};

class __DECLSPEC CHandle final
{
public:
  explicit CHandle (CHandle const & handle);
  explicit CHandle (HANDLE handle = INVALID_HANDLE_VALUE);
  explicit CHandle (CString const & fileName,
                    DWORD           dwDesiredAccess       = GENERIC_READ | GENERIC_WRITE,
                    DWORD           dwShareMode           = 0,
                    DWORD           dwCreationDisposition = OPEN_EXISTING,
                    DWORD           dwFlagsAndAttributes  = 0);
  virtual ~CHandle ();

  void Attach (HANDLE handle);
  HANDLE Detach (void);

  bool Close (void);

  CHandle & operator= (CHandle const & handle);

  inline bool IsInvalid (void) const { return m_handle == INVALID_HANDLE_VALUE; }

  inline operator HANDLE() const { return m_handle; }

private:
  mutable HANDLE m_handle;
};

class __DECLSPEC CEncryptedFile final
{
public:
  explicit CEncryptedFile (CString const & fileName, UINT flags, CString const & password = CString ());
  virtual ~CEncryptedFile ();

  void Read (std::vector <BYTE> & buffer);
  bool ReadString (CString & buffer);

  void Write (std::vector <BYTE> const & buffer);
  void WriteString (CString const & buffer);

private:
  struct SBFImpl;

  std::unique_ptr <SBFImpl> m_bfImpl;

public:
  CEncryptedFile (CEncryptedFile const &) = delete;
  CEncryptedFile & operator = (CEncryptedFile const &) = delete;
};

#undef __DECLSPEC
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
//  12/18/2009  MCC     initial revision
//  01/05/2010  MCC     implemented support for application data folder
//  05/12/2010  MCC     refactored CRC subroutine into utility library
//  06/18/2010  MCC     modified to generate dump file on abnormal termination
//  08/03/2010  MCC     added method last modified attribute
//  09/29/2010  MCC     implemented support for directory name subtype
//  03/25/2011  MCC     added function for generating temporary file name
//  05/13/2011  MCC     added barcode regular expression to plate definition
//  09/16/2011  MCC     generalized event logging interface
//  09/21/2011  MCC     baselined message handler class
//  09/22/2011  MCC     added Summit message handler to Device Editor
//  03/21/2013  MCC     implemented workaround for Windows 7 tick count problem
//  08/15/2013  MCC     corrected problem with event reporting
//  11/25/2013  TAN     implemented stacker process
//  12/04/2013  MCC     reimplemented support for repeating iterators
//  07/18/2014  MCC     implemented support for executing shell commands
//  08/13/2014  MCC     updated for Visual Studio 2013 update
//  09/09/2014  MCC     corrected problem with CoInitialize workaround
//  09/26/2014  MCC     implemented support for e-mail notification feature
//  10/08/2014  MCC     implemented e-mail notification test feature
//  11/19/2014  MCC     corrected code analysis issues
//  01/09/2015  MCC     templatized number of elements macro
//  07/02/2015  MCC     added method editor to remote control interface
//  07/15/2015  MCC     default browse controls to unique MRU directory
//  07/16/2015  MCC     refactored downstack/upstack into separate iterators
//  10/05/2016  MCC     implemented support for application context switching
//  03/31/2017  MCC     added reset method to remote control interface
//  04/04/2017  MCC     refined worker thread implementation
//  04/05/2017  MCC     updated worker thread implementation
//  11/20/2017  MCC     baselined handle wrapper class
//  12/01/2017  MCC     updated worker thread implementation
//  12/01/2017  MCC     minor cleanup of worker thread class
//  02/28/2018  MCC     removed dependency on EZMail SMTP components
//  06/06/2018  MCC     implemented support for TwinCAT 3 ADS interface
//  07/11/2018  MCC     implemented support for encrypted calibrations
//  08/08/2018  MCC     implemented more robust TwinCAT 3 ADS detection
//  08/10/2018  MCC     refactored numeric converstion templates
//  08/14/2018  MCC     updated numeric conversion templates
//
// ============================================================================

#endif
