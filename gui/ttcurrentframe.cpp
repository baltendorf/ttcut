/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2006                                                      */
/* FILE     : ttcurrentframe.cpp                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/19/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCURRENTFRAME
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

#include "ttcurrentframe.h"
#include "data/ttavlist.h"

#include <QWheelEvent>

//! Default constructor
TTCurrentFrame::TTCurrentFrame(QWidget* parent)
  :QWidget(parent)
{
  setupUi( this );

  mpeg2Stream      = 0;
  isControlEnabled = true;

  connect(pbPrevFrame, SIGNAL(clicked()), this, SLOT(onPrevBFrame()));
  connect(pbNextFrame, SIGNAL(clicked()), this, SLOT(onNextBFrame()));
  connect(pbSetMarker, SIGNAL(clicked()), this, SLOT(onSetMarker()));
}

//! Needeb by Qt Designer
void TTCurrentFrame::setTitle ( const QString & title )
{
  gbCurrentFrame->setTitle( title );
}

void TTCurrentFrame::controlEnabled( bool enabled )
{
  isControlEnabled = enabled;
  pbPrevFrame->setEnabled(enabled);
  pbNextFrame->setEnabled(enabled);
  pbSetMarker->setEnabled(enabled);

  pbPlayVideo->setEnabled( false );//enabled );
  pbPlayVideo->hide();
}


void TTCurrentFrame::onAVDataChanged(TTAVItem* avData)
{
	if (avData == 0) {
		mpegWindow->closeVideoStream();
		return;
	}

	mpeg2Stream = (TTMpeg2VideoStream*)avData->videoStream();

	mpegWindow->openVideoStream(mpeg2Stream);
	mpegWindow->moveToFirstFrame();
}

//! Returns the current frame position in stream
int TTCurrentFrame::currentFramePos()
{
  return mpeg2Stream->currentIndex();
}

void TTCurrentFrame::closeVideoStream()
{
  mpegWindow->closeVideoStream();
}

void TTCurrentFrame::wheelEvent ( QWheelEvent * e )
{
  if (!isControlEnabled)
    return;

  int currentPosition = mpeg2Stream->currentIndex();
  int wheelDelta      = TTCut::stepMouseWheel;

  if ( e->modifiers() == Qt::ControlModifier )
        wheelDelta += TTCut::stepPlusCtrl;

  //wheel was rotated forwards away from the user
  if ( e->delta() > 0 )
    currentPosition -= wheelDelta;
  else
    currentPosition += wheelDelta;

  if ( currentPosition < 0 )
    currentPosition = 0;

  if( currentPosition >= (int)mpeg2Stream->frameCount() )
    currentPosition = mpeg2Stream->frameCount()-1;

  onGotoFrame(currentPosition, 0);
}

// Signals from the navigation widget
// ----------------------------------------------------------------------------

//! Navigate to previous I-Frame
void TTCurrentFrame::onPrevIFrame()
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToPrevIFrame( );
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

//! Navigate to next I-Frame
void TTCurrentFrame::onNextIFrame()
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToNextIFrame( );
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

//! Navigate to previous P-Frame
void TTCurrentFrame::onPrevPFrame()
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToPrevPIFrame( );
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

//! Navigate to next P-Frame
void TTCurrentFrame::onNextPFrame()
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToNextPIFrame( );
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

//! Navigate to previous B-Frame
void TTCurrentFrame::onPrevBFrame()
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToPrevFrame( );
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

//! Navigate to next B-Frame
void TTCurrentFrame::onNextBFrame()
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToNextFrame( );
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

//! Navigate to marker position
void TTCurrentFrame::onGotoMarker(int markerPos)
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToIndexPos(markerPos);
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

void TTCurrentFrame::onSetMarker()
{
	if (mpeg2Stream == 0) return;

	emit setMarker(mpeg2Stream->currentIndex());
}

//! Cut in position was set
void TTCurrentFrame::onSetCutIn(__attribute__((unused))int cutInPos)
{
}

//! Cut out position was set
void TTCurrentFrame::onSetCutOut(__attribute__((unused))int cutOutPos)
{
}

//! Goto cut in position
void TTCurrentFrame::onGotoCutIn(int pos)
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToIndexPos(pos);
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

//! Goto cut out position
void TTCurrentFrame::onGotoCutOut(int pos)
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToIndexPos(pos);
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

void TTCurrentFrame::onGotoFrame(int pos)
{
  onGotoFrame(pos, 0);
}

//! Goto arbitrary frame at given position
void TTCurrentFrame::onGotoFrame(int pos, int fast)
{
  int newFramePos;

  newFramePos = mpeg2Stream->moveToIndexPos( pos, fast );
  mpegWindow->showFrameAt( newFramePos );

  updateCurrentPosition();
}

void TTCurrentFrame::onMoveNumSteps(int steps)
{
  int position = mpeg2Stream->currentIndex()+steps;
  onGotoFrame(position, 0);
}

void TTCurrentFrame::onMoveToHome()
{
  onGotoFrame(0, 0);
}

void TTCurrentFrame::onMoveToEnd()
{
  onGotoFrame(mpeg2Stream->frameCount(), 0);
}

void TTCurrentFrame::updateCurrentPosition()
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
  laCurrentPosition->setText( szTemp1 );

  laCurrentPosition->update();

  emit newFramePosition( mpeg2Stream->currentIndex() );
}

void TTCurrentFrame::saveCurrentFrame()
{
  QString      szTemp;
  QString      extension;
  QString      format;
  QStringList  fileList;
  QString      fileName;
  QFileDialog* fileDlg;

  if (mpeg2Stream == 0) return;

  // get the image file name
  fileDlg = new QFileDialog( this,
      "save current frame",
      TTCut::lastDirPath,
      "Portable Network Graphics (*.png);;JPEG (*.jpg);;Bitmap (*.bmp)" );

  // enable specifying a file that doesn't exist
  fileDlg->setFileMode( QFileDialog::AnyFile );
  fileDlg->setAcceptMode( QFileDialog::AcceptSave );

  // input filename specified
  if ( fileDlg->exec() == QDialog::Accepted )
  {
    szTemp   = fileDlg->selectedNameFilter();
    fileList = fileDlg->selectedFiles();
    fileName = fileList.at(0);

    if ( szTemp == "Portable Network Graphics (*.png)" )
    {
      format    = "PNG";
      extension = "png";
    }
    else if ( szTemp == "JPEG (*.jpg)" )
    {
      format    = "JPG";
      extension = "jpg";
    }
    else if ( szTemp == "Bitmap (*.bmp)" )
    {
      format    = "BMP";
      extension = "bmp";
    }
    else
    {
      qDebug( "unsupported format" );
      return;
    }

    fileName = ttChangeFileExt( fileName, qPrintable(extension) );

    mpegWindow->saveCurrentFrame( fileName, qPrintable(format) );
  }
  delete fileDlg;
}

