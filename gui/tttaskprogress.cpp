/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2009                                                      */
/* FILE     : tttaskprogress.cpp                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 04/14/2009 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTTASKPROGRESS
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

#include "tttaskprogress.h"

#include "common/ttthreadtask.h"

#include <QDebug>

/**
 * Constructor
 */
TTTaskProgress::TTTaskProgress(QWidget* parent, TTThreadTask* task)
    : QFrame(parent)
{
  setupUi( this );

  progressBar->setMinimum(0);
  progressBar->setMaximum(1000);

  mpTask = task;
}

/**
 * Destructor
 */
TTTaskProgress::~TTTaskProgress()
{

}

/**
 * Set progress to complete
 */
void TTTaskProgress::onTaskFinished(const QString& msg)
{
  lblAction->setText(msg);
  lblPercent->setText(tr("completed"));
  progressBar->setValue(progressBar->maximum());
}

/**
 * Refresh progress value
 */
void TTTaskProgress::onRefreshProgress(const QString& msg)
{
  lblAction->setText(msg);
  lblPercent->setText(QString("%1%").arg(mpTask->processValue()/1000.0, 0, 'f', 0));
  progressBar->setValue(mpTask->processValue()/100);
}


