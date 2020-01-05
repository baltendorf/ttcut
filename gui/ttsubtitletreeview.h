/*----------------------------------------------------------------------------*/
/* COPYRIGHT: Minei3oat (c) 2019 / github.com/Minei3oat                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2019                                                      */
/* FILE     : ttsubtitletreeview.cpp                                          */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : Minei3oat                                       DATE: 12/30/2019 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTSUBTITLEFILELIST
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

#ifndef TTSUBTITLEFILELIST_H
#define TTSUBTITLEFILELIST_H

#include "ui_subtitlefilelistwidget.h"

#include "common/ttcut.h"

class TTAVData;
class TTAVItem;
class TTSubtitleItem;

class QMenu;
class QAction;

class TTSubtitleTreeView : public QWidget, Ui::TTSubtitleFileListWidget
{
  Q_OBJECT

  public:
    TTSubtitleTreeView(QWidget* parent=0);

    void setTitle (const QString& title);
    void setControlEnabled(bool enabled);
    void clear();

  signals:
    void openFile();
    void removeItem(int index);
    void swapItems(int oldIndex, int newIndex);

  public slots:
    void onAVDataChanged(const TTAVItem* avData);
    void onItemUp();
    void onItemDown();
    void onRemoveItem();
    void onItemRemoved(int index);
    void onClearList();
    void onContextMenuRequest(const QPoint& point);
    void onAppendItem(const TTSubtitleItem& item);
    void onSwapItems(int oldIndex, int newIndex);
    void onReloadList(const TTAVItem* avData);

  private:
    void createActions();

  private:
    const TTAVItem* mpAVItem;
    QMenu*        contextMenu;
    QAction*      itemUpAction;
    QAction*      itemDownAction;
    QAction*      itemDeleteAction;
    QAction*      itemNewAction;
};
#endif
