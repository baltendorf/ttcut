/*----------------------------------------------------------------------------*/
/* COPYRIGHT: Minei3oat (c) 2019 / github.com/Minei3oat                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2019                                                      */
/* FILE     : ttopensubtitletask.cpp                                          */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : Minei3oat                                       DATE: 12/30/2019 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTOPENSUBTITLETASK
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

#include "ttopensubtitletask.h"

#include "common/ttmessagelogger.h"
#include "common/ttexception.h"
#include "avstream/ttavtypes.h"
#include "avstream/ttavstream.h"
#include "data/ttavlist.h"

#include <QDebug>

/**
 * Open subtitle stream task
 */
TTOpenSubtitleTask::TTOpenSubtitleTask(TTAVItem* avItem, QString filePath, int order) :
                    TTThreadTask("OpenSubtitleTask")
{
  mpAVItem         = avItem;
  mOrder           = order;
  mFilePath        = filePath;
  mpSubtitleType   = 0;
  mpSubtitleStream = 0;
}

/**
 * Operation abort request
 */
void TTOpenSubtitleTask::onUserAbort()
{
  abort();

  if (mpSubtitleStream != 0)
  {
    qDebug() << "open subtitle stream abort; subtitleStream is not null";
    mpSubtitleStream->setAbort(true);
  }
}

/**
 * Clean up after operation
 */
void TTOpenSubtitleTask::cleanUp()
{
  if (mpSubtitleType   != 0) delete mpSubtitleType;
  if (mpSubtitleStream == 0) return;

  disconnect(mpSubtitleStream, SIGNAL(statusReport(int, const QString&, quint64)),
             this,             SLOT(onStatusReport(int, const QString&, quint64)));
}

/**
 * Task operation method
 */
void TTOpenSubtitleTask::operation()
{  
  try
  {
    mpSubtitleType = new TTSubtitleType(mFilePath);
  }
  catch (TTException* ex)
  {
    throw new TTException(__FILE__, __LINE__, QString(tr("Unsupported subtitle type or file not found %1!")).arg(mFilePath));
  }

  if (mpSubtitleType->avStreamType() != TTAVTypes::srt_subtitle)
    throw new TTException(__FILE__, __LINE__, QString(tr("Unsupported subtitle type %1!")).arg(mFilePath));

  mpSubtitleStream = (TTSubtitleStream*) mpSubtitleType->createSubtitleStream();

  qDebug("connect subtitle stream step signal");
  connect(mpSubtitleStream, SIGNAL(statusReport(int, const QString&, quint64)),
          this,             SLOT(onStatusReport(int, const QString&, quint64)));
  qDebug("create subtitle stream header list");
  mpSubtitleStream->createHeaderList();

  emit finished(mpAVItem, mpSubtitleStream, mOrder);
}

