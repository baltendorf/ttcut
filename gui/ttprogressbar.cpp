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
  // initialize variables
  userCancel = false;

  resize( 640, 100 );
    setMinimumSize( QSize( 640, 100 ) );
    setMaximumSize( QSize( 640, 100 ) );
    setBaseSize( QSize( 640, 100 ) );

    setWindowTitle( tr( "Progress Info" ) );

    TTProgressBarLayout = new QGridLayout( this );
    TTProgressBarLayout->setSpacing( 6 );
    TTProgressBarLayout->setMargin( 11 );

    Layout1 = new QHBoxLayout;
    Layout1->setSpacing( 6 );
    Layout1->setMargin( 0 );

    laAction = new QLabel( this );
    laAction->setText( tr( "Action:" ) );
    Layout1->addWidget( laAction );

    actionString = new QLabel( this );
    actionString->setMinimumSize( QSize( 250, 0 ) );
    actionString->setText( tr( "TextLabel2" ) );
    Layout1->addWidget( actionString );

    laElapsedTime = new QLabel( this );
    laElapsedTime->setText( tr( "Elapsed time:" ) );
    Layout1->addWidget( laElapsedTime );

    elapsedTimeString = new QLabel( this );
    elapsedTimeString->setText( tr( "00:00:00" ) );
    Layout1->addWidget( elapsedTimeString );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    laPercentage = new QLabel( this );
    laPercentage->setText( tr( "Percentage complete:" ) );
    Layout1->addWidget( laPercentage );

    percentageString = new QLabel( this );
    percentageString->setText( tr( "0%" ) );
    Layout1->addWidget( percentageString );

    TTProgressBarLayout->addLayout( Layout1, 0, 0 );

    Layout2 = new QHBoxLayout;
    Layout2->setSpacing( 6 );
    Layout2->setMargin( 0 );

    progressBar = new QProgressBar( this );
    progressBar->setTextVisible( false );
    Layout2->addWidget( progressBar );

    // TODO: find a way for canceling operations
    pbCancel = new QPushButton( this );
    pbCancel->setText( tr( "Cancel" ) );
    Layout2->addWidget( pbCancel );

    TTProgressBarLayout->addLayout( Layout2, 1, 0 );

    // signals and slot connection
    connect( pbCancel, SIGNAL( clicked() ), SLOT( slotCancel() ) );

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

void TTProgressBar::setProgress2(int state, const QString& msg, int progress, QTime time)
{
  //qDebug("setProgress: %d / %s / %lld", state, qPrintable(msg), value);
  if (state == StatusReportArgs::Init)
  {
  	resetProgress();
    setActionText(msg);
  }

  if (state == StatusReportArgs::Start) {
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
	qDebug("TTProgressBar::showProcessForm");
	//hide();
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
	qDebug("TTProgressBar::hideProcessForm");
	if (processForm != 0) {
		processForm->hide();
		delete processForm;
		processForm = 0;
		//qApp->processEvents();
	}

  //show();
}
