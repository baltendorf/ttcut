/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2005/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmpeg2videostream.cpp                                          */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/*----------------------------------------------------------------------------*/

// -----------------------------------------------------------------------------
// TTMPEG2VIDEOSTREAM
// -----------------------------------------------------------------------------

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

#include "ttmpeg2videostream.h"

#include "../common/ttexception.h"
#include "../common/istatusreporter.h"
#include "../data/ttcutlist.h"
#include "../data/ttcutparameter.h"

#include <QCoreApplication>
#include <stdio.h>
#include <stdlib.h>
#include <QDir>
#include <QStack>
#include <QThread>

/*! ////////////////////////////////////////////////////////////////////////////
 * Constructor with QFileInfo
 */
TTMpeg2VideoStream::TTMpeg2VideoStream( const QFileInfo &f_info )
  : TTVideoStream( f_info )
{
  log = TTMessageLogger::getInstance();

  stream_type   = TTAVTypes::mpeg2_demuxed_video;
  header_list   = NULL;
  index_list    = NULL;
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Destructor
 */
TTMpeg2VideoStream::~TTMpeg2VideoStream()
{
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Create a shared copy from an existing videostream
 */
void TTMpeg2VideoStream::makeSharedCopy( TTMpeg2VideoStream* v_stream )
{
  current_index = 0;
  stream_info   = v_stream->stream_info;
  stream_type   = v_stream->stream_type;
  header_list   = v_stream->header_list;
  index_list    = v_stream->index_list;
  frame_rate    = v_stream->frame_rate;
  bit_rate      = v_stream->bit_rate;
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Create the list with mpeg2 header informations
 */
int TTMpeg2VideoStream::createHeaderList()
{
  QString   iddStreamName;
  QFileInfo iddStreamInfo;

  openStream();

  header_list = new TTVideoHeaderList( 2000 );

  // read the header from IDD file
  if (TTCut::readVideoIDD)
  {
    iddStreamName = ttChangeFileExt(stream_info->filePath(), "idd");
    iddStreamInfo.setFile(iddStreamName);

    if (iddStreamInfo.exists())
    {
      TTFileBuffer* iddStreamBuffer = new TTFileBuffer(iddStreamName, QIODevice::ReadOnly);
      createHeaderListFromIdd(iddStreamBuffer);

      if (iddStreamBuffer != NULL) {
        delete iddStreamBuffer;
        iddStreamBuffer = NULL;
      }
    }
  }

  // Read the header from mpeg2 stream
  if (header_list->count() == 0)
    createHeaderListFromMpeg2();

  if (header_list->count() == 0) {
    if (header_list != NULL) {
      delete header_list;
      header_list = NULL;
    }
    throw TTInvalidOperationException(QString("%1 Could not create header list!").arg(__FILE__));
  }

  return header_list->count();
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Create the mpeg2 video stream picture index list
 */
int TTMpeg2VideoStream::createIndexList()
{
  int    base_number     = 0;
  int    current_pic_num = 0;
  int    index           = 0;
  quint8 start_code      = 0xFF;
  QTime  time;
  TTPicturesHeader* current_pic  = NULL;

  index_list  = new TTVideoIndexList();

  if (TTCut::logVideoIndexInfo) {
    log->infoMsg(__FILE__, __LINE__, "Create index list");
    log->infoMsg(__FILE__, __LINE__, "---------------------------------------------");
  }

  time.start();

  while ( index < header_list->count() )
  {
  	if (mAbort) {
  		mAbort = false;
      qDebug("abort in createIndexList");
  		throw new TTAbortException("Index list creation aborted!");
  	}

    start_code = header_list->at(index)->headerType();

    switch ( start_code )
    {
      case TTMpeg2VideoHeader::group_start_code:
        base_number = current_pic_num;
        break;

      case TTMpeg2VideoHeader::picture_start_code:
        current_pic = (TTPicturesHeader*)header_list->at(index);
        if ( current_pic != NULL )
        {
          TTVideoIndex* video_index = new TTVideoIndex();

          video_index->setDisplayOrder(base_number+current_pic->temporal_reference);
          video_index->setHeaderListIndex(index);
          video_index->setPictureCodingType(current_pic->picture_coding_type);

          index_list->add( video_index );

         if(TTCut::logVideoIndexInfo) {
            log->infoMsg(__FILE__, __LINE__,
                    QString("stream-order;%1;display-order;%2;frame-type;%3;offset;%4").
                    arg(current_pic_num).arg(video_index->getDisplayOrder()).
                    arg(video_index->getPictureCodingType()).arg(current_pic->headerOffset()));
         }
         current_pic_num++;
        }
        break;
    }
    index++;
  }

  log->debugMsg(__FILE__, __LINE__, QString("time for creating index list %1ms").
      arg(time.elapsed()));
  return index_list->count();
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Return the stream type
 */
TTAVTypes::AVStreamType TTMpeg2VideoStream::streamType() const
{
  return TTAVTypes::mpeg2_demuxed_video;
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Open the mpeg2 video stream
 */
bool TTMpeg2VideoStream::openStream()
{
  stream_buffer->open();
  return ttAssigned(stream_buffer);
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Close the current mpeg2 video stream
 */
bool TTMpeg2VideoStream::closeStream()
{
  stream_buffer->close();

  return true;
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Create the mpeg2 header-list from Mpeg2Schnitt idd-file
 */
bool TTMpeg2VideoStream::createHeaderListFromIdd(TTFileBuffer* iddStream)
{
  quint8 buffer4[4];

  iddStream->open();

  if (!iddStream->readByte(buffer4, 4))
    throw TTIOException(QString("Error reading IDD file in %1 at line %2!").arg(__FILE__).arg(__LINE__));

  if (buffer4[0] != int('i') &&
      buffer4[1] != int('d') &&
      buffer4[2] != int('d')    ) {
    throw TTDataFormatException(QString("No IDD file in %1 at line %2!").arg(__FILE__).arg(__LINE__));
    }

  readIDDHeader(iddStream, buffer4[3]);

  iddStream->close();

  return (header_list->count() > 0);
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Create the mpeg2 header-list from mpeg2 stream
 */
bool TTMpeg2VideoStream::createHeaderListFromMpeg2()
{
  quint8         headerType;
  TTVideoHeader* newHeader;
  QTime          time;

  header_list->clear();

  try
  {
    time.start();
    emit statusReport(StatusReportArgs::Start, tr("Creating Mpeg2-Header List"), stream_buffer->size());

    while(!stream_buffer->atEnd())
    {
    	if (mAbort) {
    		mAbort = false;
        qDebug("abort in createHeaderList");
    		throw new TTAbortException("Headerlist creation aborted by user!");
    	}

      headerType = 0xFF;

      // search next header (start code)
      while (headerType != TTMpeg2VideoHeader::picture_start_code  &&
             headerType != TTMpeg2VideoHeader::sequence_start_code &&
             headerType != TTMpeg2VideoHeader::group_start_code    &&
             headerType != TTMpeg2VideoHeader::sequence_end_code  &&
            !stream_buffer->atEnd() )
      {
        stream_buffer->nextStartCodeTS();
        stream_buffer->readByte(headerType);
      }

      newHeader = 0;

      // create the appropriate header object
      switch ( headerType )
      {
        case TTMpeg2VideoHeader::sequence_start_code:
          newHeader = new TTSequenceHeader();
          break;
        case TTMpeg2VideoHeader::picture_start_code:
          newHeader = new TTPicturesHeader();
          break;
        case TTMpeg2VideoHeader::group_start_code:
          newHeader = new TTGOPHeader();
          break;
        case TTMpeg2VideoHeader::sequence_end_code:
          newHeader = new TTSequenceEndHeader();
          break;
      }

      if ( newHeader != 0 )
      {
        newHeader->readHeader( stream_buffer );
        header_list->add( newHeader );
      }

      emit statusReport(StatusReportArgs::Step, tr("Step..."), stream_buffer->position());
    }
    log->debugMsg(__FILE__, __LINE__, QString("time for creating header list %1ms").
        arg(time.elapsed()));
  }
  catch (TTFileBufferException* ex)
  {
    qDebug("ttfilebuffer exception...");
  }

  emit statusReport(StatusReportArgs::Finished, tr("Mpeg2-Header list created"), stream_buffer->size());

  // write an idd file with the header information
  if (header_list->count() > 0 && TTCut::createVideoIDD )
    writeIDDFile();

  return (header_list->count() > 0);
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Write MPEG2Schnit idd file
 */
void TTMpeg2VideoStream::writeIDDFile( )
{
  QString           iddStreamName;
  QFileInfo         iddStreamInfo;
  TTPicturesHeader* currentPicture;
  int               index = 0;
  quint8            headerType;
  quint64           offset;
  quint16           tempRef;
  quint8            codingType;
  quint8            buffer[8];

  // create Mpeg2Schnitt idd-stream name
  iddStreamName = ttChangeFileExt(stream_info->filePath(), "idd");

  // check for Mpeg2Schnitt idd-stream in current directory
  iddStreamInfo.setFile(iddStreamName);

  if (iddStreamInfo.exists())
  {
    QFile idd_file( iddStreamInfo.filePath() );
    idd_file.remove();
  }

  log->debugMsg(__FILE__, __LINE__, QString("create IDD-file %1").arg(iddStreamInfo.filePath()));
  emit statusReport(StatusReportArgs::Start, tr("Create Mpeg2Schnitt IDD-file"), header_list->count());

  // create new idd-stream
  TTFileBuffer* iddStream = new TTFileBuffer(iddStreamName, QIODevice::WriteOnly);

  iddStream->open();

  // IDD header data and version
  buffer[0] = int('i');
  buffer[1] = int('d');
  buffer[2] = int('d');
  buffer[3] = 2;

  iddStream->directWrite(buffer, 4);

  while (index < header_list->count())
  {
  	if (mAbort) {
  		mAbort = false;
  		throw new TTAbortException("Writing IDD file aborted!");
  	}

    headerType = header_list->at(index)->headerType();
    iddStream->directWrite(headerType);          // 1 Byte

    offset      = header_list->at(index)->headerOffset();
    iddStream->directWriteUInt64( offset );      // 8 Byte

    // picture header
    if ( headerType == TTMpeg2VideoHeader::picture_start_code )
    {
      currentPicture = (TTPicturesHeader*)header_list->at(index);
      if ( currentPicture != NULL )
      {
        tempRef = currentPicture->temporal_reference;
        iddStream->directWriteUInt16( tempRef ); // 2 Byte

        codingType = currentPicture->picture_coding_type;
        iddStream->directWrite(codingType);  // 1 Byte
      }
    }
    index++;
    emit statusReport(StatusReportArgs::Step, tr("Create Mpeg2Schnitt IDD-File"), index);
  }

  emit statusReport(StatusReportArgs::Finished, tr("IDD-File created"), header_list->count());

  // write sequence end header type
  buffer[0] = 0xB7;
  iddStream->directWrite(buffer, 1);  // 1 Byte

  // write last file offset
  offset = header_list->at(index-1)->headerOffset();
  iddStream->directWriteUInt64( offset );  // 8 Byte

  iddStream->close();
  delete iddStream;
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Read MPEG2Schnitt *.idd file (also created by projectX)
 */
void TTMpeg2VideoStream::readIDDHeader(TTFileBuffer* iddStream, quint8 iddFileVersion)
{
  quint8              pictureCodingType;
  quint16             temporalReference;
  quint8              headerType;
  quint64             offset;
  TTMpeg2VideoHeader* newHeader = NULL;

  if (iddFileVersion < 2) {
    QString msg = QString("IDD file version %1 not supported!").arg(iddFileVersion);
    throw TTDataFormatException(msg);
  }

  emit statusReport(StatusReportArgs::Start, tr("Read Mpeg2Schnitt IDD-file"), iddStream->size());

  try
  {
    while(!iddStream->atEnd())
    {
    	if (mAbort) {
    		mAbort = false;
     		throw new TTAbortException("Read IDD header file aborted!");
    	}

      iddStream->readByte(headerType);
      iddStream->readUInt64(offset);

      switch (headerType)
      {
        case TTMpeg2VideoHeader::sequence_start_code:
          newHeader = new TTSequenceHeader();
          newHeader->readHeader(stream_buffer, offset);
          break;
        case TTMpeg2VideoHeader::picture_start_code:
          newHeader = new TTPicturesHeader();
          iddStream->readUInt16(temporalReference);
          iddStream->readByte(pictureCodingType);
          ((TTPicturesHeader*)newHeader)->setHeaderOffset(offset);
          ((TTPicturesHeader*)newHeader)->temporal_reference  = temporalReference;
          ((TTPicturesHeader*)newHeader)->picture_coding_type = pictureCodingType;

          //do we realy need the vbv delay???
          newHeader->readHeader(stream_buffer, offset);

            break;
        case TTMpeg2VideoHeader::group_start_code:
          newHeader = new TTGOPHeader();
          newHeader->readHeader(stream_buffer, offset);
          break;
        case TTMpeg2VideoHeader::sequence_end_code:
          newHeader = new TTSequenceEndHeader();
          newHeader->readHeader(stream_buffer, offset);
          break;
      }

      header_list->add( newHeader );

      emit statusReport(StatusReportArgs::Step, tr("Read Mpeg2Schnitt IDD-file"), iddStream->position());
    }
    emit statusReport(StatusReportArgs::Finished, tr("Finish reading IDD-file"), iddStream->size());
  }
  catch (...)
  {
    qDebug("catch something....");
  }
}

/*! ////////////////////////////////////////////////////////////////////////////
 * IsCutInPoint
 * Returns true, if the current index position (picture) is a valid cut-in point
 * In encoder-mode every picture position is a valid cut-in position, else the
 * cut-in is only allowed on I-frames
 */
bool TTMpeg2VideoStream::isCutInPoint(int pos)
{
  if (TTCut::encoderMode)
    return true;

  int index      = (pos < 0) ? currentIndex() : pos;
  int frame_type = index_list->pictureCodingType(index);

  return (frame_type == 1);
}

/*! ////////////////////////////////////////////////////////////////////////////
 * IsCutOutPoint
 * Returns true, if the current index position (picture) is a valid cut-out
 * point.
 * In encoder-mode every picture position is a valid cut-out position, else, the
 * cut-out is only valid on P- or I-frames
 */
bool TTMpeg2VideoStream::isCutOutPoint(int pos)
{
  if (TTCut::encoderMode)
    return true;

  int index      = (pos < 0) ? currentIndex() : pos;
  int frame_type = index_list->pictureCodingType(index);

  return ((frame_type == 1) || (frame_type == 2));
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Cut Method
 * TTFileBuffer*   targetStream: The mpeg2 video target stream
 * int             startIndex:   Start index of current cut
 * int             endIndex:     End index of current cut
 * TTCutParameter: cutParams:    Cut parameter
 */
void TTMpeg2VideoStream::cut(int cutInPos, int cutOutPos, TTCutParameter* cutParams)
{
  openStream();

  log->debugMsg(__FILE__, __LINE__, QString("cut: cutIn %1 / cutOut %2").arg(cutInPos).arg(cutOutPos));

  TTVideoHeader* startObject = getCutStartObject(cutInPos, cutParams);
  TTVideoHeader* endObject   = getCutEndObject(cutOutPos,  cutParams);

  log->debugMsg(__FILE__, __LINE__, QString("startObject: %1").arg(startObject->headerOffset()));
  log->debugMsg(__FILE__, __LINE__, QString("endObject:   %1").arg(endObject->headerOffset()));

  transferCutObjects(startObject, endObject, cutParams);

  if (cutOutPos > cutParams->getCutOutIndex())
    encodePart(cutParams->getCutOutIndex()+1, cutOutPos, cutParams);

  closeStream();
}


/*! /////////////////////////////////////////////////////////////////////////////
 * Return the start object for current cut
 */
TTVideoHeader* TTMpeg2VideoStream::getCutStartObject(int cutInPos, TTCutParameter* cutParams)
{
  int iFramePos = cutInPos;

  log->debugMsg(__FILE__, __LINE__, QString("getStartObject::cutIn %1").arg(cutInPos));
  log->debugMsg(__FILE__, __LINE__, QString("frame-type is %1").arg(index_list->pictureCodingType(iFramePos)));

  // if coding type is not I-frame, we must encode
  if (index_list->pictureCodingType(iFramePos) != 1)
  {
    iFramePos = index_list->moveToNextIndexPos(cutInPos, 1);

    encodePart(cutInPos, iFramePos-1, cutParams);
  }

  TTVideoHeader* start_object = checkIFrameSequence(iFramePos, cutParams);
  cutParams->setCutInIndex(iFramePos);

  return start_object;
}

/*! /////////////////////////////////////////////////////////////////////////////
 * Check cut in position and try to insert eventually missing sequence
 */
TTVideoHeader* TTMpeg2VideoStream::checkIFrameSequence(int iFramePos, TTCutParameter* cutParams)
{
  int seqHeaderIndex  = ((index_list->headerListIndex(iFramePos) - 2) > 0)
      ? index_list->headerListIndex(iFramePos) - 2
      : 0;

  TTVideoHeader* currentHeader = (TTVideoHeader*)header_list->headerAt(seqHeaderIndex);

  //Check for sequence
  if (currentHeader->headerType() == TTMpeg2VideoHeader::sequence_start_code)
    return currentHeader;

  TTSequenceHeader* seqHeader = (TTSequenceHeader*)header_list->getNextHeader(
      seqHeaderIndex, TTMpeg2VideoHeader::sequence_start_code);

  if (!ttAssigned(seqHeader))
  {
    QString msg = QString("No sequence header for I-Frame at index %1").arg(iFramePos);
    throw TTArgumentNullException(msg);
  }

  //Check for GOP
  int gopHeaderIndex      = header_list->headerIndex((TTVideoHeader*)seqHeader) + 1;
  TTVideoHeader* gopHeader = header_list->headerAt(gopHeaderIndex);

  if (!ttAssigned(gopHeader))
  {
    QString msg = QString("No GOP Header found for I-Frame at index %1").arg(iFramePos);
    throw TTArgumentNullException(msg);
  }

  // inject the sequence header into the target stream
  copySegment(cutParams->getTargetStreamBuffer(), seqHeader->headerOffset(), gopHeader->headerOffset()-1);

  if (cutParams->getMaxBitrate() < seqHeader->bit_rate_value)
    cutParams->setMaxBitrate(seqHeader->bit_rate_value);

  QString msg = QString("Insert missing sequence header in cut target stream!");
  log->fatalMsg(__FILE__, __LINE__, msg);
  throw TTMissingMethodException(msg);
  //TODO: here we need a sequence copy constructor!
  //TTSequenceHeader* newSequence = new TTSequenceHeader();
  //newSequence->setHeaderOffset((quint64)0);
  //newSequence->aspect_ratio_information = seqHeader->aspect_ratio_information;

  //cutParams->result_header_list->add(newSequence);
  return gopHeader;
}

/*! /////////////////////////////////////////////////////////////////////////////
 * Returns the end object for current cut
 */
TTVideoHeader* TTMpeg2VideoStream::getCutEndObject(int cutOutPos, TTCutParameter* cutParams)
{
  int ipFramePos = cutOutPos;

  log->debugMsg(__FILE__, __LINE__, QString("getEndObject::cutIn %1").arg(cutOutPos));
  log->debugMsg(__FILE__, __LINE__, QString("frame-type is %1").arg(index_list->pictureCodingType(ipFramePos)));

  while (ipFramePos >= 0 && index_list->pictureCodingType(ipFramePos) == 3)
    ipFramePos--;

  if (index_list->pictureCodingType(ipFramePos) == 3) {
    QString msg = QString("No I- or P-Frame found at cut out position: %1").arg(cutOutPos);
    throw TTInvalidOperationException(msg);
  }

  int headerListPos           = index_list->headerListIndex(ipFramePos);
  TTPicturesHeader* endObject = (TTPicturesHeader*)header_list->headerAt(headerListPos);
  cutParams->setCutOutIndex(ipFramePos);

  if (headerListPos+1 >= header_list->count())
    return endObject;

  // the following B-Frames belongs to this cut out position
  do {
    headerListPos++;
    TTPicturesHeader* tmpObject = (TTPicturesHeader*)header_list->headerAt(headerListPos);

    if (tmpObject->headerType() != TTMpeg2VideoHeader::picture_start_code)
      break;

    if (tmpObject->picture_coding_type != 3)
      break;

    endObject = tmpObject;
  } while (headerListPos+1 < header_list->count());

  return endObject;
}

/*! /////////////////////////////////////////////////////////////////////////////
 * Returns the number of bytes between startObject and endObject
 */
quint64 TTMpeg2VideoStream::getByteCount(TTVideoHeader* startObject, TTVideoHeader* endObject)
{
  quint64 startOffset    = startObject->headerOffset();
  int     endObjectIndex = header_list->headerIndex(endObject);
  quint64 endOffset      = (endObjectIndex+1 < header_list->count())
           ? header_list->headerAt(endObjectIndex+1)->headerOffset()
           : stream_buffer->size();

  return (endOffset - startOffset);
}

/*! /////////////////////////////////////////////////////////////////////////////
 * Transfers the cuts objects to the target stream
 * Remark: [startObject, endObject[
 */
void TTMpeg2VideoStream::transferCutObjects(TTVideoHeader* startObject, TTVideoHeader* endObject, TTCutParameter* cr)
{
  quint8    buffer[65536];
  quint64   bytesToWrite      = getByteCount(startObject, endObject);
  quint64   bufferStartOffset = startObject->headerOffset();
  int       numPicsWritten = cr->getNumPicturesWritten();
  quint64   process        = 0;
  bool      closeNextGOP   = true;  // remove B-frames
  int       tempRefDelta   = 0;     // delta for temporal reference if closed GOP
  const int watermark      = 12;    // size of header type-code (12 byte)
  bool      objectProcessed;
  bool      isContinue = false;

  log->debugMsg(__FILE__, __LINE__, QString("transferCutObjects::bytesToWrite %1").
      arg(bytesToWrite));

  TTVideoHeader*          currentObject = startObject;
  QStack<TTBreakObject*>* break_objects = new QStack<TTBreakObject*>;

  stream_buffer->seekAbsolute( startObject->headerOffset() );

  //qDebug(qPrintable(QString("transferCutObjects -> emit start in thread %1").arg(QThread::currentThreadId())));
  emit statusReport(StatusReportArgs::Start, tr("Transfer objects"), bytesToWrite);
  qApp->processEvents();

  while( bytesToWrite > 0 )
  {
   int bytesProcessed = (bytesToWrite < 65536)
        ? stream_buffer->readByte(buffer, bytesToWrite)
        : stream_buffer->readByte(buffer, 65536);

    if (bytesProcessed <= 0)
      throw TTIOException(QString("%1 bytes from stream buffer read").arg(bytesProcessed));

    do
    {
    	if (mAbort) {
    		mAbort = false;
    		throw new TTAbortException("Read IDD header file aborted!");
    	}

      isContinue = false;

      // is start address not in current buffer
      if (currentObject->headerOffset() < bufferStartOffset || currentObject->headerOffset() > (bufferStartOffset+bytesProcessed-1)) {
        break;
      }


      objectProcessed = (currentObject->headerOffset() < bufferStartOffset+bytesProcessed-watermark);

      if (!objectProcessed) {
        stream_buffer->seekBackward(watermark);
        bytesProcessed -= watermark;
        break;
      }

      // removing unwanted objects
      if ( break_objects->count() > 0 )
      {
        TTBreakObject* current_break = (TTBreakObject*)break_objects->top();

        if ( current_break->stopObject() != NULL &&
            current_break->stopObject()->headerOffset() == currentObject->headerOffset() )
        {
          quint64 adress_delta = current_break->restartObject()->headerOffset()-currentObject->headerOffset();
          bytesProcessed       = (int)(currentObject->headerOffset()-bufferStartOffset);
          bytesToWrite        -= adress_delta;
          bufferStartOffset   += adress_delta;
          currentObject        = current_break->restartObject(); // hier gehts weiter

          stream_buffer->seekAbsolute(current_break->restartObject()->headerOffset());
          current_break->setStopObject((TTVideoHeader*)NULL );

          objectProcessed = false;
          continue;
        }


        if (current_break->restartObject()->headerOffset() == currentObject->headerOffset())
        {
          TTBreakObject* tmpBreak = break_objects->pop();
          if (ttAssigned(tmpBreak))
            delete tmpBreak;
        }
      }

      switch(currentObject->headerType())
      {
        case TTMpeg2VideoHeader::sequence_start_code:
          // Maximale Bitrate ermitteln
          if (cr->getMaxBitrate() < ((TTSequenceHeader*)currentObject)->bit_rate_value)
            cr->setMaxBitrate(((TTSequenceHeader*)currentObject)->bit_rate_value);
           break;

        case TTMpeg2VideoHeader::sequence_end_code: {
          //remove sequence end code
          TTBreakObject* new_break = new TTBreakObject();

          new_break->setStopObject(currentObject);
          new_break->setRestartObject(header_list->getNextHeader(currentObject));
          break_objects->push( new_break );
          isContinue = true;
        }
          break;

        case TTMpeg2VideoHeader::group_start_code:
          {
            TTPicturesHeader* nextPic = (TTPicturesHeader*)header_list->getNextHeader(
              currentObject, TTMpeg2VideoHeader::picture_start_code);

          if (closeNextGOP && (ttAssigned(nextPic)) && (nextPic->temporal_reference == 0))
            closeNextGOP = false;

          rewriteGOP(buffer, bufferStartOffset, (TTGOPHeader*)currentObject, closeNextGOP, cr);
        }
          break;

        case TTMpeg2VideoHeader::picture_start_code: {
          TTPicturesHeader* currentPicture = (TTPicturesHeader*)currentObject;

          if (closeNextGOP       &&
              tempRefDelta  != 0 &&
              currentPicture->picture_coding_type == 3)
          {
            removeOrphanedBFrames(break_objects, currentObject);
            closeNextGOP = false;
            isContinue   = true;
            break;
            //continue;
          }

          numPicsWritten++;
          //log->debugMsg(__FILE__, __LINE__, QString("picture %1 transfered %2").
          //    arg(numPicsWritten).arg(currentPicture->headerOffset()));
          cr->setNumPicturesWritten(numPicsWritten);

          if (currentPicture->picture_coding_type == 1) {
            tempRefDelta = (closeNextGOP)
              ? currentPicture->temporal_reference
              : 0;
          }

          // M�ssen neue tempor�rere Referenzen geschrieben werden?
          if ( tempRefDelta != 0)
          {
            rewriteTempRefData(buffer, currentPicture, bufferStartOffset, tempRefDelta);
          }
        }
          break;
      }

      if (isContinue)
        continue;


      if (currentObject == endObject) {
        break;
      }

      currentObject = header_list->getNextHeader(currentObject);

     }while(objectProcessed);

    cr->getTargetStreamBuffer()->directWrite(buffer, bytesProcessed);

    process           += bytesProcessed;
    bytesToWrite      -= bytesProcessed;
    bufferStartOffset += bytesProcessed;

    emit statusReport(StatusReportArgs::Step, tr("Transfer objects"), process);
    qApp->processEvents();

  }

  //qDebug("transferCutObjects -> emit finished");
  emit statusReport(StatusReportArgs::Finished, tr("Transfer complete"), process);
  qApp->processEvents();

  // clean-up
  for (int i = 0; i < break_objects->size(); i++) {
    TTBreakObject* tmpBreak = break_objects->at(i);
    if (ttAssigned(tmpBreak))
      delete tmpBreak;
  }

  if (ttAssigned(break_objects)) {
    delete break_objects;
    break_objects = NULL;
  }
}

/*! /////////////////////////////////////////////////////////////////////////////
 * Put orphaned B-Frames to break objects stack for removal
 */
void TTMpeg2VideoStream::removeOrphanedBFrames(QStack<TTBreakObject*>* breakObjects, TTVideoHeader* currentObject)
{
  TTVideoHeader* nextObject = currentObject;

  do
  {
    log->debugMsg(__FILE__, __LINE__, QString(">>> remove B-Frame at %1").arg(nextObject->headerOffset()));
    nextObject = header_list->getNextHeader(nextObject);
  }
  while (nextObject != NULL &&
      nextObject->headerType() == TTMpeg2VideoHeader::picture_start_code &&
      ((TTPicturesHeader*)nextObject)->picture_coding_type == 3);

  TTBreakObject* new_break = new TTBreakObject();

  new_break->setStopObject(currentObject);
  new_break->setRestartObject(nextObject);

  log->debugMsg(__FILE__, __LINE__, QString("stop object %1 / restart object %2").
      arg(currentObject->headerOffset()).
      arg(nextObject->headerOffset()));

  breakObjects->push( new_break );
}

/*! /////////////////////////////////////////////////////////////////////////////
 * Rewrites the time codes in GOP header
 */
void TTMpeg2VideoStream::rewriteGOP(quint8* buffer, quint64 abs_pos, TTGOPHeader* gop, bool close_gop, TTCutParameter* cr)
{
  if (abs_pos > gop->headerOffset()) {
    log->errorMsg(__FILE__, __LINE__, "buffer position is invalid in rewrite GOP!!!");
    return;
  }

  quint8      time_code[4];
  TTTimeCode  tc = ttFrameToTimeCode(cr->getNumPicturesWritten(), frameRate());

  time_code[0]=(quint8)(((tc.drop_frame_flag?1:0)<<7)
      +((tc.hours & 0x1f)<<2)
      +((tc.minutes & 0x30)>>4));
  time_code[1]=(quint8)(((tc.minutes & 0x0f)<<4)
      +((tc.marker_bit?1:0)<<3)
      +((tc.seconds & 0x38)>>3));
  time_code[2]=(quint8)(((tc.seconds & 0x07)<<5)
      + ((tc.pictures & 0x3e)>>1));
  time_code[3]=(quint8)((((tc.pictures & 0x01)==1)?0x80:0x00)
      | ((close_gop || gop->closed_gop)?0x40:0)
      | (gop->broken_link ? 0x20:0x00));
  buffer[(int)(gop->headerOffset()-abs_pos)+4] = time_code[0];
  buffer[(int)(gop->headerOffset()-abs_pos)+5] = time_code[1];
  buffer[(int)(gop->headerOffset()-abs_pos)+6] = time_code[2];
  buffer[(int)(gop->headerOffset()-abs_pos)+7] = time_code[3];
}

/* /////////////////////////////////////////////////////////////////////////////
 * Rewrite temporal references
 */
void TTMpeg2VideoStream::rewriteTempRefData(quint8* buffer, TTPicturesHeader* currentPicture, quint64 bufferStartOffset, int tempRefDelta)
{
  qint16 newTempRef = (qint16)(currentPicture->temporal_reference-tempRefDelta);
  int    offset     = (int)(currentPicture->headerOffset()-bufferStartOffset)+4; // Hier rein damit!

  buffer[offset]   = (quint8)(newTempRef >> 2);               // Bit 10 - 2 von 10 Bit Tempref
  buffer[offset+1] = (quint8)(((newTempRef & 0x0003) << 6) +  // Bit 1 und 0 von 10 Bit Tempref
      ((int)currentPicture->picture_coding_type << 3) +       // Bildtype auf Bit 5, 4 und 3
      (currentPicture->vbv_delay >> 13));                     // 3 Bit von VBVDelay
}

/*! ////////////////////////////////////////////////////////////////////////////
 * EncodePart
 * Decode and encode the part between start and end
 * int             start: start index (display order)
 * int             end  : end index (display orser)
 * TTCutParameter* cr   : Cut parameter
 */
void TTMpeg2VideoStream::encodePart(int start, int end, TTCutParameter* cr)
{
  // save current cut parameter
  bool savIsWriteMaxBitrate  = cr->getIsWriteMaxBitrate();
  bool savIsWriteSequenceEnd = cr->getIsWriteSequenceEnd();
  bool savCreateVideoIDD     = TTCut::createVideoIDD;
  bool savReadVideoIDD       = TTCut::readVideoIDD;

  // no sequence end code
  cr->setIsWriteSequenceEnd(false);
  cr->setIsWriteMaxBitrate(false);
  TTCut::createVideoIDD = false;
  TTCut::readVideoIDD   = false;

  log->debugMsg(__FILE__, __LINE__, QString("enocdePart start %1 / end %2").arg(start).arg(end));

  // use the sequence header according to current picture for information about
  // frame size (width x height) and aspect ratio
  TTSequenceHeader* seq_head  = getSequenceHeader(current_index);

  QDir      tempDir( TTCut::tempDirPath );
  QString   aviOutFile   = "encode.avi";
  QString   mpeg2OutFile = "encode";          // extension is added by transcode (!)
  QFileInfo aviFileInfo(tempDir, aviOutFile);
  QFileInfo mpeg2FileInfo(tempDir, mpeg2OutFile);

  TTEncodeParameter encParams;

  encParams.setAVIFileInfo(aviFileInfo);
  encParams.setMpeg2FileInfo(mpeg2FileInfo);
  encParams.setVideoWidth(seq_head->horizontal_size_value);
  encParams.setVideoHeight(seq_head->vertical_size_value);
  encParams.setVideoAspectCode(seq_head->aspect_ratio_information);
  encParams.setVideoBitrate(seq_head->bitRateKbit());

  TTTranscodeProvider* transcode_prov = new TTTranscodeProvider(encParams);

  connect(transcode_prov, SIGNAL(statusReport(int, const QString&, quint64)),
          this,           SIGNAL(statusReport(int, const QString&, quint64)));

  qApp->processEvents();
  bool success = transcode_prov->encodePart((TTVideoStream*)this, start, end);


  if (!success) {
    QString msg = QString("Error in encode part!");;
    log->fatalMsg(__FILE__, __LINE__, msg);
    throw TTInvalidOperationException(msg);
  }

  mpeg2FileInfo.setFile(tempDir, "encode.m2v");
  TTMpeg2VideoStream* new_mpeg_stream = new TTMpeg2VideoStream(mpeg2FileInfo);

  new_mpeg_stream->createHeaderList();
  new_mpeg_stream->createIndexList();
  new_mpeg_stream->indexList()->sortDisplayOrder();

  int cutOut = (end-start ==  0) ? 0 : end-start;
  new_mpeg_stream->cut(0, cutOut, cr);

  // remove temporary files
  QString rmCmd = QString("rm %1/encode.*").arg(mpeg2FileInfo.absolutePath());

  if (system(rmCmd.toAscii().data()) < 0)
    log->errorMsg(__FILE__, __LINE__, QString("system call %1 failed!").arg(rmCmd));

  cr->setIsWriteMaxBitrate(savIsWriteMaxBitrate);
  cr->setIsWriteSequenceEnd(savIsWriteSequenceEnd);
  TTCut::readVideoIDD   = savReadVideoIDD;
  TTCut::createVideoIDD = savCreateVideoIDD;

  disconnect(transcode_prov, SIGNAL(statusReport(int, const QString&, quint64)),
  		       this,           SIGNAL(statusReport(int, const QString&, quint64)));

  delete transcode_prov;
  delete new_mpeg_stream;
}
