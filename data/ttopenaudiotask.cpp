/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttopenaudiotask.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/16/2009 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTOPENAUDIOTASK
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

#include "ttopenaudiotask.h"

#include "../common/ttmessagelogger.h"
#include "../common/ttexception.h"
#include "../avstream/ttavtypes.h"
#include "../avstream/ttavstream.h"
#include "../data/ttavlist.h"

/**
 * Open audio stream task
 */
TTOpenAudioTask::TTOpenAudioTask(TTAVItem* avItem, QString filePath, int order) :
                 TTThreadTask("OpenAudioTask")
{
  mpAVItem      = avItem;
  mOrder        = order;
	mFilePath     = filePath;
  mpAudioType   = 0;
	mpAudioStream = 0;
}

/**
 * Operation abort request
 */
void TTOpenAudioTask::onUserAbort()
{
  abort();

	if (mpAudioStream != 0)
    mpAudioStream->setAbort(true);
}

/**
 * Clean up after operation
 */
void TTOpenAudioTask::cleanUp()
{
  if (mpAudioType   != 0) delete mpAudioType;
  if (mpAudioStream == 0) return;

	disconnect(mpAudioStream, SIGNAL(statusReport(int, const QString&, quint64)),
			   	   this,          SLOT(onStatusReport(int, const QString&, quint64)));
}

/**
 * Task operation method
 */
void TTOpenAudioTask::operation()
{
	try
	{
		mpAudioType = new TTAudioType(mFilePath);
	}
	catch (TTException* ex)
	{
    throw new TTException(__FILE__, __LINE__, QString(tr("Unsupported audio type or file not found %1!")).arg(mFilePath));
	}

	if (mpAudioType->avStreamType() != TTAVTypes::mpeg_audio &&
			mpAudioType->avStreamType() != TTAVTypes::ac3_audio) 
    throw new TTException(__FILE__, __LINE__, QString(tr("Unsupported audio type %1!")).arg(mFilePath));

	mpAudioStream = (TTAudioStream*) mpAudioType->createAudioStream();

  connect(mpAudioStream, SIGNAL(statusReport(int, const QString&, quint64)),
				  this,          SLOT(onStatusReport(int, const QString&, quint64)));

	mpAudioStream->createHeaderList();

	emit finished(mpAVItem, mpAudioStream, mOrder);
}

