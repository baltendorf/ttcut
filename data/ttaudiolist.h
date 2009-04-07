/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttaudiolistdata.h                                               */
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

#ifndef TTAUDIOLISTDATA_H
#define TTAUDIOLISTDATA_H


#include <QList>
#include <QString>
#include <QFileInfo>

class TTAVItem;
class TTAudioStream;

/* ///////////////////////////////////////////////////////////////////////////////////////
 * TTAudioListDataItem
 */
class TTAudioItem
{
  friend class TTAudioList;

  public:
    TTAudioItem(TTAVItem* avDataItem, TTAudioStream* aStream);
    TTAudioItem(const TTAudioItem& item);

    int   order()                   const { return mOrder; }
    void  setOrder(int value)             { mOrder = value; }

    QFileInfo      getFileInfo() const;
    TTAudioStream* getAudioStream() const;
    QString        getFileName() const;
    QString        getLength() const;
    QString        getVersion() const;
    QString        getMode() const;
    QString        getBitrate() const;
    QString        getSamplerate() const;
    QString        getDelay() const;
    TTAVItem*  avDataItem() const {return mpAVDataItem;}

    const TTAudioItem& operator=(const TTAudioItem& item);
    bool operator<(const TTAudioItem& item) const;
    bool operator==(const TTAudioItem& item) const;

  private:
    void setItemData();

  private:
    TTAVItem*  mpAVDataItem;
    TTAudioStream* audioStream;
    int            mOrder;
    QString        audioLength;
    QString        audioDelay;
};

/* /////////////////////////////////////////////////////////////////////////////
 * TTAudioListData
 */
class TTAudioList : public QObject
{
  Q_OBJECT

  public:
    TTAudioList();
    ~TTAudioList();

    void append(TTAVItem* avDataItem, TTAudioStream* aStream, int order=-1);
    void append(const TTAudioItem& item);
    void remove(const TTAudioItem& item);
    void update(const TTAudioItem& cItem, const TTAudioItem& uItem);

    const TTAudioItem& at(int index);

    int   count();
    void  clear();
    void  sortByOrder();
    int   indexOf(const TTAudioItem& item);
    void  swap(int a, int b);
    void  print();
    void  updateOrder();

  public slots:
    void onAppendItem(const TTAudioItem& item);
    void onRemoveItem(const TTAudioItem& item);
    void onUpdateItem(const TTAudioItem& item, const TTAudioItem& uItem);
    void onUpdateOrder(const TTAudioItem& item, int order);
    void onRefreshData(TTAVItem* avItem);

  signals:
		void dataChanged();
    void itemAppended(const TTAudioItem& item);
    void itemRemoved(const TTAudioItem& item);
    void itemRemoved(int index);
    void itemUpdated(const TTAudioItem& item, const TTAudioItem& uItem);
    void orderUpdated(const TTAudioItem& item, int order);
    void itemsSwapped(int oldIndex, int newIndex);

  private:
    QList<TTAudioItem>data;
};

#endif //TTAUDIOLISTDATA_H
