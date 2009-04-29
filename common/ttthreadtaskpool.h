/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttthreadtaskpool.h                                              */
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


#ifndef TTTHREADTASKPOOL_H
#define TTTHREADTASKPOOL_H

#include <QObject>
#include <QTime>
#include <QQueue>

class TTMessageLogger;
class TTThreadTask;

//TODO: rename to TTThreadTaskManager
//! Class to manage various thread tasks
class TTThreadTaskPool : public QObject
{
	Q_OBJECT

	public:
		TTThreadTaskPool();
    ~TTThreadTaskPool();

    void  init(int estimateTaskCount);
		void  start(TTThreadTask* task, bool runSyncron=false, int priority=0);
    int   overallPercentage();
    QTime overallTime();

  signals:
    void init();
    void aborted();
    void exit();
    void statusReport(TTThreadTask* task, int state, const QString& msg, quint64 value);

  public slots:
		void onUserAbortRequest();

	private slots:
	  void onThreadTaskStarted(TTThreadTask* task);
		void onThreadTaskFinished(TTThreadTask* task);
		void onThreadTaskAborted(TTThreadTask* task);
    void onStatusReport(TTThreadTask* task, int state, const QString& msg, quint64 value);

	private:
		void cleanUpQueue();
    int  runningTaskCount();

	private:
    QQueue<TTThreadTask*> mTaskQueue;
    TTMessageLogger*      log;
    QTime                 mOverallTotalTime;
    quint64               mOverallTotalSteps;
    quint64               mOverallStepCount;
    quint64               mCompletedStepCount;
    int                   mEstimateTaskCount;
    int refresh;
    
};
#endif
