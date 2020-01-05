/*----------------------------------------------------------------------------*/
/* COPYRIGHT: Minei3oat (c) 2019 / github.com/Minei3oat                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2019                                                      */
/* FILE     : ttcutsubtitletask.cpp                                           */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : Minei3oat                                       DATE: 12/30/2019 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTSUBTITLETASK
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

#include "ttcutsubtitletask.h"

#include "common/ttexception.h"
#include "avstream/ttfilebuffer.h"
#include "data/ttcutparameter.h"
#include "data/ttcutlist.h"
#include "data/ttavdata.h"
#include "avstream/ttavstream.h"

//! Cut subtitle stream task
TTCutSubtitleTask::TTCutSubtitleTask() :
                   TTThreadTask("CutSubtitleTask")
{
  mpCutList         = 0;
  mpCutStream       = 0;
  mSrcSubtitleIndex = 0;
  mMuxListItem      = 0;
}

/**
 * Init cut subtitle task
 */
void TTCutSubtitleTask::init(QString tgtFilePath, TTCutList* cutList, int srcSubtitleIndex, TTMuxListDataItem* muxListItem)
{
  mTgtFilePath      = tgtFilePath;
  mpCutList         = cutList;
  mpCutStream       = 0;
  mSrcSubtitleIndex = srcSubtitleIndex;
  mMuxListItem      = muxListItem;
}

//! Operation abort request
void TTCutSubtitleTask::onUserAbort()
{
  if (mpCutStream == 0) 
    throw new TTAbortException(QString("Task %1 with UUID %2 aborted").arg(taskName()).arg(taskID().toString()));

   mpCutStream->setAbort(true);
}

//! Clean up after operation
void TTCutSubtitleTask::cleanUp()
{
  if (mpTgtStream != 0) delete mpTgtStream;
  //if (mpCutParams != 0) delete mpCutParams;
}

//! Task operation method
void TTCutSubtitleTask::operation()
{
  if (mTgtFilePath.isEmpty())
    throw new TTInvalidOperationException(tr("No target file path given for subtitle cut!"));

  mpTgtStream = new TTFileBuffer(mTgtFilePath, QIODevice::WriteOnly);
  mpCutParams = new TTCutParameter(mpTgtStream);
  mpCutParams->setNumPicturesWritten(0);
  mpCutParams->setCutInIndex(0);
  mpCutParams->setCutOutIndex(0);

  mpTgtStream->open();

  for (int i = 0; i < mpCutList->count(); i++) {
    TTCutItem cutItem   = mpCutList->at(i);

    mpCutStream = cutItem.avDataItem()->subtitleStreamAt(mSrcSubtitleIndex);

    connect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
            this,        SLOT(onStatusReport(int, const QString&, quint64)));

    log->debugMsg(__FILE__, __LINE__,	QString("SubtitleCut %1/%2 start %3 end %4").
        arg(i).arg(mpCutList->count()).arg(cutItem.cutInTime().toString("hh:mm:ss.zzz")).arg(cutItem.cutOutTime().toString("hh:mm:ss.zzz")));

    if (i == 0)
      mpCutParams->firstCall();

    mpCutStream->cut(cutItem.cutInTime().msecsSinceStartOfDay(), cutItem.cutInTime().msecsSinceStartOfDay()+cutItem.cutLengthTime().msecsSinceStartOfDay()-1, mpCutParams);

    mpCutParams->setCutInIndex(mpCutParams->getCutOutIndex()+1);

    if (i == mpCutList->count() - 1)
      mpCutParams->lastCall();

    disconnect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
               this,        SLOT(onStatusReport(int, const QString&, quint64)));
  }

  mMuxListItem->appendSubtitleFile(mTgtFilePath);
  mpTgtStream->close();
  emit finished(mpCutStream->filePath());
}


