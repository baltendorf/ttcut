/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005/2010 / www.tritime.org                    */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttavstream.h                                                    */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/01/2008 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVSTREAM (abstract)
// TTAUDIOSTREAM
// TTVIDEOSTREAM
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

#ifndef TTAVSTREAM_H
#define TTAVSTREAM_H

#include "ttavtypes.h"

#include <QProcess>
#include <QObject>
#include <QString>
#include <QFileInfo>

class TTMessageLogger;
class TTSequenceHeader;
class TTVideoHeaderList;
class TTVideoIndexList;
class TTVideoIndex;
class TTAudioHeaderList;
class TTAudioHeader;
class TTCutParameter;

// -----------------------------------------------------------------------------
// *** TTAVStream: Abstract class TTAVStream
// -----------------------------------------------------------------------------
class TTAVStream : public QObject
{
  Q_OBJECT

protected:
  TTAVStream(const QFileInfo &f_info);
  virtual ~TTAVStream();

public:
  QFileInfo* fileInfo() { return stream_info; }
  QString fileName();
  QString filePath();
  QString fileExtension();
  quint64 streamLengthByte();
  virtual QTime streamLengthTime() = 0;
  virtual TTAVTypes::AVStreamType streamType() const = 0;
  virtual bool isCutInPoint(int pos) = 0;
  virtual bool isCutOutPoint(int pos) = 0;
  void         abort();
  void         setAbort(bool value);

public:
  virtual int  createHeaderList() = 0;
  virtual int  createIndexList() = 0;
  virtual void cut(int start, int end, TTCutParameter* cp) = 0;
  virtual void copySegment(TTFileBuffer* cut_stream, quint64 start_adr, quint64 end_adr);

protected:
	TTAVTypes::AVStreamType stream_type;
  QFileInfo*              stream_info;
  TTFileBuffer*           stream_buffer;
  bool                    mAbort;
  TTMessageLogger*        log;

signals:
  void statusReport(int state, const QString& msg, quint64 value);
};



// -----------------------------------------------------------------------------
// *** TTAudioStream: Class TTAudioStream
// -----------------------------------------------------------------------------
class TTAudioStream : public TTAVStream
{
public:
  TTAudioStream(const QFileInfo &f_info, int s_pos=0);
  virtual ~TTAudioStream();

  // header list
  TTAudioHeaderList* headerList();

  TTAudioHeader* headerAt( int index );

  // virtual cut methods
  virtual bool isCutInPoint(int)  {return true;};
  virtual bool isCutOutPoint(int)  {return true;};
  virtual int  getStartIndex(int startPos, float frameRate, float& localAudioOffset);
  virtual int  getEndIndex(int endPos, float frameRate, float& localAudioOffset);

protected:
  // header list
  TTAudioHeaderList* header_list;

  // audio_delay > 0: audio starts before video (in ms)
  // audio_delay < 0: audio starts after  video (in ms)
  int    audio_delay;
  int    start_pos;
  int    samples_count;
  int    frame_length;
  double frame_time;
};


// -----------------------------------------------------------------------------
// *** TTVideoStream: Class TTVideoStream
// -----------------------------------------------------------------------------
class TTVideoStream : public TTAVStream
{
 public:
  TTVideoStream( const QFileInfo &f_info );
  virtual ~TTVideoStream();

  // header- and index-list
  TTVideoHeaderList* headerList();
  TTVideoIndexList* indexList();

  int     frameCount();
  float   frameRate();
  float   bitRate();
  QTime   streamLengthTime();
  int     currentFrameType();
  QTime   currentFrameTime();
  quint64 currentFrameOffset();
  int     frameType(int i_pos);
  QTime   frameTime(int i_pos);
  quint64 frameOffset(int i_pos);

  TTSequenceHeader* currentSequenceHeader();
  TTSequenceHeader* getSequenceHeader(int pos);

  // navigation in index-list
  int currentIndex();

  int markerIndex();
  int setMarkerIndex( int index );

  int moveToIndexPos(int index, int f_type=0);
  int moveToNextFrame(int f_type=0);
  int moveToPrevFrame(int f_type=0);
  int moveToNextIFrame();
  int moveToPrevIFrame();
  int moveToNextPFrame();
  int moveToPrevPFrame();
  int moveToNextPIFrame();
  int moveToPrevPIFrame();

protected:
  // List-objects
  TTVideoHeaderList* header_list;
  TTVideoIndexList*  index_list;

  // Navigation
  TTVideoIndex* video_index;
  int           current_index;
  int           current_marker_index;

  // intern
  float         frame_rate;
  float         bit_rate;
};

#endif //TTAVSTREAM_H
