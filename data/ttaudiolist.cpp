/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttaudiolistdata.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAUDIOLISTDATA
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

#include "ttaudiolist.h"
#include "ttavlist.h"
#include "../avstream/ttavheader.h"
#include "../common/ttmessagelogger.h"
#include "../avstream/ttavstream.h"

#include <QFileInfo>
#include <QString>

/* /////////////////////////////////////////////////////////////////////////////
 * Class TTAudioListDataItem
 */

/*!
 * TTAudioListDataItem
 * Default constructor
 */
TTAudioItem::TTAudioItem(TTAVItem* avDataItem, TTAudioStream* aStream)
{
  mpAVDataItem  = avDataItem;
  audioStream   = aStream;
  mOrder        = -1;

  setItemData();
}

/*!
 * TTAudioListDataItem
 * Copy constructor
 */
TTAudioItem::TTAudioItem(const TTAudioItem& item)
{
  mpAVDataItem = item.mpAVDataItem;
  mOrder       = item.mOrder;
  audioStream  = item.audioStream;
  audioLength  = item.audioLength;
  audioDelay   = item.audioDelay;
}

/*!
 * setItemData
 */
void TTAudioItem::setItemData()
{
  audioLength = QString("%1 (%2 MB)")
  			.arg(audioStream->streamLengthTime().toString("hh:mm:ss.zzz"))
  			.arg((double)audioStream->streamLengthByte()/1024.0/1024.0);

  // FIXME: use real delay value for audio delay
  audioDelay  = "0";
}

/*!
 * operator =
 */
const TTAudioItem& TTAudioItem::operator=(const TTAudioItem& item)
{
  if (this == &item)
    return *this;

  mOrder       = item.mOrder;
  mpAVDataItem = item.mpAVDataItem;
  audioStream  = item.audioStream;
  audioLength  = item.audioLength;
  audioDelay   = item.audioDelay;

  return *this;
}

/*!
 * operator <
 */
bool TTAudioItem::operator<(const TTAudioItem& item) const
{
  return mOrder < item.mOrder;
}

/*!
 * operator ==
 */
bool TTAudioItem::operator==(const TTAudioItem& item) const
{
  int check = 0;
  check += (audioStream  == item.audioStream)  ? 0 : 1;
  check += (mpAVDataItem == item.mpAVDataItem) ? 0 : 1;

  return (check == 0);
}

/*!
 * fileInfo
 */
QFileInfo TTAudioItem::getFileInfo() const
{
  return *audioStream->fileInfo();
}

/*!
 * audioStream
 */
TTAudioStream* TTAudioItem::getAudioStream() const
{
  return audioStream;
}

/*!
 * fileName
 */
QString TTAudioItem::getFileName() const
{
  return audioStream->fileName();
}

/*!
 * length
 */
QString TTAudioItem::getLength() const
{
  return audioLength;
}

/*!
 * version
 */
QString TTAudioItem::getVersion() const
{
  return audioStream->headerAt(0)->descString();
}

/*!
 * mode
 */
QString TTAudioItem::getMode() const
{
  return audioStream->headerAt(0)->modeString();
}

/*!
 * bitrate
 */
QString TTAudioItem::getBitrate() const
{
  return audioStream->headerAt(0)->bitRateString();
}

/*!
 * samplerate
 */
QString TTAudioItem::getSamplerate() const
{
  return audioStream->headerAt(0)->sampleRateString();
}

/*!
 * delay
 */
QString TTAudioItem::getDelay() const
{
  return audioDelay;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Class TTAudioListData
 */

/*!
 * TTAudioListData
 */
TTAudioList::TTAudioList()
{
}

/*!
 * ~TTAudioListData
 */
TTAudioList::~TTAudioList()
{
  clear();
}

/*!
 * clear
 */
void TTAudioList::clear()
{
  for (int i = data.count()-1; i >= 0; i--) {
    TTAudioItem item = data.takeAt(i);
    //emit itemRemoved(item);
    emit itemRemoved(i);
    if (item.audioStream != 0) delete item.audioStream;
    item.audioStream = 0;
  }
}

/*!
 * append
 * Create and append an new TTAudioListDataItem to the list
 */
void TTAudioList::append(TTAVItem* avDataItem, TTAudioStream* aStream, int order)
{
  TTAudioItem item(avDataItem, aStream);

  item.setOrder(order);
  data.append(item);
  emit itemAppended(item);
}

/*!
 * append
 * Appends an existing TTAudioListDataItem to the list
 */
void TTAudioList::append(const TTAudioItem& aItem)
{
  TTAudioItem item(aItem);

  if (item.order() < 0) {
    item.setOrder(data.count());
    emit orderUpdated(item, item.order());
  }

  data.append(item);
  emit itemAppended(item);
}

/*!
 * remove
 * Removes the specified item from the list
 */
void TTAudioList::remove(const TTAudioItem& item)
{
  int index = data.indexOf(item);
  TTAudioItem rItem = data.takeAt(index);
  //emit itemRemoved(rItem);
  emit itemRemoved(index);
}

/*!
 * update
 */
void TTAudioList::update(const TTAudioItem& aItem, const TTAudioItem& uItem)
{
  int index = data.indexOf(aItem); /*! index of item to update */
  data.replace(index, uItem);
  emit itemUpdated(aItem, uItem);
}

/*!
 * at
 */
const TTAudioItem& TTAudioList::at(int index)
{
  return data.at(index);
}

/*!
 * count
 */
int  TTAudioList::count()
{
  return data.count();
}

/*!
 * sortByOrder
 */
void TTAudioList::sortByOrder()
{
  qSort(data.begin(), data.end());
}

/*!
 * indexOf
 */
int TTAudioList::indexOf(const TTAudioItem& item)
{
  return data.indexOf(item);
}

/*!
 * swap
 */
void TTAudioList::swap(int a, int b)
{
  data.swap(a, b);
  emit itemsSwapped(a, b);
}

/*!
 * updateOrder
 */
void TTAudioList::updateOrder()
{
  for (int i = 0; i < data.count(); i++) {
    data[i].setOrder(i);
    emit orderUpdated(data.at(i), i);
  }
}

/*!
 * onAppendItem
 */
void TTAudioList::onAppendItem(const TTAudioItem& item)
{
  append(item);
}

/*!
 * onRemoveItem
 */
void TTAudioList::onRemoveItem(const TTAudioItem& item)
{
  remove(item);
}

/*!
 * onUpdateItem
 */
void TTAudioList::onUpdateItem(const TTAudioItem& aItem, const TTAudioItem& uItem)
{
  update(aItem, uItem);
}

/*!
 * onUpdateOrder
 */
void TTAudioList::onUpdateOrder(const TTAudioItem& item, int order)
{
  int index = data.indexOf(item);
  if (index < 0) return;

  data[index].setOrder(order);

  emit orderUpdated(data.at(index), order);
}

//! Request to refresh the list data
void TTAudioList::onRefreshData(TTAVItem*)
{
	qDebug("TTAudioList::onRefreshData");

	emit dataChanged();
}

