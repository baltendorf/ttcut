/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutmainwindow.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTMAINWINDOW
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

#include <QtGui>
#include <QPixmap>
#include <QDebug>

#include "ttcutmainwindow.h"

#include "../common/ttexception.h"
#include "../common/ttthreadtask.h"
#include "ttcutavcutdlg.h"
#include "ttprogressbar.h"
#include "ttcutaboutdlg.h"

#include "../data/ttavdata.h"
#include "../data/ttavlist.h"

#include "../ui//pixmaps/downarrow_18.xpm"
#include "../ui/pixmaps/uparrow_18.xpm"
#include "../ui/pixmaps/cancel_18.xpm"
#include "../ui/pixmaps/fileopen_24.xpm"
#include "../ui/pixmaps/filenew_16.xpm"
#include "../ui/pixmaps/fileopen_16.xpm"
#include "../ui/pixmaps/filesave_16.xpm"
#include "../ui/pixmaps/filesaveas_16.xpm"
#include "../ui/pixmaps/saveimage_16.xpm"
#include "../ui/pixmaps/settings_16.xpm"
#include "../ui/pixmaps/settings_18.xpm"
#include "../ui/pixmaps/exit_16.xpm"
#include "../ui/pixmaps/play_18.xpm"
#include "../ui/pixmaps/stop_18.xpm"
#include "../ui/pixmaps/search_18.xpm"
#include "../ui/pixmaps/preview_18.xpm"
#include "../ui/pixmaps/chapter_18.xpm"
#include "../ui/pixmaps/cutav_18.xpm"
#include "../ui/pixmaps/cutaudio_18.xpm"
#include "../ui/pixmaps/goto_18.xpm"
#include "../ui/pixmaps/note_18.xpm"
#include "../ui/pixmaps/clock_16.xpm"
#include "../ui/pixmaps/apply_18.xpm"
#include "../ui/pixmaps/addtolist_18.xpm"
#include "../ui/pixmaps/fileclose_18.xpm"

#include <QStringList>
#include <QString>

/* /////////////////////////////////////////////////////////////////////////////
 * Application main window constructor
 */
