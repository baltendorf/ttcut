/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttaviwriter.cpp                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 08/06/2005 */
/* MODIFIED: b. altendorf                                    DATE: 11/08/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVIWRITER
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


#include "ttaviwriter.h"
#include "../common/ttmessagelogger.h"

#include <QFileInfo>
#include <QDir>

/* /////////////////////////////////////////////////////////////////////////////
 * Construct a TTAVIWriter object
 */
TTAVIWriter::TTAVIWriter(TTVideoStream* v_stream)
{
   decoder      = NULL;
   header_list  = NULL;
   index_list   = NULL;
   ref_data     = NULL;
   avi_file     = NULL;

   file_size    = 0;
   file_offset  = 0;
   mFrameRate   = v_stream->frameRate();

   log = TTMessageLogger::getInstance();

  initAVIWriter(v_stream);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Clean up object
 */
TTAVIWriter::~TTAVIWriter()
{
  if (decoder != NULL)
    delete decoder;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Initialize the AVI writer
 */
void TTAVIWriter::initAVIWriter(TTVideoStream* v_stream)
{
   video_file_name = v_stream->filePath();
   index_list      = v_stream->indexList();
   header_list     = v_stream->headerList();
   file_size       = v_stream->streamLengthByte();

   //create the decoder object
   decoder = new TTMpeg2Decoder(video_file_name, index_list, header_list );

   // must set the format for conversion here
   decoder->decodeFirstMPEG2Frame( formatYV12 );
}

/* /////////////////////////////////////////////////////////////////////////////
 * Write AVI file
 */
int TTAVIWriter::writeAVI(int startIndex, int endIndex, const QFileInfo& aviFInfo)
{
  QTime       searchTime;
  TFrameInfo* frameInfo;
  int         frameCount = endIndex - startIndex + 1;

  log->debugMsg(__FILE__, __LINE__, "---------------------------------------------");
  log->debugMsg(__FILE__, __LINE__, QString("write AVI start %1 / end %2 / count %3").
      arg(startIndex).arg(endIndex).arg(frameCount));

  avi_file = AVI_open_output_file(aviFInfo.absoluteFilePath().toLatin1().data());

  //TODO: go back to previous sequence to ensure correct encoding of
  //      open GOP's
  current_frame = decoder->moveToFrameIndex(startIndex);

  // decode the current slice
  //frameInfo = decoder->decodeMPEG2Frame( formatYV12 );
  frameInfo = decoder->getFrameInfo();

  log->debugMsg(__FILE__, __LINE__, QString("AVI %1 width %2 x height %3").
      arg(aviFInfo.absoluteFilePath()).
      arg(frameInfo->width).
      arg(frameInfo->height));

  AVI_set_video(avi_file, frameInfo->width, frameInfo->height, mFrameRate, (char*)"YV12");

  ref_data = new quint8[frameInfo->size+2*frameInfo->chroma_size];

  //for (int i = start_frame_pos; i <= end_frame_pos; i++)
  for (int i = 1; i <= frameCount; i++)
  {
    // get decoded mpeg-frame data
    decoder->getCurrentFrameData(ref_data);

    log->debugMsg(__FILE__, __LINE__, QString("write frame %1 with frame-type %2").
        arg(startIndex+i-1).
        arg(frameInfo->type));

    // write the mpeg-frame data to AVI file
    AVI_write_frame(avi_file, (char*)ref_data, frameInfo->size+2*frameInfo->chroma_size, 2);

    // decode the next mpeg-frame
    decoder->moveToFrameIndex(startIndex+i);
    frameInfo = decoder->getFrameInfo();
    //frameInfo = decoder->decodeMPEG2Frame( formatYV12 );

  }
  log->debugMsg(__FILE__, __LINE__, "---------------------------------------------");

  delete [] ref_data;

  return frameCount;
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
bool TTAVIWriter::closeAVI()
{
  AVI_close(avi_file);

  avi_file = NULL;

  return true;
}


