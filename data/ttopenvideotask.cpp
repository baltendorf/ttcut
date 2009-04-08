/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttopenvideotask.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/16/2009 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTOPENVIDEOTASK
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

#include "ttopenvideotask.h"

#include "../common/ttexception.h"
#include "../data/ttavlist.h"
#include "../avstream/ttmpeg2videostream.h"

//! Open video stream task
TTOpenVideoTask::TTOpenVideoTask(TTAVItem* avItem, QString fileName, int order) :
                 TTThreadTask("OpenVideoTask")
{
  mpAVItem      = avItem;
  mOrder        = order;
	mFileName     = fileName;
	mpVideoStream = 0;
}

//! Request for aborting current operation
void TTOpenVideoTask::onUserAbort()
{
  qDebug("TTOpenVideoTask::onUserAbort called");
	if (mpVideoStream == 0) {
    qDebug("no video stream allocated!");
		//throw new TTAbortException("OpenVideoTask aborted!");
    emit aborted(this);
    return;
  }

	if (!mpAVItem->isInList())
		delete mpAVItem;

  qDebug("TTOpenVideoTask::onUserAbort -> setAbort");
	mpVideoStream->setAbort(true);

  qDebug("after setAbort..");
}

//! Clean up after operation
void TTOpenVideoTask::cleanUp()
{
  if (mpVideoType   != 0) delete mpVideoType;
  if (mpVideoStream == 0) return;

  disconnect(mpVideoStream, SIGNAL(statusReport(int, const QString&, quint64)),
 					   this,          SLOT(onStatusReport(int, const QString&, quint64)));
}

//! Task operation method
void TTOpenVideoTask::operation()
{
	QFileInfo fInfo(mFileName);

	if (!fInfo.exists()) {
		QString err = QString(tr("file %1 does not exists!")).arg(fInfo.filePath());
		log->errorMsg(__FILE__, __LINE__, err);
		throw new TTFileNotFoundException(err);
	}

	mpVideoType = new TTVideoType(fInfo.absoluteFilePath());

	if (mpVideoType->avStreamType() != TTAVTypes::mpeg2_demuxed_video) {
		QString err = QString(tr("unsupported video type %1")).arg(fInfo.filePath());
		log->errorMsg(__FILE__, __LINE__, err);
		throw new TTDataFormatException(err);
	}

	mpVideoStream = new TTMpeg2VideoStream(fInfo);

	connect(mpVideoStream, SIGNAL(statusReport(int, const QString&, quint64)),
					this,          SLOT(onStatusReport(int, const QString&, quint64)));

  qDebug("start creating header and index lists");
  mpVideoStream->createHeaderList();
 	mpVideoStream->createIndexList();

	mpVideoStream->indexList()->sortDisplayOrder();

  qDebug("emit finished signal...");
	emit finished(mpAVItem, mpVideoStream, mOrder);
}

