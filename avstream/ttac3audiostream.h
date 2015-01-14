/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttac3audiostream.h                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAC3AUDIOSTREAM
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

#ifndef TTAC3AUDIOSTREAM_H
#define TTAC3AUDIOSTREAM_H

#include "common/ttmessagelogger.h"
#include "ttavstream.h"
#include "ttac3audioheader.h"

/* \brief This class represents an AC3 audio stream object
 *
 */
class TTAC3AudioStream : public TTAudioStream
{
 public:
  TTAC3AudioStream(const QFileInfo &f_info, int s_pos=0);
  virtual ~TTAC3AudioStream();

  virtual TTAVTypes::AVStreamType streamType() const;

  void searchNextSyncByte();
  void readAudioHeader(TTAC3AudioHeader* audio_header);

  virtual void cut(int start, int end, TTCutParameter* cp);

  virtual int  createHeaderList();
  virtual int  createIndexList(){return 0;};
  QString      streamExtension();
  QTime        streamLengthTime();
};

#endif //TTAC3AUDIOSTREAM_H
