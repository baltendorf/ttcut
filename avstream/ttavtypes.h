/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttavtypes.h                                                     */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/14/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVTYPES
// TTAUDIOTYPES
// TTVIDEOTYPES
// TTSUBTITLETYPES
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

#ifndef TTAVTYPES_H
#define TTAVTYPES_H

#include "ttcommon.h"
#include "ttfilebuffer.h"

class QString;
class QFileInfo;

class TTMessageLogger;
class TTAudioStream;
class TTVideoStream;
class TTSubtitleStream;

/* /////////////////////////////////////////////////////////////////////////////
 * Base class for AV stream types
 */
class TTAVTypes
{
 public:
  TTAVTypes( QString f_name );
  virtual ~TTAVTypes();

  // known AV stream types
  enum AVStreamType
  {
    mpeg_audio,
    ac3_audio,
    mpeg2_demuxed_video,
    mpeg2_mplexed_video,
    srt_subtitle,
    unknown
  };

  virtual QFileInfo&   avStreamInfo();
  virtual AVStreamType avStreamType();
  virtual quint64      typeHeaderOffset();
  virtual long         typeHeaderLength();

 protected:
	 TTMessageLogger* log;
  QFileInfo*      av_stream_info;
  bool            av_stream_exists;
  TTFileBuffer*   av_stream;
  AVStreamType    av_stream_type;
  quint64         type_header_offset;
  long            type_header_length;
};

/* /////////////////////////////////////////////////////////////////////////////
 * Audio-stream type
 */
class TTAudioType : public TTAVTypes
{
 public:
  TTAudioType( QString f_name );
  ~TTAudioType();

  TTAudioStream* createAudioStream();

 protected:
  void getAudioStreamType();

 private:
  int     start_pos;
  quint8  frame_size_code;
  quint16 sample_rate;
  quint16 frame_len;
};

/* /////////////////////////////////////////////////////////////////////////////
 * Video stream type
 */
class TTVideoType : public TTAVTypes
{
 public:
  TTVideoType( QString f_name );
  ~TTVideoType();

  TTVideoStream* createVideoStream();

 protected:
  void getVideoStreamType();
};

class TTSubtitleType : public TTAVTypes
{
 public:
  TTSubtitleType( QString f_name );
  ~TTSubtitleType();

  TTSubtitleStream* createSubtitleStream();

 protected:
  void getSubtitleStreamType();
};

#endif //TTAVTYPES_H
