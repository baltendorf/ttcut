/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttphononwidget.h                                                */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 11/24/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMEDIAPLAYERWIDGET
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
#include <QWidget>

#ifndef TTMEDIAPLAYERWIDGET_H
#define TTMEDIAPLAYERWIDGET_H

#include "ttvideoplayer.h"

#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>
#include <QResizeEvent>

class TTMediaPlayerWidget : public TTVideoPlayer
{
   Q_OBJECT
   Q_PROPERTY(bool getControlsVisible READ getControlsVisible WRITE setControlsVisible)

  public:
    TTMediaPlayerWidget(QWidget *parent);
    ~TTMediaPlayerWidget();
    void resizeEvent(QResizeEvent* e);

    void cleanUp();
    void load(const QString& value);
    void play();
    void stop();
    void seek(int seconds);

    bool isPlaying() const;

  public slots:
    void onPlayerFinished();
    void onStateChanged(QMediaPlayer::MediaStatus state);
    void onPositionChanged(qint64 progress);
    void onDurationChanged(qint64 duration);
    void onBufferingProgress(int progress);
    void onVideoAvailableChanged(bool value);
    void onPlaylistPositionChanged(int index);
    void onError();

  protected:
    void addMedia(const QString& value, bool select=true);
    void updateDurationInfo(qint64 currentInfo);

  protected:
    QMediaPlayer* player;
    QMediaPlaylist* playlist;
    QVideoWidget* videoWidget;
};

#endif
