/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttnavigatordisplay.cpp                                          */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/03/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTNAVIGATORDISPLAY
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

#include "ttnavigatordisplay.h"

#include "../common/ttcut.h"
#include "../avstream/ttavstream.h"
#include "../data/ttavlist.h"
#include "../data/ttcutlist.h"

#include <QPainter>

/*!
 * TTNavigatorDisplay
 */
TTNavigatorDisplay::TTNavigatorDisplay(QWidget* parent)
  :QFrame(parent)
{
  setupUi( this );

  mAVDataItem      = 0;
  isControlEnabled = false;
}

/*!
 * controlEnabled
 */
void TTNavigatorDisplay::controlEnabled(bool enabled)
{
  isControlEnabled = enabled;
}

/*!
 * resizeEvent
 */
void TTNavigatorDisplay::resizeEvent(QResizeEvent*)
{
  scaleFactor = navigatorDisplay->geometry().width() / (double)(maxValue-minValue);
}

/*!
 * paintEvent
 */
void TTNavigatorDisplay::paintEvent(QPaintEvent*)
{
  if (mAVDataItem != 0 && isControlEnabled)
    drawCutList();
}

/*!
 * drawCutList
 */
void TTNavigatorDisplay::drawCutList()
{
  int   cutIn;
  int   cutOut;
  int   cutWidth;
  QRect clientRect = navigatorDisplay->geometry();
  int   startX = clientRect.x();
  int   startY = clientRect.y();
  int   height = clientRect.height();
  int   width  = 0;

  scaleFactor = navigatorDisplay->geometry().width() / (double)(maxValue-minValue);

  QPainter painter(this);

  painter.fillRect(clientRect, QBrush(Qt::gray));

  if (mAVDataItem == 0) return;

  for (int i = 0; i < mAVDataItem->cutCount(); i++) {
      TTCutItem item = mAVDataItem->cutListItemAt(i);

      cutIn    = item.cutInIndex();
      cutOut   = item.cutOutIndex();
      cutWidth = cutOut - cutIn;
      startX   = clientRect.x() + (int)(cutIn*scaleFactor);
      width    = (int)((cutOut-cutIn)*scaleFactor);

      painter.fillRect(startX, startY, width, height, QBrush(Qt::green));
  }
}

/*!
 * onAVItemChanged
 */
void TTNavigatorDisplay::onAVItemChanged(TTAVItem* avDataItem)
{
	if (avDataItem == 0) {
		mAVDataItem      = 0;
		minValue         = 0;
		maxValue         = 0;
		isControlEnabled = false;
		return;
	}

  minValue         = 0;
  maxValue         = avDataItem->videoStream()->frameCount()-1;
  mAVDataItem      = avDataItem;
  isControlEnabled = true;

  repaint();
}


