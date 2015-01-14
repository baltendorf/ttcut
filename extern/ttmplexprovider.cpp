/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2009                                                      */
/* FILE     : ttmplexprovider.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/17/2009 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMPLEXPROVIDER
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

#include "ttmplexprovider.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#define EVENT_LOOP_INTERVALL 10

/**
 *Usage: mplex [params] -o <output filename pattern> <input file>...
 *%d in the output file name is by segment count
 *where possible params are:
 *--verbose|-v num
 *  Level of verbosity. 0 = quiet, 1 = normal 2 = verbose/debug
 *--format|-f fmt
 *  Set defaults for particular MPEG profiles
 *  [0 = Generic MPEG1, 1 = VCD, 2 = user-rate VCD, 3 = Generic MPEG2,
 *   4 = SVCD, 5 = user-rate SVCD
 *   6 = VCD Stills, 7 = SVCD Stills, 8 = DVD with NAV sectors, 9 = DVD]
 *--mux-bitrate|-r num
 *  Specify data rate of output stream in kbit/sec
 *  (default 0=Compute from source streams)
 *--video-buffer|-b num [, num...]
 *  Specifies decoder buffers size in kB.  [ 20...2000]
 *--lpcm-params | -L samppersec:chan:bits [, samppersec:chan:bits]
 *--mux-limit|-l num
 *  Multiplex only num seconds of material (default 0=multiplex all)
 *--sync-offset|-O num ms|s|mpt
 *  Specify offset of timestamps (video-audio) in mSec
 *--sector-size|-s num
 *  Specify sector size in bytes for generic formats [256..16384]
 *--vbr|-V
 *  Multiplex variable bit-rate video
 *--packets-per-pack|-p num
 *  Number of packets per pack generic formats [1..100]
 *--system-headers|-h
 *  Create System header in every pack in generic formats
 *--max-segment-size|-S size
 *  Maximum size of output file(s) in Mbyte (default: 0) (no limit)
 *--split-segment|-M
 *  Simply split a sequence across files rather than building run-out/run-in
 *--workaround|-W workaround [, workaround ]
 *--help|-?
 *  Print this lot out!
 */

/*! \brief Provides access to the external multiplexer <mplex>
 *
 * In the future, this modul should be implemented as plugin
 */
TTMplexProvider::TTMplexProvider(TTMuxListData* muxList) : IStatusReporter()
{
  log       = TTMessageLogger::getInstance();
  mpMuxList = muxList;
}

//! Clean up used resources
TTMplexProvider::~TTMplexProvider()
{
}

/*! \brief Write mux data in muxscript
 *
 * Theire should be only one muxscript for one session. Purpose of the muxscript
 * is batch-muxing all cutted videos from the current session, after(!) quitting
 * TTCut.
 */
void TTMplexProvider::writeMuxScript()
{
  QStringList mplexCmd;
  QFileInfo   muxInfo(QDir(TTCut::cutDirPath), TTCut::muxFileName);
  QFile       muxFile(muxInfo.absoluteFilePath());

  muxFile.remove();
  muxFile.open(QIODevice::WriteOnly | QIODevice::Text);

  QTextStream muxOutStream(&muxFile);

  muxOutStream << "# TTCut - Mplex script ver. 1.0" << "\n";
  //TODO: use /usr/bin/env bash if available
  //muxOutStream << "#!/bin/sh" << "\n";
  muxOutStream << "#\n";

  for (int i=0; i < mpMuxList->count(); i++)
  {
    mplexCmd.clear();
    mplexCmd << "mplex";
    mplexCmd << createMplexArguments(mpMuxList->videoFilePathAt(i), mpMuxList->audioFilePathsAt(i), true);

    muxOutStream << mplexCmd.join(" ") << "\n#\n";
  }

  muxFile.flush();
  muxFile.close();

  // make muxscript executeable by the owner/user
  bool result = muxFile.setPermissions(
        QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner|
        QFile::ReadUser |QFile::WriteUser |QFile::ExeUser |
        QFile::ReadGroup|QFile::ExeGroup);

  if (!result)
    log->warningMsg(__FILE__, __LINE__, "Cannot modify permissions of the muxscript!");
}

//! Call mplex to mux current stream.
void TTMplexProvider::mplexPart(int index)
{
  mCurrentMuxIndex = index;
  int  update      = EVENT_LOOP_INTERVALL;
  proc             = new QProcess();

  proc->setReadChannelMode( QProcess::MergedChannels );
  qApp->processEvents();

  connect(proc, SIGNAL(error(QProcess::ProcessError)),        SLOT(onProcError(QProcess::ProcessError)));
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),  SLOT(onProcFinished(int, QProcess::ExitStatus)));
  connect(proc, SIGNAL(readyRead()),                          SLOT(onProcReadOut()));
  connect(proc, SIGNAL(started()),                            SLOT(onProcStarted()));
  connect(proc, SIGNAL(stateChanged(QProcess::ProcessState)), SLOT(onProcStateChanged(QProcess::ProcessState)));

  QString     mplexCmd  = "mplex";
  QStringList mplexArgs = createMplexArguments(mpMuxList->videoFilePathAt(index), mpMuxList->audioFilePathsAt(index), false);

  log->debugMsg(__FILE__, __LINE__, QString("Mplex command string: %1").arg(mplexArgs.join(" ")));

  emit statusReport(
      ((TTCut::muxPause) ? StatusReportArgs::ShowProcessFormBlocking : StatusReportArgs::ShowProcessForm), 
      "Starting mplex", 0);
  qApp->processEvents();
  proc->start(mplexCmd, mplexArgs);

  // just a very simple event loop ;-)
  while (proc->state() == QProcess::Starting ||
         proc->state() == QProcess::Running     )
  {
    update--;
    if ( update == 0 )
    {
      qApp->processEvents();
      update = EVENT_LOOP_INTERVALL;
    }
  }

  emit statusReport(StatusReportArgs::HideProcessForm, "Finished mplex", 0);

  qApp->processEvents();
}

