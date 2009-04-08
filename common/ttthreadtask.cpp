/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttthreadtask.cpp                                                */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 01/11/2009 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTTHREADTASK
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

#include "ttthreadtask.h"
#include "ttmessagelogger.h"
#include "ttexception.h"

//TODO: exctract status args from ttprogressbar
#include "../gui/ttprogressbar.h"

#include <QThread>
#include <QDebug>

//! Constructor
TTThreadTask::TTThreadTask(QString name) : QObject()
{
	setAutoDelete(false);

  mTaskName   = name;
	log         = TTMessageLogger::getInstance();
  mTaskID     = QUuid::createUuid();
  mTotalSteps = 0;
  mStepCount  = 0;
}

//! Destructor
TTThreadTask::~TTThreadTask()
{
}

//! Returns the task name
QString TTThreadTask::taskName() const
{
  return mTaskName;
}

//! Returns the unique task ID
QUuid TTThreadTask::taskID() const
{
  return mTaskID;
}

//! Elapsed time since task was started in msecs
int TTThreadTask::elapsedTime() const
{
  return (mTaskTime.elapsed() <= 8640000) ? mTaskTime.elapsed() : 0;
}

//! Return the estimate number of total task steps
quint64 TTThreadTask::totalSteps() const
{
  return mTotalSteps;
}

//! Return the current step count
quint64 TTThreadTask::stepCount() const
{
  return mStepCount;
}

//! Wrap status report signal and append reference to task
void TTThreadTask::onStatusReport(int state, const QString& msg, quint64 value)
{
	onStatusReport(this, state, msg, value);
}

//! Status report signal with current as task
void TTThreadTask::onStatusReport(TTThreadTask* task, int state, const QString& msg, quint64 value)
{
  if (state == StatusReportArgs::Start) {
    mStepCount  = 0;
    mTotalSteps = value;
  }

  if (state == StatusReportArgs::Step ||
  		state == StatusReportArgs::Finished)
    mStepCount = value;


  if (state == StatusReportArgs::ShowProcessForm ||
  		state == StatusReportArgs::HideProcessForm)
  	qDebug("process form event...");

  emit statusReport(task, state, msg, value);
}

//! Runable run method
void TTThreadTask::run()
{
  mTaskTime.start();

  try
  {
    emit started(this);
    qApp->processEvents();

    operation();

    emit finished(this);
    qApp->processEvents();

    cleanUp();
  }
  catch(TTAbortException* ex)
  {
    qDebug("TTThreadTask::run -> catched TTAbortException");
    emit aborted(this);
    qApp->processEvents();
    cleanUp();
  }
  catch(TTException* ex)
  {
    qDebug("TTThreadTask::run -> catched TTException");
    emit aborted(this);
    qApp->processEvents();
    cleanUp();

    throw ex;
  }
}

//! Returns the current task progress in % * 1000
int TTThreadTask::processValue() const
{
  int value = (mTotalSteps > 0)
      ? (int)((double)mStepCount / (double)mTotalSteps * 100000.0)
      : 0;

  return value;
}
