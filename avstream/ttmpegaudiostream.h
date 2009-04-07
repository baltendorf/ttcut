/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttmpegaudiostream.h                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/11/2005 */
/* MODIFIED: b. altendorf                                    DATE: 08/13/2005 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMPEGAUDIOSTREAM
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Overview
// -----------------------------------------------------------------------------
//
//                               +- TTMpegAudioStream
//             +- TTAudioStream -|
//             |                 +- TTAC3AudioStream
// TTAVStream -|
//             |
//             +- TTVideoStream -TTMpeg2VideoStream
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

#ifndef TTMPEGAUDIOSTREAM_H
#define TTMPEGAUDIOSTREAM_H

#include "ttavstream.h"
#include "ttmpegaudioheader.h"

/* \brief This class represents an MPEG audio stream
 *
 */
class TTMPEGAudioStream : public TTAudioStream
{
 public:
  TTMPEGAudioStream( const QFileInfo &f_info, int s_pos=0 );
  virtual ~TTMPEGAudioStream();

  TTAVTypes::AVStreamType streamType() const;

  void searchNextSyncByte();
  void parseAudioHeader( quint8* data, int offset, TTMpegAudioHeader* audio_header );

  virtual void cut(int start, int end, TTCutParameter* cp);

  void    readAudioHeader( TTMpegAudioHeader* audio_header );

  virtual int createHeaderList( );
  virtual int createIndexList(){return 0;};

  QString streamExtension();
  QTime   streamLengthTime();
  int     searchIndex( double s_time );
};

#endif //TTMPEGAUDIOSTREAM_H
