#if !defined (BIT_H)
#define BIT_H

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
//            Name: Bit.h
//
//     Description: Bit Twiddling Function declarations
//
//          Author: Mike Conner
//
// ============================================================================

// ============================================================================
//
//      %subsystem: 1 %
//           %name: bit.h %
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

#if defined (__QACPP_VERSION)
#pragma PRQA_MESSAGES_OFF 2134
#endif
template <class T> inline void SetBits (T & operand, T mask)
  { operand |= mask; }
template <class T> inline void TglBits (T & operand, T mask)
  { operand ^= mask; }
template <class T> inline void ClrBits (T & operand, T mask)
  { operand &= ~mask; }
template <class T> inline bool IsBitSet (T operand, T mask)
  { return (operand & mask) ? true : false; }
template <class T> inline bool IsBitClr (T operand, T mask)
  { return (operand & mask) ? false : true; }
inline void SetBit (std::vector <BYTE> & a, int i)
  { a[i >> 3] |= static_cast <BYTE> (0x80 >> (i % 8)); }
inline void TglBit (std::vector <BYTE> & a, int i)
  { a[i >> 3] ^= static_cast <BYTE> (0x80 >> (i % 8)); }
inline void ClrBit (std::vector <BYTE> & a, int i)
  { a[i >> 3] &= static_cast <BYTE> (~(0x80 >> (i % 8))); }
inline bool IsBitSet (const std::vector <BYTE> & a, int i)
  { return (a[i >> 3] & (0x80 >> (i % 8))) ? true : false; }
inline bool IsBitClr (const std::vector <BYTE> & a, int i)
  { return (a[i >> 3] & (0x80 >> (i % 8))) ? false : true; }
inline void SetBit (CByteArray & a, int i)
  { a[i >> 3] |= static_cast <BYTE> (0x80 >> (i % 8)); }
inline void TglBit (CByteArray & a, int i)
  { a[i >> 3] ^= static_cast <BYTE> (0x80 >> (i % 8)); }
inline void ClrBit (CByteArray & a, int i)
  { a[i >> 3] &= static_cast <BYTE> (~(0x80 >> (i % 8))); }
inline bool IsBitSet (const CByteArray & a, int i)
  { return (a[i >> 3] & (0x80 >> (i % 8))) ? true : false; }
inline bool IsBitClr (const CByteArray & a, int i)
  { return (a[i >> 3] & (0x80 >> (i % 8))) ? false : true; }
#if defined (__QACPP_VERSION)
#pragma PRQA_MESSAGES_ON
#endif

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
//  04/27/2006  MCC     initial revision
//  06/09/2014  MCC     refactored TwinCAT I/O interface
//
// ============================================================================

#endif
