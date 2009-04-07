/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttac3audiostream.cpp                                            */
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


#include "ttac3audiostream.h"
#include "ttaudioheaderlist.h"

#include "../common/istatusreporter.h"
#include "../common/ttexception.h"
#include "../data/ttcutlist.h"
#include "../data/ttcutparameter.h"

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTAC3AudioStream: AC3 audio stream class
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

//! Constructor with QFileInfo and start position
TTAC3AudioStream::TTAC3AudioStream(const QFileInfo &f_info, int s_pos)
                 :TTAudioStream(f_info, s_pos)
{
  log = TTMessageLogger::getInstance();
}

//! Destructor
TTAC3AudioStream::~TTAC3AudioStream()
{
}

//! Return the stream type
TTAVTypes::AVStreamType TTAC3AudioStream::streamType() const
{
  return TTAVTypes::ac3_audio;
}

//! Returns the streams file extension
QString TTAC3AudioStream::streamExtension()
{
  return (stream_info != 0)
        ? stream_info->suffix().toLower()
        : ".ac3";
}

//! Return the stream length as QTime
QTime TTAC3AudioStream::streamLengthTime()
{
  if (header_list == 0 || header_list->count() == 0) 
    return QTime(0, 0, 0, 0);

  TTAC3AudioHeader* audio_header = (TTAC3AudioHeader*)header_list->audioHeaderAt( header_list->count()-1 );

  return ttMsecToTimeD(audio_header->abs_frame_time);
}

//! Search the next sync byte in stream
void TTAC3AudioStream::searchNextSyncByte()
{
  quint8  byte1;
  quint8  byte2;
  quint16 sync_word;

  stream_buffer->readByte(byte2);

  while ( !stream_buffer->atEnd() )
  {
    byte1 = byte2;
    stream_buffer->readByte(byte2);
    sync_word = (byte1<<8) + byte2;

    if (sync_word == 0x0B77)
      break;
  }
}

//! Read audio header information from stream
void TTAC3AudioStream::readAudioHeader( TTAC3AudioHeader* audio_header)
{
  quint8* daten = new quint8[6];
  quint16 stuff;

  stream_buffer->readByte(daten, 6);

  audio_header->setHeaderOffset( stream_buffer->position() - 8 ); // +Syncwort

  audio_header->crc1            = daten[0]<<8+daten[1];
  audio_header->fscod           = (quint8)((daten[2]&0xc0)>>6);
  audio_header->frmsizecod      = (quint8)(daten[2]&0x3f);
  audio_header->syncframe_words = AC3FrameLength[audio_header->fscod][audio_header->frmsizecod];
  audio_header->frame_length    = audio_header->syncframe_words*2;
  audio_header->frame_time      = 1000.0*((double)audio_header->syncframe_words*16.0)/(double)audio_header->bitRate();
  audio_header->bsid            = (quint8)(daten[3]>>3);
  audio_header->bsmod           = (quint8)(daten[3] & 0x07);
  audio_header->acmod           = (quint8)(daten[4]>>5);

  //log->debugMsg(__FILE__, __LINE__, QString("sbit rate %1 sframe length %2").
  //    arg(audio_header->bitRate()).
  //    arg(audio_header->frame_length));
  
  frame_length = audio_header->frame_length;
  frame_time   = audio_header->frame_time;

  stuff = (quint16)((daten[4]<<8)+daten[5]);
  stuff <<= 3;

  if ((audio_header->acmod & 0x01)!=0 && audio_header->acmod!=1)
    stuff<<=2;
  if ((audio_header->acmod & 0x04)!=0)
    stuff<<=2;
  if (audio_header->acmod == 0x02)
    stuff<<=2;

  audio_header->lfeon=(stuff&0x8000)!=0;

  delete [] daten;
}

//! Create the header list
int TTAC3AudioStream::createHeaderList()
{
  TTAC3AudioHeader* audio_header;
  TTAC3AudioHeader* prev_audio_header;

  header_list = new TTAudioHeaderList( 1000 );

  stream_buffer->seekRelative( start_pos );

  try
  {
    emit statusReport(StatusReportArgs::Start, "Create audio-header list", stream_buffer->size());

    while (!stream_buffer->atEnd())
    {
      if (mAbort) {
        mAbort = false;
        throw new TTAbortException("User abort in TTAC3AudioStream::createHeaderList!");
      }

      searchNextSyncByte();

      audio_header = new TTAC3AudioHeader();

      readAudioHeader( audio_header );

      if ( header_list->count() == 0 )
        audio_header->abs_frame_time = 0.0;
      else
      {
        prev_audio_header = (TTAC3AudioHeader*)header_list->at(header_list->count()-1);
        audio_header->abs_frame_time = prev_audio_header->abs_frame_time
          +prev_audio_header->frame_time;
      }

      header_list->add( audio_header );

      stream_buffer->seekRelative(audio_header->syncframe_words*2-8);

      emit statusReport(StatusReportArgs::Step, "Create audio-header list", stream_buffer->position());
    }
    emit statusReport(StatusReportArgs::Finished, "Audio-header list created", stream_buffer->position());
  }
  catch (TTFileBufferException)
  {
  }

  log->debugMsg(__FILE__, __LINE__, QString("header list created: %1").arg(header_list->count()));
  log->debugMsg(__FILE__, __LINE__, QString("abs stream length:   %1").arg(streamLengthTime().toString("hh:mm:ss")));

  return header_list->count();
}

//! Cut the audio stream
void TTAC3AudioStream::cut(int start, int end, TTCutParameter* cp)
{
  quint64 start_offset = header_list->audioHeaderAt(start)->headerOffset();
  quint64 end_offset   = header_list->audioHeaderAt(end)->headerOffset()-1;

  copySegment(cp->getTargetStreamBuffer(), start_offset, end_offset );
}

