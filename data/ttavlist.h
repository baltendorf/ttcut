/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttavdatalist.h                                                  */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 11/13/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVDATALIST
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

#ifndef TTAVDATALIST_H
#define TTAVDATALIST_H

#include <QObject>
#include <QList>
#include <QListIterator>

#include "ttcutlist.h"
#include "ttmarkerlist.h"
#include "ttaudiolist.h"


class TTVideoStream;
class TTAudioStream;
class TTMessageLogger;

/* /////////////////////////////////////////////////////////////////////////////
 * TTAVDataItem
 */
class TTAVItem : public QObject
{
  Q_OBJECT

  friend class TTAVList;
  friend class TTAVData;

  public:
    TTAVItem(TTVideoStream* videoStream);
    ~TTAVItem();

    bool           isInList();
    TTVideoStream* videoStream()              const { return mpVideoStream; };
    void           setVideoStream(TTVideoStream* videoStream);
    TTAudioStream* audioStreamAt(int index)   const { return mpAudioList->at(index).getAudioStream(); }
    int            audioCount()               const { return mpAudioList->count(); }
    int            cutCount()                 const { return mpCutList->count(); }
    int            markerCount()              const { return mpMarkerList->count(); }
    TTAudioItem    audioListItemAt(int index) const { return mpAudioList->at(index); }
    TTCutItem      cutListItemAt(int index)   const { return mpCutList->at(index); }
    int            cutIndexOf(const TTCutItem& item) const { return mpCutList->indexOf(item); }
    TTMarkerItem   markerAt(int index)               const { return mpMarkerList->at(index); }
    int            markerIndexOf(const TTMarkerItem& item) const { return mpMarkerList->indexOf(item); }

    void canCutWith(const TTAVItem* avItem, int cutIn, int cutOut);

    void appendAudioEntry(TTAudioStream* aStream, int order=-1);
    void appendAudioEntry(const TTAudioItem& aItem);
    void removeAudioEntry(const TTAudioItem& aItem);
    void updateAudioEntry(const TTAudioItem& aItem, const TTAudioItem& uItem);

    void appendCutEntry(int cutIn, int cutOut, int order=-1);
    void appendCutEntry(const TTCutItem& cItem);
    void removeCutEntry(const TTCutItem& cItem);
    void updateCutEntry(const TTCutItem& cItem, int cutIn, int cutOut);
    void updateCutEntry(const TTCutItem& cItem, const TTCutItem& uItem);

    void appendMarker(int markerPos, int order=-1);
    void appendMarker(const TTMarkerItem& cItem);
    void removeMarker(const TTMarkerItem& cItem);
    void updateMarker(const TTMarkerItem& cItem, int markerPos);
    void updateMarker(const TTMarkerItem& cItem, const TTMarkerItem& uItem);

  public slots:
    void onRemoveAudioItem(int index);
    void onSwapAudioItems(int oldIndex, int newIndex);

  signals:
		void updated(TTAVItem* avItem);
    void audioItemAppended(const TTAudioItem& item);
    void audioItemRemoved(const TTAudioItem& item);
    void audioItemRemoved(int index);
    void audioItemUpdated(const TTAudioItem& cItem, const TTAudioItem& uItem);
    void audioOrderUpdated(const TTAudioItem& item, int order);
    void audioItemsSwapped(int oldIndex, int newIndex);

  private:
    TTAudioList*  audioDataList() { return mpAudioList; }
    TTCutList*    cutDataList()   { return mpCutList; }
    TTMarkerList* markerList()    { return mpMarkerList; }
    void          checkCut(int cutIn, int cutOut);

  private:
  	bool           mIsInList;
    TTVideoStream* mpVideoStream;
    TTAudioList*   mpAudioList;
    TTCutList*     mpCutList;
    TTMarkerList*  mpMarkerList;
};

/* /////////////////////////////////////////////////////////////////////////////
 * TTAVDataList
 */
class TTAVList : public QObject
{
  Q_OBJECT

  public:
    TTAVList();
    ~TTAVList();

    void      append(TTAVItem* item);
    TTAVItem* at(int i);
    int       indexOf(TTAVItem* item) { return mpAVList.indexOf(item); }
    void      clear();
    int       count();
    void      removeAt(int i);
    void      swap(int a, int b);

  signals:
    void itemAppended(const TTAVItem& item);
    void itemRemoved(int index);
    void itemUpdated(const TTAVItem&, const TTAVItem&);
    void itemsSwapped(int oldIndex, int newIndex);

  private:
    QList<TTAVItem*> mpAVList;
};

#endif //TTAVDATALIST_H
