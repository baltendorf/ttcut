/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttmpegaudiostream.cpp                                           */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/11/2005 */
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


#include "ttmpegaudiostream.h"
#include "ttaudioheaderlist.h"
#include "../common/ttexception.h"
#include "../common/istatusreporter.h"
#include "../data/ttcutlist.h"
#include "../data/ttcutparameter.h"

#include <math.h>

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// TTMPEGAudioStream
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

// constructor with file info and start position
// -----------------------------------------------------------------------------
TTMPEGAudioStream::TTMPEGAudioStream( const QFileInfo &f_info, int s_pos )
  : TTAudioStream( f_info, s_pos)
{
  log = TTMessageLogger::getInstance();
}

TTMPEGAudioStream::~TTMPEGAudioStream()
{
}

TTAVTypes::AVStreamType TTMPEGAudioStream::streamType() const
{
  return TTAVTypes::mpeg_audio;
}

//! Returns the stream file extension
QString TTMPEGAudioStream::streamExtension()
{
  return (stream_info != 0)
        ? stream_info->suffix().toLower()
        : ".mp2";
}

//! Returns the stream length as QTime
QTime TTMPEGAudioStream::streamLengthTime()
{
  if (header_list == 0 || header_list->count() == 0)
    return QTime(0, 0, 0, 0);

  TTMpegAudioHeader* audio_header = (TTMpegAudioHeader*)header_list->audioHeaderAt( header_list->count()-1 );

  return ttMsecToTimeD( audio_header->abs_frame_time );
}

//! Returns the index in header list for given time in seconds
int TTMPEGAudioStream::searchIndex( double s_time )
{
  return (ttAssigned( header_list ))
      ? header_list->searchTimeIndex( s_time )
      : -1;
}

// search next sync byte in stream
// -----------------------------------------------------------------------------
void TTMPEGAudioStream::searchNextSyncByte()
{
  quint8  byte1;
  quint8  byte2;
  quint16 sync_word;

  stream_buffer->readByte( byte2 );

  while (!stream_buffer->atEnd() )
  {
    byte1 = byte2;
    stream_buffer->readByte( byte2 );

    sync_word = (byte1<<8) + byte2;

    if ((sync_word & 0xffe0) == 0xffe0)
    {
      stream_buffer->seekBackward(1);
      break;
    }
  }
}

// parse mpeg audio header data
// -----------------------------------------------------------------------------
void TTMPEGAudioStream::parseAudioHeader( quint8* data, int offset, TTMpegAudioHeader* audio_header )
{
  audio_header->version            = (data[offset] & 0x18) >> 3;
  audio_header->layer              = (data[offset] & 0x06) >> 1;
  audio_header->protection_bit     = (data[offset] & 0x01) == 1;

  audio_header->bitrate_index      = (data[offset+1] & 0xf0) >> 4;
  audio_header->sampling_frequency = (data[offset+1] & 0x0c) >> 2;
  audio_header->padding_bit        = (data[offset+1] & 0x02) == 2;
  audio_header->private_bit        = (data[offset+1] & 0x01) == 1;

  audio_header->mode               = (data[offset+2] & 0xc0) >> 6;
  audio_header->mode_extension     = (data[offset+2] & 0x30) >> 4;
  audio_header->copyright          = (data[offset+2] & 0x08) == 8;
  audio_header->original_home      = (data[offset+2] & 0x04) == 4;
  audio_header->emphasis           = (data[offset+2] & 0x03);

  switch (audio_header->version)
  {
  case 3: // Mpeg 1
    if (audio_header->layer == 3) // Layer I
      frame_length = (int)trunc((12*audio_header->bitRate()/audio_header->sampleRate()+audio_header->padding_bit)*4);
    else // Layer II, Layer III
      frame_length = (int)trunc(144*audio_header->bitRate()/audio_header->sampleRate()+audio_header->padding_bit);
    break;
  case 0: // Mpeg 2.5
  case 2: // Mpeg 2
    if (audio_header->layer==3) // Layer I
      frame_length = (int)trunc((6*audio_header->bitRate()/audio_header->sampleRate()+audio_header->padding_bit)*4);
    else // Layer II, Layer III
      frame_length = (int)trunc(72*audio_header->bitRate()/audio_header->sampleRate()+audio_header->padding_bit);
    break;
  default:
    log->errorMsg(__FILE__, __LINE__, "error parsing audio header!");
    frame_length = 0;
    frame_time   = (double)0.0;
    break;
  }

  if (frame_length > 0)
  {
    //log->debugMsg(__FILE__, __LINE__, "bit rate: %d",audio_header->bitRate());
    if ( audio_header->bitRate() > 0 )
    {
      audio_header->frame_length = (int)frame_length;
      audio_header->frame_time   = (double)(audio_header->frame_length * 8000.0) / (double)audio_header->bitRate();
      frame_time                 = audio_header->frame_time;
      //log->debugMsg(__FILE__, __LINE__, QString("frame length/time: %1 / %2").
      //    arg(frame_length).arg(frame_time));
    }
    else
    {
      log->errorMsg(__FILE__, __LINE__, "bitrate error: bitrate is 0!");
      audio_header->frame_length = (int)0;
      audio_header->frame_time   = (double)0.0;
    }
  }
}


