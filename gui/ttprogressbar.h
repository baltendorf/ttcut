/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttprogressbar.h                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/11/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/05/2005 */
/* MODIFIED: b. altendorf                                    DATE: 04/24/2007 */
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

#include "ui_ttprogressform.h"
#include "../common/ttcut.h"
#include "ttprocessform.h"

class TTThreadTask;

class TTProgressBar : public QDialog, Ui::TTProgressForm
{
    Q_OBJECT

public:
    TTProgressBar(QWidget* parent = 0);
    ~TTProgressBar();

    // public methods
    void setActionText( QString action );
    void setElapsedTime( QTime time );
    void setPercentages( float percent );

    void setTotalSteps(quint64  t_steps, int r_int=0);
    void setProgress(int progress, QTime time);
    void setComplete();
    void resetProgress();

    void showBar();
    void hideBar();

    bool isCanceled();

    public slots:
    void slotCancel();
    void setProgress2(int state, const QString& msg, int progress, QTime time);

    signals:
      void cancel();

private:
    void updateProgressBar();
    void showProcessForm();
    void addProcessLine(const QString& line);
    void hideProcessForm();

 private:
    TTProcessForm* processForm;

    bool          userCancel;
    QTime         elapsedTime;
    int           elapsedMsec;
    QString       strPercentage;
    quint64      totalSteps;
    int           refresh_intervall;
    int           refresh;
    int           normTotalSteps;
    int           normProgress;
    double        progressPercent;

};
#endif // TTPROGRESSBAR_H
