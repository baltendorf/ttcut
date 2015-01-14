/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutoutframe.h                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/19/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTOUTFRAME
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

#ifndef TTCUTOUTFRAME_H

#include "ui_h/ui_cutoutframewidget.h"
#include "avstream/ttmpeg2videostream.h"
#include "data/ttcutlist.h"
#include "ttprogressbar.h"

class TTAVItem;

class TTCutOutFrame: public QWidget, Ui::TTCutOutFrameWidget
{
	Q_OBJECT

	public:
		TTCutOutFrame(QWidget* parent = 0);
		~TTCutOutFrame();

		void setTitle(const QString & title);
		void controlEnabled(bool enabled);
		int  currentFramePos();
		void closeVideoStream();

	public slots:
		void onAVDataChanged(TTAVItem* avData);
		void onCutOutChanged(const TTCutItem& cutItem);
		void onGotoCutOut(int pos);
		void onPrevCutOutPos();
		void onNextCutOutPos();
		void onSearchFrame();

	signals:
	  void searchEqualFrame(TTAVItem* avItem, int startIndex);

	private:
		void updateCurrentPosition();

	private:
		TTAVItem*           currentAVItem;
		//const TTCutItem*    currentCutItem;
		int                 currentCutItemIndex;
		TTMpeg2VideoStream* mpeg2Stream;
		TTMpeg2VideoStream* currentMpeg2Stream;
		int                 currentPosition;
		bool                isCutOut;
};

#endif //TTCUTOUTFRAME_H
