/*----------------------------------------------------------------------------*/
/* COPYRIGHT: Minei3oat (c) 2019 / github.com/Minei3oat                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2019                                                      */
/* FILE     : ttsubtitletreeview.cpp                                          */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : Minei3oat                                       DATE: 12/30/2019 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTSUBTITLETREEVIEW
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


#include "ttsubtitletreeview.h"

#include "data/ttsubtitlelist.h"
#include "data/ttavlist.h"
#include "avstream/ttavstream.h"

#include <QHeaderView>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QAction>

/* /////////////////////////////////////////////////////////////////////////////
 * Construct a new TTSubtitleFileList widget.
 */
TTSubtitleTreeView::TTSubtitleTreeView(QWidget* parent)
  :QWidget(parent)
{
  setupUi( this );

  mpAVItem = 0;

  subtitleListView->setRootIsDecorated(false);
  QHeaderView* header = subtitleListView->header();
  header->resizeSection(0, 320);
  header->resizeSection(1, 220);
  header->resizeSection(2, 140);

  createActions();

  // signal and slot connections
  connect(pbSubtitleFileOpen,    SIGNAL(clicked()), SIGNAL(openFile()));
  connect(pbSubtitleEntryUp,     SIGNAL(clicked()), SLOT(onItemUp()));
  connect(pbSubtitleEntryDown,   SIGNAL(clicked()), SLOT(onItemDown()));
  connect(pbSubtitleEntryDelete, SIGNAL(clicked()), SLOT(onRemoveItem()));
  connect(subtitleListView,      SIGNAL(customContextMenuRequested(const QPoint&)), SLOT( onContextMenuRequest(const QPoint&)));
}

//! Set the group box title string. This method is needed by designer.
void TTSubtitleTreeView::setTitle (const QString&)
{
}

/* /////////////////////////////////////////////////////////////////////////////
 * setAVData
 */
void TTSubtitleTreeView::onAVDataChanged(const TTAVItem* avData)
{
  if (avData == 0) {
    mpAVItem = 0;
    clear();
    return;
  }

  if (mpAVItem != 0) {
    disconnect(this,   SIGNAL(removeItem(int)),             mpAVItem, SLOT(onRemoveSubtitleItem(int)));
    disconnect(this,   SIGNAL(swapItems(int, int)),         mpAVItem, SLOT(onSwapSubtitleItems(int, int)));

    disconnect(mpAVItem, SIGNAL(subtitleItemAppended(const TTSubtitleItem&)), this, SLOT(onAppendItem(const TTSubtitleItem&)));
    disconnect(mpAVItem, SIGNAL(subtitleItemRemoved(int)),                    this, SLOT(onItemRemoved(int)));
    disconnect(mpAVItem, SIGNAL(subtitleItemsSwapped(int, int)),              this, SLOT(onSwapItems(int, int)));
  }

  mpAVItem = avData;

  connect(mpAVItem, SIGNAL(subtitleItemAppended(const TTSubtitleItem&)), SLOT(onAppendItem(const TTSubtitleItem&)));
  connect(mpAVItem, SIGNAL(subtitleItemRemoved(int)),                    SLOT(onItemRemoved(int)));
  connect(mpAVItem, SIGNAL(subtitleItemsSwapped(int, int)),              SLOT(onSwapItems(int, int)));

  connect(this,   SIGNAL(removeItem(int)),             mpAVItem, SLOT(onRemoveSubtitleItem(int)));
  connect(this,   SIGNAL(swapItems(int, int)),         mpAVItem, SLOT(onSwapSubtitleItems(int, int)));

  onReloadList(mpAVItem);
}

