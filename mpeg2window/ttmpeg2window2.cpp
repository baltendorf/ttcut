/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmpeg2window.cpp                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 16/12/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMPEG2WINDOW
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

#include "ttmpeg2window2.h"

/*!
 * TTMPEG2Window2
 */
TTMPEG2Window2::TTMPEG2Window2(QWidget *parent )
: QLabel(parent)
{
  log = TTMessageLogger::getInstance();

  this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  this->setAutoFillBackground(true);
  this->setStyleSheet("QLabel { background-color: black }");

  mpVideoStream = 0;
  mpeg2Decoder  = 0;
  currentIndex  = 0;
  picBuffer     = 0;
  videoWidth    = 0;
  videoHeight   = 0;
  frameInfo     = 0;
}

/*!
 * resizeEvent
 */
void TTMPEG2Window2::resizeEvent (QResizeEvent*)
{
	showVideoFrame();
}

/*!
 * show the current video frame (picBuffer)
 */
void TTMPEG2Window2::showVideoFrame()
{
	if (mpeg2Decoder == 0) return;
	if (frameInfo    == 0) return;

   /*if (frameInfo->type != mpeg2Decoder->desiredFrameType) {
    	  TTMessageLogger::getInstance()->errorMsg(__FILE__, __LINE__,
    			  QString("decoded frame is %1 / desired was %2").
    			  arg(frameInfo->type).
    			  arg(mpeg2Decoder->desiredFrameType));
   }*/

   float scaleFactorY = 1.0;

  if (mpVideoStream != 0) {
  	TTSequenceHeader* seqHeader = mpVideoStream->getSequenceHeader(currentIndex);
  	if (seqHeader->aspectRatio() == 3) {
  		scaleFactorY = (float)(videoWidth*9.0/(videoHeight*16.0));
  	}
  }

  if (picBuffer != 0)
  {
    QImage frame(picBuffer, videoWidth, videoHeight, QImage::Format_RGB32);
    QImage scale = frame.scaled(videoWidth, videoHeight*scaleFactorY, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    this->setPixmap(QPixmap::fromImage(scale.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  }
}

void TTMPEG2Window2::showFrameAt(int index)
{
  moveToVideoFrame(index);
}

/*!
 * decode and show the first video frame
 */
void TTMPEG2Window2::moveToFirstFrame(bool show)
{
	if (mpeg2Decoder == 0) return;

	try
	{
		mpeg2Decoder->decodeFirstMPEG2Frame( formatRGB32 );
		getFrameInfo();
	}
	catch (TTMpeg2DecoderException& ex)
	{
		log->errorMsg(__FILE__, __LINE__, ex.message());
	}

  if (show && picBuffer != 0)
    showVideoFrame();
}

/*!
 * Open a video file and assign the mpeg2 decoder object
 */
void TTMPEG2Window2::openVideoFile( QString fName, TTVideoIndexList* viIndex, TTVideoHeaderList* viHeader )
{
	if (fName.isEmpty()) return;

  if (mpeg2Decoder != 0) delete mpeg2Decoder;

  try
  {
  	mpeg2Decoder = new TTMpeg2Decoder(fName, viIndex, viHeader);
  }
  catch (TTMpeg2DecoderException& ex)
  {
  	log->errorMsg(__FILE__, __LINE__, ex.message());
  }
}

/*!
 * openVideoStream
 */
void TTMPEG2Window2::openVideoStream(TTMpeg2VideoStream* vStream)
{
	mpVideoStream = vStream;
	openVideoFile(vStream->filePath(), vStream->indexList(), vStream->headerList());
}

/*!
 * Close vide stream
 */
void TTMPEG2Window2::closeVideoStream()
{
  if (mpeg2Decoder != 0)
  {
    delete mpeg2Decoder;
    mpeg2Decoder = 0;
    picBuffer    = 0;
  }

  QImage dummy;
  this->setPixmap(QPixmap::fromImage(dummy));
  repaint();
}

/*!
 * Move to specified frame position
 */
void TTMPEG2Window2::moveToVideoFrame(int iFramePos)
{
	if (mpeg2Decoder == 0)            return;
	if (iFramePos    == currentIndex) return;

	try
	{
		mpeg2Decoder->moveToFrameIndex(iFramePos);
		currentIndex = iFramePos;
		getFrameInfo();
	}
	catch (TTMpeg2DecoderException& ex)
	{
		log->errorMsg(__FILE__, __LINE__, ex.message());
	}

  showDecodedSlice();
}

/*!
 * Save current frame to file (jpeg, png, bmp)
 */
void TTMPEG2Window2::saveCurrentFrame(QString fName, const char* format)
{
  if (picBuffer == 0) return;

  QImage screenShot(picBuffer, videoWidth, videoHeight, QImage::Format_RGB32);
  screenShot.save(fName, format);
}

/*!
 * Show current decoded slice
 */
void TTMPEG2Window2::showDecodedSlice()
{
	if (mpeg2Decoder == 0) return;

	getFrameInfo();
  showVideoFrame();
}

/*!
 * Decode current video frame and show the resulting slice
 */
void TTMPEG2Window2::decodeAndShowSlice()
{
	if (mpeg2Decoder == 0) return;

	try
	{
		mpeg2Decoder->decodeMPEG2Frame(formatRGB24);
		getFrameInfo();
	}
	catch (TTMpeg2DecoderException& ex)
	{
		log->errorMsg(__FILE__, __LINE__, ex.message());
	}

  showVideoFrame();
}

/*!
 * getFrameInfo
 */
void TTMPEG2Window2::getFrameInfo()
{
	frameInfo   = mpeg2Decoder->getFrameInfo();
	picBuffer   = frameInfo->Y;
	videoWidth  = frameInfo->width;
	videoHeight = frameInfo->height;
}
