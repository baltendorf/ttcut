/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttphononwidget.h                                                */
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

#include <QObject>
#include <QtGui/QWidget>

#ifndef TTPHONONWIDGET_H
#define TTPHONONWIDGET_H

#include <videoplayer.h>
#include "ttvideoplayer.h"
#include <QResizeEvent>

class TTPhononWidget : public TTVideoPlayer
{
   Q_OBJECT
   Q_PROPERTY(bool getControlsVisible READ getControlsVisible WRITE setControlsVisible)

  public:
    TTPhononWidget(QWidget *parent);
    ~TTPhononWidget();
    void resizeEvent(QResizeEvent* e);

    void cleanUp();
    void load(QString value);
    void play();
    void stop();

  public slots:
    void onPlayerFinished();
    void onStateChanged(Phonon::State newState, Phonon::State oldState);

  protected:
    Phonon::VideoPlayer* player;
};

#endif
