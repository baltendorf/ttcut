/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttavtypes.cpp                                                   */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/14/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/10/2005 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// *** TTAVTYPES
// *** TTAUDIOTYPES
// *** TTVIDEOTYPES
// *** TTSUBTITLETYPES
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Overview
// -----------------------------------------------------------------------------
//
//            +- TTAudioType
//            |
// TTAVTypes -+- TTSubtitleType
//            |
//            +- TTVideoType
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

#include "ttavtypes.h"

#include "ttac3audiostream.h"
#include "ttac3audioheader.h"
#include "ttmpegaudiostream.h"
#include "ttmpegaudioheader.h"
#include "ttmpeg2videostream.h"
#include "ttsrtsubtitlestream.h"
#include "common/ttmessagelogger.h"
#include "common/ttexception.h"

#include <QString>
#include <qfileinfo.h>


// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// TTAVTypes: Base class for AV stream types
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

// construct a TTAVTypes object
// -----------------------------------------------------------------------------
TTAVTypes::TTAVTypes( QString f_name )
{
  av_stream_info     = new QFileInfo( f_name );
  av_stream_exists   = av_stream_info->exists();
  av_stream_type     = unknown;
  av_stream          = NULL;
  type_header_offset = 0;
  type_header_length = 0;

  log = TTMessageLogger::getInstance();

  if (!av_stream_info->exists()) {
	  throw TTIOException(QString("AV stream %1 does not exist!").arg(f_name));
  }
}

// destructor
// -----------------------------------------------------------------------------
TTAVTypes::~TTAVTypes()
{
  if (av_stream_info != NULL) {
    delete av_stream_info;
  }


  if ( av_stream != NULL ) {
    //FIXME: Why here is a file buffer????
    av_stream->close();
    delete av_stream;
  }
}

// return current stream QFileInfo
// -----------------------------------------------------------------------------
QFileInfo& TTAVTypes::avStreamInfo()
{
  return *av_stream_info;
}

// return current stream type
// -----------------------------------------------------------------------------
TTAVTypes::AVStreamType TTAVTypes::avStreamType()
{
  return av_stream_type;
}

// return stream type header offset
// -----------------------------------------------------------------------------
quint64 TTAVTypes::typeHeaderOffset()
{
  return type_header_offset;
}

// return stream type header length
// -----------------------------------------------------------------------------
long TTAVTypes::typeHeaderLength()
{
  return type_header_length;
}


// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTAudioType: Audio-stream type
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

// construct TTAudioType object
// -----------------------------------------------------------------------------
TTAudioType::TTAudioType( QString f_name )
  : TTAVTypes( f_name )
{
  start_pos = 0;

  // if audio file exists get audio stream type
  if ( av_stream_exists )
  {
    getAudioStreamType();
  }
}

// destructor
// -----------------------------------------------------------------------------
 TTAudioType::~TTAudioType()
{

}

// create audio stream object according to stream type
// -----------------------------------------------------------------------------
TTAudioStream* TTAudioType::createAudioStream()
{
  switch ( av_stream_type )
  {
  case ac3_audio:
    return new TTAC3AudioStream( *av_stream_info, start_pos );
  case mpeg_audio:
    return new TTMPEGAudioStream( *av_stream_info, start_pos );

  default:
    log->errorMsg(__FILE__, __LINE__, "Unsupported audio stream type!");
    return (TTAudioStream*)0;
  }
}

