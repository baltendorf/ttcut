/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutaudiotask.cpp                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/16/2009 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTAUDIOTASK
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

#include "ttcutaudiotask.h"

#include "../common/ttexception.h"
#include "../avstream/ttfilebuffer.h"
#include "../data/ttcutparameter.h"
#include "../data/ttcutlist.h"
#include "../data/ttavdata.h"
#include "../avstream/ttavstream.h"

//! Cut audio stream task
TTCutAudioTask::TTCutAudioTask(QString tgtFilePath, TTCutList* cutList, int srcAudioIndex, TTMuxListDataItem* muxListItem) :
                TTThreadTask("CutAudioTask")
{
  mTgtFilePath   = tgtFilePath;
  mpCutList      = cutList;
  mpCutStream    = 0;
  mSrcAudioIndex = srcAudioIndex;
  mMuxListItem   = muxListItem;
}

//! Operation abort request
void TTCutAudioTask::onUserAbort()
{
  if (mpCutStream == 0) {
    emit aborted(this);
    return;
  }

   mpCutStream->setAbort(true);
}

//! Clean up after operation
void TTCutAudioTask::cleanUp()
{
  if (mpTgtStream != 0) delete mpTgtStream;
  //if (mpCutParams != 0) delete mpCutParams;
}

//! Task operation method
void TTCutAudioTask::operation()
{
  mpTgtStream = new TTFileBuffer(mTgtFilePath, QIODevice::WriteOnly);
	mpCutParams = new TTCutParameter(mpTgtStream);

	float localAudioOffset = 0.0f;

	mpTgtStream->open();

  for (int i = 0; i < mpCutList->count(); i++) {
    TTCutItem cutItem   = mpCutList->at(i);
    float     frameRate = cutItem.avDataItem()->videoStream()->frameRate();

    mpCutStream = cutItem.avDataItem()->audioStreamAt(mSrcAudioIndex);

    connect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
        this,        SLOT(onStatusReport(int, const QString&, quint64)));

    int startIndex = mpCutStream->getStartIndex(cutItem.cutInIndex(),
        frameRate, localAudioOffset);
    int endIndex = mpCutStream->getEndIndex(cutItem.cutOutIndex(),
        frameRate, localAudioOffset);

    log->debugMsg(__FILE__, __LINE__,	QString("AudioCut %1/%2 start %3 end %4").
        arg(i).arg(mpCutList->count()).arg(startIndex).arg(endIndex));

    if (i == 0)
      mpCutParams->firstCall();

    mpCutStream->cut(startIndex, endIndex, mpCutParams);

    if (i == mpCutList->count() - 1)
      mpCutParams->lastCall();

    disconnect(mpCutStream, SIGNAL(statusReport(int, const QString&, quint64)),
               this,        SLOT(onStatusReport(int, const QString&, quint64)));
  }

  mMuxListItem->appendAudioFile(mTgtFilePath);
  mpTgtStream->close();
  emit finished(mpCutStream->filePath());
}


