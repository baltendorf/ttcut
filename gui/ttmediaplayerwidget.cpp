/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttphononwidget.cpp                                              */
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

#include <gui/ttmediaplayerwidget.h>

#include <QTime>

TTMediaPlayerWidget::TTMediaPlayerWidget(QWidget *parent)
  : TTVideoPlayer(parent)
{
  player = new QMediaPlayer(this);
  videoWidget = new QVideoWidget(parent);

	playlist = new QMediaPlaylist();
	playlist->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
  player->setVideoOutput(videoWidget);
  player->setPlaylist(playlist);

  videoWidget->show();

  connect(playlist, SIGNAL(currentIndexChanged(int)), SLOT(onPlaylistPositionChanged(int)));

  connect(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), SLOT(onStateChanged(QMediaPlayer::MediaStatus)));
  connect(player, SIGNAL(positionChanged(qint64)),     SLOT(onPositionChanged(qint64)));
  connect(player, SIGNAL(durationChanged(qint64)),     SLOT(onDurationChanged(qint64)));
  connect(player, SIGNAL(bufferStatusChanged(int)),    SLOT(onBufferingProgress(int)));
  connect(player, SIGNAL(videoAvailableChanged(bool)), SLOT(onVideoAvailableChanged(bool)));
  connect(player, SIGNAL(error(QMediaPlayer::Error)),  SLOT(onError()));
}

TTMediaPlayerWidget::~TTMediaPlayerWidget()
{
  if (player != 0) {
    cleanUp();
  }
}

void TTMediaPlayerWidget::resizeEvent(QResizeEvent* e)
{
	videoWidget->resize(e->size().width(), e->size().height());
}

bool TTMediaPlayerWidget::isPlaying() const
{
	return player->state() == QMediaPlayer::PlayingState;
}

void TTMediaPlayerWidget::cleanUp()
{

}

void TTMediaPlayerWidget::load(const QString& value)
{
  if (player->state() != QMediaPlayer::StoppedState)
  {
  	player->stop();
  }

  addMedia(value);
}

void TTMediaPlayerWidget::play()
{
  player->play();
}

void TTMediaPlayerWidget::stop()
{
  //player->stop();
  player->pause();
}

void TTMediaPlayerWidget::onPlayerFinished()
{
  emit playerFinished();
}

void TTMediaPlayerWidget::onStateChanged(QMediaPlayer::MediaStatus state)
{
  switch (state)
  {
  	case QMediaPlayer::UnknownMediaStatus:
  		qDebug("unknown");
  		break;
  	case QMediaPlayer::NoMedia:
  		qDebug("no media");
  		break;
  	case QMediaPlayer::LoadedMedia:
  		//qDebug("loaded");
  		break;
  	case QMediaPlayer::BufferingMedia:
  		qDebug("buffering");
  		break;
  	case QMediaPlayer::BufferedMedia:
  		//qDebug("buffered");
  		break;
  	case QMediaPlayer::LoadingMedia:
      //qDebug("Loading...");
      break;
  	case QMediaPlayer::StalledMedia:
  	  qDebug("stalled media");
      break;
  	case QMediaPlayer::EndOfMedia:
      qDebug("end of media");
      break;
  	case QMediaPlayer::InvalidMedia:
      qDebug("invalid media");
      break;
  }
}

void TTMediaPlayerWidget::onPositionChanged(qint64 progress)
{
    updateDurationInfo(progress / 1000);
}

void TTMediaPlayerWidget::onDurationChanged(qint64 /*duration*/)
{
	updateDurationInfo(0);
}

void TTMediaPlayerWidget::onBufferingProgress(int progress)
{
	qDebug("buffering %d", progress);
}

void TTMediaPlayerWidget::onVideoAvailableChanged(bool /*value*/)
{

}

void TTMediaPlayerWidget::onError()
{

}

void TTMediaPlayerWidget::onPlaylistPositionChanged(int /*index*/)
{

}

void TTMediaPlayerWidget::addMedia(const QString& value, bool select)
{
  QUrl url = QUrl::fromLocalFile(value);

  for (int index = 0; index < playlist->mediaCount(); index++)
  {
  	if (playlist->media(index).canonicalUrl() == url)
  	{
  		if (select)
  		{
  			playlist->setCurrentIndex(index);
  			player->setPosition(0);
  			player->pause();
  		}
  		return;
  	}
  }

  playlist->addMedia(url);
	playlist->setCurrentIndex(playlist->mediaCount()-1);
	player->setPosition(0);
	player->pause();
}

void TTMediaPlayerWidget::seek(int seconds)
{
    player->setPosition(seconds * 1000);
}

void TTMediaPlayerWidget::updateDurationInfo(qint64 currentInfo)
{
		qint64 duration = player->duration() / 1000;

    QString tStr;
    if (currentInfo || duration) {
        QTime currentTime((currentInfo/3600)%60, (currentInfo/60)%60, currentInfo%60, (currentInfo*1000)%1000);
        QTime totalTime((duration/3600)%60, (duration/60)%60, duration%60, (duration*1000)%1000);
        QString format = "mm:ss";
        if (duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }

    //qDebug("current duration: %s", tStr.toStdString().c_str());
}

