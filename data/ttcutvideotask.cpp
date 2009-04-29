/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutvideotask.cpp                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/16/2009 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTVIDEOTASK
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

#include "ttcutvideotask.h"

#include "../common/ttexception.h"
#include "../common/ttthreadtaskpool.h"
#include "../avstream/ttfilebuffer.h"
#include "../data/ttcutparameter.h"
#include "../avstream/ttavstream.h"
#include "../data/ttavdata.h"
#include "../data/ttcutlist.h"

#include <QDebug>

/**
 * Cut video stream task
 */
//TTCutVideoTask::TTCutVideoTask(TTAVData* avData, QString tgtFilePath, TTCutList* cutList) :
TTCutVideoTask::TTCutVideoTask(TTAVData* avData) :
                TTThreadTask("CutVideoTask")
{
  mpAVData     = avData;
  mpCutList    = 0;
  mpCutStream  = 0;
  mpCutTask    = new TTCutTask();
}

/**
 * Init task
 */
void TTCutVideoTask::init(QString tgtFilePath, TTCutList* cutList)
{
  mTgtFilePath = tgtFilePath;
  mpCutList    = cutList;
  
  mMuxListItem.setVideoName(tgtFilePath);
}

/**
 * Operation abort request
 */
void TTCutVideoTask::onUserAbort()
{
  abort();
}

/**
 * Clean up after operation
 */
void TTCutVideoTask::cleanUp()
{
  //if (mpTgtStream != 0) delete mpTgtStream;
  //if (mpCutParams != 0) delete mpCutParams;
}

/**
 * Returns the mux list item
 */
TTMuxListDataItem* TTCutVideoTask::muxListItem()
{
  return &mMuxListItem;
}

/**
 * Task operation method
 */
void TTCutVideoTask::operation()
{
  if (mTgtFilePath.isEmpty())
    throw new TTInvalidOperationException(__FILE__, __LINE__, tr("No target file path given for video cut!"));

 	mpTgtStream = new TTFileBuffer(mTgtFilePath, QIODevice::WriteOnly);
  mpCutParams = new TTCutParameter(mpTgtStream);

	mpCutParams->setIsWriteSequenceEnd(true);
	mpTgtStream->open();

  onStatusReport(this, StatusReportArgs::Start, QString(tr("Cut 1 from %1").arg(mpCutList->count())), mpCutList->count());

  for (int i = 0; i < mpCutList->count(); i++) {

    if (isAborted())
      throw new TTAbortException(__FILE__, __LINE__, tr("Operation aborted!"));

	  TTCutItem cutItem = mpCutList->at(i);
	  int       cutIn   = cutItem.cutInIndex();
	  int       cutOut  = cutItem.cutOutIndex();

    mpCutStream = cutItem.avDataItem()->videoStream();

	  mpCutParams->setCutInIndex(cutIn);
	  mpCutParams->setCutOutIndex(cutOut);

		log->debugMsg(__FILE__, __LINE__,	QString("VideoCut %1/%2 start %3 end %4").
        arg(i).arg(mpCutList->count()).arg(cutIn).arg(cutOut));
		log->debugMsg(__FILE__, __LINE__, QString("VideoCut length %1").
        arg((cutOut - cutIn + 1) * 1000.0 / 25.0));

		if (i == 0)
      mpCutParams->firstCall();

    mpCutTask->init(mpCutStream, mpCutParams);
    mpAVData->threadTaskPool()->start(mpCutTask, true);

		if (i == mpCutList->count() - 1)
		  mpCutParams->lastCall();

    onStatusReport(this, StatusReportArgs::Step, QString(tr("Cut %1 from %2")).arg(i+1).arg(mpCutList->count()), i+1);
	}

  log->debugMsg(__FILE__, __LINE__, QString("close target stream %1").arg(mTgtFilePath));

  delete mpTgtStream;

  qDebug("cut video task -> emit finished signal!");
  emit finished(mMuxListItem);
}

/**
 * Video cut task
 */
TTCutTask::TTCutTask() : TTThreadTask("CutTask")
{
  mpCutStream    = 0;
  mpCutParameter = 0;
}

/**
 * Init the video cut task
 */
void TTCutTask::init(TTVideoStream* cutStream, TTCutParameter* cutParameter)
{
  mpCutStream    = cutStream;
  mCutIn         = cutParameter->getCutInIndex();
  mCutOut        = cutParameter->getCutOutIndex();
  mpCutParameter = cutParameter;
}

/**
 * Clean up after task operation
 */
void TTCutTask::cleanUp()
{
  if (mpCutStream == 0) return;

  disconnect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
	    			 this,        SLOT(onStatusReport(int, const QString&, quint64)));
}

/**
 * User abort request
 */
void TTCutTask::onUserAbort()
{
  abort();

  if (mpCutStream != 0)
    mpCutStream->setAbort(true);
}

/**
 * Task operation
 */
void TTCutTask::operation()
{
  if (mpCutStream == 0)
    throw new TTInvalidOperationException(__FILE__, __LINE__, tr("No cut stream specified!"));

	connect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
	  			this,        SLOT(onStatusReport(int, const QString&, quint64)));

  mpCutStream->cut(mCutIn, mCutOut, mpCutParameter);
}



