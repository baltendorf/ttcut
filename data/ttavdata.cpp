//*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttavdata.cpp                                                    */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 12/09/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVDATA
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
#include "ttcutlist.h"
#include "ttavdata.h"
#include "ttmuxlistdata.h"
#include "ttcutprojectdata.h"
#include "ttcutlist.h"
#include "ttaudiolist.h"
#include "ttcutparameter.h"
#include "../avstream/ttmpeg2videostream.h"
#include "../common/ttthreadtaskpool.h"
#include "../common/ttexception.h"
#include "../common/ttmessagelogger.h"
#include "../common/istatusreporter.h"

#include "../extern/ttmplexprovider.h"

#include "ttopenvideotask.h"
#include "ttopenaudiotask.h"
#include "ttcutpreviewtask.h"
#include "ttcutvideotask.h"
#include "ttcutaudiotask.h"
#include "ttframesearchtask.h"

#include <QThreadPool>
#include <QList>
#include <QDir>
#include <QDebug>

/* /////////////////////////////////////////////////////////////////////////////
 * Class TTAVData
 */
TTAVData::TTAVData()
{
	mpThreadTaskPool  = new TTThreadTaskPool();
	cutPreviewTask    = 0;

	log               = TTMessageLogger::getInstance();
	mpCurrentAVItem   = 0;
  mpMuxList         = new TTMuxListData();
	mpAVList          = new TTAVList();
	mpCutList         = new TTCutList();
	mpMarkerList      = new TTMarkerList();

	connect(mpThreadTaskPool, SIGNAL(init()), this, SLOT(onThreadPoolInit()));
  connect(mpThreadTaskPool, SIGNAL(exit()), this, SLOT(onThreadPoolExit()));
  connect(mpThreadTaskPool, SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)),
	                          SIGNAL(statusReport(TTThreadTask*, int, const QString&, quint64)));

  connect(mpAVList,  SIGNAL(itemAppended(const TTAVItem&)),                   SIGNAL(avItemAppended(const TTAVItem&)));
	connect(mpAVList,  SIGNAL(itemRemoved(int)),                                SIGNAL(avItemRemoved(int)));
	connect(mpAVList,  SIGNAL(itemUpdated(const TTAVItem&, const TTAVItem&)),   SIGNAL(avItemUpdated(const TTAVItem&, const TTAVItem&)));
	connect(mpAVList,  SIGNAL(itemsSwapped(int, int)),                          SIGNAL(avItemsSwapped(int, int)));

	connect(mpCutList, SIGNAL(itemAppended(const TTCutItem&)),                  SIGNAL(cutItemAppended(const TTCutItem&)));
	connect(mpCutList, SIGNAL(itemRemoved(int)),                                SIGNAL(cutItemRemoved(int)));
	connect(mpCutList, SIGNAL(orderUpdated(const TTCutItem&, int)),             SIGNAL(cutOrderUpdated(const TTCutItem&, int)));
	connect(mpCutList, SIGNAL(itemUpdated(const TTCutItem&, const TTCutItem&)), SIGNAL(cutItemUpdated(const TTCutItem&, const TTCutItem&)));

	connect(mpMarkerList, SIGNAL(itemAppended(const TTMarkerItem&)),                     SIGNAL(markerAppended(const TTMarkerItem&)));
	connect(mpMarkerList, SIGNAL(itemRemoved(int)),                                      SIGNAL(markerRemoved(int)));
	connect(mpMarkerList, SIGNAL(orderUpdated(const TTMarkerItem&, int)),                SIGNAL(markerUpdated(const TTMarkerItem&, int)));
	connect(mpMarkerList, SIGNAL(itemUpdated(const TTMarkerItem&, const TTMarkerItem&)), SIGNAL(markerUpdated(const TTMarkerItem&, const TTMarkerItem&)));
}

/* /////////////////////////////////////////////////////////////////////////////
 * Destructor
 */
