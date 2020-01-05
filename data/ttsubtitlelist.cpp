/*----------------------------------------------------------------------------*/
/* COPYRIGHT: Minei3oat (c) 2019 / github.com/Minei3oat                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2019                                                      */
/* FILE     : ttsubtitlelistdata.cpp                                          */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : Minei3oat                                       DATE: 12/30/2019 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTSUBTITLELISTDATA
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

#include "ttsubtitlelist.h"
#include "ttavlist.h"
#include "avstream/ttavheader.h"
#include "avstream/ttsubtitleheaderlist.h"
#include "common/ttmessagelogger.h"
#include "avstream/ttavstream.h"

#include <QFileInfo>
#include <QString>

/* /////////////////////////////////////////////////////////////////////////////
 * Class TTSubtitleListDataItem
 */

/*!
 * TTSubtitleListDataItem
 * Default constructor
 */
TTSubtitleItem::TTSubtitleItem(TTAVItem* avDataItem, TTSubtitleStream* sStream)
{
  mpAVDataItem   = avDataItem;
  subtitleStream = sStream;
  mOrder         = -1;

  setItemData();
}

/*!
 * TTSubtitleListDataItem
 * Copy constructor
 */
TTSubtitleItem::TTSubtitleItem(const TTSubtitleItem& item)
{
  mpAVDataItem    = item.mpAVDataItem;
  mOrder          = item.mOrder;
  subtitleStream  = item.subtitleStream;
  subtitleLength  = item.subtitleLength;
  subtitleDelay   = item.subtitleDelay;
}

/*!
 * setItemData
 */
void TTSubtitleItem::setItemData()
{
  subtitleLength = QString("%1 (%2)")
                        .arg(subtitleStream->streamLengthTime().toString("hh:mm:ss.zzz"))
                        .arg(subtitleStream->headerList()->count());

  // FIXME: use real delay value for subtitle delay
  subtitleDelay  = "0";
}

/*!
 * operator =
 */
const TTSubtitleItem& TTSubtitleItem::operator=(const TTSubtitleItem& item)
{
  if (this == &item)
    return *this;

  mOrder          = item.mOrder;
  mpAVDataItem    = item.mpAVDataItem;
  subtitleStream  = item.subtitleStream;
  subtitleLength  = item.subtitleLength;
  subtitleDelay   = item.subtitleDelay;

  return *this;
}

/*!
 * operator <
 */
bool TTSubtitleItem::operator<(const TTSubtitleItem& item) const
{
  return mOrder < item.mOrder;
}

/*!
 * operator ==
 */
bool TTSubtitleItem::operator==(const TTSubtitleItem& item) const
{
  int check = 0;
  check += (subtitleStream  == item.subtitleStream)  ? 0 : 1;
  check += (mpAVDataItem    == item.mpAVDataItem)    ? 0 : 1;

  return (check == 0);
}

/*!
 * fileInfo
 */
QFileInfo TTSubtitleItem::getFileInfo() const
{
  return *subtitleStream->fileInfo();
}

/*!
 * subtitleStream
 */
TTSubtitleStream* TTSubtitleItem::getSubtitleStream() const
{
  return subtitleStream;
}

/*!
 * fileName
 */
QString TTSubtitleItem::getFileName() const
{
  return subtitleStream->fileName();
}

/*!
 * length
 */
QString TTSubtitleItem::getLength() const
{
  return subtitleLength;
}

/*!
 * delay
 */
QString TTSubtitleItem::getDelay() const
{
  return subtitleDelay;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Class TTSubtitleListData
 */

/*!
 * TTSubtitleListData
 */
TTSubtitleList::TTSubtitleList()
{
}

/*!
 * ~TTSubtitleListData
 */
TTSubtitleList::~TTSubtitleList()
{
  clear();
}

/*!
 * clear
 */
void TTSubtitleList::clear()
{
  for (int i = data.count()-1; i >= 0; i--) {
    TTSubtitleItem item = data.takeAt(i);
    //emit itemRemoved(item);
    emit itemRemoved(i);
    if (item.subtitleStream != 0) delete item.subtitleStream;
    item.subtitleStream = 0;
  }
}

/*!
 * append
 * Create and append an new TTSubtitleListDataItem to the list
 */
void TTSubtitleList::append(TTAVItem* avDataItem, TTSubtitleStream* sStream, int order)
{
  TTSubtitleItem item(avDataItem, sStream);

  item.setOrder(order);
  data.append(item);
  emit itemAppended(item);
}

/*!
 * append
 * Appends an existing TTSubtitleListDataItem to the list
 */
void TTSubtitleList::append(const TTSubtitleItem& sItem)
{
  TTSubtitleItem item(sItem);

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
void TTSubtitleList::remove(const TTSubtitleItem& item)
{
  int index = data.indexOf(item);
  TTSubtitleItem rItem = data.takeAt(index);
  //emit itemRemoved(rItem);
  emit itemRemoved(index);
}

/*!
 * update
 */
void TTSubtitleList::update(const TTSubtitleItem& sItem, const TTSubtitleItem& uItem)
{
  int index = data.indexOf(sItem); /*! index of item to update */
  data.replace(index, uItem);
  emit itemUpdated(sItem, uItem);
}

/*!
 * at
 */
const TTSubtitleItem& TTSubtitleList::at(int index)
{
  return data.at(index);
}

/*!
 * count
 */
int  TTSubtitleList::count()
{
  return data.count();
}

/*!
 * sortByOrder
 */
void TTSubtitleList::sortByOrder()
{
  qSort(data.begin(), data.end());
}

/*!
 * indexOf
 */
int TTSubtitleList::indexOf(const TTSubtitleItem& item)
{
  return data.indexOf(item);
}

/*!
 * swap
 */
void TTSubtitleList::swap(int a, int b)
{
  data.swap(a, b);
  emit itemsSwapped(a, b);
}

/*!
 * updateOrder
 */
void TTSubtitleList::updateOrder()
{
  for (int i = 0; i < data.count(); i++) {
    data[i].setOrder(i);
    emit orderUpdated(data.at(i), i);
  }
}

/*!
 * onAppendItem
 */
void TTSubtitleList::onAppendItem(const TTSubtitleItem& item)
{
  append(item);
}

/*!
 * onRemoveItem
 */
void TTSubtitleList::onRemoveItem(const TTSubtitleItem& item)
{
  remove(item);
}

/*!
 * onUpdateItem
 */
void TTSubtitleList::onUpdateItem(const TTSubtitleItem& sItem, const TTSubtitleItem& uItem)
{
  update(sItem, uItem);
}

/*!
 * onUpdateOrder
 */
void TTSubtitleList::onUpdateOrder(const TTSubtitleItem& item, int order)
{
  int index = data.indexOf(item);
  if (index < 0) return;

  data[index].setOrder(order);

  emit orderUpdated(data.at(index), order);
}

//! Request to refresh the list data
void TTSubtitleList::onRefreshData(TTAVItem*)
{
  qDebug("TTSubtitleList::onRefreshData");

  emit dataChanged();
}

