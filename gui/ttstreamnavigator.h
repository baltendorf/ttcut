/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttstreamnavigator.h                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/03/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// *** TTSTREAMNAVIGATOR
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

#ifndef TTSTREAMNAVIGATOR_H
#define TTSTREAMNAVIGATOR_H

#include "ui_streamnavigationwidget.h"

class TTAVItem;

class TTStreamNavigator : public QWidget, public Ui::TTStreamNavigatorWidget
{
  Q_OBJECT

  public:
    TTStreamNavigator(QWidget* parent);

    void setTitle(const QString& title);
    void controlEnabled(bool enabled);

    QSlider* slider();

  public slots:
    void onNewSliderValue(int value);
    void onSliderMoved(int value);
    void onRefreshDisplay();
    void onAVItemChanged(TTAVItem* avDataItem);

  signals:
    void sliderValueChanged(int value);
};

#endif //TTSTREAMNAVIGATOR_H
