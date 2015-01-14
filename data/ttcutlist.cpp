/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutlist.cpp                                                   */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTLIST
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

#include "ttcutlist.h"
#include "ttavlist.h"

#include "common/ttcut.h"
#include "avstream/ttavheader.h"
#include "avstream/ttavstream.h"
#include "avstream/ttmpeg2videostream.h"

#include <QDateTime>
#include <QFileInfo>
#include <QString>

/*!
 * TTCutItem
 */
TTCutItem::TTCutItem(TTAVItem* avDataItem, int cutIn, int cutOut)
{
  mpAVDataItem = avDataItem;
  mCutInIndex  = cutIn;
  mCutOutIndex = cutOut;

  mID = QUuid::createUuid();
}

/*!
 * TTCutItem
 * Copy constructor
 */
TTCutItem::TTCutItem(const TTCutItem& item)
{
	mID          = item.mID;
  mCutInIndex  = item.mCutInIndex;
  mCutOutIndex = item.mCutOutIndex;
  mpAVDataItem = item.mpAVDataItem;
  mOrder       = item.mOrder;
}

/*!
 * update
 */
void TTCutItem::update(int cutIn, int cutOut)
{
  mCutInIndex  = cutIn;
  mCutOutIndex = cutOut;
}

/*!
 * operator =
 */
const TTCutItem& TTCutItem::operator=(const TTCutItem& item)
{
  if (this == &item) return *this;

  mID          = item.mID;
  mOrder       = item.mOrder;
  mCutInIndex  = item.mCutInIndex;
  mCutOutIndex = item.mCutOutIndex;
  mpAVDataItem = item.mpAVDataItem;

  return *this;
}

/*!
 * operator <
 */
bool TTCutItem::operator<(const TTCutItem& item) const
{
  return (mOrder < item.mOrder);
}

/*!
 * Equal operator for data item
 */
bool TTCutItem::operator==(const TTCutItem& item) const
{
  return (mID == item.mID);
}

/*!
 * cutInIndex
 */
int TTCutItem::cutInIndex() const
{
  return mCutInIndex;
}

/*!
 * cutOutIndex
 */
int TTCutItem::cutOutIndex() const
{
  return mCutOutIndex;
}

/*!
 * avDataItem
 */
TTAVItem* TTCutItem::avDataItem() const
{
  return mpAVDataItem;
}

/*!
 * cutInFrameType
 */
int TTCutItem::cutInFrameType()const
{
  return (avDataItem()->videoStream() != 0)
      ? avDataItem()->videoStream()->frameType(mCutInIndex)
      : -1;
}

/*!
 * cutOutFrameType
 */
int TTCutItem::cutOutFrameType() const
{
  return (avDataItem()->videoStream() != 0)
      ? avDataItem()->videoStream()->frameType(mCutOutIndex)
      : -1;
}

/*!
 * cutInTime
 */
QTime TTCutItem::cutInTime() const
{
  return (avDataItem()->videoStream() != 0)
      ? avDataItem()->videoStream()->frameTime(mCutInIndex)
      : QTime();
}

/*!
 * cutOutTime
 */
QTime TTCutItem::cutOutTime() const
{
  return (avDataItem()->videoStream() != 0)
      ? avDataItem()->videoStream()->frameTime(mCutOutIndex)
      : QTime();
}

/*!
 * cutLengthFrames
 */
int TTCutItem::cutLengthFrames() const
{
  return (mCutOutIndex > mCutInIndex) ? mCutOutIndex - mCutInIndex + 1 : mCutInIndex - mCutOutIndex + 1;
}

/*!
 * cutLengthTime
 */
QTime TTCutItem::cutLengthTime() const
{
  return (avDataItem()->videoStream() != 0)
      ? ttFramesToTime(cutLengthFrames(), avDataItem()->videoStream()->frameRate())
      : QTime();
}

/*!
 * cutLengthBytes
 */
quint64 TTCutItem::cutLengthBytes() const
{
  if (avDataItem()->videoStream() == 0) return 0;

  qint64 length = avDataItem()->videoStream()->frameOffset(mCutOutIndex) -
                  avDataItem()->videoStream()->frameOffset(mCutInIndex);

  return (length > 0) ? length : (-1)*length;
}

/*!
 * fileName
 */
QString TTCutItem::fileName() const
{
  return (avDataItem()->videoStream() != 0)
    ? avDataItem()->videoStream()->fileName()
    : "";
}

