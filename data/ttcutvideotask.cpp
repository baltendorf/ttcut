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

/**
 * Cut video stream task
 */
TTCutVideoTask::TTCutVideoTask(TTAVData* avData, QString tgtFilePath, TTCutList* cutList) :
                TTThreadTask("CutVideoTask")
{
  mpAVData     = avData;
  mTgtFilePath = tgtFilePath;
  mpCutList    = cutList;
  mpCutStream  = 0;
  mpCutTask    = new TTCutTask();

  mMuxListItem.setVideoName(tgtFilePath);
}

/**
 * Operation abort request
 */
void TTCutVideoTask::onUserAbort()
{
  if (mpCutStream == 0) {
    emit aborted(this);
    return;
  }

   mpCutStream->setAbort(true);
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
 	mpTgtStream = new TTFileBuffer(mTgtFilePath, QIODevice::WriteOnly);
  mpCutParams = new TTCutParameter(mpTgtStream);

	mpCutParams->setIsWriteSequenceEnd(true);
	mpTgtStream->open();

  onStatusReport(this, StatusReportArgs::Start, tr("Do video cut"), mpCutList->count());

  for (int i = 0; i < mpCutList->count(); i++) {
	  TTCutItem cutItem   = mpCutList->at(i);
	  int       cutIn     = cutItem.cutInIndex();
	  int       cutOut    = cutItem.cutOutIndex();


    mpCutStream = cutItem.avDataItem()->videoStream();

	  mpCutParams->setCutInIndex(cutIn);
	  mpCutParams->setCutOutIndex(cutOut);

		log->debugMsg(__FILE__, __LINE__,	QString("VideoCut %1/%2 start %3 end %4").
        arg(i).arg(mpCutList->count()).arg(cutIn).arg(cutOut));
		log->debugMsg(__FILE__, __LINE__, QString("VideoCut length %1").
        arg((cutOut - cutIn + 1) * 1000.0 / 25.0));

		//connect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
		//  			this,        SLOT(onStatusReport(int, const QString&, quint64)));

		if (i == 0)
      mpCutParams->firstCall();

		//FIXME: cutIn and cutOut are redundant in cutParams ?-)
		//mpCutStream->cut(cutIn, cutOut, mpCutParams);

    mpCutTask->init(mpCutStream, cutIn, cutOut, mpCutParams);
    mpAVData->threadTaskPool()->start(mpCutTask, true);

		if (i == mpCutList->count() - 1)
		  mpCutParams->lastCall();

    onStatusReport(this, StatusReportArgs::Step, QString(tr("Cut %1 from %2")).arg(i+1).arg(mpCutList->count()), i+1);
    //disconnect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
	  //    			 this,        SLOT(onStatusReport(int, const QString&, quint64)));
	}

  log->debugMsg(__FILE__, __LINE__, QString("close target stream %1").arg(mTgtFilePath));

  delete mpTgtStream;
  emit finished(mMuxListItem);
}


TTCutTask::TTCutTask() : TTThreadTask("CutTask")
{
  mpCutStream    = 0;
  mpCutParameter = 0;
}

void TTCutTask::init(TTVideoStream* cutStream, int cutIn, int cutOut, TTCutParameter* cutParameter)
{
  mpCutStream    = cutStream;
  mCutIn         = cutIn;
  mCutOut        = cutOut;
  mpCutParameter = cutParameter;
}

void TTCutTask::cleanUp()
{
}

void TTCutTask::onUserAbort()
{
}

void TTCutTask::operation()
{
  if (mpCutStream == 0) return;

	connect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
	  			this,        SLOT(onStatusReport(int, const QString&, quint64)));

  mpCutStream->cut(mCutIn, mCutOut, mpCutParameter);

  disconnect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
	    			 this,        SLOT(onStatusReport(int, const QString&, quint64)));
}



