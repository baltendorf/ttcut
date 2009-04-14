/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttprogressbar.cpp                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/11/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/05/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/02/2006 */
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

#include "ttprogressbar.h"

#include "../common/ttexception.h"
#include "../common/istatusreporter.h"
#include "../common/ttthreadtask.h"

#include <QDebug>
#include <QApplication>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QLayout>
#include <QVariant>
#include <QToolTip>
#include <QDateTime>
#include <QHBoxLayout>
#include <QGridLayout>

	 /*  static const int Init = 0x01;
	   static const int Start = 0x02;
	   static const int Step = 0x03;
	   static const int Finished = 0x04;
	  static const int  Exit = 0x05;
	  static const int  Canceled = 0x06;
	  static const int  Error = 0x07;
	  static const int  ShowProcessForm = 0x08;
	  static const int  AddProcessLine = 0x09;
	  static const int  HideProcessForm = 0x10;*/

/* /////////////////////////////////////////////////////////////////////////////
 * COnstructor
 */
TTProgressBar::TTProgressBar(QWidget* parent)
              : QDialog(parent)
{
  setupUi(this);

  // initialize variables
  userCancel = false;

  // signals and slot connection
  connect( pbCancel, SIGNAL( clicked() ), SLOT( slotCancel() ) );

  processForm = 0;

  // initialize
  elapsedMsec = 0;
  elapsedTime.start();

  totalSteps     = 0;
  normTotalSteps = 1000;

  progressBar->setMinimum( 0 );
  progressBar->setMaximum( normTotalSteps );
}

/* /////////////////////////////////////////////////////////////////////////////
 *  Destroys the object and frees any allocated resources
 */
TTProgressBar::~TTProgressBar()
{
 //elapsedTime.elapsed();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Set the displayed action text
 */
void TTProgressBar::setActionText( QString action )
{
  actionString->setText( action );
}

/* /////////////////////////////////////////////////////////////////////////////
 * Set the displayed elapsed time
 */
void TTProgressBar::setElapsedTime( __attribute__ ((unused))QTime time )
{

}

/* /////////////////////////////////////////////////////////////////////////////
 * Set the displayed percent value
 */
void TTProgressBar::setPercentages( __attribute__ ((unused))float percent )
{

}

/* /////////////////////////////////////////////////////////////////////////////
 * Initialize the progress bar with the total number of steps
 */
//TODO: Remove second parameter for refresh intervall
void TTProgressBar::setTotalSteps(quint64 t_steps, int )
{
  totalSteps += t_steps;

  //int currentProgress = progressBar->value();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Set the current progress
 */
void TTProgressBar::setProgress(int progress, QTime time)
{
    strPercentage.sprintf( "%02.0lf%%", (float)progress/10.0);

    percentageString->setText(strPercentage);
    progressBar->setValue(progress);
    elapsedTimeString->setText(time.toString("hh:mm:ss"));
}

/* /////////////////////////////////////////////////////////////////////////////
 * Set the progress bar to 100%
 */
void TTProgressBar::setComplete()
{
	progressBar->setValue(normTotalSteps);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Reset the progress bar
 */
void TTProgressBar::resetProgress()
{
	totalSteps = 0;

  progressBar->reset();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Button cancel clicked
 */
void TTProgressBar::slotCancel()
{
  qDebug("TTProgressBar::slotCancel");
  userCancel = true;
  emit cancel();
  //qApp->processEvents();
}

void TTProgressBar::setProgress2(QUuid id, int state, const QString& msg, int progress, QTime time)
{
  //qDebug("setProgress: %d / %s / %lld", state, qPrintable(msg), progress);
  if (state == StatusReportArgs::Init)
  {
  	resetProgress();
    setActionText(msg);
  }

  if (state == StatusReportArgs::Start) {
    qDebug() <<  "init progress for taskID " << id;
  	setActionText(msg);
  }

  if (state == StatusReportArgs::ShowProcessForm)
  	showProcessForm();

  if (state == StatusReportArgs::HideProcessForm)
  	hideProcessForm();

  if (state == StatusReportArgs::Step)
    setProgress(progress, time);

  if (state == StatusReportArgs::AddProcessLine)
  	addProcessLine(msg);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Hide the progress bar
 */
void TTProgressBar::hideBar()
{
  //if ( isVisible() )
  //{
    hide();
    setModal( true );
  //}
  //else
  //{
  //  show();
  //  setModal( false );
  //}
  qApp->processEvents();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Show the progress bar
 */
void TTProgressBar::showBar()
{
  //if ( !isVisible() )
  //{
    setModal( false );
    show();
  //}
  //else
  //{
  //  hide();
  //  setModal( true );
  //}
  qApp->processEvents();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Property for user cancel action
 */
bool TTProgressBar::isCanceled()
{
  return userCancel;
}

/*!
 * showProcessForm
 */
void TTProgressBar::showProcessForm()
{
  if (processForm != 0) return;

	processForm = new TTProcessForm(TTCut::mainWindow);
	processForm->setModal(true);
	processForm->showCancelButton(false);
	processForm->show();
}

/*!
 * addProcessLine
 */
void TTProgressBar::addProcessLine(const QString& line)
{
	if (processForm == 0) return;

	processForm->addLine(line);
}

/*!
 * hideProcessForm
 */
void TTProgressBar::hideProcessForm()
{
	if (processForm != 0) {
		processForm->hide();
		delete processForm;
		processForm = 0;
	}
}
