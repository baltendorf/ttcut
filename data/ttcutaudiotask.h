/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutaudiotask.h                                                */
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

#ifndef TTCUTAUDIOTASK_H
#define TTCUTAUDIOTASK_H

#include "../common/ttthreadtask.h"
#include "../data/ttmuxlistdata.h"

class TTFileBuffer;
class TTCutParameter;
class TTAudioStream;
class TTCutList;

//! Runable task for cuttting audio streams
class TTCutAudioTask : public TTThreadTask
{
  Q_OBJECT

  public:
    TTCutAudioTask(QString tgtFilePath, TTCutList* cutList, int srcAudioIndex, TTMuxListDataItem* muxListItem);

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
    int                mSrcAudioIndex;
    TTFileBuffer*      mpTgtStream;
    TTCutParameter*    mpCutParams;
    TTAudioStream*     mpCutStream;
    TTMuxListDataItem* mMuxListItem;
};

#endif
