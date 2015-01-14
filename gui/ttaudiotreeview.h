/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttaudiofilelist.h                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/28/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAUDIOFILELIST
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

#ifndef TTAUDIOFILELIST_H
#define TTAUDIOFILELIST_H

#include "ui_audiofilelistwidget.h"

#include "common/ttcut.h"

class TTAVData;
class TTAVItem;
class TTAudioItem;

class QMenu;
class QAction;

class TTAudioTreeView : public QWidget, Ui::TTAudioFileListWidget
{
  Q_OBJECT

  public:
    TTAudioTreeView(QWidget* parent=0);

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
    void onAppendItem(const TTAudioItem& item);
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
