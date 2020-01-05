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

#ifndef TTCUTSUBTITLETASK_H
#define TTCUTSUBTITLETASK_H

#include "common/ttthreadtask.h"
#include "data/ttmuxlistdata.h"

class TTFileBuffer;
class TTCutParameter;
class TTSubtitleStream;
class TTCutList;

//! Runable task for cuttting subtitle streams
class TTCutSubtitleTask : public TTThreadTask
{
  Q_OBJECT

  public:
    TTCutSubtitleTask();
    void init(QString tgtFilePath, TTCutList* cutList, int srcSubtitleIndex, TTMuxListDataItem* muxListItem);

  protected:
    void cleanUp();
    void operation();

  public slots:
    void onUserAbort();

  signals:
    void finished(QString filePath);

  private:
    QString            mTgtFilePath;
    TTCutList*         mpCutList;
    int                mSrcSubtitleIndex;
    TTFileBuffer*      mpTgtStream;
    TTCutParameter*    mpCutParams;
    TTSubtitleStream*  mpCutStream;
    TTMuxListDataItem* mMuxListItem;
};

#endif
