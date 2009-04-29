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
#include "../gui/ttprogressbar.h"

#include <QThreadPool>
#include <QDebug>

/**
 * Threadtaskpool constructor
 */
TTThreadTaskPool::TTThreadTaskPool() : QObject()
{
  QThreadPool::globalInstance()->setExpiryTimeout(100);

  mOverallTotalSteps  = 0;
  mOverallStepCount   = 0;
  mCompletedStepCount = 0;
  mEstimateTaskCount  = 1;
  
  refresh = 0;

  log = TTMessageLogger::getInstance();
}

/**
 * Threadtaskpool destructor
 */
TTThreadTaskPool::~TTThreadTaskPool()
{
	cleanUpQueue();
}

/**
 * Initialize the task pool with the estimate number of tasks
 */
void TTThreadTaskPool::init(int estimateTaskCount)
{
  mEstimateTaskCount = estimateTaskCount;
}

/**
 * Remove all tasks from the pool
 */
void TTThreadTaskPool::cleanUpQueue()
{
  QThreadPool::globalInstance()->waitForDone();

  QMutableListIterator<TTThreadTask*> t(mTaskQueue);
    while (t.hasNext()) {
    TTThreadTask* task = t.next();

    if (task == 0) continue;

    disconnect(task, SIGNAL(started(TTThreadTask*)),  this, SLOT(onThreadTaskStarted(TTThreadTask*)));
    disconnect(task, SIGNAL(finished(TTThreadTask*)), this, SLOT(onThreadTaskFinished(TTThreadTask*)));
    disconnect(task, SIGNAL(aborted(TTThreadTask*)),  this, SLOT(onThreadTaskAborted(TTThreadTask*)));

    disconnect(task, SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)),
               this, SLOT(onStatusReport(TTThreadTask*, int, const QString&, quint64)));


    //qDebug() << "remove task " << task->taskName() << " with UUID " << task->taskID();
    t.remove();
  }

  mOverallTotalSteps  = 0;
  mOverallStepCount   = 0;
  mCompletedStepCount = 0;
  mEstimateTaskCount  = 1;

  refresh = 0;
}

/**
 * Threadtask has emitted start signal
 */
void TTThreadTaskPool::start(TTThreadTask* task, bool runSyncron, int priority)
{
	connect(task, SIGNAL(started(TTThreadTask*)),  this, SLOT(onThreadTaskStarted(TTThreadTask*)));
	connect(task, SIGNAL(finished(TTThreadTask*)), this, SLOT(onThreadTaskFinished(TTThreadTask*)));
	connect(task, SIGNAL(aborted(TTThreadTask*)),  this, SLOT(onThreadTaskAborted(TTThreadTask*)));

  connect(task, SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)),
          this, SLOT(onStatusReport(TTThreadTask*, int, const QString&, quint64)));

  if (runningTaskCount() == 0) {
    emit init();
  }

  if (!mTaskQueue.contains(task)) 
    mTaskQueue.enqueue(task);

  //log->debugMsg(__FILE__, __LINE__, QString("enqueue task %1, current task count %2").
  //    arg(task->taskName()).
  //    arg(mTaskQueue.count()));
  //qDebug() << "enqueue task " << (runSyncron ? "(synchron) " : "(asynchron)" ) << task->taskName() << " with UUID " << task->taskID();


  if (runSyncron)
  	task->runSynchron();
  else
  	QThreadPool::globalInstance()->start(task, priority);
}

/**
 * Threadtask emitted start signal
 */
void TTThreadTaskPool::onThreadTaskStarted(TTThreadTask* task)
{
  //qDebug() << "started " << task->taskName() << " UUID " << task->taskID() << "% " << overallPercentage();
  qDebug() << "started " << task->taskName() << " completed count " << mCompletedStepCount << "% " << overallPercentage();
   //qDebug() << "start " << task->taskName() << " with UUID " << task->taskID() << " active threads " << runningTaskCount() << " queue count " << mTaskQueue.count();
}

/**
 * Threadtask emitted finished signal
 */
void TTThreadTaskPool::onThreadTaskFinished(TTThreadTask* task)
{
	disconnect(task, SIGNAL(started(TTThreadTask*)),  this, SLOT(onThreadTaskStarted(TTThreadTask*)));
	disconnect(task, SIGNAL(finished(TTThreadTask*)), this, SLOT(onThreadTaskFinished(TTThreadTask*)));
	disconnect(task, SIGNAL(aborted(TTThreadTask*)),  this, SLOT(onThreadTaskAborted(TTThreadTask*)));

  disconnect(task, SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)),
             this, SLOT(onStatusReport(TTThreadTask*, int, const QString&, quint64)));


  qDebug() << "finished " << task->taskName() << " UUID " << task->taskID() << "% " << overallPercentage();
  //qDebug() << "finished " << task->taskName() << " with UUID " << task->taskID() << " active threads " << runningTaskCount() << " queue count " << mTaskQueue.count();

  if (runningTaskCount() == 0) {
    cleanUpQueue();
    emit exit();
  }
}