TTAVData::~TTAVData()
{
	clear();

	if (mpAVList         != 0) delete mpAVList;
	if (mpCutList        != 0) delete mpCutList;
	if (mpMarkerList     != 0) delete mpMarkerList;
  if (mpMuxList        != 0) delete mpMuxList;
  if (mpThreadTaskPool != 0) delete mpThreadTaskPool;
}

/* /////////////////////////////////////////////////////////////////////////////
 * clear
 */
void TTAVData::clear()
{
	mpAVList->clear();
	mpCutList->clear();
	mpMarkerList->clear();
}

/*!
 * appendAudioStream
 */
void TTAVData::appendAudioStream(TTAVItem* avItem, const QFileInfo& fInfo, int)
{
  doOpenAudioStream(avItem, fInfo.absoluteFilePath());
}

/* /////////////////////////////////////////////////////////////////////////////
 * Cut list handling
 */

/*!
 * appendCutEntry
 */
void TTAVData::appendCutEntry(TTAVItem* avItem, int cutIn, int cutOut)
{
	for (int i = 0; i < mpAVList->count(); i++) {
		mpAVList->at(i)->canCutWith(avItem, cutIn, cutOut);
	}

	avItem->appendCutEntry(cutIn, cutOut);
}

/*!
 * copyCutEntry
 */
void TTAVData::copyCutEntry(const TTCutItem& cutItem)
{
	if (mpCurrentAVItem == 0)
		throw new TTInvalidOperationException("No current AV-Data set!");

	appendCutEntry(mpCurrentAVItem, cutItem.cutIn(), cutItem.cutOut());
}

/*!
 * sortCutItemsByOrder
 */