TTCutMainWindow::TTCutMainWindow()
: QMainWindow()
{
  // setup Qt Designer UI
  setupUi( this );

  // images
  // --------------------------------------------------------------------------
  TTCut::imgDownArrow  = new QPixmap( downarrow_18_xpm );
  TTCut::imgUpArrow    = new QPixmap( uparrow_18_xpm );
  TTCut::imgDelete     = new QPixmap( cancel_18_xpm );
  TTCut::imgFileOpen24 = new QPixmap( fileopen_24_xpm );
  TTCut::imgFileNew    = new QPixmap( filenew_16_xpm );
  TTCut::imgFileOpen   = new QPixmap( fileopen_16_xpm );
  TTCut::imgFileSave   = new QPixmap( filesave_16_xpm );;
  TTCut::imgFileSaveAs = new QPixmap( filesaveas_16_xpm );
  TTCut::imgSaveImage  = new QPixmap( saveimage_16_xpm );
  TTCut::imgSettings   = new QPixmap( settings_16_xpm );
  TTCut::imgSettings18 = new QPixmap( settings_18_xpm );
  TTCut::imgExit       = new QPixmap( exit_16_xpm );
  TTCut::imgPlay       = new QPixmap( play_18_xpm );
  TTCut::imgStop       = new QPixmap( stop_18_xpm );
  TTCut::imgSearch     = new QPixmap( search_18_xpm );
  TTCut::imgChapter    = new QPixmap( chapter_18_xpm );
  TTCut::imgPreview    = new QPixmap( preview_18_xpm );
  TTCut::imgCutAV      = new QPixmap( cutav_18_xpm );
  TTCut::imgCutAudio   = new QPixmap( cutaudio_18_xpm );
  TTCut::imgGoTo       = new QPixmap( goto_18_xpm );
  TTCut::imgMarker     = new QPixmap( note_18_xpm );
  TTCut::imgClock      = new QPixmap( clock_16_xpm );
  TTCut::imgApply      = new QPixmap( apply_18_xpm );
  TTCut::imgAddToList  = new QPixmap( addtolist_18_xpm );
  TTCut::imgFileClose  = new QPixmap( fileclose_18_xpm );

  setFocusPolicy(Qt::StrongFocus);

  // Message logger instance
  log = TTMessageLogger::getInstance();

  // Get the current Qt version at runtime
  log->infoMsg(__FILE__, __LINE__, QString("TTCut-Version: %1").arg(TTCut::versionString));
  log->infoMsg(__FILE__, __LINE__, QString("Qt-Version:    %1").arg(qVersion()));

  // Settings
  TTCut::recentFileList.clear();
  settings = new TTCutSettings();
  settings->readSettings();
  log->enableLogFile(TTCut::createLogFile);
  log->setLogModeConsole(TTCut::logModeConsole);
  log->setLogModeExtended(TTCut::logModeExtended);

  //AV stream controller instance
  mpAVData = new TTAVData();

  videoFileList->setAVData(mpAVData);
  cutList->setAVData(mpAVData);
  markerList->setAVData(mpAVData);

  // no navigation
  navigationEnabled( false );

  // init
  mpCurrentAVDataItem     = 0;
  progressBar            = 0;
  TTCut::projectFileName = "";

  // Signal and slot connections
  //
  // Connect signals from main menu
  // --------------------------------------------------------------------------
  connect(actionOpenVideo,        SIGNAL(triggered()), SLOT(onOpenVideoFile()));
  connect(actionOpenAudio,        SIGNAL(triggered()), SLOT(onOpenAudioFile()));
  connect(actionFileNew,          SIGNAL(triggered()), SLOT(onFileNew()));
  connect(actionFileOpen,         SIGNAL(triggered()), SLOT(onFileOpen()));
  connect(actionFileSave,         SIGNAL(triggered()), SLOT(onFileSave()));
  connect(actionFileSaveAs,       SIGNAL(triggered()), SLOT(onFileSaveAs()));
  connect(actionExit,             SIGNAL(triggered()), SLOT(onFileExit()));
  connect(actionSaveCurrentFrame, SIGNAL(triggered()), SLOT(onActionSave()));
  connect(actionSettings,         SIGNAL(triggered()), SLOT(onActionSettings()));
  connect(actionAbout,            SIGNAL(triggered()), SLOT(onHelpAbout()));

  // recent files
  for (int i = 0; i < MaxRecentFiles; ++i) {
    recentFileAction[i] = new QAction(this);
    recentFileAction[i]->setVisible(false);
    menuRecentProjects->addAction(recentFileAction[i]);
    connect(recentFileAction[i], SIGNAL(triggered()), SLOT(onFileRecent()));
  }

  updateRecentFileActions();
  connect(mpAVData,  SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)),
                     SLOT(onStatusReport(TTThreadTask*, int, const QString&, quint64)));


  // Connect signals from video and audio info
  // --------------------------------------------------------------------------
  connect(videoFileInfo,  SIGNAL(openFile()),       SLOT(onOpenVideoFile()));
  connect(videoFileInfo,  SIGNAL(nextAVClicked()),  SLOT(onNextAVData()));
  connect(videoFileInfo,  SIGNAL(prevAVClicked()),  SLOT(onPrevAVData()));

  connect(videoFileList,  SIGNAL(openFile()),                      SLOT(onOpenVideoFile()));
  connect(audioFileList,  SIGNAL(openFile()),                      SLOT(onOpenAudioFile()));

  // Connect signals from navigation widget
  // --------------------------------------------------------------------------
  connect(navigation, SIGNAL(prevIFrame()),          currentFrame, SLOT(onPrevIFrame()));
  connect(navigation, SIGNAL(nextIFrame()),          currentFrame, SLOT(onNextIFrame()));
  connect(navigation, SIGNAL(prevPFrame()),          currentFrame, SLOT(onPrevPFrame()));
  connect(navigation, SIGNAL(nextPFrame()),          currentFrame, SLOT(onNextPFrame()));
  //connect(navigation, SIGNAL(prevBFrame()),          currentFrame, SLOT(onPrevBFrame()));
  //connect(navigation, SIGNAL(nextBFrame()),          currentFrame, SLOT(onNextBFrame()));
  connect(navigation, SIGNAL(setCutOut(int)),        this, SLOT(onSetCutOut(int)));

  connect(navigation, SIGNAL(setCutOut(int)),        cutOutFrame,  SLOT(onGotoCutOut(int)));

  connect(navigation, SIGNAL(setCutIn(int)),         currentFrame, SLOT(onSetCutIn(int)));
  connect(navigation, SIGNAL(gotoCutIn(int)),        currentFrame, SLOT(onGotoCutIn(int)));
  connect(navigation, SIGNAL(gotoCutOut(int)),       currentFrame, SLOT(onGotoCutOut(int)));
  connect(navigation, SIGNAL(addCutRange(int, int)),               SLOT(onAppendCutEntry(int, int)));
  //connect(navigation, SIGNAL(gotoMarker(int)),     currentFrame, SLOT(onGotoMarker(int)));
  connect(navigation, SIGNAL(moveNumSteps(int)),     currentFrame, SLOT(onMoveNumSteps(int)));
  connect(navigation, SIGNAL(moveToHome()),          currentFrame, SLOT(onMoveToHome()));
  connect(navigation, SIGNAL(moveToEnd()),           currentFrame, SLOT(onMoveToEnd()));

  // Connect signal from video slider
  // --------------------------------------------------------------------------
  connect(streamNavigator, SIGNAL(sliderValueChanged(int)), SLOT(onVideoSliderChanged(int)));

  // Connect signals from cut-out frame widget
  // --------------------------------------------------------------------------
  connect(cutOutFrame, SIGNAL(searchEqualFrame(TTAVItem*, int)), mpAVData, SLOT(onDoFrameSearch(TTAVItem*, int)));

  // Connect signals from current frame widget
  // --------------------------------------------------------------------------
  connect(currentFrame, SIGNAL(newFramePosition(int)), SLOT(onNewFramePos(int)));
  connect(currentFrame, SIGNAL(setMarker(int)),        mpAVData, SLOT(onAppendMarker(int)));

  // Connect signals from cut list widget
  // --------------------------------------------------------------------------
  connect(cutList, SIGNAL(selectionChanged(const TTCutItem&)),            SLOT(onCutSelectionChanged(const TTCutItem&)));
  connect(cutList, SIGNAL(entryEdit(const TTCutItem&)),  navigation,      SLOT(onEditCut(const TTCutItem&)));
  connect(cutList, SIGNAL(gotoCutIn(int)),               currentFrame,    SLOT(onGotoFrame(int)));
  connect(cutList, SIGNAL(gotoCutOut(int)),              currentFrame,    SLOT(onGotoFrame(int)));
  connect(cutList, SIGNAL(refreshDisplay()),             streamNavigator, SLOT(onRefreshDisplay()));
  connect(cutList, SIGNAL(setCutOut(const TTCutItem&)),                   SLOT(onSetSelectedCutOut(const TTCutItem&)));
  connect(cutList, SIGNAL(previewCut(TTCutList*)),                        SLOT(onCutPreview(TTCutList*)));
  connect(cutList, SIGNAL(audioVideoCut(bool, TTCutList*)),               SLOT(onAudioVideoCut(bool, TTCutList*)));
  connect(cutList, SIGNAL(itemUpdated(const TTCutItem&)),    cutOutFrame, SLOT(onCutOutChanged(const TTCutItem&)));

  connect(markerList, SIGNAL(gotoMarker(const TTMarkerItem&)), SLOT(onGotoMarker(const TTMarkerItem&)));

  connect(mpAVData, SIGNAL(currentAVItemChanged(TTAVItem*)), SLOT(onAVItemChanged(TTAVItem*)));
  connect(mpAVData, SIGNAL(foundEqualFrame(int)),           currentFrame, SLOT(onGotoFrame(int)));
}

