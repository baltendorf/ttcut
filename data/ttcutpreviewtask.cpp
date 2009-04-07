/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutpreviewtask.cpp                                            */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 01/24/2009 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTPREVIEWTASK
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

#include "ttcutpreviewtask.h"

#include "../common/ttexception.h"
#include "../common/ttthreadtaskpool.h"
#include "../common/istatusreporter.h"
#include "../avstream/ttfilebuffer.h"
#include "../data/ttcutparameter.h"
#include "../data/ttcutlist.h"
#include "../data/ttavdata.h"
#include "../avstream/ttavstream.h"
#include "../data/ttcutaudiotask.h"
#include "../data/ttcutvideotask.h"
#include "../data/ttmuxlistdata.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

//! Create cut preview clips task
TTCutPreviewTask::TTCutPreviewTask(TTAVData* avData, TTCutList* cutList) :
                  TTThreadTask("CutPreviewTask")
{
	mpAVData  = avData;
	mpCutList = cutList;
	mAbort    = false;
}

//! Operation abort requested
void TTCutPreviewTask::onUserAbort()
{
	qDebug("TTCutPreviewTask::onUserAbort");
	if (cutVideoTask != 0) cutVideoTask->onUserAbort();
	if (cutAudioTask != 0) cutAudioTask->onUserAbort();
	mAbort = true;
}

//! Clean up after operation
void TTCutPreviewTask::cleanUp()
{
}

//! Task operation method
void TTCutPreviewTask::operation()
{
  mpPreviewCutList = createPreviewCutList(mpCutList);

	int  iPos;
	bool hasAudio   = false;
	int  numPreview = mpPreviewCutList->count() / 2 + 1;

	emit statusReport(this, StatusReportArgs::Start, tr("create cut preview clips"), 2*numPreview);

  for (int i = 0; i < numPreview; i++) {
  	if (mAbort)
  		throw new TTAbortException("User abort request in TTCutPreviewTask!");

    TTCutList* tmpCutList = new TTCutList();

    // first cut-in
    if (i == 0) {
      iPos = i;
      TTCutItem item = mpPreviewCutList->at(i);
      tmpCutList->append(item.avDataItem(), item.cutInIndex(),	item.cutOutIndex());
    }

    // cut i-i
    if (numPreview > 1 && i > 0 && i < numPreview - 1) {
      iPos = (i - 1) * 2 + 1;
      TTCutItem item1 = mpPreviewCutList->at(iPos);
      TTCutItem item2 = mpPreviewCutList->at(iPos + 1);
      tmpCutList->append(item1.avDataItem(), item1.cutInIndex(),	item1.cutOutIndex());
      tmpCutList->append(item2.avDataItem(), item2.cutInIndex(),	item2.cutOutIndex());
    }

    //last cut out
    if (i == numPreview - 1) {
      iPos = (i - 1) * 2 + 1;
      TTCutItem item = mpPreviewCutList->at(iPos);
      tmpCutList->append(item.avDataItem(), item.cutInIndex(),item.cutOutIndex());
    }

    try
    {
    	cutVideoTask = new TTCutVideoTask(createPreviewFileName(i + 1, "m2v"), tmpCutList);
    	mpAVData->threadTaskPool()->start(cutVideoTask, true);

    	if (tmpCutList->at(0).avDataItem()->audioCount() > 0) {
    		hasAudio = true;
    		cutAudioTask = new TTCutAudioTask(createPreviewFileName(i + 1, "mpa"), tmpCutList, 0, cutVideoTask->muxListItem());
    		mpAVData->threadTaskPool()->start(cutAudioTask, true);
    	}
    }
    catch (TTException* ex)
    {
    	delete tmpCutList;
    	throw ex;
    }

    emit statusReport(this, StatusReportArgs::Step, QString("preview cut %1 from %2 created").
        arg(i+1).arg(numPreview), i+1);
    delete tmpCutList;
  }

  for (int i = 0; i < numPreview; i++) {
  	emit statusReport(this, StatusReportArgs::Step, QString("mplexing preview cut %1 from %2").
      arg(i+1).arg(numPreview), i+4);
  	qApp->processEvents();

    QString mplex_command = hasAudio
			? QString("mplex -f 8 -o %1 %2 %3 2>/dev/null").
						arg(createPreviewFileName(i + 1, "mpg")).
						arg(createPreviewFileName(i + 1, "m2v")).
						arg(createPreviewFileName(i + 1, "mpa"))
      : QString("mv %1 %2 2>/dev/null").
						arg(createPreviewFileName(i + 1, "m2v")).
						arg(createPreviewFileName(i + 1, "mpg"));

    system(qPrintable(mplex_command));
  }

  emit statusReport(this, StatusReportArgs::Finished, tr("preview cuts done"), 0);
  emit finished(mpPreviewCutList);
}

//! Creates the preview cut list
TTCutList* TTCutPreviewTask::createPreviewCutList(TTCutList* cutList)
{
	TTVideoStream* vStream        = cutList->at(0).avDataItem()->videoStream();
	TTCutList*     previewCutList = new TTCutList();
	QTime          previewTime;
	long           previewFrames;

	previewTime.setHMS(0, 0, 0);
	previewTime   = previewTime.addSecs(TTCut::cutPreviewSeconds);
	previewFrames = ttTimeToFrames(previewTime, vStream->frameRate()) / 2;

	for (int i = 0; i < cutList->count(); i++) {
		TTCutItem      cutItem      = cutList->at(i);
		TTVideoStream* pVideoStream = cutItem.avDataItem()->videoStream();
		int            startIndex   = cutItem.cutInIndex();
		int            endIndex     = startIndex + previewFrames;

		if (endIndex >= pVideoStream->frameCount())
			endIndex = pVideoStream->frameCount() - 1;

		// cut should end at an I-frame or P-frame
		int frameType = pVideoStream->frameType(endIndex);

		while (frameType == 3 && endIndex < pVideoStream->frameCount() - 1) {
			endIndex++;
			frameType = pVideoStream->frameType(endIndex);
		}

		previewCutList->append(cutItem.avDataItem(), startIndex, endIndex);

		endIndex   = cutItem.cutOutIndex();
		startIndex = (endIndex - previewFrames >= 0) ? endIndex - previewFrames	: 0;

		// cut should start at an I-frame
		frameType = pVideoStream->frameType(startIndex);
		while (frameType != 1 && startIndex > 0) {
			startIndex--;
			frameType = pVideoStream->frameType(startIndex);
		}
		previewCutList->append(cutItem.avDataItem(), startIndex, endIndex);
	}
	return previewCutList;
}

//! Creates the filenames used for preview clips
QString TTCutPreviewTask::createPreviewFileName(int index, QString extension)
{
	 QString   previewFileName;

	 previewFileName.sprintf("preview_%03d.%s", index, qPrintable(extension));

	 return QFileInfo(QDir(TTCut::tempDirPath), previewFileName).absoluteFilePath();
}
