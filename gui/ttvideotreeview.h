/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttvideotreeview.h                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 12/07/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTVIDEOTREEVIEW
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

#ifndef TTVIDEOTREEVIEW_H
#define TTVIDEOTREEVIEW_H

#include "ui_videofilelistwidget.h"

#include "../common/ttcut.h"

class TTAVData;
class TTAVItem;

class QMenu;
class QAction;

class TTVideoTreeView : public QWidget, Ui::TTVideoFileListWidget
{
  Q_OBJECT

  public:
    TTVideoTreeView(QWidget* parent=0);

    void clear();
    void setTitle (const QString& title);
    void setAVData(TTAVData* avData);
    void setControlEnabled(bool enabled);

  signals:
    void openFile();
    void selectionChanged(int index);
    void removeItem(int index);
    void swapItems(int oldIndex, int newIndex);

  public slots:
    void onItemSelectionChanged();
    void onItemUp();
    void onItemDown();
    void onRemoveItem();
    void onItemRemoved(int index);
    void onItemsSwapped(int oldIndex, int newIndex);
    void onClearList();
    void onContextMenuRequest(const QPoint& point);
    void onAppendItem(const TTAVItem& item);
    void onReloadList();

  private:
    void createActions();

  private:
    TTAVData* mAVData;
    bool      allowSelectionChanged;
    QMenu*    contextMenu;
    QAction*  itemUpAction;
    QAction*  itemDownAction;
    QAction*  itemDeleteAction;
    QAction*  itemNewAction;
};

#endif
