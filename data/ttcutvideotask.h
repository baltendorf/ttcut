/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutvideotask.h                                                */
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

#ifndef TTCUTVIDEOTASK_H
#define TTCUTVIDEOTASK_H

#include "../common/ttthreadtask.h"
#include "../data/ttmuxlistdata.h"

class TTAVData;
class TTFileBuffer;
class TTCutParameter;
class TTVideoStream;
class TTCutList;
class TTCutTask;

//! Runable task for cutting video streams
class TTCutVideoTask : public TTThreadTask
{
  Q_OBJECT

  public:
    TTCutVideoTask(TTAVData* avData);
    void init(QString tgtFilePath, TTCutList* cutList);
    TTMuxListDataItem* muxListItem();

  protected:
    void cleanUp();
    void operation();

  public slots:
    void onUserAbort();

  signals:
    void finished(const TTMuxListDataItem& muxListItem);

  private:
    TTAVData*         mpAVData;
    QString           mTgtFilePath;
    TTMuxListDataItem mMuxListItem;
    TTCutList*        mpCutList;
    TTFileBuffer*     mpTgtStream;
    TTCutParameter*   mpCutParams;
    TTVideoStream*    mpCutStream;
    TTCutTask*        mpCutTask;
};

class TTCutTask : public TTThreadTask
{
  Q_OBJECT

  public:
    TTCutTask();
    void init(TTVideoStream* cutStream, TTCutParameter* cutParameter);

  protected:
    void cleanUp();;
    void operation();

  public slots:
    void onUserAbort();

  private:  
    int             mCutIn;
    int             mCutOut;
    TTVideoStream*  mpCutStream;
    TTCutParameter* mpCutParameter;
};
#endif
