/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttthreadtask.h                                                  */
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

#ifndef TTTHREADTASK_H
#define TTTHREADTASK_H

#include <QRunnable>
#include <QObject>
#include <QTime>
#include <QUuid>

#include "../common/istatusreporter.h"

class TTMessageLogger;

//! Baseclass for all task runables
class TTThreadTask : public QObject, public QRunnable
{
	Q_OBJECT

	public:
		TTThreadTask(QString name);
    virtual ~TTThreadTask();

		void    run();
    QString taskName() const;
    QUuid   taskID() const;
    int     elapsedTime() const;
    quint64 totalSteps() const;
    quint64 stepCount() const;
    int     processValue() const;
    bool    isRunning() const;

  protected:
    virtual void operation() = 0;
    virtual void cleanUp() = 0;

	public slots:
		virtual void onUserAbort() = 0;

  protected slots:
    virtual void onStatusReport(int state, const QString& msg, quint64 value);
    virtual void onStatusReport(TTThreadTask* task, int state, const QString& msg, quint64 value);

	signals:
	  void started(TTThreadTask* task);   /**<internal signal thread was started  */
	  void finished(TTThreadTask* task);  /**<internal signal thread has finished */
	  void aborted(TTThreadTask* task);   /**<internal signal thread was aborted  */

		void statusReport(TTThreadTask* task, int state, const QString& msg, quint64 value);

	protected:
		QUuid            mTaskID;      /**<unique task ID                             */
    QTime            mTaskTime;    /**<time set to current time when task started */
    quint64          mTotalSteps;  /**<estimate count of total task steps         */
    quint64          mStepCount;   /**<current step count                         */
    TTMessageLogger* log;          /**<message logger istance                     */
    QString          mTaskName;    /**<task name                                  */
    bool             mIsRunning;
};

#endif
