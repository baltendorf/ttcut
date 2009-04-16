/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttthreadtaskpool.cpp                                            */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 01/11/2009 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTTHREADTASKPOOL
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


#include "ttthreadtaskpool.h"
#include "ttthreadtask.h"

#include "../common/ttmessagelogger.h"
#include "../common/ttexception.h"

//TODO: exctract status args from ttprogressbar
#include "../gui/ttprogressbar.h"

#include <QThreadPool>
#include <QDebug>

//! Constructor
TTThreadTaskPool::TTThreadTaskPool() : QObject()
{
  QThreadPool::globalInstance()->setExpiryTimeout(100);

  log                = TTMessageLogger::getInstance();
	mActiveThreadCount = 0;
}

//! Destructor
TTThreadTaskPool::~TTThreadTaskPool()
{
	cleanUpQueue();
}

//! Remove all tasks from the pool
void TTThreadTaskPool::cleanUpQueue()
{
  QThreadPool::globalInstance()->waitForDone();

  QMutableListIterator<TTThreadTask*> t(mTaskQueue);
    while (t.hasNext()) {
    TTThreadTask* task = t.next();

    if (task == 0) continue;

    t.remove();
  }
}

//! Start the given task
void TTThreadTaskPool::start(TTThreadTask* task, bool runSyncron, int priority)
{
	connect(task, SIGNAL(started(TTThreadTask*)),  this, SLOT(onThreadTaskStarted(TTThreadTask*)));
	connect(task, SIGNAL(finished(TTThreadTask*)), this, SLOT(onThreadTaskFinished(TTThreadTask*)));
	connect(task, SIGNAL(aborted(TTThreadTask*)),  this, SLOT(onThreadTaskAborted(TTThreadTask*)));

  connect(task, SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)),
          this, SLOT(onStatusReport(TTThreadTask*, int, const QString&, quint64)));

  if (mActiveThreadCount == 0) {
    emit init();
  }

  mActiveThreadCount++;

  if (!mTaskQueue.contains(task))
    mTaskQueue.enqueue(task);

  log->debugMsg(__FILE__, __LINE__, QString("enqueue task %1, current task count %2").
      arg(task->taskName()).
      //arg(mActiveThreadCount));
      arg(mTaskQueue.count()));

  if (runSyncron)
  	task->run();
  else
  	QThreadPool::globalInstance()->start(task, priority);
}

//! Thread task started
void TTThreadTaskPool::onThreadTaskStarted(TTThreadTask*)
{
}

//! Thread task finished
void TTThreadTaskPool::onThreadTaskFinished(TTThreadTask* task)
{
	disconnect(task, SIGNAL(started(TTThreadTask*)),  this, SLOT(onThreadTaskStarted(TTThreadTask*)));
	disconnect(task, SIGNAL(finished(TTThreadTask*)), this, SLOT(onThreadTaskFinished(TTThreadTask*)));
	disconnect(task, SIGNAL(aborted(TTThreadTask*)),  this, SLOT(onThreadTaskAborted(TTThreadTask*)));

  disconnect(task, SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)),
             this, SLOT(onStatusReport(TTThreadTask*, int, const QString&, quint64)));

	mActiveThreadCount--;

  if (mActiveThreadCount < 0) {
    qDebug("Thread count gets negative!");
  	throw new TTInvalidOperationException(
				QString("Exception during task abort %1. Active thread count gets invalid!").
				arg(task->taskName()));
  }

  if (mActiveThreadCount == 0) {
    cleanUpQueue();
    emit exit();
  }
}

//! Thread task aborted
void TTThreadTaskPool::onThreadTaskAborted(TTThreadTask* task)
{
  qDebug(qPrintable(QString("TTThreadTaskPool::Task %1 with uuid %2 aborted").
					arg(task->taskName()).
  				arg(task->taskID())));
	mActiveThreadCount--;

	if (mActiveThreadCount < 0)
			throw new TTInvalidOperationException(
						QString("Exception during task abort %1. Active thread count gets invalid!").
						arg(task->taskName()));

	if (mActiveThreadCount == 0) {
    cleanUpQueue();
    emit aborted();
    emit exit();
  }
}

//! Task status reporting
void TTThreadTaskPool::onStatusReport(TTThreadTask* task, int state, const QString& msg, quint64 value)
{
  if (state == StatusReportArgs::Start) {
  	//qDebug(qPrintable(QString("TTThreadTaskPool::Start value is %1").
  	//		arg(value)));
    /*qDebug(qPrintable(QString("TTThreadTaskPool::Start %1 in thread %2").
        arg(task->taskName()).
        arg((int)QThread::currentThreadId())));
    qDebug(qPrintable(QString("TTThreadTaskPool::Start %1 percent %2 time %3").
        arg(task->taskName()).
        arg(overallPercentage()).
        arg(overallTime().toString("hh:mm:ss"))));*/
   }

  //if (state == StatusReportArgs::Step) {
    /*  qDebug(qPrintable(QString("TTThreadTaskPool::Step %1 percent %2 time %3").
          arg(task->taskName()).
          arg(overallPercentage()).
          arg(overallTime().toString("hh:mm:ss"))));*/
  	//qDebug(qPrintable(QString("task step %1 percent %2").arg(task->taskName()).arg(task->processValue())));
   //}

  if (state == StatusReportArgs::Finished) {
      //qDebug(qPrintable(QString("TTThreadTaskPool::Finished %1 percent %2 time %3").
      //    arg(task->taskName()).
      //    arg(overallPercentage()).
      //    arg(overallTime().toString("hh:mm:ss"))));
  }
  emit statusReport(task, state, msg, value);
}

//! User request to abort all current operations
void TTThreadTaskPool::onUserAbortRequest()
{
	for (int i=0; i < mTaskQueue.count(); i++) {
		TTThreadTask* task = mTaskQueue.at(i);

		task->onUserAbort();
		qApp->processEvents();
	}
}

//! Calculate the total percentage progress value of all enqueued tasks
int TTThreadTaskPool::overallPercentage()
{
  mOverallStepCount  = 0;

  for (int i=0; i < mTaskQueue.count(); i++) {
    TTThreadTask* task = mTaskQueue.at(i);

    if (task == 0) {
      qDebug() << "task was already finished; current overall step count "
               << mOverallStepCount;
      continue;
    } 

    mOverallStepCount += task->processValue();
  }


  return (mOverallStepCount > 0)
      ? (int)((double)mOverallStepCount / (double)(100000.0*mTaskQueue.count()) * 1000.0)
      : 0;
}

//! Calculate the total progress time value of all enqueued tasks
QTime TTThreadTaskPool::overallTime()
{
  mOverallTotalTime.setHMS(0, 0, 0, 0);
  int totalTimeMsecs = 0;

  for (int i=0; i < mTaskQueue.count(); i++) {
    TTThreadTask* task = mTaskQueue.at(i);

    if (task == 0) continue;

    totalTimeMsecs += task->elapsedTime();
  }

  return QTime(0,0,0,0).addMSecs(totalTimeMsecs);
}

