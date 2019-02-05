# ============================================================================
#
#                              CONFIDENTIAL
#
#        GENOMICS INSTITUTE OF THE NOVARTIS RESEARCH FOUNDATION (GNF)
#
#  This is an unpublished work of authorship, which contains trade secrets,
#  created in 2001.  GNF owns all rights to this work and intends to maintain
#  it in confidence to preserve its trade secret status.  GNF reserves the
#  right, under the copyright laws of the United States or those of any other
#  country that may have jurisdiction, to protect this work as an unpublished
#  work, in the event of an inadvertent or deliberate unauthorized publication.
#  GNF also reserves its rights under all copyright laws to protect this work
#  as a published work, when appropriate.  Those having access to this work
#  may not copy it, use it, modify it or disclose the information contained
#  in it without the written authorization of GNF.
#
# ============================================================================

# ============================================================================
#
#            Name: Build.pl
#
#     Description: Build number incrementing script
#
#          Author: Mike Conner
#
# ============================================================================

# ============================================================================
#
#      %subsystem: 1 %
#           %name: build.pl %
#        %version: 4 %
#          %state: %
#         %cvtype: perl %
#     %derived_by: mconner %
#  %date_modified: %
#
# ============================================================================

use Cwd;
use File::Copy;
use File::Temp qw/ tempfile /;

sub update_build_num
{
  /\d{1,3}$_[0]\d{1,3}$_[0](\d{1,5})/ || die "Unable to recognize version format: $_";

  $buildNum = $1;

  if ($buildNum < 65535)
    {
      ++$buildNum;
    }
  else
    {
      die "Cannot increment build number beyond 65535";
    }

  s/(\d{1,3})$_[0](\d{1,3})$_[0]\d{1,5}/$1$_[0]$2$_[0]$buildNum/;
}

open (INFILE, $ARGV[0]) || die "Cannot open $ARGV[0] for reading: $!";
($fh, $filename) = tempfile (DIR => getcwd ());
binmode $fh, crlf;

while (<INFILE>)
  {
    if (/AssemblyVersion/)
      {
        update_build_num (".");
      }
    elsif (/AssemblyFileVersion/)
      {
        update_build_num (".");
      }
    elsif (/FILEVERSION/)
      {
        update_build_num (",");
      }
    elsif (/FileVersion/)
      {
        update_build_num (".");
      }

    print $fh $_;
  }

close INFILE;
close $fh;

move ($filename, $ARGV[0]) || die "Cannot move $filename to $ARGV[0]: $!";

# ============================================================================
#  R E V I S I O N    N O T E S
# ============================================================================
#
#  For each change to this file, record the following:
#
#   1. who made the change and when the change was made
#   2. why the change was made and the intended result
#
# ============================================================================
#
#  Date        Author  Description
# ----------------------------------------------------------------------------
#  03/25/2004  MCC     initial revision
#  03/26/2004  MCC     implemented body of script
#  01/18/2008  MEG     improved portability of temp file generation
#  10/10/2008  MCC     modified to work with .NET assembly information files
#
# ============================================================================
