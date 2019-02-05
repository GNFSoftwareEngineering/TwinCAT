:: ============================================================================
::
::                              CONFIDENTIAL
::
::        GENOMICS INSTITUTE OF THE NOVARTIS RESEARCH FOUNDATION (GNF)
::
::  This is an unpublished work of authorship, which contains trade secrets,
::  created in 2001.  GNF owns all rights to this work and intends to maintain
::  it in confidence to preserve its trade secret status.  GNF reserves the
::  right, under the copyright laws of the United States or those of any other
::  country that may have jurisdiction, to protect this work as an unpublished
::  work, in the event of an inadvertent or deliberate unauthorized publication.
::  GNF also reserves its rights under all copyright laws to protect this work
::  as a published work, when appropriate.  Those having access to this work
::  may not copy it, use it, modify it or disclose the information contained
::  in it without the written authorization of GNF.
::
:: ============================================================================

:: ============================================================================
::
::            Name: Build.bat
::
::     Description: Batch File for TwinCAT Build
::
::          Author: Mike Conner
::
:: ============================================================================

:: ============================================================================
::
::      %subsystem: 12 %
::           %name: build.bat %
::        %version: 2.1.2 %
::          %state: %
::         %cvtype: shsrc %
::     %derived_by: mgustafs %
::  %date_modified: %
::
:: ============================================================================

@echo off

:: increment the build number for the application...

perl .\build.pl .\TwinCAT.rc2

:BuildDebug

echo build the 'Debug' version of the application...

if "%1" == "/F" (
  "%DEVENV_BIN%\devenv.exe" /rebuild Debug TwinCAT.sln
) else (
  "%DEVENV_BIN%\devenv.exe" /build Debug TwinCAT.sln
)

if ERRORLEVEL 1 GOTO BuildDebug

:BuildRelease

echo build the 'Release' version of the application...

if "%1" == "/F" (
  "%DEVENV_BIN%\devenv.exe" /rebuild Release TwinCAT.sln
) else (
  "%DEVENV_BIN%\devenv.exe" /build Release TwinCAT.sln
)

if ERRORLEVEL 1 GOTO BuildRelease

:: ============================================================================
::  R E V I S I O N    N O T E S
:: ============================================================================
::
::  For each change to this file, record the following:
::
::   1. who made the change and when the change was made
::   2. why the change was made and the intended result
::
:: ============================================================================
::
::  Date        Author  Description
:: ----------------------------------------------------------------------------
::  09/24/2008  MCC     initial revision
::  05/04/2010  MCC     implemented support for Visual Studio 2010
::  01/28/2013  MCC     refactored path variables to environment
::  06/01/2018  MEG     added automatic retry if build fails
::
:: ============================================================================