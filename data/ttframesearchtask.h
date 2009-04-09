/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttframesearchtask.h                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/16/2009 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTFRAMESEARCHTASK
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

#ifndef TTFRAMESEARCHTASK_H
#define TTFRAMESEARCHTASK_H

#include "../common/ttthreadtask.h"
#include "../mpeg2decoder/ttmpeg2decoder.h"

class TTVideoStream;

//! Runable task for frame comparison and searching
class TTFrameSearchTask : public TTThreadTask
{
  Q_OBJECT

  public:
    TTFrameSearchTask(TTVideoStream* referenceStream, int referenceIndex,
                      TTVideoStream* searchStream,    int searchIndex);

  protected:
    void    initFrameSearch();
    quint64 compareFrames(TFrameInfo* frameInfo, quint8* refData, quint8* searchData);
    void    cleanUp();
    void    operation();

  public slots:
    void onUserAbort();

  signals:
    void finished(int index);

  private:
    TTVideoStream*  mpReferenceStream;
    TTVideoStream*  mpSearchStream;
    TFrameInfo*     mpRefFrameInfo;
    int             mReferenceIndex;
    int             mSearchIndex;
    quint8*         mpReferenceData;
    quint8*         mpSearchData;
    bool            mAbort;
};

#endif
