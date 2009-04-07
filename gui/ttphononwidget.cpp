/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttphononwidget.cpp                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 11/24/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTPHONONWIDGET
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

#include "ttphononwidget.h"

TTPhononWidget::TTPhononWidget(QWidget *parent)
  : TTVideoPlayer(parent)
{
  player = new Phonon::VideoPlayer(Phonon::VideoCategory, this);

  mIsPlaying = false;

  connect(player, SIGNAL(finished()),  SLOT(onPlayerFinished()));
  connect((QObject*)(player->mediaObject()), SIGNAL(stateChanged(Phonon::State, Phonon::State)), SLOT(onStateChanged(Phonon::State, Phonon::State)));
}

TTPhononWidget::~TTPhononWidget()
{
  if (player != 0) {
    cleanUp();
    delete player;
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * resizeEvent
 */
void TTPhononWidget::resizeEvent(QResizeEvent* e)
{
	player->resize(e->size().width(), e->size().height());
}

void TTPhononWidget::cleanUp()
{
    QString dummyFile = "/tmp/dummy";
    player->load(dummyFile);
}

void TTPhononWidget::load(QString value)
{
  mIsPlaying = false;
  player->stop();
  player->load(value);
}

void TTPhononWidget::play()
{
  mIsPlaying = true;
  player->play();
}

void TTPhononWidget::stop()
{
  mIsPlaying = false;
  player->stop();
}

void TTPhononWidget::onPlayerFinished()
{
  mIsPlaying = false;
  emit playerFinished();
}

void TTPhononWidget::onStateChanged(Phonon::State state, Phonon::State)
{
  switch(state) {
    case Phonon::LoadingState:
    //qDebug("LoadingState");
    break;
    case Phonon::StoppedState:
    //qDebug("StoppedState");
    break;
    case Phonon::PlayingState:
    //qDebug("PlayingState");
    break;
    case Phonon::BufferingState:
    //qDebug("BufferingState");
    break;
    case Phonon::PausedState:
    //qDebug("PausedState");
    break;
    case Phonon::ErrorState:
    //qDebug("ErrorState");
    break;
  }
}


