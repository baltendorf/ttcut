/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutpreview.cpp                                                */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/13/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTPREVIEW
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

#include "../common/ttexception.h"
#include "ttcutpreview.h"
#include "../avstream/ttavstream.h"
#include "../data/ttavdata.h"
#include "../data/ttavlist.h"
#include "../data/ttcutpreviewtask.h"
#include "ttmplayerwidget.h"
#include "ttphononwidget.h"

#include <QApplication>
#include <QDir>

/* /////////////////////////////////////////////////////////////////////////////
 * TTCutPreview constructor
 */
TTCutPreview::TTCutPreview(QWidget* parent, int prevW, int prevH)
  : QDialog(parent)
{
  setupUi(this);

  videoPlayer = new TTPhononWidget(videoFrame);

  setObjectName("TTCutPreview");

  // set desired video width x height
  previewWidth  = prevW;
  previewHeight = prevH;

  cbCutPreview->setEditable( false );
  cbCutPreview->setMinimumSize( 160, 20 );
  cbCutPreview->setInsertPolicy( QComboBox::InsertAfterCurrent );

  connect(videoPlayer,  SIGNAL(optimalSizeChanged()),     SLOT(onOptimalSizeChanged()));
  connect(videoPlayer,  SIGNAL(playerPlaying()),          SLOT(onPlayerPlaying()));
  connect(videoPlayer,  SIGNAL(playerFinished()),         SLOT(onPlayerFinished()));
  connect(cbCutPreview, SIGNAL(currentIndexChanged(int)), SLOT(onCutSelectionChanged(int)));
  connect(pbPlay,       SIGNAL(clicked()),                SLOT(onPlayPreview()));
  connect(pbExit,       SIGNAL(clicked()),                SLOT(onExitPreview()));
}

/* /////////////////////////////////////////////////////////////////////////////
 * Destroys the object and frees any allocated resources
 */
TTCutPreview::~TTCutPreview()
{
}

/* /////////////////////////////////////////////////////////////////////////////
 * resizeEvent
 */
void TTCutPreview::resizeEvent(QResizeEvent*)
{
	videoPlayer->resize(videoFrame->width()-2, videoFrame->height()-2);
}

void TTCutPreview::onOptimalSizeChanged()
{
}

/* /////////////////////////////////////////////////////////////////////////////
 * Event handler to receive widgets close events
 */
void TTCutPreview::closeEvent(QCloseEvent* event)
{
  cleanUp();
  event->accept();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Initialize preview parameter
 */
void TTCutPreview::initPreview(TTCutList* cutList)
{
  int       iPos;
  QString   preview_video_name;
  QFileInfo preview_video_info;
  QString   selectionString;

  int numPreview = cutList->count()/2+1;

  // Create video and audio preview clips
  for (int i = 0; i < numPreview; i++ ) {
    // first cut-in
    if (i == 0) {
      TTCutItem item = cutList->at(i);
      selectionString = QString("Start: %1").arg(item.cutInTime().toString("hh:mm:ss"));
      cbCutPreview->addItem( selectionString );
    }

    // cut i-i
    if (numPreview > 1 && i > 0 && i < numPreview-1) {
      iPos = (i-1)*2+1;

      TTCutItem item1 = cutList->at(iPos);
      TTCutItem item2 = cutList->at(iPos+1);
      selectionString.sprintf( "Cut %d-%d: %s - %s",i,i+1,
            qPrintable(item1.cutInTime().toString("hh:mm:ss")),
            qPrintable(item2.cutOutTime().toString("hh:mm:ss")));
        cbCutPreview->addItem( selectionString );
    }

    //last cut out
    if (i == numPreview-1) {
      iPos = (i-1)*2+1;

      TTCutItem item = cutList->at(iPos);
      selectionString.sprintf( "End: %s", qPrintable(item.cutOutTime().toString("hh:mm:ss")));
      cbCutPreview->addItem( selectionString );
    }
  }

  // set the current cut preview to the first cut clip
  preview_video_name.sprintf("preview_001.mpg");
  preview_video_info.setFile(QDir(TTCut::tempDirPath), preview_video_name);

  current_video_file = preview_video_info.absoluteFilePath();
  onCutSelectionChanged(0);
}

/* /////////////////////////////////////////////////////////////////////////////
 * ComboBox selectionChanged event handler: load the selected movie
 */
void TTCutPreview::onCutSelectionChanged( int iCut )
{
  QString   preview_video_name;
  QFileInfo preview_video_info;

  preview_video_name.sprintf("preview_%03d.mpg",iCut+1);
  preview_video_info.setFile( QDir(TTCut::tempDirPath), preview_video_name );
  current_video_file = preview_video_info.absoluteFilePath();

  qDebug("load preview %s", qPrintable(current_video_file));
  videoPlayer->load(current_video_file);
  pbPlay->setText(tr("Play"));
  pbPlay->setIcon(QIcon(":/pixmaps/pixmaps/play_18.xpm"));
}

/* /////////////////////////////////////////////////////////////////////////////
 * Play/Pause the selected preview clip
 */
void TTCutPreview::onPlayPreview()
{
  if (videoPlayer->isPlaying()) {
    videoPlayer->stop();
    pbPlay->setText(tr("Play"));
    pbPlay->setIcon(QIcon(":/pixmaps/pixmaps/play_18.xpm"));
    return;
  }

  pbPlay->setText(tr("Stop"));
  pbPlay->setIcon(QIcon(":/pixmaps/pixmaps/stop_18.xpm"));
  videoPlayer->play();
}

void TTCutPreview::onPlayerPlaying()
{
  onPlayPreview();
}

void TTCutPreview::onPlayerFinished()
{
  videoPlayer->load(current_video_file);
  pbPlay->setText(tr("Play"));
  pbPlay->setIcon(QIcon(":/pixmaps/pixmaps/play_18.xpm"));
}

/* /////////////////////////////////////////////////////////////////////////////
 * Exit the preview window
 */
void TTCutPreview::onExitPreview()
{
  close();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Housekeeping: Remove the temporary created preview clips
 */
void TTCutPreview::cleanUp()
{
  QString   rmCommand = "rm ";
  QString   fileName  = "preview*";
  QFileInfo fileInfo;

  videoPlayer->cleanUp();

  // clean up preview* files in temp directory
  fileInfo.setFile(QDir(TTCut::tempDirPath), fileName);
  rmCommand += fileInfo.absoluteFilePath();
  rmCommand += " 2>/dev/null";

  //system(rmCommand.toAscii().data());
}