void TTSubtitleTreeView::clear()
{
  subtitleListView->clear();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Enable or disable the widget
 */
void TTSubtitleTreeView::setControlEnabled( bool enabled )
{
  pbSubtitleFileOpen->setEnabled(enabled);
  pbSubtitleEntryUp->setEnabled(enabled);
  pbSubtitleEntryDelete->setEnabled(enabled);
  pbSubtitleEntryDown->setEnabled(enabled);
  subtitleListView->setEnabled(enabled);
}

/* /////////////////////////////////////////////////////////////////////////////
 * onClearList
 */
void TTSubtitleTreeView::onClearList()
{
  subtitleListView->clear();
}

/* /////////////////////////////////////////////////////////////////////////////
 * onAppendItem
 */
void TTSubtitleTreeView::onAppendItem(const TTSubtitleItem& item)
{
  QTreeWidgetItem* treeItem = new QTreeWidgetItem(subtitleListView);

  treeItem->setText(0, item.getFileName());
  treeItem->setText(1, item.getLength());
  treeItem->setText(2, item.getDelay());
}

/* //////////////////////////////////////////////////////////////////////////////
 * Swap two items in list
 */
void TTSubtitleTreeView::onSwapItems(int oldIndex, int newIndex)
{
  QTreeWidgetItem* listItem = subtitleListView->takeTopLevelItem(oldIndex);

  subtitleListView->insertTopLevelItem(newIndex, listItem);
  subtitleListView->setCurrentItem(listItem);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Event handler for item up button
 */
void TTSubtitleTreeView::onItemUp()
{
  if (subtitleListView->currentItem() == 0)  return;

  int index = subtitleListView->indexOfTopLevelItem(subtitleListView->currentItem());

  if (index <= 0) return;

  emit swapItems(index, index-1);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Event handler for item down button
 */
void TTSubtitleTreeView::onItemDown()
{
  if (subtitleListView->currentItem() == 0)  return;

  int index = subtitleListView->indexOfTopLevelItem(subtitleListView->currentItem());

  if (index >= subtitleListView->topLevelItemCount()-1) return;

  emit swapItems(index, index+1);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Event handler for remove item button
 */
void TTSubtitleTreeView::onRemoveItem()
{
  if (subtitleListView->currentItem() == 0) return;

  int index = subtitleListView->indexOfTopLevelItem(subtitleListView->currentItem());

  emit removeItem(index);
}

/* //////////////////////////////////////////////////////////////////////////////
 *
 */
void TTSubtitleTreeView::onItemRemoved(int index)
{
  delete subtitleListView->takeTopLevelItem(index);
}

/* /////////////////////////////////////////////////////////////////////////////
 * onContextMenuRequest
 * User requested a context menu
 */
void TTSubtitleTreeView::onContextMenuRequest(const QPoint& point)
{
  if (subtitleListView->currentItem() == 0)
    return;

  QMenu contextMenu(this);
  contextMenu.addAction(itemNewAction);
  contextMenu.addSeparator();
  contextMenu.addAction(itemUpAction);
  contextMenu.addAction(itemDeleteAction);
  contextMenu.addAction(itemDownAction);

  contextMenu.exec(subtitleListView->mapToGlobal(point));
}

void TTSubtitleTreeView::onReloadList(const TTAVItem* avData)
{
  onClearList();

  for (int i = 0; i < avData->subtitleCount(); i++) {
    onAppendItem(avData->subtitleListItemAt(i));
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * createAction
 * Create the actions used by the context menu.
 */
void TTSubtitleTreeView::createActions()
{
  itemNewAction = new QAction(tr("&Insert subtitlefile"), this);
  //itemNewAction->setShortcut(tr("Ctrl+I"));
  itemNewAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/fileopen_24.xpm")));
  itemNewAction->setStatusTip(tr("Open a new subtitlefile and insert to list"));
  connect(itemNewAction, SIGNAL(triggered()), SIGNAL(openFile()));

  itemUpAction = new QAction(tr("Move &up"), this);
  //itemUpAction->setShortcut(tr("Ctrl+U"));
  itemUpAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/uparrow_18.xpm")));
  itemUpAction->setStatusTip(tr("Move selected subtitlefile one position upward"));
  connect(itemUpAction, SIGNAL(triggered()), SLOT(onItemUp()));

  itemDeleteAction = new QAction(tr("&Delete"), this);
  //itemDeleteAction->setShortcut(tr("Ctrl+D"));
  itemDeleteAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/cancel_18.xpm")));
  itemDeleteAction->setStatusTip(tr("Remove selected subtitlefile from list"));
  connect(itemDeleteAction, SIGNAL(triggered()), SLOT(onRemoveItem()));

  itemDownAction = new QAction(tr("Move d&own"), this);
  //itemDownAction->setShortcut(tr("Ctrl+O"));
  itemDownAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/downarrow_18.xpm")));
  itemDownAction->setStatusTip(tr("Move selected subtitlefile one position downward"));
  connect(itemDownAction, SIGNAL(triggered()), SLOT(onItemDown()));
}

