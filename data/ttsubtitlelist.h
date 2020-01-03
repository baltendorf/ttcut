/*----------------------------------------------------------------------------*/
/* COPYRIGHT: Minei3oat (c) 2019 / github.com/Minei3oat                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2019                                                      */
/* FILE     : ttsubtitlelistdata.h                                            */
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

#ifndef TTSUBTITLELISTDATA_H
#define TTSUBTITLELISTDATA_H


#include <QList>
#include <QString>
#include <QFileInfo>

class TTAVItem;
class TTSubtitleStream;

/* ///////////////////////////////////////////////////////////////////////////////////////
 * TTAudioListDataItem
 */
class TTSubtitleItem
{
  friend class TTSubtitleList;

  public:
    TTSubtitleItem(TTAVItem* avDataItem, TTSubtitleStream* sStream);
    TTSubtitleItem(const TTSubtitleItem& item);

    int   order()             const { return mOrder; }
    void  setOrder(int value)       { mOrder = value; }

    QFileInfo         getFileInfo() const;
    TTSubtitleStream* getSubtitleStream() const;
    QString           getFileName() const;
    QString           getLength() const;
    QString           getDelay() const;
    TTAVItem*         avDataItem() const {return mpAVDataItem;}

    const TTSubtitleItem& operator= (const TTSubtitleItem& item);
    bool                  operator< (const TTSubtitleItem& item) const;
    bool                  operator==(const TTSubtitleItem& item) const;

  private:
    void setItemData();

  private:
    TTAVItem*         mpAVDataItem;
    TTSubtitleStream* subtitleStream;
    int               mOrder;
    QString           subtitleLength;
    QString           subtitleDelay;
};

/* /////////////////////////////////////////////////////////////////////////////
 * TTAudioListData
 */
class TTSubtitleList : public QObject
{
  Q_OBJECT

  public:
    TTSubtitleList();
    ~TTSubtitleList();

    void append(TTAVItem* avDataItem, TTSubtitleStream* sStream, int order=-1);
    void append(const TTSubtitleItem& item);
    void remove(const TTSubtitleItem& item);
    void update(const TTSubtitleItem& cItem, const TTSubtitleItem& uItem);

    const TTSubtitleItem& at(int index);

    int   count();
    void  clear();
    void  sortByOrder();
    int   indexOf(const TTSubtitleItem& item);
    void  swap(int a, int b);
    void  print();
    void  updateOrder();

  public slots:
    void onAppendItem(const TTSubtitleItem& item);
    void onRemoveItem(const TTSubtitleItem& item);
    void onUpdateItem(const TTSubtitleItem& item, const TTSubtitleItem& uItem);
    void onUpdateOrder(const TTSubtitleItem& item, int order);
    void onRefreshData(TTAVItem* avItem);

  signals:
    void dataChanged();
    void itemAppended(const TTSubtitleItem& item);
    void itemRemoved(const TTSubtitleItem& item);
    void itemRemoved(int index);
    void itemUpdated(const TTSubtitleItem& item, const TTSubtitleItem& uItem);
    void orderUpdated(const TTSubtitleItem& item, int order);
    void itemsSwapped(int oldIndex, int newIndex);

  private:
    QList<TTSubtitleItem>data;
};

#endif //TTSUBTITLELISTDATA_H
