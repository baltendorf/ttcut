/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutvideoinfo.cpp                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (b.altendorf@tritime.de)           DATE: 12/07/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTVIDEOINFO
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

#include "ttcutvideoinfo.h"
#include "../data/ttavdata.h"
#include "../data/ttavlist.h"
#include "../common/ttcut.h"
#include "../avstream/ttavtypes.h"
#include "../avstream/ttmpeg2videostream.h"

#include <QFileInfo>
#include <QFileDialog>

/*
 * Constructor
 */
TTCutVideoInfo::TTCutVideoInfo(QWidget* parent)
  :QWidget(parent)
{
  setupUi( this );

  connect(pbVideoOpen, SIGNAL(clicked()), SIGNAL(openFile()));
  connect(pbPrevVideo, SIGNAL(clicked()), SIGNAL(prevAVClicked()));
  connect(pbNextVideo, SIGNAL(clicked()), SIGNAL(nextAVClicked()));
}

/*
 * Set widget title
 */
void TTCutVideoInfo::setTitle(const QString&)
{
}

/*
 * Enable / disable the control and his child controls
 */
void TTCutVideoInfo::controlEnabled(bool value)
{
  pbVideoOpen->setEnabled(value);
  pbPrevVideo->setEnabled(videoName->text() != "---");
  pbNextVideo->setEnabled(videoName->text() != "---");
}

/*
 * Reset the video file info text labels to default values
 */
void TTCutVideoInfo::clearControl()
{
  videoName->setText("---");
  videoLength->setText("---");
  videoResolution->setText("---");
  videoAspectratio->setText("---");
  currentIndex->setText("-/-");
}

/*
 * AV-Item changed
 */
void TTCutVideoInfo::onAVDataChanged(TTAVData* av, TTAVItem* avData)
{
	if (avData == 0 || av->avCount() == 0) {
		clearControl();
		return;
	}

  TTMpeg2VideoStream* mpeg2Stream     = (TTMpeg2VideoStream*)avData->videoStream();
  TTSequenceHeader*   currentSequence = mpeg2Stream->currentSequenceHeader();

  if (currentSequence == NULL)
	return;

  // video file name
  videoName->setText(mpeg2Stream->fileName());

  // video length
  int   numFrames = mpeg2Stream->frameCount();
  QTime time      = ttFramesToTime( numFrames, mpeg2Stream->frameRate());
  setLength(time, numFrames);

  // set video resolution
  setResolution(mpeg2Stream->currentSequenceHeader()->horizontalSize(),
                mpeg2Stream->currentSequenceHeader()->verticalSize());

  // set aspect
  videoAspectratio->setText(mpeg2Stream->currentSequenceHeader()->aspectRatioText());

  // set index
  currentIndex->setText(QString("%1/%2").arg(av->avIndexOf(avData)+1).arg(av->avCount()));
}

/*
 * Refresh item info / horizontal and vertical size / aspect ratio info
 */
void TTCutVideoInfo::refreshInfo(TTAVItem* avItem)
{
	if (avItem == 0)  return;

	TTMpeg2VideoStream* mpeg2Stream     = (TTMpeg2VideoStream*)avItem->videoStream();
  TTSequenceHeader*   currentSequence = mpeg2Stream->currentSequenceHeader();

  if (currentSequence == NULL)
	return;

  // set video resolution
  setResolution(mpeg2Stream->currentSequenceHeader()->horizontalSize(),
                mpeg2Stream->currentSequenceHeader()->verticalSize());

  // set aspect
  videoAspectratio->setText(mpeg2Stream->currentSequenceHeader()->aspectRatioText());
}

/*
 * Set's the stream length text
 */
void TTCutVideoInfo::setLength(QTime total, int numFrames)
{
  videoLength->setText(QString("%1 (%2)").arg(total.toString("hh:mm:ss:zzz")).arg(numFrames));
}

/*
 * Set the video stream resolution as width x heigth
 */
void TTCutVideoInfo::setResolution(int width, int height)
{
  videoResolution->setText(QString("%1x%2").arg(width).arg(height));
}

/*
 * Set the video stream aspect ratio as QString
 */
void TTCutVideoInfo::setAspect(QString aspect)
{
  videoAspectratio->setText(aspect);
}

