/*----------------------------------------------------------------------------*/
/* COPYRIGHT: Minei3oat (c) 2019 / github.com/Minei3oat                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2019                                                      */
/* FILE     : ttsrtsubtitlestream.cpp                                         */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : Minei3oat                                       DATE: 12/29/2019 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTSRTSUBTITLESTREAM
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
//             +- TTVideoStream - TTMpeg2VideoStream
//             |
//             +- TTSubtitleStream - TTSrtSubtitleStream
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

#include "ttsrtsubtitlestream.h"
#include "ttsubtitleheaderlist.h"

#include "common/istatusreporter.h"
#include "common/ttexception.h"
#include "data/ttcutparameter.h"

#include <QCoreApplication>

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// *** TTSrtSubtitleStream: Srt subtitle stream class
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

//! Constructor with QFileInfo and start position
TTSrtSubtitleStream::TTSrtSubtitleStream(const QFileInfo &f_info)
  :TTSubtitleStream(f_info)
{
  log = TTMessageLogger::getInstance();
}

//! Destructor
TTSrtSubtitleStream::~TTSrtSubtitleStream()
{
}

//! Return the stream type
TTAVTypes::AVStreamType TTSrtSubtitleStream::streamType() const
{
  return TTAVTypes::srt_subtitle;
}

//! Returns the streams file extension
QString TTSrtSubtitleStream::streamExtension()
{
  return (stream_info != 0)
        ? stream_info->suffix().toLower()
        : ".srt";
}

//! Return the stream length as QTime
QTime TTSrtSubtitleStream::streamLengthTime()
{
  TTSubtitleHeader* lastHeader = (TTSubtitleHeader*)header_list->at(header_list->count()-1);
  return lastHeader->endTime();
}

//! Cut the subtitle stream
void TTSrtSubtitleStream::cut(int start, int end, TTCutParameter* cp)
{
  int index = header_list->searchTimeIndex(start);
  cp->setCutOutIndex(cp->getCutInIndex()+end-start);
  TTFileBuffer* stream_buffer = cp->getTargetStreamBuffer();
  int picsWritten = cp->getNumPicturesWritten();
  int progress = 0;
  int offsett = cp->getCutInIndex()-start;

  emit statusReport(StatusReportArgs::Start, "Subtitle-cut: copy subtitle", header_list->searchTimeIndex(end) - index + 1);
  qApp->processEvents();

  while (index < header_list->count())
  {
    if (mAbort) {
      mAbort = false;
      throw new TTAbortException("User abort request in TTSrtSubtitleStream::cut!");
    }
    TTSubtitleHeader* header = (TTSubtitleHeader*)header_list->at(index);
    if (header->startMSec() > end)
      return;

    picsWritten++;
    QTime subtitleStart  = header->startMSec() <= start ? QTime::fromMSecsSinceStartOfDay(start) : header->startTime();
    QTime subtitleEnd    = header->endMSec() <= end ? header->endTime() : QTime::fromMSecsSinceStartOfDay(end);
    QString subtitleCode = QString("%1\r\n%2 --> %3\r\n%4\r\n\r\n")
        .arg(picsWritten)
        .arg(subtitleStart.addMSecs(offsett).toString("hh:mm:ss,zzz"))
        .arg(subtitleEnd.addMSecs(offsett).toString("hh:mm:ss,zzz"))
        .arg(header->text());

    stream_buffer->directWrite((quint8*)subtitleCode.toUtf8().data(), subtitleCode.toUtf8().length());

    cp->setNumPicturesWritten(picsWritten);
    index++;
    progress++;
    emit statusReport(StatusReportArgs::Step, "Subtitle-cut: copy subtitle", progress);
    qApp->processEvents();
  }
  emit statusReport(StatusReportArgs::Step, "Subtitle-cut: copy segment", progress);
  emit statusReport(StatusReportArgs::Finished, "Subtitle-cut: finished", progress);
  qApp->processEvents();
}

//! Read subtitles
int TTSrtSubtitleStream::createHeaderList()
{
  header_list = new TTSubtitleHeaderList( 100 );

  try
  {
    emit statusReport(StatusReportArgs::Start, "Create subtitle-header list", stream_buffer->size());

    QString lineEnd;
    quint8 byte = 0;
    quint8 lastByte = 0;
    while (!stream_buffer->atEnd())
    {
      stream_buffer->readByte(byte);
      if (byte == '\n')
      {
        lineEnd = lastByte == '\r' ? "\r\n" : "\n";
        break;
      }
      lastByte = byte;
    }
    stream_buffer->seekAbsolute(0);

    QString line;
    int counter = -1;

    while (!stream_buffer->atEnd())
    {
      while (line.isEmpty())
      {
        if (stream_buffer->atEnd())
          return header_list->count();
        line  = stream_buffer->readLine(lineEnd).simplified();
      }
      if (line.toInt() != counter + 1 && counter != -1)
        log->warningMsg("TTSrtSubtitleStream", QString("Subtitles in %1 missing. Reading subtitle %2, last was %3.").arg(fileName()).arg(counter).arg(line));
      counter = line.toInt();

      line = stream_buffer->readLine(lineEnd).simplified();
      TTSubtitleHeader* header = new TTSubtitleHeader();
      header->setStartTime(QTime::fromString(line.left(12), "hh:mm:ss,zzz"));
      header->setEndTime(QTime::fromString(line.right(12), "hh:mm:ss,zzz"));

      QString text;
      do
      {
        line = stream_buffer->readLine(lineEnd);
        text.append(line);
        text.append("\r\n");
      }
      while (!line.isEmpty());
      while(text.right(2) == "\r\n")
        text = text.left(text.length()-2);
      header->setText(text);

      header_list->append(header);

      emit statusReport(StatusReportArgs::Step, "Create subtitle-header list", stream_buffer->position());
    }
    emit statusReport(StatusReportArgs::Finished, "Subtitle-header list created", stream_buffer->position());
  }
  catch (TTFileBufferException)
  {
  }

  log->debugMsg(__FILE__, __LINE__, QString("header list created: %1").arg(header_list->count()));
  log->debugMsg(__FILE__, __LINE__, QString("abs stream length:   %1").arg(streamLengthTime().toString("hh:mm:ss.zzz")));

  return header_list->count();
}