// read one audio header from stream
// -----------------------------------------------------------------------------
void TTMPEGAudioStream::readAudioHeader( TTMpegAudioHeader* audio_header )
{
  quint8* data = new quint8[3];

  // read 3 byte from stream
  stream_buffer->readByte( data, 3 );

  // audio header offset
  audio_header->setHeaderOffset( stream_buffer->position() - 4 );

  // parse current audio header and fill header struct
  parseAudioHeader( data, 0, audio_header );

  delete []data;
}

// create the audio header list
// -----------------------------------------------------------------------------
int TTMPEGAudioStream::createHeaderList( )
{
  TTMpegAudioHeader* audio_header;
  TTMpegAudioHeader* prev_audio_header;

  header_list = new TTAudioHeaderList( 1000 );

  stream_buffer->seekAbsolute( (quint64)start_pos );

  try
  {
    emit statusReport(StatusReportArgs::Start, "Create audio-header list", stream_buffer->size());

    while ( !stream_buffer->atEnd() )
    {
    	if (mAbort) {
    		mAbort = false;
        qDebug("TTMpegAudioStream::createHeaderList -> user abort");
    		throw new TTAbortException(tr("Index list creation aborted!"));
    	}

      searchNextSyncByte();
      audio_header = new TTMpegAudioHeader();

      // read and parse current audio header
      readAudioHeader( audio_header );

      // claculate the absolute frame time
      // -----------------------------------------------------------------------
      // first audio header: abs_frame_time = 0.0 (msec)
      if ( header_list->count() == 0 )
        audio_header->abs_frame_time = (double)0.0;
      else
      {
        // previous frame header
        prev_audio_header = (TTMpegAudioHeader*)header_list->at(header_list->count()-1);

        // absolute frame time for current frame in msec
        audio_header->abs_frame_time = prev_audio_header->abs_frame_time+
          prev_audio_header->frame_time;
      }

      // add audio header to header list
      header_list->add( audio_header );
      stream_buffer->seekRelative( audio_header->frame_length-4 );

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
void TTMPEGAudioStream::cut(int start, int end, TTCutParameter* cp)
{
  TTMpegAudioHeader* audio_header = (TTMpegAudioHeader*)header_list->audioHeaderAt(0);
  frame_time = audio_header->frame_time;

  quint64 start_offset = header_list->audioHeaderAt(start)->headerOffset();
  quint64 end_offset   = header_list->audioHeaderAt(end)->headerOffset()-1;

  log->debugMsg(__FILE__, __LINE__, QString("cut audio start %1 offset %2 end %3 offset %4").
      arg(start).arg(start_offset).
      arg(end).arg(end_offset));

  copySegment(cp->getTargetStreamBuffer(), start_offset, end_offset );
}


