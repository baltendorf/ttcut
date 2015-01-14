/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttopenvideotask.h                                               */
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

#ifndef TTOPENVIDEOTASK_H
#define TTOPENVIDEOTASK_H

#include <QString>
#include <QFileInfo>

#include "common/ttthreadtask.h"

class TTVideoType;
class TTVideoStream;
class TTAVItem;

//! Runable task for opening video streams
class TTOpenVideoTask : public TTThreadTask
{
	Q_OBJECT

	public:
		TTOpenVideoTask(TTAVItem* avItem, QString fileName, int order);

  protected:
    void operation();
    void cleanUp();

	public slots:
		void onUserAbort();

  signals:
    void finished(TTAVItem*, TTVideoStream*, int);

	private:
    TTAVItem*      mpAVItem;        /**<AV Data item            */
    int            mOrder;          /**<AV item order in list   */
		QString        mFileName;       /**<Stream file name        */
    TTVideoType*   mpVideoType;     /**<Stream video type       */
		TTVideoStream* mpVideoStream;   /**<Stream object reference */
};

#endif
