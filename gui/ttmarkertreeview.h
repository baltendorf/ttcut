/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmarkertreeview.h                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 12/18/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMARKERTREEVIEW
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

#ifndef TTMARKERTREEVIEW_H
#define TTMARKERTREEVIEW_H

#include "ui_markerlistwidget.h"

#include "common/ttcut.h"
#include "data/ttavdata.h"

class TTAVItem;
class TTMarkerItem;

class QMenu;
class QAction;

class TTMarkerTreeView : public QWidget, Ui::TTMarkerListWidget
{
  Q_OBJECT

  public:
    TTMarkerTreeView(QWidget* parent=0);

    void controlEnabled(bool enabled);
    void setAVData(TTAVData* avData);
    void clear();

  signals:
    void removeItem(const TTMarkerItem& mItem);
    void itemOrderChanged(int oldIndex, int newIndex);
    void itemUpdated(const TTMarkerItem& mItem);
    void refreshDisplay();
    void swapItems(int oldIndex, int newIndex);
    void gotoMarker(const TTMarkerItem& mItem);

  public slots:
		void onActivateMarker();
    void onItemUp();
    void onItemDown();
    void onRemoveItem();
    void onItemRemoved(int index);
    void onClearList();
    void onContextMenuRequest(const QPoint& point);
    void onAppendItem(const TTMarkerItem& item);
    void onUpdateItem(const TTMarkerItem& mItem, const TTMarkerItem& uItem);
    void onSwapItems(int oldIndex, int newIndex);
    void onReloadList();

  private:
  	QTreeWidgetItem* findItem(const TTMarkerItem& cutItem);
    void createActions();

  private:
  	TTAVData* mpAVData;
    QMenu*        contextMenu;
    QAction*      itemUpAction;
    QAction*      itemDownAction;
    QAction*      itemDeleteAction;
    QAction*      itemNewAction;
};
#endif
