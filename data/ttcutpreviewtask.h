/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutpreviewtask.h                                              */
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

#ifndef TTCUTPREVIEWTASK_H
#define TTCUTPREVIEWTASK_H

#include "common/ttthreadtask.h"

class TTAVData;
class TTCutList;
class TTCutVideoTask;
class TTCutAudioTask;

//! Runable task for creating cut preview clips
class TTCutPreviewTask : public TTThreadTask
{
	Q_OBJECT

	public:
		TTCutPreviewTask(TTAVData* avData, TTCutList* cutList);

		static QString createPreviewFileName(int index, QString extension);

	protected:
    void cleanUp();
    void operation();

  public slots:
		void onUserAbort();

  signals:
    void finished(TTCutList* cutList);

	private:
		TTCutList* createPreviewCutList(TTCutList* cutList);

	private:
		TTAVData*       mpAVData;
		TTCutList*      mpCutList;
		TTCutList*      mpPreviewCutList;
		TTCutVideoTask* cutVideoTask;
		TTCutAudioTask* cutAudioTask;
};


#endif
