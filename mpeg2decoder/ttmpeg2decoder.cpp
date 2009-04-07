/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmpeg2decoder.cpp                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/23/2005 */
/* MODIFIED: b. altendorf (mostly rewritten)                 DATE: 12/16/2007 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMPEG2DECODER
// ----------------------------------------------------------------------------

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


#include "ttmpeg2decoder.h"

static TFrameInfo frameInfo;

/* /////////////////////////////////////////////////////////////////////////////
 * Constructor with filename, index- and header-list
 */
TTMpeg2Decoder::TTMpeg2Decoder(QString cFName, TTVideoIndexList* viIndex, TTVideoHeaderList* viHeader, TPixelFormat pixelFormat)
{
  mpeg2Stream        = NULL;
  mpeg2Decoder       = NULL;
  decoderBuffer      = NULL;
  decoderBufferSize  = 0;
  streamBuffer       = NULL;
  streamBufferSize   = 0;
  sliceData          = NULL;
  videoIndexList     = viIndex;
  videoHeaderList    = viHeader;
  t_frame_info       = NULL;

   if (!ttAssigned(videoIndexList) && !ttAssigned(videoHeaderList))
    throw TTMpeg2DecoderException(TTMpeg2DecoderException::ArgumentNull);

  openMPEG2File(cFName);

  // initialize mpeg-2 decoder objekt
  initDecoder(pixelFormat);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Desctructor
 */
TTMpeg2Decoder::~TTMpeg2Decoder()
{
  mpeg2_close( mpeg2Decoder );

  if ( ttAssigned(mpeg2Stream) )
  {
    mpeg2Stream->close();
    delete mpeg2Stream;
  }

  if (ttAssigned(streamBuffer))
     delete [] streamBuffer;

  if (ttAssigned(decoderBuffer))
    delete [] decoderBuffer;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Initialize libmpeg2 and create a decoder object
 */
void TTMpeg2Decoder::initDecoder(TPixelFormat pixelFormat)
{
  convType     =  pixelFormat;
  mpeg2Decoder = mpeg2_init();

  if (mpeg2Decoder == NULL)
    throw TTMpeg2DecoderException(TTMpeg2DecoderException::DecoderInit);

  // initialize the decoder buffer
  decoderBuffer     = new quint8 [initialDecoderBufferSize];
  decoderBufferSize = initialDecoderBufferSize;

  mpeg2Info = mpeg2_info(mpeg2Decoder);

  if (!ttAssigned(mpeg2Info))
    throw TTMpeg2DecoderException(TTMpeg2DecoderException::DecoderInit);

  decodeFirstMPEG2Frame(pixelFormat);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Open a mpeg2 file stream and init buffered file I/O
 */
void TTMpeg2Decoder::openMPEG2File(QString cFName)
{
  if (mpeg2Stream == NULL)
  {
    mpeg2Stream = new QFile(cFName);
    mpeg2Stream->open(QIODevice::ReadOnly);

     if (!mpeg2Stream->isOpen())
       throw TTMpeg2DecoderException(TTMpeg2DecoderException::StreamOpen);
  }
  else
     mpeg2Stream->open(QIODevice::ReadOnly );

  // initialize the stream buffer for file I/O
  if (streamBuffer == NULL)
  {
      streamBufferSize = initialStreamBufferSize;
      streamBuffer = new quint8[streamBufferSize];
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Return current stream size in byte
 */
quint64 TTMpeg2Decoder::fileSize()
{
  return mpeg2Stream->size();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Return the current stream filename
 */
QString TTMpeg2Decoder::fileName()
{
  return mpeg2Stream->fileName();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Decode the next following slice (picture) in the stream.
 */
int TTMpeg2Decoder::gotoNextFrame()
{
  return decodeNextFrame();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Skip count number of frames by decoding each skiped slice.
 */
int TTMpeg2Decoder::skipFrames(int count)
{
  if (count <= 0)
    return 0;

  while (count > 0)
  {
    if (decodeNextFrame() == 0)
      return 0;

    count--;
  }

  return 1;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Seek to given offset in stream and decode the next following slice in stream
 */
int TTMpeg2Decoder::seek(quint64 seekOffset)
{
  mpeg2Stream->seek(seekOffset);

  // full_reset=0: lib starts decoding at the next picture
  // full_reset=1: lib starts decoding at the next sequence header
  int resetMode = (seekOffset == 0) ? 1 : 0;

  mpeg2_reset(mpeg2Decoder, resetMode);

  return decodeNextFrame();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Decode the next mpeg2 frame and return an TTFrameInfo struct or null if
 * no slice can be found.
 */
int TTMpeg2Decoder::decodeNextFrame()
{
  qint64 readSize    = streamBufferSize;
  bool   isStreamEnd = false;
  t_frame_info       = NULL;

  do
  {
    state = mpeg2_parse (mpeg2Decoder);

    switch (state)
    {
      case STATE_BUFFER:
        readSize = mpeg2Stream->read((char*)streamBuffer, streamBufferSize );

        if (readSize > 0)
          isStreamEnd = false;
        else
        {
          if (!isStreamEnd)
          {
            readSize = 4;
            streamBuffer[0] = 0x00;
            streamBuffer[1] = 0x00;
            streamBuffer[2] = 0x01;
            streamBuffer[3] = 0xb7;
          }
        }
        mpeg2_buffer (mpeg2Decoder, streamBuffer, streamBuffer + readSize);
        break;
    case STATE_SEQUENCE:
        switch ( convType )
        {
          case formatRGB8:
          case formatYV12:
          case formatYUV24:
            break;
          case formatRGB24:
            mpeg2_convert (mpeg2Decoder, mpeg2convert_rgb24, NULL);
            break;
          case formatRGB32:
            mpeg2_convert (mpeg2Decoder, mpeg2convert_rgb32, NULL);
            break;
        }
      case STATE_END:
        isStreamEnd = true;
      case STATE_SLICE:
      case STATE_INVALID_END:
        if ( mpeg2Info->display_fbuf )
        {
          t_frame_info            = &frameInfo;
          frameInfo.Y             = mpeg2Info->display_fbuf->buf[0];
          frameInfo.U             = mpeg2Info->display_fbuf->buf[1];
          frameInfo.V             = mpeg2Info->display_fbuf->buf[2];
          frameInfo.width         = mpeg2Info->sequence->width;
          frameInfo.height        = mpeg2Info->sequence->height;
          frameInfo.type          = mpeg2Info->display_picture->flags&0x03;
          frameInfo.chroma_width  = mpeg2Info->sequence->chroma_width;
          frameInfo.chroma_height = mpeg2Info->sequence->chroma_height;


          switch (convType)
          {
            case formatRGB24:
            	//qDebug("formatRGB24");
              frameInfo.size=frameInfo.width*frameInfo.height*3;
              frameInfo.chroma_size=0;
              break;
            case formatRGB32:
            	//qDebug("formatRGB32");
              frameInfo.size=frameInfo.width*frameInfo.height*4;
              frameInfo.chroma_size=0;
              break;
            case formatYV12:
            	//qDebug("formatYV12");
              frameInfo.size=frameInfo.width*frameInfo.height;
              frameInfo.chroma_size=frameInfo.chroma_width*frameInfo.chroma_height;
            default:
              break;
          }
          return 1;
        }
        break;
      default:
        break;
    }
  } while (readSize > 0);

  t_frame_info = NULL;
  return 0;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Seek to position 0 and decode the first slice
 */
TFrameInfo* TTMpeg2Decoder::decodeFirstMPEG2Frame(TPixelFormat pixelFormat)
{
  convType = pixelFormat;
  seek(0);

  TTVideoIndex* currentFrameIndex = videoIndexList->videoIndexAt(0);
  desiredFramePos   = 0;
  desiredFrameType  = currentFrameIndex->getPictureCodingType();

  return t_frame_info;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Get the current frame data (decoded picture)
 */
void TTMpeg2Decoder::getCurrentFrameData(quint8* data )
{
  switch ( convType )
  {
  case formatRGB24:
  case formatRGB32:
    memcpy(data,t_frame_info->Y,t_frame_info->size);
    break;
  case formatYV12:
    memcpy(data,t_frame_info->Y,t_frame_info->size);
    data+=t_frame_info->size;
    memcpy(data,t_frame_info->V,t_frame_info->chroma_size);
    data+=t_frame_info->chroma_size;
    memcpy(data,t_frame_info->U,t_frame_info->chroma_size);
    break;
  default:
    break;
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Get the current decoded frame info
 */
TFrameInfo* TTMpeg2Decoder::getFrameInfo()
{
  return t_frame_info;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Decode the next frame
 */
TFrameInfo* TTMpeg2Decoder::decodeMPEG2Frame(TPixelFormat pixelFormat)
{
  convType = pixelFormat;

  if (!mpeg2Stream->atEnd())
    decodeNextFrame();

  return t_frame_info;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Move to given index position and decode the frame
 */
int TTMpeg2Decoder::moveToFrameIndex(int framePosition)
{
  int           intraFramePosition;
  TTVideoIndex* currentFrameIndex;

  currentFrameIndex = videoIndexList->videoIndexAt(framePosition);

  intraFramePosition = framePosition;
  desiredFramePos    = framePosition;
  desiredFrameType   = currentFrameIndex->getPictureCodingType();

  while (currentFrameIndex->getPictureCodingType() != 1 && intraFramePosition >= 0) {
  	intraFramePosition--;
  	currentFrameIndex = videoIndexList->videoIndexAt(intraFramePosition);
  }

  // search intra frame before framePositions
  /*if (currentFrameIndex->getPictureCodingType() == 1)
    intraFramePosition = framePosition;
  else
  {
    intraFramePosition = framePosition-1;

    while (intraFramePosition >= 0)
    {
      currentFrameIndex = videoIndexList->videoIndexAt(intraFramePosition);
      if (currentFrameIndex->getPictureCodingType() == 1)
        break;

      intraFramePosition--;
    }
  }

  if (intraFramePosition < 0)
  {
    //qDebug("No previous intra frame found for positin %d! What should i do?", framePosition);
    intraFramePosition = 0;
  }*/

  //currentFrameIndex = videoIndexList->videoIndexAt(intraFramePosition);

  if (currentFrameIndex->getPictureCodingType() != 1)
    qDebug("no intra frame at index: %d", intraFramePosition);

  // look for according sequence header
  int headerListIndex = currentFrameIndex->getHeaderListIndex();

  while (headerListIndex >= 0 &&
         videoHeaderList->headerTypeAt(headerListIndex) != TTMpeg2VideoHeader::sequence_start_code)
     headerListIndex--;

  quint64 sequenceOffset = 0;

  if (headerListIndex >= 0 && videoHeaderList->headerTypeAt(headerListIndex) == TTMpeg2VideoHeader::sequence_start_code)
  {
    TTSequenceHeader* seqHeader = videoHeaderList->sequenceHeaderAt(headerListIndex);
    sequenceOffset = seqHeader->headerOffset();
  }
  //else
  //  qDebug("no sequence at: %d", headerListIndex);

  // position the lib
  seek(sequenceOffset);

  while (t_frame_info != NULL && frameInfo.type != 1)
    decodeNextFrame();

  skipFrames(framePosition-intraFramePosition);

  return framePosition;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Exception class for TTMpeg2Decoder
 */
TTMpeg2DecoderException::TTMpeg2DecoderException(ExceptionType type)
{
  ex_type = type;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Return the exception message
 */
QString TTMpeg2DecoderException::message()
{
  switch (ex_type)
  {
    case TTMpeg2DecoderException::DecoderInit:
      return "Failure initializing libmpeg2!";
    case TTMpeg2DecoderException::StreamOpen:
      return "Failure opening mpeg2 stream!";
    default:
      return "Unknown exception type!";
  }
}
