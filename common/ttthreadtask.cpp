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

#include "../gui/ttprogressbar.h"

#include <QThread>
#include <QDebug>

/**
 * Threadtask construtctor
 */
TTThreadTask::TTThreadTask(QString name) : QObject()
{
	setAutoDelete(false);

  mTaskName   = name;
	log         = TTMessageLogger::getInstance();
  mTaskID     = QUuid::createUuid();
  mTotalSteps = 0;
  mStepCount  = 0;
  mIsSynchron = false;
  mIsRunning  = false;
  mIsAborted  = false;
}

/**
 * Destructor
 */
TTThreadTask::~TTThreadTask()
{
}

/**
 * Returns the task name
 */
QString TTThreadTask::taskName() const
{
  return mTaskName;
}

/**
 * Returns the unique task ID
 */
QUuid TTThreadTask::taskID() const
{
  return mTaskID;
}

/**
 * Elapsed time since task was started in msecs
 */
int TTThreadTask::elapsedTime() const
{
  return (mTaskTime.elapsed() <= 8640000) ? mTaskTime.elapsed() : 0;
}

/**
 * Return the estimate number of total task steps
 */
quint64 TTThreadTask::totalSteps() const
{
  return mTotalSteps;
}

/** 
 * Return the current step count
 */
quint64 TTThreadTask::stepCount() const
{
  return mStepCount;
}

/**
 * Returns true if task is in running state, otherwise false
 */
bool TTThreadTask::isRunning() const
{
  return mIsRunning;
}

//TODO: isRunning should not be set from outside
void TTThreadTask::setIsRunning(bool value)
{
  mIsRunning = value;
}

/**
 * Returns true if the task is sheduled for aborting
 */
bool TTThreadTask::isAborted() const 
{
  return mIsAborted;
}

/**
 * Wrap status report signal and append reference to task
 */
void TTThreadTask::onStatusReport(int state, const QString& msg, quint64 value)
{
	onStatusReport(this, state, msg, value);
}

/**
 * Status report signal with current as task
 */
void TTThreadTask::onStatusReport(TTThreadTask* task, int state, const QString& msg, quint64 value)
{
  if (state == StatusReportArgs::Start) {
    mStepCount  = 0;
    mTotalSteps = value;
  }

  if (state == StatusReportArgs::Step ||
  		state == StatusReportArgs::Finished)
    mStepCount = value;

  emit statusReport(task, state, msg, value);
}

/**
 * Task abort
 */
void TTThreadTask::abort()
{
  qDebug() << "Task " << taskName() << " with UUID " << taskID() << " get's abort request. Is running " << isRunning() << " is aborted " << mIsAborted;

  if (!mIsRunning && !mIsAborted) {
    emit aborted(this);
    qApp->processEvents();
    cleanUp();
  }

  mIsAborted = true;  
}

/** 
 * Run's the task operation synchronus
 */
void TTThreadTask::runSynchron()
{
  qDebug() << "running task " << taskName() << " with uuid " << taskID() << " synchron";
  mIsSynchron = true;
  run();
}

/**
 * Runable run method
 * 
 */
void TTThreadTask::run()
{
 mTaskTime.start();

  try
  {
    if (mIsAborted) {
      qDebug() << taskName() << " entering running state while already aborted!";
      throw new TTAbortException("Aborting operation!");
    }

     //qDebug() << "run task " << taskName() << " with uuid " << taskID();
    mIsRunning = true;
    emit started(this);
    qApp->processEvents();

    operation();

    mIsRunning = false;
    //qDebug() << "emit finished for task " << taskName() << " with UUID " << taskID();
    emit finished(this);
    qApp->processEvents();
    cleanUp();
  }
  catch(TTAbortException* ex)
  {
    qDebug() << taskName() << " with UUID " << taskID() << " catched TTAbortException";
    mIsRunning = false;
    emit aborted(this);
    qApp->processEvents();
    cleanUp();

    if (mIsSynchron) {
      qDebug() << taskName() << " with UUID " << taskID() << " redirect TTAbortException";
      throw ex;
    }
  }
  catch(TTException* ex)
  {
    qDebug() << taskName() << "with UUID " << taskID() << " catched TTException";
    mIsRunning = false;
    emit aborted(this);
    qApp->processEvents();
    cleanUp();
  }
}

/**
 * Returns the current task progress in % * 1000
 */
int TTThreadTask::processValue() const
{
  int value = (mTotalSteps > 0)
      ? (int)((double)mStepCount / (double)mTotalSteps * 100000.0)
      : 0;

  return value;
}
