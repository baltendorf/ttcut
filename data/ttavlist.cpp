/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttavlist.cpp                                                    */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 11/13/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAVLIST
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

#include "ttavlist.h"
#include "ttcutprojectdata.h"
#include "ttcutlist.h"
#include "ttaudiolist.h"
#include "../avstream/ttmpeg2videoheader.h"
#include "../avstream/ttavstream.h"
#include "../common/ttexception.h"
#include "../common/ttmessagelogger.h"

#include <QList>
#include <QDir>

/* /////////////////////////////////////////////////////////////////////////////
 * TTAVItem
 */
TTAVItem::TTAVItem(TTVideoStream* videoStream)
{
	mpVideoStream = videoStream;
	mIsInList     = false;
	mpAudioList   = new TTAudioList();
	mpCutList     = new TTCutList();
	mpMarkerList  = new TTMarkerList();

  connect(mpAudioList, SIGNAL(itemAppended(const TTAudioItem&)),                     SIGNAL(audioItemAppended(const TTAudioItem&)));
	//connect(mpAudioDataList, SIGNAL(itemRemoved(const TTAudioListDataItem&)),
  //                         SIGNAL(audioItemRemoved(const TTAudioListDataItem&)));
	connect(mpAudioList, SIGNAL(itemRemoved(int)),                                     SIGNAL(audioItemRemoved(int)));
  connect(mpAudioList, SIGNAL(itemUpdated(const TTAudioItem&, const TTAudioItem&)),  SIGNAL(audioItemUpdated(const TTAudioItem&, const TTAudioItem&)));
  //connect(mpAudioDataList, SIGNAL(orderUpdated(const TTAudioListDataItem&, int)),
  //                         SIGNAL(audioOrderUpdated(const TTAudioListDataItem&, int)));
	connect(mpAudioList, SIGNAL(itemsSwapped(int, int)),                  												 SIGNAL(audioItemsSwapped(int, int)));

	connect(this,        SIGNAL(updated(TTAVItem*)),    mpAudioList,  SLOT(onRefreshData(TTAVItem*)));
	connect(this,        SIGNAL(updated(TTAVItem*)),    mpCutList,    SLOT(onRefreshData(TTAVItem*)));
	connect(this,        SIGNAL(updated(TTAVItem*)),    mpMarkerList, SLOT(onRefreshData(TTAVItem*)));
}

/* /////////////////////////////////////////////////////////////////////////////
 * Destructor
 */
TTAVItem::~TTAVItem()
{
  if (mpAudioList   != 0) delete mpAudioList;
  if (mpCutList     != 0) delete mpCutList;
  if (mpVideoStream != 0) delete mpVideoStream;
}

/*!
 * setVideoStream
 */
void TTAVItem::setVideoStream(TTVideoStream* videoStream)
{
  mpVideoStream = videoStream;
}

bool TTAVItem::isInList()
{
	return mIsInList;
}

