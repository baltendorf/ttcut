/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmplayerwidget.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/03/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMPLAYERWIDGET
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

#include "ttmplayerwidget.h"

#include <QtGui/QLayout>

/* /////////////////////////////////////////////////////////////////////////////
 * Constructor for MplayerWidget
 */
TTMplayerWidget::TTMplayerWidget(QWidget *parent)
    : TTVideoPlayer(parent)
{
  log = TTMessageLogger::getInstance();

  mplayerProc = NULL;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Destructor: clean up used resources
 */
TTMplayerWidget::~TTMplayerWidget()
{
  stopMplayer();
}

void TTMplayerWidget::cleanUp()
{
  stopMplayer();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Play the current movie; You have to load the current movie first!
 */
void TTMplayerWidget::play()
{
  mplayerProc = new QProcess();
  playMplayer(currentMovie);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Stop playin the movie
 */
void TTMplayerWidget::stop()
{
  stopMplayer();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Load the current movie from file
 */
void TTMplayerWidget::load(QString fileName)
{
  currentMovie = fileName;

  stopMplayer();

  if (parentWidget() == NULL)
    resize(640, 480);
  else
    resize(parentWidget()->width()-1, parentWidget()->height()-1);

   emit optimalSizeChanged();
}

QSize TTMplayerWidget::sizeHint() const
{
  if (parentWidget() == NULL)
    return QSize(640, 480);

  return QSize(parentWidget()->width()-1, parentWidget()->height()-1);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns a value indicating if the native movie controller controls are
 * visible.
 * Remark: For mplayer this method returns always false!
 */
bool TTMplayerWidget::getControlsVisible() const
{
  return false;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Set a value indicating if the native movie controller controls are visible.
 * Remark: For mplayer this value is ignored!
 */
void TTMplayerWidget::setControlsVisible(bool visible)
{
  areControlsVisible = visible;
}

/* /////////////////////////////////////////////////////////////////////////////
 * MPlayer interface
 *
 * start mplayer playing file <fileName>
 */
bool TTMplayerWidget::playMplayer(QString videoFile)
{
  QString     str_cmd;
  QStringList mplayer_cmd;

  if (!ttAssigned(mplayerProc))
    return false;

  //FIXME:
  //if (isPlaying)
  //  return false;

  // Setup interface with MPlayer
  mplayer_cmd.clear();

  // ----------------------------------------------------------------------
  // slave-mode
  // ----------------------------------------------------------------------
  // Switches on slave mode, in which MPlayer works as a backend for other
  // programs. Instead of intercepting keyboard events, MPlayer will read
  // commands from stdin.
  // NOTE: See -input cmdlist for a list of slave commands and
  // DOCS/tech/slave.txt for their description.
  // ----------------------------------------------------------------------

  // Every argument must have it's own addArgument
  mplayer_cmd << "-slave"
              << "-identify"
              << "-quiet"
              << "-wid";
  str_cmd.sprintf( "%ld",(long)winId() );
  mplayer_cmd << str_cmd
              << "-geometry";
  str_cmd.sprintf( "%dx%d+0+0", movieSize.width(), movieSize.height());
  mplayer_cmd << str_cmd
              << videoFile;

  log->infoMsg(__FILE__, __LINE__, QString("mplayer command: %1").arg(mplayer_cmd.join(" ")));

  if (mplayerProc->state() == QProcess::Running)
  {
      log->errorMsg(__FILE__, __LINE__, "error starting mplayer!");
      return false;
  }

  // start the mplayer process
  mplayerProc->start( "mplayer", mplayer_cmd );

  // signal and slot connection for the mplayer process
  // detect when mplayer has information ready for us
  connect(mplayerProc, SIGNAL(started()),                           this, SLOT( mplayerStarted()));
  connect(mplayerProc, SIGNAL(readyRead()),                         this, SLOT( readFromStdout()));
  connect(mplayerProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT( exitMplayer(int, QProcess::ExitStatus) ) );
  connect(mplayerProc, SIGNAL(error(QProcess::ProcessError)),       this, SLOT( errorMplayer(QProcess::ProcessError) ) );
  connect(mplayerProc, SIGNAL(stateChanged(QProcess::ProcessState)),this, SLOT( stateChangedMplayer(QProcess::ProcessState) ) );

  mIsPlaying = true;
  //emit isPlayingEvent(isPlaying);
  return true;
}

/* /////////////////////////////////////////////////////////////////////////////
 * stop mplayer playing
 */
bool TTMplayerWidget::stopMplayer()
{
  if (!ttAssigned(mplayerProc))
    return false;

  if (!isPlaying())
    return false;

  // stop mplayer
  mplayerProc->write( "quit\n" );
  mIsPlaying = false;

  //emit isPlayingEvent(isPlaying);
  return true;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Slot was called, when mplayer process was started
 */
void TTMplayerWidget::mplayerStarted()
{
  log->infoMsg(__FILE__, __LINE__, "mplayer process started");
}

/* /////////////////////////////////////////////////////////////////////////////
 * Read messages from mplayer process from stdout
 */
void TTMplayerWidget::readFromStdout()
{
  char       temp_str[101];
  int        i, i_pos;
  QString    line;
  QByteArray ba;

  if(mplayerProc->state() == QProcess::Running)
  {
    ba = mplayerProc->readAll();

    i_pos = 0;

    for ( i = 0; i < ba.size(); ++i) 
    {
      if ( ba.at(i) != '\n' && i_pos < 100)
      {
        temp_str[i_pos] = ba.at(i);
        i_pos++;
      }
      else
      {
        temp_str[i_pos] = '\0';
        line = temp_str;
        log->infoMsg(__FILE__, __LINE__, QString("%1").arg(line));
        i_pos = 0;
      }
    }
  }
  else
  {
    line = "mplayer finished... done(0)";
    mIsPlaying = false;
    emit playerFinished();

    log->infoMsg(__FILE__, __LINE__, QString("%1").arg(line));
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * exit mplayer process
 */
void TTMplayerWidget::exitMplayer(int e_code, QProcess::ExitStatus e_status)
{
  log->debugMsg(__FILE__, __LINE__, QString("exit code %1 / exit status %2").arg(e_code).arg(e_status));

  // delete the mplayer process
  delete mplayerProc;

  mIsPlaying = false;
  emit playerFinished();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Error message to logfile
 */
void TTMplayerWidget::errorMplayer( QProcess::ProcessError error )
{
  log->errorMsg(__FILE__, __LINE__, QString("error: %1").arg(error));

  delete mplayerProc;
  mIsPlaying = false;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Mplayer process state changed
 */
void TTMplayerWidget::stateChangedMplayer( QProcess::ProcessState newState )
{
  log->debugMsg(__FILE__, __LINE__, QString("state changed: %1").arg(newState));

  if(newState == QProcess::NotRunning){
    mIsPlaying = false;
  }
}



