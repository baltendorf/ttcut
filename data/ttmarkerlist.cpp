/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmarkerlist.cpp                                                */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 12/18/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMARKERLIST
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

#include "ttmarkerlist.h"
#include "ttavlist.h"

#include "../common/ttcut.h"
#include "../avstream/ttavheader.h"
#include "../avstream/ttavstream.h"
#include "../avstream/ttmpeg2videostream.h"

#include <QDateTime>
#include <QFileInfo>
#include <QString>

/*!
 * TTMarkerItem
 */
TTMarkerItem::TTMarkerItem(TTAVItem* avDataItem, int markerPos)
{
  mpAVDataItem = avDataItem;
  mMarkerPos   = markerPos;
  mOrder       = -1;

  mID = QUuid::createUuid();
}

/*!
 * TTMarkerItem
 * Copy constructor
 */
TTMarkerItem::TTMarkerItem(const TTMarkerItem& item)
{
	mID          = item.mID;
	mMarkerPos   = item.mMarkerPos;
  mpAVDataItem = item.mpAVDataItem;
  mOrder       = item.mOrder;
}

/*!
 * update
 */
void TTMarkerItem::update(int markerPos)
{
	mMarkerPos = markerPos;
}

QString TTMarkerItem::fileName() const
{
	return (mpAVDataItem->videoStream() != 0)
			? mpAVDataItem->videoStream()->fileName()
			: "";
}

QString TTMarkerItem::markerPosString() const
{
	return QString("%1").arg(mMarkerPos);
}

QString TTMarkerItem::markerTimeString() const
{
	return markerTime().toString("hh:mm:ss.zzz");
}

TTAVItem* TTMarkerItem::avDataItem() const
{
	return mpAVDataItem;
}

QTime TTMarkerItem::markerTime() const
{
	return (mpAVDataItem->videoStream() != 0)
			? mpAVDataItem->videoStream()->frameTime(mMarkerPos)
			: QTime();
}

/*!
 * operator =
 */
const TTMarkerItem& TTMarkerItem::operator=(const TTMarkerItem& item)
{
  if (this == &item)
    return *this;

  mID          = item.mID;
  mOrder       = item.mOrder;
  mMarkerPos   = item.mMarkerPos;
  mpAVDataItem = item.mpAVDataItem;

  return *this;
}

/*!
 * operator <
 */
bool TTMarkerItem::operator<(const TTMarkerItem& item) const
{
  return (mOrder < item.mOrder);
}

/*!
 * Equal operator for data item
 */
bool TTMarkerItem::operator==(const TTMarkerItem& item) const
{
  return (mID == item.mID);
}

/* /////////////////////////////////////////////////////////////////////////////
 * TTMarkerList
 */

/*!
 * TTMarkerList
 */
TTMarkerList::TTMarkerList()
{
}

/*!
 * ~TTMarkerList
 */
TTMarkerList::~TTMarkerList()
{
  clear();
}

/*!
 * clear
 */
void TTMarkerList::clear()
{
  for (int i = data.count()-1; i >= 0; i--) {
    TTMarkerItem item = data.takeAt(i);
    emit itemRemoved(item);
    emit itemRemoved(i);
  }
  data.clear();
}

/*!
 * append
 */
void TTMarkerList::append(TTAVItem* avItem, int markerPos, int order)
{
  TTMarkerItem item(avItem, markerPos);

  item.setOrder(order);
  data.append(item);
  emit itemAppended(item);
}

/*!
 * append
 */
void TTMarkerList::append(const TTMarkerItem& cItem)
{
  TTMarkerItem item(cItem);

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
void TTMarkerList::remove(const TTMarkerItem& cItem)
{
  int       index = data.indexOf(cItem);
  TTMarkerItem item  = data.takeAt(index);

  emit itemRemoved(item);
  emit itemRemoved(index);
}

/*!
 * update
 */
void TTMarkerList::update(const TTMarkerItem& cItem, const TTMarkerItem& uItem)
{
  int index = data.indexOf(cItem);

  if (index < 0) {
    //qDebug("TTMarkerList::update error in updata cut entry!");
    return;
  }

  data[index].mMarkerPos = uItem.markerPos();

  emit itemUpdated(data[index], uItem);
}

/*!
 * at
 */
const TTMarkerItem& TTMarkerList::at(int index)
{
  return data.at(index);
}

/*!
 * indexOf
 */
int TTMarkerList::indexOf(const TTMarkerItem& item)
{
  return data.indexOf(item);
}

/*!
 * count
 */
int TTMarkerList::count()
{
  return data.count();
}

/*!
 * swap
 */
void TTMarkerList::swap(int a, int b)
{
  data.swap(a, b);
  updateOrder();
}

/*!
 * updateOrder
 */
void TTMarkerList::updateOrder()
{
  for (int i = 0; i < data.count(); i++) {
    data[i].setOrder(i);
    emit orderUpdated(data.at(i), i);
  }
}

/*!
 * onAppendItem
 */
void TTMarkerList::onAppendItem(const TTMarkerItem& item)
{
	append(item);
}

/*!
 * onRemoveItem
 */
void TTMarkerList::onRemoveItem(const TTMarkerItem& item)
{
  remove(item);
  updateOrder();
}

/*!
 * onUpdateItem
 */
void TTMarkerList::onUpdateItem(const TTMarkerItem& cItem, const TTMarkerItem& uItem)
{
  update(cItem, uItem);
}

/*!
 * onUpdateOrder
 */
void TTMarkerList::onUpdateOrder(const TTMarkerItem& item, int order)
{
  int index = data.indexOf(item);

  if (index < 0) {
  	//qDebug("TTMarkerList::onUpdateOrder item not in list! %s", qPrintable(item.fileName()));
  	return;
  }

  data[index].setOrder(order);

  emit orderUpdated(data.at(index), order);
}

void TTMarkerList::onRefreshData(TTAVItem*)
{
	qDebug("TTMarkerList:onRefreshData");

	emit dataChanged();
}

/*!
 * sortByOrder
 */
void TTMarkerList::sortByOrder()
{
  qSort(data.begin(), data.end());
}
