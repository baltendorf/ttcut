/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttencodeparameter.cpp                                           */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 11/07/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTENCODEPARAMETER
// ----------------------------------------------------------------------------

/*----------------------------------------------------------------------------*/
/* This program is free software; you can redistribute it and/or modify it    */
/* under the terms of the GNU General Public License as published by the Free */
/* Software Foundation;                                                       */
/* either version 2 of the License, or (at your option) any later version.    */
/*                                                                            */
/* This program is distributed in the hope that it will be useful, but WITHOUT*/
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      */
/* FITNESS FOR A PARTICULAR PURPOSE.                                          */
/* See the GNU General Public License for more details.                       */
/*                                                                            */
/* You should have received a copy of the GNU General Public License along    */
/* with this program; if not, write to the Free Software Foundation,          */
/* Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.              */
/*----------------------------------------------------------------------------*/

#include "ttencodeparameter.h"
#include "common/ttmessagelogger.h"

/* /////////////////////////////////////////////////////////////////////////////
 * Print out the encode parameter for debug and logging purpose
 */
void print(char* prefix)
{
  TTMesasgeLogger* log = TTMessageLogger::getInstance();

  log->debugMsg( prefix, "----------------------------------------------------" );
  log->debugMsg( prefix, "encoder parameter:" );
  log->debugMsg( prefix, "----------------------------------------------------" );
  log->debugMsg( prefix, "avi-file    : %s",    qPrintable(aviFileInfo().absoluteFilePath()) );
  log->debugMsg( prefix, "mpeg-file   : %s",    qPrintable(mpeg2FileInfo().absoluteFilePath()) );
  log->debugMsg( prefix, "widhtxheight: %dx%d", videoWidth(), videoHeight() );
  log->debugMsg( prefix, "aspect-code : %d",    videoAspectCode() );
  log->debugMsg( prefix, "bitrate     : %f",    videoBitrate() );
  log->debugMsg( prefix, "max bitrate : %f",    videoMaxBitrate() );
  log->debugMsg( prefix, "framerate   : %f",    videoFPS() );
  log->debugMsg( prefix, "----------------------------------------------------" );
}