// evaluate audio stream and estimate audio stream type
// -----------------------------------------------------------------------------
void TTAudioType::getAudioStreamType()
{
  int     count  = 0;
  quint8* buffer = new quint8[65536];
  quint16 sync_word;

  log->debugMsg(__FILE__, __LINE__, QString("Get audio stream type"));

  av_stream_type  = unknown;
  start_pos       = 0;

  // open audio-stream for reading
  // FIXME: av_stream seems to be local
  av_stream = new TTFileBuffer(av_stream_info->filePath(), QIODevice::ReadOnly );

  // read buffer from stream
  count = av_stream->readByte( buffer, 65536 );

  // --------------------------------------------------------------------------
  // analyze buffer
  // --------------------------------------------------------------------------
  for ( start_pos = 0; start_pos < count-1; start_pos++ )
  {
    sync_word = (uint)((buffer[start_pos]<<8)+buffer[start_pos+1]);

    //log->debugMsg(__FILE__, __LINE__, QString("sync word %1").arg(sync_word));

    // AC3 audio type
    // -------------------------------------------------------------------------
    if ( sync_word == 0x0b77 )
    {
      //log->debugMsg(__FILE__, __LINE__, QString("Found AC3 sync word %1").arg(start_pos));

      frame_size_code = buffer[start_pos+4] & 0x3F;
      sample_rate     = (buffer[start_pos+4] & 0xC0)>>6;
      frame_len       = 2*AC3FrameLength[sample_rate][frame_size_code];

      //log->debugMsg(__FILE__, __LINE__, QString("AC3 frame size code: %1").arg(frame_size_code));
      //log->debugMsg(__FILE__, __LINE__, QString("AC3 sample rate    : %1").arg(sample_rate ));
      //log->debugMsg(__FILE__, __LINE__, QString("AC3 frame len      : %1").arg(frame_len));

      if ( frame_len > 0           &&
          start_pos+frame_len+1 < count &&
          (buffer[start_pos+frame_len]<<8)+buffer[start_pos+1+frame_len] == 0x0b77 )
      {
        av_stream_type = ac3_audio;
        break;
      }
    }

    // MPEG audio type
    // -------------------------------------------------------------------------
    if ( (sync_word & 0xffe0) == 0xffe0 )
    {
      //log->debugMsg(__FILE__, __LINE__, QString("Found MPEG audio sync word: %1").arg(start_pos));

      TTMpegAudioHeader* mpeg_header = new TTMpegAudioHeader();
      TTMPEGAudioStream* mpeg_stream = new TTMPEGAudioStream(*av_stream_info, 0);

      mpeg_stream->parseAudioHeader( buffer, start_pos+1, mpeg_header );

      //log->debugMsg(__FILE__, __LINE__, QString("frame length   : %1").arg(mpeg_stream->frameLength()));
      //log->debugMsg(__FILE__, __LINE__, QString("sync word      : %1").arg(((buffer[start_pos+mpeg_stream->frameLength()]<<8)+buffer[start_pos+1+mpeg_stream->frameLength()]) & 0xFFE0));

      if ( mpeg_header->frameLength() > 0 &&
          start_pos+mpeg_header->frameLength()+1 < count &&
          // next sync_word is MPEG audio
          (((buffer[start_pos+mpeg_header->frameLength()]<<8)+buffer[start_pos+1+mpeg_header->frameLength()]) & 0xFFE0) == 0xFFE0 )
      {
        av_stream_type = mpeg_audio;

        if (mpeg_stream != 0) delete mpeg_stream;
        if (mpeg_header != 0) delete mpeg_header;

        break;
      }
    }
  }

  if (av_stream != 0) {
    av_stream->close();
    delete av_stream;
    av_stream = 0;
  }

  delete [] buffer;

  //qDebug( "%s-----------------------------------------------",c_name );
}

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTVideoType: Video stream type
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

// construct TTVideoType object
// -----------------------------------------------------------------------------
  TTVideoType::TTVideoType( QString f_name )
: TTAVTypes( f_name )
{
  // if video file exists get video stream type
  if ( av_stream_exists )
  {
    getVideoStreamType();
  }
}

// destructor
// -----------------------------------------------------------------------------
TTVideoType::~TTVideoType()
{

}

// create video stream object according to video stream type
// -----------------------------------------------------------------------------
TTVideoStream* TTVideoType::createVideoStream()
{
  return (av_stream_exists)
      ? new TTMpeg2VideoStream(*av_stream_info)
      : (TTVideoStream*)NULL;
}

// evaluate video stream and estimate video stream type
// currently we use only mpeg2 video, so this is a simple task
void TTVideoType::getVideoStreamType()
{
  av_stream_type = mpeg2_demuxed_video;
}

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTSubtitleType: Subtitle stream type
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

// construct TTSubtitleType object
// -----------------------------------------------------------------------------
TTSubtitleType::TTSubtitleType( QString f_name )
  : TTAVTypes( f_name )
{
  // if subtitle file exists get video stream type
  if ( av_stream_exists )
  {
    getSubtitleStreamType();
  }
}

// destructor
// -----------------------------------------------------------------------------
TTSubtitleType::~TTSubtitleType()
{

}

// create subitle stream object according to video stream type
// -----------------------------------------------------------------------------
TTSubtitleStream* TTSubtitleType::createSubtitleStream()
{
  switch ( av_stream_type )
  {
  case srt_subtitle:
    return new TTSrtSubtitleStream( *av_stream_info );

  default:
    log->errorMsg(__FILE__, __LINE__, "Unsupported subtitle stream type!");
    return (TTSubtitleStream*)0;
  }
}

// evaluate subtitle stream and estimate subtitle stream type
// currently we use only srt subtitle, so this is a simple task
// -----------------------------------------------------------------------------
void TTSubtitleType::getSubtitleStreamType()
{
  av_stream_type = srt_subtitle;
}
