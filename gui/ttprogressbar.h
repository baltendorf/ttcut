/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2009                                                      */
/* FILE     : ttprogressbar.h                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/11/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTPROGRESSBAR
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

#ifndef TTPROGRESSBAR_H
#define TTPROGRESSBAR_H

#include <QDialog>
#include <QDateTime>
#include <QUuid>
#include <QHash>

#include "ui_ttprogressform.h"
#include "../common/ttcut.h"
#include "ttprocessform.h"

class TTThreadTask;
class TTTaskProgress;

class TTProgressBar : public QDialog, Ui::TTProgressForm
{
  Q_OBJECT

  public:
    TTProgressBar(QWidget* parent = 0);
    ~TTProgressBar();

    void setActionText( QString action );
    void showBar();
    void hideBar();

    public slots:
      void onDetailsStateChanged(int);
      void onBtnCancelClicked();
      void onSetProgress(TTThreadTask* task, int state, const QString& msg, int totalProgress, QTime totalTime);

    signals:
      void cancel();

    private:
      void addTaskProgress(TTThreadTask* task);
      void setTotalSteps(quint64  t_steps, int r_int=0);
      void setTotalProgress(int progress, QTime time);
      void setTaskProgress(TTThreadTask* task, const QString& msg);
      void setTaskFinished(TTThreadTask* task, const QString& msg);

      void setComplete();
      void resetProgress();

      void updateProgressBar();
      void showProcessForm();
      void addProcessLine(const QString& line);
      void hideProcessForm();

  private:
    TTProcessForm* processForm;
    QHash<QUuid, TTTaskProgress*>* taskProgressHash;
    int            normTotalSteps;

};
#endif // TTPROGRESSBAR_H
