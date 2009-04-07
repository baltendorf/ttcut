/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttopenaudiotask.h                                               */
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

#include "../common/ttthreadtask.h"

#include <QString>
#include <QFileInfo>

class TTAudioType;
class TTAudioStream;
class TTAVItem;

//! Runable task for opening audio streams
class TTOpenAudioTask : public TTThreadTask
{
	Q_OBJECT;

	public:
		TTOpenAudioTask(TTAVItem* avItem, QString filePath, int order);

  protected:
    void cleanUp();
    void operation();

	public slots:
		void onUserAbort();

	signals:
		void finished(TTAVItem*, TTAudioStream*, int);

	private:
    TTAVItem*      mpAVItem;
    int            mOrder;
		QString        mFilePath;
		TTAudioStream* mpAudioStream;
    TTAudioType*   mpAudioType;
};
