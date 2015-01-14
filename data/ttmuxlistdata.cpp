/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttmuxlistdata.cpp                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/11/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMUXLISTDATA
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

#include "ttmuxlistdata.h"

#include "avstream/ttavheader.h"

#include <QFileInfo>
#include <QString>
#include <QStringList>

/*! ////////////////////////////////////////////////////////////////////////////
 * TTMuxListItem class
 */

//! This class represent an item in the list
TTMuxListDataItem::TTMuxListDataItem()
{
}

TTMuxListDataItem::TTMuxListDataItem(const TTMuxListDataItem& item)
{
  this->videoFileName = item.videoFileName;

  this->audioFileNames.clear();

  for(int i=0; i<item.audioFileNames.count(); i++)
    this->audioFileNames.append(item.audioFileNames[i]);
}

const TTMuxListDataItem& TTMuxListDataItem::operator=(const TTMuxListDataItem& item)
{
  if (this == &item) return *this;

  this->videoFileName = item.videoFileName;

  this->audioFileNames.clear();

  for(int i=0; i<item.audioFileNames.count(); i++)
    this->audioFileNames.append(item.audioFileNames[i]);

  return *this;
}


TTMuxListDataItem::TTMuxListDataItem(QString video, QStringList audio)
{
  videoFileName  = video;
  audioFileNames = audio;
}

QString TTMuxListDataItem::getVideoName()
{
  return videoFileName;
}

void TTMuxListDataItem::setVideoName(QString videoFileName)
{
  this->videoFileName = videoFileName;
}

QStringList TTMuxListDataItem::getAudioNames()
{
  return audioFileNames;
}

void TTMuxListDataItem::appendAudioFile(QString audioFileName)
{
  audioFileNames.append(audioFileName);
}

/*! ////////////////////////////////////////////////////////////////////////////
 * TTMuxList Container
 */

//! Construct the audio list data object
TTMuxListData::TTMuxListData()
{
  log = TTMessageLogger::getInstance();
}

//! Destruct object
TTMuxListData::~TTMuxListData()
{
  data.clear();
}

void TTMuxListData::appendItem(const TTMuxListDataItem& item)
{
  data.append(item);
}

int TTMuxListData::addItem(QString video)
{
  TTMuxListDataItem item = createMuxListItem(video);

  item.audioFileNames.clear();

  return data.count()-1;
}

int TTMuxListData::addItem(QString video, QString audio)
{
  TTMuxListDataItem item = createMuxListItem(video);

  item.audioFileNames.clear();
  item.audioFileNames.append(audio);

  return data.count()-1;
}

//! Add item to list
int TTMuxListData::addItem(QString video, QStringList audio)
{
  TTMuxListDataItem item = createMuxListItem(video);

  item.audioFileNames.clear();
  item.audioFileNames = audio;

  return data.count()-1;
}

//!
TTMuxListDataItem TTMuxListData::createMuxListItem(QString videoFilePath)
{
  for (int i=0; i< data.count(); i++) {
    if (data[i].videoFileName == videoFilePath)
      return data[i];
  }

  TTMuxListDataItem item;
  item.videoFileName = videoFilePath;

  data.append(item);

  return item;
}

void TTMuxListData::appendAudioName(int index, QString audio)
{
  data[index].audioFileNames.append(audio);
}

QString TTMuxListData::videoFilePathAt(int index)
{
  return data[index].videoFileName;
}

//! Returns the audio file-paths string list
QStringList TTMuxListData::audioFilePathsAt(int index)
{
  return data[index].audioFileNames;
}

//! Returns the data item at position index
TTMuxListDataItem& TTMuxListData::itemAt(int index)
{
  return data[index];
}

//! Returns the number of entries in list
int  TTMuxListData::count()
{
  return data.count();
}

//! Delete all item from list
void TTMuxListData::deleteAll()
{
  data.clear();
}

//! Remove item at position index from list
void TTMuxListData::removeAt(int index)
{
  data.removeAt(index);
}

//! Print the data list for debug purpose
void TTMuxListData::print()
{
  log->infoMsg(__FILE__, __LINE__, "mux-list data:");

  for(int i=0; i < data.count(); i++) {
    log->infoMsg(__FILE__, __LINE__, "--------------------------------");
    log->infoMsg(__FILE__, __LINE__, QString("video-file: %1").arg(data[i].getVideoName()));
    QStringList audioNames = data[i].getAudioNames();
    for (int j=0; j < audioNames.size(); j++) {
      log->infoMsg(__FILE__, __LINE__, QString("audio-file: %1").arg(audioNames.at(j)));
    }
    log->infoMsg(__FILE__, __LINE__, "--------------------------------");
  }
}