/* /////////////////////////////////////////////////////////////////////////////
 * Destructor
 */
TTCutMainWindow::~TTCutMainWindow()
{
  if (mpAVData    != 0) delete mpAVData;
  if (settings    != 0) delete settings;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Signals from the application menu
 */
void TTCutMainWindow::keyPressEvent(QKeyEvent* e)
{
  navigation->keyPressEvent(e);
}

/* //////////////////////////////////////////////////////////////////////////////
 * show video file open dialog
 */
void TTCutMainWindow::onOpenVideoFile()
{
  QString fn = QFileDialog::getOpenFileName( this,
      tr("Open video file"),
      TTCut::lastDirPath,
      "Video (*.m2v *.mpv)" );

  if (fn.isEmpty()) return;

  QFileInfo fInfo( fn );
  TTCut::lastDirPath = fInfo.absolutePath();
  onReadVideoStream(fn);
}

/* //////////////////////////////////////////////////////////////////////////////
 * show audio file open dialog
 */
void TTCutMainWindow::onOpenAudioFile()
{
	if (mpAVData->avCount() == 0) return;

	QString fn = QFileDialog::getOpenFileName( this,
      tr("Open audio file"),
      TTCut::lastDirPath,
      "Audio (*.mpa *.mp2 *.ac3)" );

  if (fn.isEmpty())
    return;

  QFileInfo fInfo(fn);
  TTCut::lastDirPath = fInfo.absolutePath();
  onReadAudioStream(fn);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Menu "File new" action
 */
void TTCutMainWindow::onFileNew()
{
  if (mpAVData->avCount() == 0) return;

  closeProject();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Menu "File open" action
 */
void TTCutMainWindow::onFileOpen()
{
  QString fn = QFileDialog::getOpenFileName(this,
      tr("Open project-file"),
      TTCut::lastDirPath,
      "Project(*.prj)");

  if (!fn.isEmpty()) {
    openProjectFile(fn);
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Menu "File save" action
 */
void TTCutMainWindow::onFileSave()
{
  if (mpAVData->avCount() == 0) return;

  // Ask for file name
  if (TTCut::projectFileName.isEmpty())
  {
    TTCut::projectFileName = ttChangeFileExt(mpCurrentAVDataItem->videoStream()->fileName(), "prj");
    QFileInfo prjFileInfo(QDir(TTCut::lastDirPath), TTCut::projectFileName);

    TTCut::projectFileName = QFileDialog::getSaveFileName(this,
        tr("Save project-file"),
        prjFileInfo.absoluteFilePath(),
        "Project(*.prj)");

    if (TTCut::projectFileName.isEmpty()) return;
  }

  // append project file extension
  QFileInfo fInfo(TTCut::projectFileName);

  if (fInfo.suffix().isEmpty())
    TTCut::projectFileName.append(".prj");

  try
  {
    mpAVData->writeProjectFile(fInfo);
  }
  catch (TTException ex)
  {
    log->errorMsg(__FILE__, __LINE__, QString(tr("error save project file: %1").arg(TTCut::projectFileName)));
    return;
  }
}


/* /////////////////////////////////////////////////////////////////////////////
 * Menu "File save as" action
 */
void TTCutMainWindow::onFileSaveAs()
{
  if (mpAVData->avCount() == 0) {
    return;
  }

  TTCut::projectFileName = ttChangeFileExt(mpCurrentAVDataItem->videoStream()->fileName(), "prj");
  QFileInfo prjFileInfo(QDir(TTCut::lastDirPath), TTCut::projectFileName);

  TTCut::projectFileName = QFileDialog::getSaveFileName( this,
      tr("Save project-file as"),
      prjFileInfo.absoluteFilePath(),
      "Project(*.prj)" );

  if (!TTCut::projectFileName.isEmpty())
  {
    QFileInfo fInfo(TTCut::projectFileName);
    TTCut::lastDirPath = fInfo.absolutePath();

    onFileSave();
  }
}


/* /////////////////////////////////////////////////////////////////////////////
 * Menu "Recent files..." action
 */
void TTCutMainWindow::onFileRecent()
{
  QAction* action = qobject_cast<QAction*>(sender());

  if (action)
    openProjectFile(action->data().toString());
}


/* /////////////////////////////////////////////////////////////////////////////
 * Menu "Exit" action
 */
void TTCutMainWindow::onFileExit()
{
  close();

  qApp->quit();
}

/* /////////////////////////////////////////////////////////////////////////////
 * React to the application window close event
 * - save settings
 * - ask for saving changes
 * - close the project
 */
void TTCutMainWindow::closeEvent(QCloseEvent* event)
{
  if (settings != 0)  settings->writeSettings();

  closeProject();

  if (mpAVData    != 0) delete mpAVData;
  if (settings    != 0) delete settings;

  event->accept();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Menu "Save current frame" action
 */
void TTCutMainWindow::onActionSave()
{
  currentFrame->saveCurrentFrame();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Menu "Settings" action
 */
void TTCutMainWindow::onActionSettings()
{
  TTCutSettingsDlg* settingsDlg = new TTCutSettingsDlg( this );
  settingsDlg->exec();

  log->enableLogFile(TTCut::createLogFile);
  log->setLogModeConsole(TTCut::logModeConsole);
  log->setLogModeExtended(TTCut::logModeExtended);

  if (settings != 0) settings->writeSettings();

  delete settingsDlg;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Menu "About" action
 */
void TTCutMainWindow::onHelpAbout()
{
  TTCutAboutDlg about(this);
  about.exec();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Signals from the video info widget
 */

/* /////////////////////////////////////////////////////////////////////////////
 * Signal from open video action
 */
void TTCutMainWindow::onReadVideoStream(QString fName)
{
  mpAVData->openAVStreams(fName);

  //if (mpCurrentAVDataItem->audioCount() == 0)
  //  onOpenAudioFile();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Signals from the audio list view widget
 */

/* /////////////////////////////////////////////////////////////////////////////
 * Signal from open audio action
 */
void TTCutMainWindow::onReadAudioStream(QString fName)
{
  QFileInfo fInfo(fName);
  mpAVData->appendAudioStream(mpCurrentAVDataItem, fInfo);
}

void TTCutMainWindow::onAppendCutEntry(int cutIn, int cutOut)
{
  if (mpAVData->avCount() == 0) return;

  try
  {
  mpAVData->appendCutEntry(mpCurrentAVDataItem, cutIn, cutOut);
  }
  catch (TTInvalidOperationException* ex)
  {
  	QMessageBox msgBox;
  	msgBox.setText(ex->getMessage());
  	msgBox.exec();
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Video slider position changed signal
 */
void TTCutMainWindow::onVideoSliderChanged(int sPos)
{
  if (mpAVData->avCount() == 0) return;

  if(sliderUpdateFrame) {
    if( TTCut::fastSlider )
      currentFrame->onGotoFrame( sPos, 1 );
    else
      currentFrame->onGotoFrame( sPos, 0 );

    videoFileInfo->refreshInfo(mpCurrentAVDataItem);
    navigation->checkCutPosition(mpCurrentAVDataItem);
  }
  sliderUpdateFrame = true;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Signals from the current frame widget
 */

/* /////////////////////////////////////////////////////////////////////////////
 * New current frame position
 */
void TTCutMainWindow::onNewFramePos(int newPos)
{
  sliderUpdateFrame = false;
  streamNavigator->slider()->setValue( newPos );
  videoFileInfo->refreshInfo(mpCurrentAVDataItem);
  navigation->checkCutPosition(mpCurrentAVDataItem);
}

void TTCutMainWindow::onSetSelectedCutOut(const TTCutItem& cutItem)
{
	cutOutFrame->onAVDataChanged(cutItem.avDataItem());
	cutOutFrame->onGotoCutOut(cutItem.cutOut());
}

void TTCutMainWindow::onGotoMarker(const TTMarkerItem& marker)
{
	mpAVData->onChangeCurrentAVItem(marker.avDataItem());

	onVideoSliderChanged(marker.markerPos());
}

/* /////////////////////////////////////////////////////////////////////////////
 * Signals from the cut list widget
 */

void TTCutMainWindow::onCutSelectionChanged(const TTCutItem& cutItem)
{
	mpAVData->onChangeCurrentAVItem(cutItem.avDataItem());

 if (mpAVData->cutIndexOf(cutItem) == 0) {
	 cutOutFrame->onCutOutChanged(cutItem);
 } else {
	 int index = mpAVData->cutIndexOf(cutItem);
	 cutOutFrame->onCutOutChanged(mpAVData->cutItemAt(index-1));
 }

 currentFrame->onGotoCutIn(cutItem.cutIn());
}

/* /////////////////////////////////////////////////////////////////////////////
 * Create cut preview for current cut list
 */
void TTCutMainWindow::onCutPreview(TTCutList* cutList)
{
  if (cutList == 0 || cutList->count() == 0)
    return;

  connect(mpAVData, SIGNAL(cutPreviewFinished(TTCutList*)), this, SLOT(onCutPreviewFinished(TTCutList*)));
  mpAVData->doCutPreview(cutList);
}

/*!
 * onCutPreviewFinished
 */
void TTCutMainWindow::onCutPreviewFinished(TTCutList* cutList)
{
  TTCutPreview* cutPreview = new TTCutPreview(this);

  qDebug("MainWindow::oncutPreviewFinished");
  cutPreview->initPreview(cutList);
  cutPreview->exec();

  disconnect(mpAVData, SIGNAL(cutPreviewFinished(TTCutList*)), this, SLOT(onCutPreviewFinished(TTCutList*)));
}

/* /////////////////////////////////////////////////////////////////////////////
 * Do video and audio cut
 */
void TTCutMainWindow::onAudioVideoCut(bool, TTCutList* cutData)
{
  // no video stream open or no cut sequences defined; exit
  if (mpAVData->avCount() == 0 || cutData->count() == 0 )
    return;

  if (settings != 0)
      settings->writeSettings();

  // compose video cut name from video file name and set to global variable
  TTCut::cutVideoName = QString("%1_%2.%3").
      arg(QFileInfo(mpCurrentAVDataItem->videoStream()->fileName()).completeBaseName()).
      arg("cut").
      arg(QFileInfo(mpCurrentAVDataItem->videoStream()->fileName()).suffix());

  // start dialog for cut options
  TTCutAVCutDlg* cutAVDlg = new TTCutAVCutDlg(this);

  // user cancel; exit
  if ( cutAVDlg->exec() == 1 )
  {
    delete cutAVDlg;
    return;
  }

  // dialog exit with start
  delete cutAVDlg;

  mpAVData->onDoCut(QFileInfo(QDir(TTCut::cutDirPath), TTCut::cutVideoName).absoluteFilePath(), cutData);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Service methods
 */

/* /////////////////////////////////////////////////////////////////////////////
 * Close current project or video file
 */
void TTCutMainWindow::closeProject()
{
	disconnect(cutList, SIGNAL(selectionChanged(const TTCutItem&)), this, SLOT(onCutSelectionChanged(const TTCutItem&)));
  disconnect(mpAVData, SIGNAL(currentAVItemChanged(TTAVItem*)),   this, SLOT(onAVItemChanged(TTAVItem*)));

  videoFileInfo->onAVDataChanged(mpAVData, 0);
  audioFileList->onAVDataChanged(0);
  streamNavigator->onAVItemChanged(0);
  currentFrame->onAVDataChanged(0);
  cutOutFrame->onAVDataChanged(0);

  TTCut::projectFileName = "";
  navigationEnabled(false);

  mpAVData->clear();

  connect(cutList, SIGNAL(selectionChanged(const TTCutItem&)), this, SLOT(onCutSelectionChanged(const TTCutItem&)));
  connect(mpAVData, SIGNAL(currentAVItemChanged(TTAVItem*)),   this, SLOT(onAVItemChanged(TTAVItem*)));
}

/* /////////////////////////////////////////////////////////////////////////////
 * Enable or disable navigation
 */
void TTCutMainWindow::navigationEnabled( bool enabled )
{
	videoFileInfo->controlEnabled(true);
  cutOutFrame->controlEnabled(enabled);
  currentFrame->controlEnabled(enabled);
  navigation->controlEnabled(enabled);
  streamNavigator->controlEnabled(enabled);
  cutList->controlEnabled(enabled);
  markerList->controlEnabled(enabled);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Open project file
 */
void TTCutMainWindow::openProjectFile(QString fName)
{
  if (mpAVData->avCount() > 0) {
    closeProject();
  }

  QFileInfo fInfo(fName );
  TTCut::lastDirPath = fInfo.absolutePath();

  mpAVData->readProjectFile(fInfo);

  //TODO: catch read project finished signal and update recent file info
  //insertRecentFile(fName);
}

//! Open project file finished
void TTCutMainWindow::onOpenProjectFileFinished()
{
  if (mpCurrentAVDataItem == 0) return;

  insertRecentFile(mpCurrentAVDataItem->videoStream()->fileName());
}


/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTCutMainWindow::onNextAVData()
{
  if (mpCurrentAVDataItem == 0) return;

  int index = mpAVData->avIndexOf(mpCurrentAVDataItem);

  if (index+1 < 0 || index+1 >= mpAVData->avCount()) return;

  mpAVData->onChangeCurrentAVItem(mpAVData->avItemAt(index+1));
}

/* //////////////////////////////////////////////////////////////////////////////
 *
 */
void TTCutMainWindow::onPrevAVData()
{
  if (mpCurrentAVDataItem == 0) return;

  int index = mpAVData->avIndexOf(mpCurrentAVDataItem);

  if (index-1 < 0 || index-1 >= mpAVData->avCount()) return;

  mpAVData->onChangeCurrentAVItem(mpAVData->avItemAt(index-1));
}

void TTCutMainWindow::onAVItemChanged(TTAVItem* avItem)
{
  if (avItem == mpCurrentAVDataItem)  return;

  if (avItem == 0) {
	  closeProject();
	  return;
   }

  mpCurrentAVDataItem = avItem;

  videoFileInfo->onAVDataChanged(mpAVData, avItem);
  currentFrame->onAVDataChanged(avItem);
  cutOutFrame->onAVDataChanged(avItem);
  audioFileList->onAVDataChanged(avItem);

  onNewFramePos( 0 );

  streamNavigator->onAVItemChanged(mpCurrentAVDataItem);

  navigationEnabled( true );
}

// needed by frame navigation!
void TTCutMainWindow::onSetCutOut(int index)
{
	if (mpCurrentAVDataItem == 0) return;

	cutOutFrame->onAVDataChanged(mpCurrentAVDataItem);
  cutOutFrame->onGotoCutOut(index);
}

/* /////////////////////////////////////////////////////////////////////////////
 * onStatusReport;
 */
void TTCutMainWindow::onStatusReport(TTThreadTask* task, int state, const QString& msg, quint64)
{
  switch(state) {
    case StatusReportArgs::Init:
      if (progressBar == 0) {
        progressBar = new TTProgressBar(this);
        connect(progressBar, SIGNAL(cancel()), mpAVData, SLOT(onUserAbortRequest()));
      }
      break;

    case StatusReportArgs::Start:
      if (progressBar != 0)
        progressBar->showBar();
      break;

    case StatusReportArgs::Exit:
    case StatusReportArgs::Error:
    case StatusReportArgs::Canceled:
      if (progressBar != 0) {
        progressBar->hideBar();
      }
      statusBar()->showMessage("ready");
      break;
  }

  if (progressBar != 0)
    progressBar->onSetProgress(task, state, msg, mpAVData->totalProcess(), mpAVData->totalTime());
}

/* /////////////////////////////////////////////////////////////////////////////
 * Update recent file menu actions
 */
void TTCutMainWindow::updateRecentFileActions()
{
  int numRecentFiles = qMin(TTCut::recentFileList.size(), (int)MaxRecentFiles);

  for (int i = 0; i < numRecentFiles; ++i) {
    QString text = tr("&%1 %2").arg(i+1).
      arg(QFileInfo(TTCut::recentFileList[i]).fileName());
    recentFileAction[i]->setText(text);
    recentFileAction[i]->setData(TTCut::recentFileList[i]);
    recentFileAction[i]->setVisible(true);
  }

  for (int j = numRecentFiles; j < MaxRecentFiles; ++j) {
    recentFileAction[j]->setVisible(false);
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Insert new file in recent file list
 */
void TTCutMainWindow::insertRecentFile(const QString& fName)
{
  TTCut::recentFileList.removeAll(fName);
  TTCut::recentFileList.prepend(fName);

  while (TTCut::recentFileList.size() > MaxRecentFiles) {
    TTCut::recentFileList.removeLast();
  }

  foreach (QWidget* widget, QApplication::topLevelWidgets()) {
    TTCutMainWindow* mainWin = qobject_cast<TTCutMainWindow*>(widget);
    if (mainWin) {
      mainWin->updateRecentFileActions();
    }
  }
}