/*
 *  ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::appendAudioEntry(TTAudioStream* aStream, int order)
{
	mpAudioList->append(this, aStream, order);
}

/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::appendAudioEntry(const TTAudioItem& aItem)
{
	mpAudioList->append(aItem);
}

/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::removeAudioEntry(const TTAudioItem& aItem)
{
	mpAudioList->remove(aItem);
}

/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::updateAudioEntry(const TTAudioItem& aItem, const TTAudioItem& uItem)
{
	mpAudioList->update(aItem, uItem);
}

/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::onRemoveAudioItem(int index)
{
	mpAudioList->remove(mpAudioList->at(index));
}

void TTAVItem::onSwapAudioItems(int oldIndex, int newIndex)
{
	mpAudioList->swap(oldIndex, newIndex);
}

/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::appendCutEntry(int cutIn, int cutOut, int order)
{
	checkCut(cutIn, cutOut);
	mpCutList->append(this, cutIn, cutOut, order);
}

/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::appendCutEntry(const TTCutItem& cItem)
{
	checkCut(cItem.cutIn(), cItem.cutOut());
	mpCutList->append(cItem);
}

/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::removeCutEntry(const TTCutItem& cItem)
{
	mpCutList->remove(cItem);
}

/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::updateCutEntry(const TTCutItem& cItem, int cutIn,	int cutOut)
{
	TTCutItem uItem(this, cutIn, cutOut);
	mpCutList->update(cItem, uItem);
}

/* ///////////////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::updateCutEntry(const TTCutItem& cItem,const TTCutItem& uItem)
{
	mpCutList->update(cItem, uItem);
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAVItem::canCutWith(const TTAVItem* avItem, int cutIn, int cutOut)
{
	TTVideoStream*    video1  = videoStream();
	TTVideoStream*    video2  = avItem->videoStream();

	if (video1->frameRate() != video2->frameRate())
		throw new TTInvalidOperationException(tr("Video files to cut must have the same framerate!"));

	if (audioCount() != avItem->audioCount())
		throw new TTInvalidOperationException(tr("Video files to cut must have the same count of audio files!"));

	TTSequenceHeader* seqIn2  = video2->getSequenceHeader(cutIn);
	TTSequenceHeader* seqOut2 = video2->getSequenceHeader(cutOut);

	for (int i = 0; i < cutCount(); i++) {
		TTCutItem         cutItem1 = cutListItemAt(i);
		TTSequenceHeader* seqIn1   = video2->getSequenceHeader(cutIn);
		TTSequenceHeader* seqOut1  = video2->getSequenceHeader(cutOut);


	if (seqIn1->aspectRatio() != seqIn2->aspectRatio() || seqOut1->aspectRatio() != seqOut2->aspectRatio())
		throw new TTInvalidOperationException(tr("Video files to cut must have the same aspect ratio!"));

	if (seqIn1->horizontalSize() != seqIn2->horizontalSize() || seqOut1->horizontalSize() != seqOut2->horizontalSize())
		throw new TTInvalidOperationException(tr("Video files to cut must have the same horizontal size!"));

	if (seqIn1->verticalSize() != seqIn2->verticalSize() || seqOut1->verticalSize() != seqOut2->verticalSize())
		throw new TTInvalidOperationException(tr("Video files to cut must have the same vertical size!"));
	}

	for (int i = 0; i < audioCount(); i++) {
		const TTAudioItem& audio1 = audioListItemAt(i);
		const TTAudioItem& audio2 = avItem->audioListItemAt(i);

		if (audio1.getBitrate()    != audio2.getBitrate())
			throw new TTInvalidOperationException(tr("Audio files to cut must have the same bitrate!"));

		if (audio1.getSamplerate() != audio2.getSamplerate())
			throw new TTInvalidOperationException(tr("Audio files to cut must have the same samplerate!"));

		if (audio1.getVersion() != audio2.getVersion())
			throw new TTInvalidOperationException(tr("Audio files to cut must have the same version!"));

		//if (audio1.getMode()       != audio2.getMode())
		//	throw new TTInvalidOperationException(tr("Audio files to cut must have the same mode!"));
	}
}

/* /////////////////////////////////////////////////////////////////////////////
 * checkCut
 */
void TTAVItem::checkCut(int , int )
{
  if (videoStream() == 0) return;

  //TODO: what's about this check; remove it
//	if (cutOut > videoStream()->frameCount())
//		throw new TTIndexOutOfRangeException(tr("Cut out exceeds video frame count!"));
}

/*!
 * Marker
 */
void TTAVItem::appendMarker(int markerPos, int order)
{
	mpMarkerList->append(this, markerPos, order);
}

void TTAVItem::appendMarker(const TTMarkerItem& cItem)
{
	mpMarkerList->append(cItem);
}

void TTAVItem::removeMarker(const TTMarkerItem& cItem)
{
	mpMarkerList->remove(cItem);
}

void TTAVItem::updateMarker(const TTMarkerItem& cItem, int markerPos)
{
	TTMarkerItem uItem(this, markerPos);
	mpMarkerList->update(cItem, uItem);
}

void TTAVItem::updateMarker(const TTMarkerItem& cItem, const TTMarkerItem& uItem)
{
	mpMarkerList->update(cItem, uItem);
}


/* /////////////////////////////////////////////////////////////////////////////
 * TTAVDataList
 */

/*!
 * TTAVList
 */
TTAVList::TTAVList()
{
}

/*!
 * ~TTAVList
 */
TTAVList::~TTAVList()
{
	clear();
}

/*!
 * append
 */
void TTAVList::append(TTAVItem* item)
{
	item->mIsInList = true;
	mpAVList.append(item);
	emit itemAppended(*item);
}

/*!
 * at
 */
TTAVItem* TTAVList::at(int i)
{
	return mpAVList.at(i);
}

/*!
 * clear
 */
void TTAVList::clear()
{
	while (mpAVList.count() > 0) {
		TTAVItem* item = mpAVList.takeLast();
		delete item;
		item = 0;
		emit itemRemoved(mpAVList.count());
	}
	//removeAt(mpAVList.indexOf(mpAVList.last()));
}

/*!
 * count
 */
int TTAVList::count()
{
	return mpAVList.count();
}

/*!
 * removeAt
 */
void TTAVList::removeAt(int i)
{
	TTAVItem* item = mpAVList.takeAt(i);

	delete item;

	emit itemRemoved(i);
}

/*!
 * swap
 */
void TTAVList::swap(int a, int b)
{
	if ((a < 0 || b < 0) || (a >= count() || b >= count()))
		return; //TODO: throw an index out of bound exception

	mpAVList.swap(a, b);

  emit itemsSwapped(a, b);
}


