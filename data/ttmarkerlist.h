/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmarkerlist.h                                                  */
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

#ifndef TTMARKERLIST_H
#define TTMARKERLIST_H

#include <QList>
#include <QUuid>
#include <QTime>
#include <QString>
#include <QFileInfo>

class TTAVItem;

/* //////////////////////////////////////////////////////////////////////////////
 * TTCutListDataItem
 */
class TTMarkerItem
{
  friend class TTMarkerList;

  public:
    TTMarkerItem(TTAVItem* avDataItem, int markerPos);
    TTMarkerItem(const TTMarkerItem& item);

    QUuid ID() const { return mID; }

    void  update(int markerPos);
    int   order()             const { return mOrder; }
    void  setOrder(int value)       { mOrder = value; }
    int   markerPos()         const { return mMarkerPos; }

    QString fileName() const;
    QTime   markerTime() const;
    QString markerPosString() const;
    QString markerTimeString() const;

    TTAVItem* avDataItem() const;

    const TTMarkerItem& operator=(const TTMarkerItem& item);
    bool operator==(const TTMarkerItem& item) const;
    bool operator<(const TTMarkerItem& item) const;

  private:
  	QUuid     mID;
    int       mOrder;
    int       mMarkerPos;
    TTAVItem* mpAVDataItem;
};

/* //////////////////////////////////////////////////////////////////////////////
 * TTMarkerList
 */
class TTMarkerList : public QObject
{
  Q_OBJECT

  public:
    TTMarkerList();
    ~TTMarkerList();

    void append(TTAVItem* avItem, int markerPos, int order=-1);
    void append(const TTMarkerItem& item);
    void remove(const TTMarkerItem& item);
    void update(const TTMarkerItem& cItem, const TTMarkerItem& uItem);

    const TTMarkerItem& at(int index);

    void clear();
    int  count();
    int  indexOf(const TTMarkerItem& item);

    void swap(int a, int b);
    void sortByOrder();
    void updateOrder();

  public slots:
		void onRefreshData(TTAVItem* avItem);
    void onAppendItem(const TTMarkerItem& item);
    void onRemoveItem(const TTMarkerItem& item);
    void onUpdateItem(const TTMarkerItem& cItem, const TTMarkerItem& uItem);
    void onUpdateOrder(const TTMarkerItem& item, int order);

  signals:
		void dataChanged();
    void itemAppended(const TTMarkerItem& item);
    void itemRemoved(const TTMarkerItem& item);
    void itemRemoved(int index);
    void itemUpdated(const TTMarkerItem& cItem, const TTMarkerItem& uItem);
    void orderUpdated(const TTMarkerItem& item, int order);

  private:
    QList<TTMarkerItem> data;
};
#endif
