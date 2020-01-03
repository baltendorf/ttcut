/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttavdata.h                                                      */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 12/09/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVDEODATA
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

#ifndef TTAVDATA_H
#define TTAVDATA_H

#include <QThread>
#include <QObject>
#include <QList>
#include <QListIterator>

#include "ttcutlist.h"
#include "ttmarkerlist.h"
#include "ttavlist.h"

class TTThreadTaskPool;
class TTThreadTask;
class TTMessageLogger;
class TTAVStream;
class TTAudioStream;
class TTVideoStream;
class TTOpenVideoTask;
class TTOpenAudioTask;
class TTOpenSubtitleTask;
class TTCutPreviewTask;
class TTCutVideoTask;
class TTCutAudioTask;
class TTCutSubtitleTask;
class TTCutProjectData;
class TTMuxListData;
class TTMuxListDataItem;

/* /////////////////////////////////////////////////////////////////////////////
 * TTAVData
 */
class TTAVData : public QObject
{
  Q_OBJECT

  public:
    TTAVData();
    ~TTAVData();

    void clear();

    void      openAVStreams(const QString& videoFilePath);
    void      writeProjectFile(const QFileInfo& fInfo);
    void      readProjectFile(const QFileInfo& fInfo);

    void      appendAudioStream(TTAVItem* avItem, const QFileInfo& fInfo, int order=-1);
    void      appendSubtitleStream (TTAVItem* avItem, const QFileInfo& fInfo, int order=-1);

    void      appendCutEntry(TTAVItem* avItem, int cutIn, int cutOut);
    void      copyCutEntry(const TTCutItem& cutItem);
    void      sortCutItemsByOrder();

    void      appendMarker(TTAVItem* avItem, int markerPos);
    void      copyMarker(const TTMarkerItem& markerItem);
    void      sortMarkerByOrder();

    TTAVItem* avItemAt(int index)         { return mpAVList->at(index); }
    int       avCount()                   { return mpAVList->count(); }
    int       avIndexOf(TTAVItem* item)   { return mpAVList->indexOf(item); }

    TTCutItem cutItemAt(int index)        { return mpCutList->at(index); }
    int       cutIndexOf(const TTCutItem& item) { return mpCutList->indexOf(item); }
    int       cutCount()                  { return mpCutList->count(); }

    TTMarkerItem markerAt(int index)                     { return mpMarkerList->at(index); }
    int          markerIndexOf(const TTMarkerItem& item) { return mpMarkerList->indexOf(item); }
    int          markerCount()                           { return mpMarkerList->count(); }


    TTAVItem* doOpenVideoStream(const QString& filePath, int order=-1);
    void      doOpenAudioStream(TTAVItem* avItem, const QString& filePath, int order=-1);
    void      doOpenSubtitleStream(TTAVItem* avItem, const QString& filePath, int order=-1);
    void      doCutPreview(TTCutList* cutList);

    int       totalProcess() const;
    QTime     totalTime() const;

    //TODO: just for testing purpose
    TTThreadTaskPool* threadTaskPool() const;
    TTCutList*        cutList() const;



  public slots:
    void onChangeCurrentAVItem(int index);
    void onChangeCurrentAVItem(TTAVItem* avItem);

    void onRemoveAVItem(int index);
    void onSwapAVItems(int oldIndex, int newIndex);

    void onRemoveCutItem(const TTCutItem& item);
    void onCutOrderChanged(int, int);

    void onAppendMarker(int);
    void onRemoveMarker(const TTMarkerItem& mItem);
    void onMarkerOrderChanged(int, int);

    void onDoFrameSearch(TTAVItem* avItem, int startIndex);

    void onUserAbortRequest();

    void onDoCut(QString tgtFileName, TTCutList* cutList);
    void onCutFinished();
    void onCutAborted();

    void onMplexStep(const QString& msg, quint64 value);

  private slots:
    void onOpenVideoFinished(TTAVItem* avItem, TTVideoStream* vStream, int order);
    void onOpenAVStreamsAborted();

    void onOpenAudioFinished(TTAVItem* avItem, TTAudioStream* aStream, int order);
    void onOpenAudioAborted(TTAVItem* avItem);

    void onOpenSubtitleFinished(TTAVItem* avItem, TTSubtitleStream* sStream, int order);
    void onOpenSubtitleAborted(TTAVItem* avItem);

    void onCutPreviewFinished(TTCutList* cutList);
    void onCutPreviewAborted();

    void onReadProjectFileFinished();
    void onReadProjectFileAborted();

    void onThreadPoolInit();
    void onThreadPoolExit();

    void onStatusReport(int state, const QString& msg, quint64 value);

  signals:
    void threadPoolExit();
    void statusReport(int state, const QString& msg, quint64 value);
    void statusReport(TTThreadTask* task, int state, const QString& msg, quint64 value);
    void dataReady();

    void readProjectFileFinished(const QString&);

    void avItemAppended(const TTAVItem& item);
    void avItemRemoved(int index);
    void avItemsSwapped(int oldIndex, int newIndex);
    void avItemUpdated(const TTAVItem& cItem, const TTAVItem& uItem);
    void avDataReloaded();
    void currentAVItemChanged(TTAVItem* avData);

    void cutItemAppended(const TTCutItem& item);
    void cutItemRemoved(int index);
    void cutOrderUpdated(const TTCutItem& item, int order);
    void cutItemUpdated(const TTCutItem& citem, const TTCutItem& uitem);
    void cutDataReloaded();

    void markerAppended(const TTMarkerItem& item);
    void markerRemoved(int index);
    void markerUpdated(const TTMarkerItem& item, int order);
    void markerUpdated(const TTMarkerItem& citem, const TTMarkerItem& uitem);
    void markerDataReloaded();

    void foundEqualFrame(int index);
    void cutPreviewFinished(TTCutList* cutList);

  private:
    TTAVItem*      createAVItem();
    TTAVList*      videoDataList() { return mpAVList; }
    QFileInfoList  getAudioNames(const QFileInfo& vFileInfo);
    QFileInfoList  getSubtitleNames (const QFileInfo& vFileInfo);
    QString        createAudioCutFileName(QString cutBaseFileName, QString audioFileName, int index);
    QString        createSubtitleCutFileName(QString cutBaseFileName, QString subtitleFileName, int index);

  private:
    TTThreadTaskPool*   mpThreadTaskPool;
    TTMessageLogger*    log;
    TTAVItem*           mpCurrentAVItem;
    TTAVList*           mpAVList;
    TTCutList*          mpCutList;
    TTMarkerList*       mpMarkerList;
    TTMuxListData*      mpMuxList;
    TTOpenVideoTask*    openVideoTask;
    TTOpenAudioTask*    openAudioTask;
    TTOpenSubtitleTask* openSubtitleTask;
    TTCutPreviewTask*   cutPreviewTask;
    TTCutVideoTask*     cutVideoTask;
    TTCutAudioTask*     cutAudioTask;
    TTCutSubtitleTask*  cutSubtitleTask;
    TTCutProjectData*   mpProjectData;
};


#endif //TTAVDATA_H
