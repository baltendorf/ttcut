/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : tmpegaudioheader.h                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/11/2005 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TMPEGAUDIOHEADER
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Overview
// -----------------------------------------------------------------------------
//
//                               +- TTMpegAudioHeader
//             +- TTAudioHeader -|                  
//             |                 +- TTAC3AudioHeader 
// TTAVHeader -|                 
//             |
//             |                                     +- TTSequenceHeader
//             |                                     |
//             |                                     +- TTSequenceEndHeader
//             +- TTVideoHeader -TTMpeg2VideoHeader -|
//             |                                     +- TTPicturesHeader
//             |                                     |
//             |                                     +- TTGOPHeader
//             |
//             +- TTVideoIndex
//             |
//             +- TTBreakObject
//
// -----------------------------------------------------------------------------

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

#include "ttmpegaudioheader.h"

#include <QString>

TTMpegAudioHeader::TTMpegAudioHeader()
: TTAudioHeader()
{
  if (str_description.isEmpty()) str_description = "unknown";
  if (str_mode.isEmpty())        str_mode        = "unknown";
  if (str_bit_rate.isEmpty())    str_bit_rate    = "unknown";
  if (str_sample_rate.isEmpty()) str_sample_rate = "unknown";
}

QString& TTMpegAudioHeader::descString()
{
  switch ( version )
  {
  case 0:
    str_description = "Mpeg 2.5";
    break;
  case 2:
    str_description = "Mpeg 2";
    break;
  case 3:
    str_description = "Mpeg 1";
    break;
  default:
    str_description = "reserved";
    break;
  }

  switch ( layer )
  {
  case 1:
    str_description.append( "-Layer 3" );
    break;
  case 2:
    str_description.append( "-Layer 2" );
    break;
  case 3:
    str_description.append( "-Layer 1" );
    break;
  default:
    str_description.append( "-undef." );
    break;
  }
  if (protection_bit==true) // Achtung: false=true!
    str_description.append( ",noCRC" );
  else
    str_description.append( ",CRC" );

  return str_description;
}

QString& TTMpegAudioHeader::modeString()
{
  switch ( mode )
  {
  case 0:
    str_mode = "stereo";
    break;
  case 1:
    str_mode = "joint stereo";
    break;
  case 2:
    str_mode = "dual channel";
    break;
  case 3:
    str_mode = "single channel";
    break;
  }

  return str_mode;
}

int TTMpegAudioHeader::bitRate()
{
  //qDebug( "version, layer, bitrate: %d/%d/%d:%d",version,layer,bitrate_index,mpeg_bit_raten[version][layer][bitrate_index] );
  return 1000*mpeg_bit_raten[version][layer][bitrate_index];
}
 
QString& TTMpegAudioHeader::bitRateString()
{
  str_bit_rate = QString("%1 KBit/s").arg(bitRate());
  //str_bit_rate->sprintf( "%d KBit/s", bitRate() );

  return str_bit_rate;
}

 int TTMpegAudioHeader::sampleRate()
{
  //qDebug( "version, sample_index: %d/%d:%d",version,sampling_frequency,mpeg_sample_raten[version][sampling_frequency] );
  int result = mpeg_sample_raten[version][sampling_frequency];

  if (result == 0)
  {
    qDebug( "version, sample_index: %d/%d:%d",version,sampling_frequency,mpeg_sample_raten[version][sampling_frequency] );
    result = 1;
  }

  return result;
}

 QString& TTMpegAudioHeader::sampleRateString()
 {
   str_sample_rate = QString("%1").arg(sampleRate());
   //str_sample_rate->sprintf( "%d",sampleRate() );

  return str_sample_rate;
 }