void TTAVData::sortCutItemsByOrder()
{
	mpCutList->sortByOrder();
	emit cutDataReloaded();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Marker handling
 */

/*!
 * appendMarker
 */
void TTAVData::appendMarker(TTAVItem* avItem, int markerPos)
{
	avItem->appendMarker(markerPos);
}

/*!
 * onAppendMarker
 */
void TTAVData::onAppendMarker(int markerPos)
{
	if (mpCurrentAVItem == 0)
		return;

	mpCurrentAVItem->appendMarker(markerPos);
}

/*!
 * onRemoveMarker
 */
void TTAVData::onRemoveMarker(const TTMarkerItem& mItem)
{
	TTAVItem* avItem = mItem.avDataItem();
	avItem->removeMarker(mItem);
}

/*!
 * copyMarker
 */
void TTAVData::copyMarker(const TTMarkerItem& markerItem)
{
	if (mpCurrentAVItem == 0)
		throw new TTInvalidOperationException("No current AV-Data set!");

	appendMarker(mpCurrentAVItem, markerItem.markerPos());
}

/*!
 * sortMarkerByOrder
 */
void TTAVData::sortMarkerByOrder()
{
	mpMarkerList->sortByOrder();
	emit markerDataReloaded();
}

int TTAVData::totalProcess() const
{
	return mpThreadTaskPool->overallPercentage();
}

QTime TTAVData::totalTime() const
{
	return mpThreadTaskPool->overallTime();
}

/* /////////////////////////////////////////////////////////////////////////////
 * createAVDataItem
 * Create an AVData item, connect Signals and Slots
 */
TTAVItem* TTAVData::createAVItem()
{
	try
  {
		TTAVItem* avItem = new TTAVItem(0);

		connect(avItem->mpCutList,  SIGNAL(itemAppended(const TTCutItem&)),
            mpCutList,          SLOT(onAppendItem(const TTCutItem&)));
		connect(avItem->mpCutList,  SIGNAL(itemRemoved(const TTCutItem&)),
            mpCutList,          SLOT(onRemoveItem(const TTCutItem&)));
		connect(avItem->mpCutList,  SIGNAL(itemUpdated(const TTCutItem&, const TTCutItem&)),
            mpCutList,          SLOT(onUpdateItem(const TTCutItem&, const TTCutItem&)));
		connect(mpCutList,          SIGNAL(orderUpdated(const TTCutItem&, int)),
            avItem->mpCutList,  SLOT(onUpdateOrder(const TTCutItem&, int)));

		connect(avItem->mpMarkerList, SIGNAL(itemAppended(const TTMarkerItem&)),
            mpMarkerList,         SLOT(onAppendItem(const TTMarkerItem&)));
		connect(avItem->mpMarkerList, SIGNAL(itemRemoved(const TTMarkerItem&)),
            mpMarkerList,         SLOT(onRemoveItem(const TTMarkerItem&)));
		connect(avItem->mpMarkerList, SIGNAL(itemUpdated(const TTMarkerItem&, const TTMarkerItem&)),
            mpMarkerList,         SLOT(onUpdateItem(const TTMarkerItem&, const TTMarkerItem&)));
		connect(mpMarkerList,         SIGNAL(orderUpdated(const TTMarkerItem&, int)),
            avItem->mpMarkerList, SLOT(onUpdateOrder(const TTMarkerItem&, int)));

  	return avItem;
	}
  catch (TTException ex)
  {
		log->fatalMsg(__FILE__, __LINE__, tr("exception in createAVDataItem!"));
		return 0;
	}
}

/*!
 * openAVStreams
 * Open the video stream and all according audio streams and add them to AVData
 */
void TTAVData::openAVStreams(const QString& videoFilePath)
{
  connect(mpThreadTaskPool, SIGNAL(aborted()),
          this,             SLOT(onOpenAVStreamsAborted()));

  TTAVItem* avItem = doOpenVideoStream(videoFilePath);

  QFileInfoList audioInfoList = getAudioNames(QFileInfo(videoFilePath));
	QListIterator<QFileInfo> info(audioInfoList);

	while (info.hasNext()) {
		doOpenAudioStream(avItem, info.next().absoluteFilePath());
	}
}

// *****************************************************************************
//! Just for testing purpose! Remove it!
//! Returns a reference to the thread pool
TTThreadTaskPool* TTAVData::threadTaskPool() const
{
  return mpThreadTaskPool;
}

//! Returns a reference to the main cut list
TTCutList* TTAVData::cutList() const
{
  return mpCutList;
}
// *****************************************************************************


/*!
 * doOpenVideoStream
 */
TTAVItem* TTAVData::doOpenVideoStream(const QString& filePath, int order)
{
  TTAVItem*        avItem        = createAVItem();
  TTOpenVideoTask* openVideoTask = new TTOpenVideoTask(avItem, filePath, order);

  connect(openVideoTask, SIGNAL(finished(TTAVItem*, TTVideoStream*, int)),
          this,          SLOT(onOpenVideoFinished(TTAVItem*, TTVideoStream*, int)));

  mpThreadTaskPool->start(openVideoTask);

  return avItem;
}

/*!
 * doOpenAudioStream
 */
void TTAVData::doOpenAudioStream(TTAVItem* avItem, const QString& filePath, int order)
{
  TTOpenAudioTask* openAudioTask = new TTOpenAudioTask(avItem, filePath, order);

  connect(openAudioTask, SIGNAL(finished(TTAVItem*, TTAudioStream*, int)),
          this,          SLOT(onOpenAudioFinished(TTAVItem*, TTAudioStream*, int)));

  mpThreadTaskPool->start(openAudioTask);
}

/*!
 * onOpenVideoFinished
 */
void TTAVData::onOpenVideoFinished(TTAVItem* avItem, TTVideoStream* vStream, int)
{
  avItem->setVideoStream(vStream);

  mpAVList->append(avItem);

  this->avDataReloaded();
  this->cutDataReloaded();
  this->markerDataReloaded();

  mpCurrentAVItem = avItem;
  emit currentAVItemChanged(avItem);
}

void TTAVData::onOpenAVStreamsAborted()
{
  disconnect(mpThreadTaskPool, SIGNAL(aborted()),
             this,             SLOT(onOpenAVStreamsAborted()));

  mpCurrentAVItem = (mpAVList->count() > 0) ? mpAVList->at(mpAVList->count()-1) : 0;
  emit currentAVItemChanged(mpCurrentAVItem);
}

/*!
 * onOpenAudioFinished
 */
void TTAVData::onOpenAudioFinished(TTAVItem* avItem, TTAudioStream* aStream, int order)
{
  if (avItem  == 0) return;
  if (aStream == 0) return;

  avItem->appendAudioEntry(aStream, order);
}

/*!
 * onOpenAudioAborted
 */
void TTAVData::onOpenAudioAborted(TTAVItem*)
{
  qDebug("TTAVData::onOpenAudioAborted called...");
}

/*  ////////////////////////////////////////////////////////////////////////////
 * Slots for av data list
 */

void TTAVData::onChangeCurrentAVItem(TTAVItem* avItem)
{
	mpCurrentAVItem = avItem;

	emit currentAVItemChanged(avItem);
}

void TTAVData::onChangeCurrentAVItem(int index)
{
	if (index < 0 || index > mpAVList->count()) return;

	mpCurrentAVItem = avItemAt(index);

	emit currentAVItemChanged(mpCurrentAVItem);
}

void TTAVData::onRemoveAVItem(int index)
{
	if (index-1 >= 0 && avCount() > 1)
		mpCurrentAVItem = avItemAt(index-1);

	if (index+1 < avCount() && avCount() > 1)
		mpCurrentAVItem = avItemAt(index+1);

	if (avCount() > 1)
	  emit currentAVItemChanged(mpCurrentAVItem);

	//  mpCurrentAVItem = (avCount() > 0)
//      ? avItemAt(avCount()-1)
//      : 0;
  //emit currentAVItemChanged(mpCurrentAVItem);

  mpAVList->removeAt(index);

  if (avCount() == 0) {
  	mpCurrentAVItem = 0;
    emit currentAVItemChanged(mpCurrentAVItem);
  }
}

void TTAVData::onSwapAVItems(int oldIndex, int newIndex)
{
	mpAVList->swap(oldIndex, newIndex);
}


void TTAVData::onRemoveCutItem(const TTCutItem& item)
{
	TTAVItem* avItem = item.avDataItem();
	avItem->removeCutEntry(item);
}

void TTAVData::onCutOrderChanged(int oldIndex, int newIndex)
{
	mpCutList->swap(oldIndex, newIndex);
}

void TTAVData::onMarkerOrderChanged(int oldIndex, int newIndex)
{
	mpMarkerList->swap(oldIndex, newIndex);
}

//! Search equal frame
void TTAVData::onDoFrameSearch(TTAVItem* avItem, int startIndex)
{
	if (mpCurrentAVItem == 0) return;

	TTFrameSearchTask* frameSearch = new TTFrameSearchTask(
      avItem->videoStream(),          startIndex,
      mpCurrentAVItem->videoStream(), mpCurrentAVItem->videoStream()->currentIndex());

	connect(frameSearch, SIGNAL(finished(int)), this, SIGNAL(foundEqualFrame(int)));

	mpThreadTaskPool->start(frameSearch);
}

//! User request to abort current operation
void TTAVData::onUserAbortRequest()
{
	mpThreadTaskPool->onUserAbortRequest();
}

void TTAVData::onThreadPoolInit()
{
  emit statusReport(0, StatusReportArgs::Init, tr("starting thread pool"), 0);
}

void TTAVData::onThreadPoolExit()
{
  emit statusReport(0, StatusReportArgs::Exit, tr("exiting thread pool"), 0);
  emit threadPoolExit();
}

/* /////////////////////////////////////////////////////////////////////////////
 * getAudioNames
 * Search for audiofiles acording to the video file name; Valid audio extensions
 * are: mpa, mp2, ac3
 */
QFileInfoList TTAVData::getAudioNames(const QFileInfo& vFileInfo)
{
	QDir audioDir(vFileInfo.absoluteDir());

	QStringList audioFilters;
	audioFilters << vFileInfo.completeBaseName() + "*" + ".mpa"
			<< vFileInfo.completeBaseName() + "*" + ".mp2"
			<< vFileInfo.completeBaseName() + "*" + ".ac3";

	audioDir.setNameFilters(audioFilters);
	audioDir.setFilter(QDir::Files);

	return audioDir.entryInfoList();
}

// ////////////////////////////////////////////////////////////////////////////
// Project file related methods 
/**
 * Write the xml project file
 */
void TTAVData::writeProjectFile(const QFileInfo& fInfo)
{
	TTCutProjectData* prj = new TTCutProjectData(fInfo);

	for (int i = 0; i < mpAVList->count(); i++) {
		prj->serializeAVDataItem(mpAVList->at(i));
	}

	prj->writeXml();

	delete prj;
}

/**
 * Read the TTCut xml project file
 */
void TTAVData::readProjectFile(const QFileInfo& fInfo)
{
  connect(mpThreadTaskPool, SIGNAL(exit()),    this, SLOT(onReadProjectFileFinished()));
  connect(mpThreadTaskPool, SIGNAL(aborted()), this, SLOT(onReadProjectFileAborted()));

  mpProjectData = new TTCutProjectData(fInfo);

  try
  {
	  mpProjectData->readXml();
	  mpProjectData->deserializeAVDataItem(this);
  }
  catch (TTException* ex)
  {
		log->errorMsg(__FILE__, __LINE__, ex->getMessage());
    onReadProjectFileAborted();
  }
}

/**
 * Reading TTCut project file finished
 */
void TTAVData::onReadProjectFileFinished()
{
  disconnect(mpThreadTaskPool, SIGNAL(aborted()), this, SLOT(onReadProjectFileAborted()));
  disconnect(mpThreadTaskPool, SIGNAL(exit()),    this, SLOT(onReadProjectFileFinished()));

  emit avDataReloaded();

  if (avCount() > 0)
    emit currentAVItemChanged(avItemAt(0));

  emit readProjectFileFinished(mpProjectData->filePath());
  
  delete mpProjectData;
  mpProjectData = 0;
}

/**
 * Reading TTCut project file aborted or error reading project file
 */
void TTAVData::onReadProjectFileAborted()
{
  qDebug() << "TAVData::onReadProjectFileAborted";
  disconnect(mpThreadTaskPool, SIGNAL(exit()), this, SLOT(onReadProjectFileFinished()));
  disconnect(mpThreadTaskPool, SIGNAL(aborted()), this, SLOT(onReadProjectFileAborted()));

  emit currentAVItemChanged(0);

  if (mpProjectData != 0) {
    delete mpProjectData;
    mpProjectData = 0;
  }
}

// /////////////////////////////////////////////////////////////////////////////
// Cut preview
/**
 * Create the cut preview clips
 */
void TTAVData::doCutPreview(TTCutList* cutList)
{
  if (cutPreviewTask != 0) delete cutPreviewTask;
  cutPreviewTask = new TTCutPreviewTask(this, cutList);

  connect(cutPreviewTask,   SIGNAL(finished(TTCutList*)),
          this,             SLOT(onCutPreviewFinished(TTCutList*)));
  connect(mpThreadTaskPool, SIGNAL(aborted()),
					this,             SLOT(onCutPreviewAborted()));

  mpThreadTaskPool->start(cutPreviewTask);
}

//! Finished creating cut preview clips
void TTAVData::onCutPreviewFinished(TTCutList* cutList)
{
	emit cutPreviewFinished(cutList);
}

//! Cut preview aborted by user
void TTAVData::onCutPreviewAborted()
{
  disconnect(cutPreviewTask,   SIGNAL(finished(TTCutList*)),
             this,             SLOT(onCutPreviewFinished(TTCutList*)));
  disconnect(mpThreadTaskPool, SIGNAL(aborted()),
			   		 this,             SLOT(onCutPreviewAborted()));

	if (cutPreviewTask != 0) {
    delete cutPreviewTask;
    cutPreviewTask = 0;
  }
}

/*!
 * createAudioCutFileName
 */
QString TTAVData::createAudioCutFileName(QString cutBaseFileName, QString audioFileName, int index)
{
  QString audioCutFileName = QString("%1_%2.%3").
    arg(QFileInfo(cutBaseFileName).completeBaseName()).
    arg(index, 3, 10, QLatin1Char('0')).
    arg(QFileInfo(audioFileName).suffix());

  return QFileInfo(QDir(TTCut::cutDirPath), audioCutFileName).absoluteFilePath();
}

// /////////////////////////////////////////////////////////////////////////////
// Audio and video cut
//
//! Do the audio and video cut for given cut-list
void TTAVData::onDoCut(QString tgtFileName, TTCutList* cutList)
{
  if (cutList == 0) cutList = mpCutList;

  cutVideoTask = new TTCutVideoTask(this);
  cutVideoTask->init(tgtFileName, cutList);

  connect(mpThreadTaskPool, SIGNAL(exit()),    this, SLOT(onCutFinished()));
  connect(mpThreadTaskPool, SIGNAL(aborted()), this, SLOT(onCutAborted()));

  mpThreadTaskPool->start(cutVideoTask);

  // all video must have the same count of audio streams!
  for (int i = 0; i < cutList->at(0).avDataItem()->audioCount(); i++) {
    TTAudioStream* audioStream = cutList->at(0).avDataItem()->audioStreamAt(i);

    QString tgtAudioFilePath = createAudioCutFileName(tgtFileName, audioStream->fileName(), i+1);

    log->debugMsg(__FILE__, __LINE__, QString("current audio stream %1").arg(audioStream->fileName()));
    log->debugMsg(__FILE__, __LINE__, QString("audio cut file %1").arg(tgtAudioFilePath));

    // audio file exists
    if (QFileInfo(tgtAudioFilePath).exists()) {
      // TODO: Warning about deleting file
      log->warningMsg(__FILE__, __LINE__, QString(tr("deleting existing audio cut file: %1").arg(tgtAudioFilePath)));
      QFile tempFile(tgtAudioFilePath);
      tempFile.remove();
      tempFile.close();
    }

    cutAudioTask = new TTCutAudioTask();
    cutAudioTask->init(tgtAudioFilePath, cutList, i, cutVideoTask->muxListItem());

    mpThreadTaskPool->start(cutAudioTask);
  }
}

//! Audio video cut finished
void TTAVData::onCutFinished()
{
	disconnect(mpThreadTaskPool, SIGNAL(exit()), this, SLOT(onCutFinished()));

  mpMuxList->appendItem(*(cutVideoTask->muxListItem()));

 // mux list / direct mux
 mpMuxList->print();
 TTMplexProvider* mplexProvider = new TTMplexProvider(mpMuxList);

 connect(mplexProvider, SIGNAL(statusReport(int, const QString&, quint64)), 
         this,          SLOT(onStatusReport(int, const QString&, quint64)));

 if (TTCut::muxMode == 1)
   mplexProvider->writeMuxScript();
 else
   mplexProvider->mplexPart(mpMuxList->count()-1);

 delete mplexProvider;
}

void TTAVData::onCutAborted()
{
  disconnect(mpThreadTaskPool, SIGNAL(exit()),    this, SLOT(onCutFinished()));
  disconnect(mpThreadTaskPool, SIGNAL(aborted()), this, SLOT(onCutAborted()));
}

void TTAVData::onStatusReport(int state, const QString& msg, quint64 value)
{
  emit statusReport(0, state, msg, value);
  qApp->processEvents();
}

void TTAVData::onMplexStep(const QString& msg, quint64 value)
{
  emit statusReport(0, StatusReportArgs::Step, msg, value);
  qApp->processEvents();
}
