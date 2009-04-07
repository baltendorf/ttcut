/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutoutframe.cpp                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/19/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTOUTFRAME
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

#include "ttcutoutframe.h"
#include "../data/ttavlist.h"

/*!
 * Constructor
 */
TTCutOutFrame::TTCutOutFrame(QWidget* parent)
  :QWidget(parent)
{
  setupUi( this );

  currentAVItem       = 0;
  currentPosition     = -1;
  currentCutItemIndex = -1;
  isCutOut            = false;

  connect(pbPrevCutOutFrame, SIGNAL(clicked()), SLOT(onPrevCutOutPos()));
  connect(pbNextCutOutFrame, SIGNAL(clicked()), SLOT(onNextCutOutPos()));
  connect(pbSearchFrame,     SIGNAL(clicked()), SLOT(onSearchFrame()));
}

/*!
 * Destructor
 */
TTCutOutFrame::~TTCutOutFrame()
{
	if (mpeg2Stream != 0) delete mpeg2Stream;
}

/*!
 * Needed by Qt Designer
 */
void TTCutOutFrame::setTitle ( const QString & title )
{
  gbCutOutFrame->setTitle( title );
}

/*!
 * controlEnabled
 */
void TTCutOutFrame::controlEnabled( bool enabled )
{
  pbPrevCutOutFrame->setEnabled((!enabled) ? enabled : isCutOut);
  pbNextCutOutFrame->setEnabled((!enabled) ? enabled : isCutOut);
  pbSearchFrame->setEnabled((!enabled) ? enabled : isCutOut);
}

/*!
 * onAVDataChanged
 */
void TTCutOutFrame::onAVDataChanged(TTAVItem* avItem)
{
	if (avItem == 0) {
		mpegWindow->closeVideoStream();

		//TODO: cannot delete shared copy this way! if (mpeg2Stream != 0) delete mpeg2Stream;
		mpeg2Stream = 0;
		return;
	}

	if (currentAVItem == avItem)
		return;

	isCutOut           = false;
	currentAVItem      = avItem;
	currentMpeg2Stream = (TTMpeg2VideoStream*)avItem->videoStream();
	mpeg2Stream        = new TTMpeg2VideoStream(*avItem->videoStream()->fileInfo());
	mpeg2Stream->makeSharedCopy((TTMpeg2VideoStream*)avItem->videoStream());

	mpegWindow->openVideoStream(mpeg2Stream);
	controlEnabled(isCutOut);
}

/*!
 * onCutOutChanged
 */
void TTCutOutFrame::onCutOutChanged(const TTCutItem& cutItem)
{
	isCutOut = true;
	onAVDataChanged(cutItem.avDataItem());
	onGotoCutOut(cutItem.cutOut());

	currentCutItemIndex = currentAVItem->cutIndexOf(cutItem);
}

/*!
 * Returns the current frame position in stream
 */
int TTCutOutFrame::currentFramePos()
{
  return mpeg2Stream->currentIndex();
}

/*!
 * closeVideoStream
 */
void TTCutOutFrame::closeVideoStream()
{
	mpegWindow->closeVideoStream();
}

/*
 * Goto specified cut out position
 */
void TTCutOutFrame::onGotoCutOut(int pos)
{
  currentPosition = mpeg2Stream->moveToIndexPos(pos);
  mpegWindow->showFrameAt( currentPosition );

  updateCurrentPosition();
}

//! Goto previous possible cut-out position
void TTCutOutFrame::onPrevCutOutPos()
{
  int cutOutIndex;

  cutOutIndex = (!TTCut::encoderMode)
		? mpeg2Stream->moveToPrevPIFrame()
		: mpeg2Stream->moveToPrevFrame();

  if (currentCutItemIndex >= 0) {
  	TTCutItem cutItem = currentAVItem->cutListItemAt(currentCutItemIndex);
  	currentAVItem->updateCutEntry(cutItem, cutItem.cutIn(), cutOutIndex);
  }

  mpegWindow->showFrameAt(cutOutIndex);
  updateCurrentPosition();
}

/*!
 * Goto next possible cut-out position
 */
void TTCutOutFrame::onNextCutOutPos()
{
   int cutOutIndex;

  cutOutIndex = (!TTCut::encoderMode)
		  ? mpeg2Stream->moveToNextPIFrame()
		  : mpeg2Stream->moveToNextFrame();

  if (currentCutItemIndex >= 0) {
	  TTCutItem cutItem = currentAVItem->cutListItemAt(currentCutItemIndex);
		currentAVItem->updateCutEntry(cutItem, cutItem.cutIn(), cutOutIndex);
  }

  mpegWindow->showFrameAt(cutOutIndex);
  updateCurrentPosition();
}

/*!
 * On search equal frame action
 */
void TTCutOutFrame::onSearchFrame()
{
	emit searchEqualFrame(currentAVItem, mpeg2Stream->currentIndex());
}

/*
 * updateCurrentPosition
 */
void TTCutOutFrame::updateCurrentPosition()
{
  QString szTemp;
  QString szTemp1, szTemp2;
  int     frame_type = mpeg2Stream->currentFrameType();

  szTemp1 = mpeg2Stream->currentFrameTime().toString("hh:mm:ss.zzz");

  szTemp2.sprintf(" (%d)",mpeg2Stream->currentIndex());

  if ( frame_type == 1 ) szTemp2 += " [I]";
  if ( frame_type == 2 ) szTemp2 += " [P]";
  if ( frame_type == 3 ) szTemp2 += " [B]";

  szTemp1 += szTemp2;
  laCutOutFramePosition->setText( szTemp1 );

  laCutOutFramePosition->update();
	controlEnabled(isCutOut);
}