/**
 * Threadtask was successfully aborted
 */
void TTThreadTaskPool::onThreadTaskAborted(TTThreadTask* task)
{
  /*qDebug(qPrintable(QString("TTThreadTaskPool::Task %1 with uuid %2 aborted. IsRunning %3").
					arg(task->taskName()).
  				arg(task->taskID()).
          arg(task->isRunning())));*/

	disconnect(task, SIGNAL(started(TTThreadTask*)),  this, SLOT(onThreadTaskStarted(TTThreadTask*)));
	disconnect(task, SIGNAL(finished(TTThreadTask*)), this, SLOT(onThreadTaskFinished(TTThreadTask*)));
	disconnect(task, SIGNAL(aborted(TTThreadTask*)),  this, SLOT(onThreadTaskAborted(TTThreadTask*)));

  disconnect(task, SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)),
             this, SLOT(onStatusReport(TTThreadTask*, int, const QString&, quint64)));

  mTaskQueue.removeAll(task);

  if (mTaskQueue.count() == 0) {
  //if (runningTaskCount() == 0) {
    qDebug() << "Last thread task aborted -> exit the thread queue!";
    emit aborted();
    emit exit();
  }
}

/**
 * Status reporting
 */
void TTThreadTaskPool::onStatusReport(TTThreadTask* task, int state, const QString& msg, quint64 value)
{
  //if (state != StatusReportArgs::Step)
  //  qDebug() << "pool status report from task " << task->taskName() << " with UUID " << task->taskID() << " state is " << state;

  if (state == StatusReportArgs::Start)
    mOverallTotalSteps = value;

  if(state == StatusReportArgs::Step) {
    refresh++;
    mOverallStepCount = value;

    if (refresh == 20) {
      refresh = 0;
      qDebug() << "step " << task->taskName() << " UUID " << task->taskID() << "% " << overallPercentage();
    }
  }

  if (state == StatusReportArgs::Finished) {
    mCompletedStepCount++;
  }

 
  emit statusReport(task, state, msg, value);
}

/**
 * User request to abort all current operations
 */
void TTThreadTaskPool::onUserAbortRequest()
{
  //qDebug() << "-----------------------------------------------------";
  //qDebug() << "TTThreadTaskPool -> request to abort all tasks";

	for (int i=0; i < mTaskQueue.count(); i++) {
		TTThreadTask* task = mTaskQueue.at(i);

  	disconnect(task, SIGNAL(started(TTThreadTask*)),  this, SLOT(onThreadTaskStarted(TTThreadTask*)));
  	disconnect(task, SIGNAL(finished(TTThreadTask*)), this, SLOT(onThreadTaskFinished(TTThreadTask*)));
  	//disconnect(task, SIGNAL(aborted(TTThreadTask*)),  this, SLOT(onThreadTaskAborted(TTThreadTask*)));

    //disconnect(task, SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)),
    //           this, SLOT(onStatusReport(TTThreadTask*, int, const QString&, quint64)));

    //onStatusReport(task, StatusReportArgs::Step, "Aborting task...", 0);
    task->onUserAbort();
		qApp->processEvents();
  }

  //qDebug() << "-----------------------------------------------------";
}

//! Calculate the total percentage progress value of all enqueued tasks
int TTThreadTaskPool::overallPercentage()
{
  double percent = (mOverallStepCount > 0)
        ? (int)((double)(mOverallStepCount) / (double)(mOverallTotalSteps) * 1000.0 / mEstimateTaskCount)
        : 0;

  return mCompletedStepCount * (1000.0/(double)mEstimateTaskCount) + percent;
}

//! Calculate the total progress time value of all enqueued tasks
QTime TTThreadTaskPool::overallTime()
{
  mOverallTotalTime.setHMS(0, 0, 0, 0);
  int totalTimeMsecs = 0;

  for (int i=0; i < mTaskQueue.count(); i++) {
    TTThreadTask* task = mTaskQueue.at(i);

    //if (task == 0) continue;

    totalTimeMsecs += task->elapsedTime();
  }

  return QTime(0,0,0,0).addMSecs(totalTimeMsecs);
}

/**
 * Returns the current running task count
 */
int TTThreadTaskPool::runningTaskCount()
{
  int runningCount = 0;

  for (int i=0; i<mTaskQueue.count(); i++) {
    TTThreadTask* task = mTaskQueue.at(i);
    //if (task == 0) continue;
    if (task->isRunning()) runningCount++;
  }
  return runningCount;
}
