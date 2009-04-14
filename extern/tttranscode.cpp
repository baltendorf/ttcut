/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : tttranscode.cpp                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 08/07/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTTRANSCODE
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

//TODO: make an IEnocdeProvider as interface for other encode
//
#include "tttranscode.h"
#include "ttencodeparameter.h"

#include "../avstream/ttaviwriter.h"

#include <QDebug>
#include <QCoreApplication>
#include <QTimer>
#include <QTextStream>
#include <QByteArray>

/* ////////////////////////////////////////////////////////////////////////////
 * Create the process form for displaying the output of the encode
 */
TTTranscodeProvider::TTTranscodeProvider(TTEncodeParameter& enc_par )
                    : IStatusReporter()
{
  log               = TTMessageLogger::getInstance();
  this->enc_par     = enc_par;
  str_command       = "transcode";
  transcode_success = false;

  buildCommandLine();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Clean up used resources
 */
TTTranscodeProvider::~TTTranscodeProvider()
{
}

/* /////////////////////////////////////////////////////////////////////////////
 * Parameter for the encoder
 */
void TTTranscodeProvider::buildCommandLine()
{
  //transcode -i encode.avi --pre_clip 0 -y ffmpeg --export_prof dvd-pal --export_asr 2 -o encode
  QString str_aspect  = QString("%1").arg(enc_par.videoAspectCode());
  QString str_format  = QString("%1x%2").arg(enc_par.videoWidth()).arg(enc_par.videoHeight());
  QString str_bitrate = QString("%1").arg(enc_par.videoBitrate());

  strl_command_line.clear();

  strl_command_line << "-i"
		    << enc_par.aviFileInfo().absoluteFilePath()
		    << "--pre_clip"
		    << "0"
	      << "--export_prof"
		    << "dvd"            // dvd-pal
		    << "--export_asr"
		    << str_aspect
		    << "-o"
		    << enc_par.mpeg2FileInfo().absoluteFilePath();

  log->infoMsg(__FILE__, __LINE__, QString("%1").arg(strl_command_line.join(" ")));
}

/* /////////////////////////////////////////////////////////////////////////////
 * converts the mpeg2 stream from encode params to temporary avi
 */
void TTTranscodeProvider::writeAVIFile(TTVideoStream* vs, int start, int end)
{
  TTAVIWriter* aviWriter = new TTAVIWriter(vs);

  aviWriter->writeAVI(start, end, enc_par.aviFileInfo());
  aviWriter->closeAVI();

  delete aviWriter;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Create encoder process and start it
 */
bool TTTranscodeProvider::encodePart(TTVideoStream* vStream, int start, int end)
{
	emit statusReport(StatusReportArgs::ShowProcessForm, "encode part", 0);
	qApp->processEvents();

  writeAVIFile(vStream, start, end);

  transcode_success = false;
  proc = new QProcess();

  // read both channels: stderr and stdout
  proc->setReadChannelMode(QProcess::MergedChannels);

  connectSignals(proc);

  // start the process; if successfully started() was emitted otherwise error()
  proc->start(str_command, strl_command_line);

  int update = 10;
  //proc->waitForFinished();
  // just a very simple event loop ;-)
   while (proc->state() == QProcess::Starting ||
          proc->state() == QProcess::Running     )
   {
     update--;
     if ( update == 0 )
     {
       qApp->processEvents();
       update = 10;
     }
   }

  emit statusReport(StatusReportArgs::HideProcessForm, "encode finished", 0);
  qApp->processEvents();

  delete proc;
  proc = NULL;


  return transcode_success;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Signal and slot connection
 */
void TTTranscodeProvider::connectSignals(QProcess* proc)
{
  connect(proc, SIGNAL(error(QProcess::ProcessError)),       SLOT(onProcError(QProcess::ProcessError)));
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(onProcFinished(int, QProcess::ExitStatus)));
  connect(proc, SIGNAL(readyRead()),                         SLOT(onProcReadOut()) );
  connect(proc, SIGNAL(started()),                           SLOT(onProcStarted()) );
  connect(proc, SIGNAL(stateChanged(QProcess::ProcessState)),SLOT(onProcStateChanged(QProcess::ProcessState)));
}

/* /////////////////////////////////////////////////////////////////////////////
 * This signal is emitted once every time new data is available for reading from
 * the device.
 */
void TTTranscodeProvider::onProcReadOut()
{
  procOutput();
}

/* /////////////////////////////////////////////////////////////////////////////
 * This signal is emitted when the process has started; state() returns Running
 */
void TTTranscodeProvider::onProcStarted()
{
  procOutput();
}

/* /////////////////////////////////////////////////////////////////////////////
 * This signal is emitted when the process finishes
 */
void TTTranscodeProvider::onProcFinished(int e_code, QProcess::ExitStatus e_status)
{
  QString procMsg;

  switch (e_status) {
    case QProcess::NormalExit:
      procMsg = tr("Transcode exit normally ... done(0)");
      transcode_success = true;
      break;
    case QProcess::CrashExit:
      procMsg = tr("Transcode crashed");
      transcode_success = false;
      break;
    default:
      procMsg = tr("unknown process exit status");
      transcode_success = false;
      break;
  }
  log->debugMsg(__FILE__, __LINE__, QString("ProcMessage %1 ProcStatus %2 ProcCode %3").
      arg(procMsg).arg(e_status).arg(e_code));
  exit_code = e_code;
}

/* /////////////////////////////////////////////////////////////////////////////
 * This signal is emitted when an error occurs with the process
 */
void TTTranscodeProvider::onProcError(QProcess::ProcessError proc_error)
{
  QString errorMsg;

  switch (proc_error) {
    case QProcess::FailedToStart:
      errorMsg = tr("The process failed to start.");
      break;
    case QProcess::Crashed:
      errorMsg = tr("The process crashed some time after starting successfully.");
      break;
    case QProcess::Timedout:
      errorMsg = tr("The last waitFor...() function timed out.");
      break;
    case QProcess::WriteError:
      errorMsg = tr("An error occured when attempting to write to the process.");
      break;
    case QProcess::ReadError:
      errorMsg = tr("An error occured when attempting to read from the process.");
      break;
    case QProcess::UnknownError:
    default:
      errorMsg = QString(tr("An unknown error occured: %1")).arg(proc_error);
      break;
  }
  log->errorMsg(__FILE__, __LINE__, QString("%1").arg(errorMsg));
  transcode_success = false;
}

/* /////////////////////////////////////////////////////////////////////////////
 * This signal is emitted whenever the state changed
 */
void TTTranscodeProvider::onProcStateChanged(QProcess::ProcessState proc_state)
{
  QString stateMsg;

  switch (proc_state) {
    case QProcess::NotRunning:
      stateMsg = "The process is not running.";
      break;
    case QProcess::Starting:
      stateMsg = "The process is starting, but program has not yet been invoked.";
      break;
    case QProcess::Running:
      stateMsg = "The process is running and is ready for reading and writing.";
      break;
    default:
      stateMsg = "Unknown process state!";
      break;
  }
  log->debugMsg(__FILE__, __LINE__, QString("%1").arg(stateMsg));
  qApp->processEvents();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Kills the current process, causing it to exit immediately
 */
void TTTranscodeProvider::onProcKill( )
{
  log->debugMsg(__FILE__, __LINE__, "kill the current process!");
  transcode_success = false;
  proc->kill();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Write process output to process window
 */
void TTTranscodeProvider::procOutput()
{
  QByteArray ba = proc->readAll();
  QTextStream out(&ba);
int count = 0;
  while (1) {//!out.atEnd()) {
    QString line = out.readLine();
    count++;
    //log->infoMsg(__FILE__, __LINE__, QString("%1").arg(line));
    //emit processOutput(line);
    emit statusReport(StatusReportArgs::AddProcessLine, line, 0);
    qApp->processEvents();
    if (out.atEnd()) {
    	//qDebug("out is at end! %d", count);
    	return;
    }
  }
}