/*!
 * cutInString
 */
QString TTCutItem::cutInString() const
{
  return QString("%1 (%2)").
    arg(cutInTime().toString("hh:mm:ss")).
    arg(mCutInIndex);
}

/*!
 * cutOutString
 */
QString TTCutItem::cutOutString() const
{
  return QString("%1 (%2)").
    arg(cutOutTime().toString("hh:mm:ss")).
    arg(mCutOutIndex);
}

/*!
 * cutLengthString
 */
QString TTCutItem::cutLengthString() const
{
  return QString("%1 (%2 MB)").
    arg(cutLengthTime().toString("hh:mm:ss")).
    arg(cutLengthBytes()/1024.0/1024.0);
}



/* /////////////////////////////////////////////////////////////////////////////
 * TTCutListData
 */

/*!
 * TTCutList
 */
TTCutList::TTCutList()
{
}

/*!
 * ~TTCutList
 */
TTCutList::~TTCutList()
{
  clear();
}

/*!
 * clear
 */
void TTCutList::clear()
{
  for (int i = data.count()-1; i >= 0; i--) {
    TTCutItem item = data.takeAt(i);
    emit itemRemoved(item);
    emit itemRemoved(i);
  }
  data.clear();
}

/*!
 * append
 */
void TTCutList::append(TTAVItem* avItem, int cutIn, int cutOut, int order)
{
  TTCutItem item(avItem, cutIn, cutOut);

  item.setOrder(order);
  data.append(item);
  emit itemAppended(item);
}

/*!
 * append
 */
void TTCutList::append(const TTCutItem& cItem)
{
  TTCutItem item(cItem);

  if (item.order() < 0) {
    item.setOrder(count());
    emit orderUpdated(item, item.order());
  }

  data.append(item);


  emit itemAppended(item);
}

/*!
 * remove
 */
void TTCutList::remove(const TTCutItem& cItem)
{
  int       index = data.indexOf(cItem);
  TTCutItem item  = data.takeAt(index);

  emit itemRemoved(item);
  emit itemRemoved(index);
}

/*!
 * update
 */
void TTCutList::update(const TTCutItem& cItem, const TTCutItem& uItem)
{
  int index = data.indexOf(cItem);

  if (index < 0) {
    //qDebug("TTCutList::update error in updata cut entry!");
    return;
  }

  data[index].mCutInIndex  = uItem.cutInIndex();
  data[index].mCutOutIndex = uItem.cutOutIndex();

  emit itemUpdated(data[index], uItem);
}

/*!
 * at
 */
const TTCutItem& TTCutList::at(int index)
{
  return data.at(index);
}

/*!
 * indexOf
 */
int TTCutList::indexOf(const TTCutItem& item)
{
  return data.indexOf(item);
}

/*!
 * count
 */
int TTCutList::count()
{
  return data.count();
}

/*!
 * swap
 */
void TTCutList::swap(int a, int b)
{
  data.swap(a, b);
  updateOrder();
}

/*!
 * updateOrder
 */
void TTCutList::updateOrder()
{
  for (int i = 0; i < data.count(); i++) {
    data[i].setOrder(i);
    emit orderUpdated(data.at(i), i);
  }
}

/*!
 * onAppendItem
 */
void TTCutList::onAppendItem(const TTCutItem& item)
{
	append(item);
}

/*!
 * onRemoveItem
 */
void TTCutList::onRemoveItem(const TTCutItem& item)
{
  remove(item);
  updateOrder();
}

/*!
 * onUpdateItem
 */
void TTCutList::onUpdateItem(const TTCutItem& cItem, const TTCutItem& uItem)
{
  update(cItem, uItem);
}

/*!
 * onUpdateOrder
 */
void TTCutList::onUpdateOrder(const TTCutItem& item, int order)
{
  int index = data.indexOf(item);

  if (index < 0) {
  	//qDebug("TTCutList::onUpdateOrder item not in list! %s", qPrintable(item.fileName()));
  	return;
  }

  data[index].setOrder(order);

  emit orderUpdated(data.at(index), order);
}

//! Refresh data request for list items
void TTCutList::onRefreshData(TTAVItem*)
{
	qDebug("TTCutList::onRefreshData");

	emit dataChanged();
}

/*!
 * sortByOrder
 */
void TTCutList::sortByOrder()
{
  qSort(data.begin(), data.end());
}