//! Creates the muxed video output file path
QString TTMplexProvider::createOutputFilePath(const QString& videoFilePath)
{
  QFileInfo outputFileInfo(videoFilePath);
  QDir      outputDirectory((TTCut::muxOutputPath.isEmpty())
        ? outputFileInfo.absolutePath()
        : TTCut::muxOutputPath);

  outputFileInfo.setFile(outputDirectory, outputFileInfo.fileName());

  return ttChangeFileExt(outputFileInfo.absoluteFilePath(), "mpg");
}

//! Create the mplex arguments for given video and audio files
QStringList TTMplexProvider::createMplexArguments(const QString& videoFilePath, const QStringList& audioFilePaths, bool escapeFileNames)
{
  QStringList mplexArgs;

  mplexArgs << "-f8"
            << "-o"
            << (escapeFileNames
									? toAscii(QString("\"%1\"").arg(createOutputFilePath(videoFilePath)))
									: toAscii(QString("%1").arg(createOutputFilePath(videoFilePath))))
            << (escapeFileNames
									? toAscii(QString("\"%1\"").arg(videoFilePath))
			            : toAscii(QString("%1").arg(videoFilePath)));

  QStringListIterator listIterator(audioFilePaths);
  while(listIterator.hasNext())
    mplexArgs << (escapeFileNames
						? toAscii(QString("\"%1\"").arg(listIterator.next()))
						: toAscii(QString("%1").arg(listIterator.next())));

  return mplexArgs;
}

//! Delete the elementary streams from disk
void TTMplexProvider::deleteElementaryStreams(const QString& videoFilePath, const QStringList& audioFilePaths)
{
  QFile videoFile(videoFilePath);
  bool  success = videoFile.remove();

  log->debugMsg(__FILE__, __LINE__, QString("Removing video stream %1 (%2)").
        arg(videoFilePath).
        arg(success ? "success" : "failed"));

  QStringListIterator listIterator(audioFilePaths);
  while(listIterator.hasNext()) {
    QString audioFilePath = listIterator.next();
    QFile   audioFile(audioFilePath);
    success = audioFile.remove();

    log->debugMsg(__FILE__, __LINE__, QString("Removing audio stream %1 (%2)").
        arg(audioFilePath).
        arg(success ? "success" : "failed"));
  }
}

// /////////////////////////////////////////////////////////////////////////////
// Event Processing
//
//! This signal is emitted when an error occurs with the process
void TTMplexProvider::onProcError(QProcess::ProcessError procError)
{
  log->errorMsg(__FILE__, QString("QProcess error %1").arg(procError), __LINE__);
}

//! This signal is emitted once every time new data is available for reading from
//! the device
void TTMplexProvider::onProcReadOut()
{
  procOutput();
}

//! This signal is emitted when the process has started;
//! state() returns running
void TTMplexProvider::onProcStarted()
{
  emit statusReport(StatusReportArgs::AddProcessLine, "start mplex process", 0);
  qApp->processEvents();
  procOutput();
}

//! This signal is emitted when the process finishes
void TTMplexProvider::onProcFinished(int, QProcess::ExitStatus exitStatus)
{
  if (exitStatus != QProcess::NormalExit) return;
  if (!TTCut::muxDeleteES)                return;

  for (int i=0; i<mpMuxList->count(); i++)
    deleteElementaryStreams(mpMuxList->videoFilePathAt(i), mpMuxList->audioFilePathsAt(i));
}

//! This signal is emitted whenever the state changed
void TTMplexProvider::onProcStateChanged(QProcess::ProcessState procState)
{
  QString stateMsg;

  switch (procState) {
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

  log->debugMsg(__FILE__, stateMsg, __LINE__);
  emit statusReport(StatusReportArgs::AddProcessLine, stateMsg, 0);
  qApp->processEvents();
}

//! Kills the current process, causing it to exit immediately
void TTMplexProvider::onProcKill()
{
  proc->kill();
}

//! Write the process output to process form
void TTMplexProvider::procOutput()
{
  if (proc == 0) return;

  QString    line;
  QByteArray ba = proc->readAll();
  QTextStream out(&ba);

  while (!out.atEnd())
  {
    line = out.readLine();
    log->debugMsg(__FILE__, __LINE__, QString("* %1").arg(line));
    emit statusReport(StatusReportArgs::AddProcessLine, line, 0);
    qApp->processEvents();
  }
}

// /////////////////////////////////////////////////////////////////////////////
// Settings
// /////////////////////////////////////////////////////////////////////////////
//
//! Level of verbosity (--verbose|-v num)
int TTMplexProvider::createVerboseHash()
{
  verbose["quiet"]         = 0;
  verbose["normal"]        = 1;
  verbose["verbose/debug"] = 2;

  return verbose.size();
}

//! Set defaults for particular MPEG profiles (--format|-f fmt)
int TTMplexProvider::createFormatHash()
{
  format["Generic MPEG1"]        = 0;
  format["VCD"]                  = 1;
  format["user-rate VCD"]        = 2;
  format["Generic MPEG2"]        = 3;
  format["SVCD"]                 = 4;
  format["user-rate VCD"]        = 5;
  format["VCD Stills"]           = 6;
  format["SVCD Stills"]          = 7;
  format["DVD with NAV sectors"] = 8;
  format["DVD"]                  = 9;

  return format.size();
}
