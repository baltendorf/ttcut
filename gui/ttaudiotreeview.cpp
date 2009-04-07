/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttaudiotreeview.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/28/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTAUDIOTREEVIEW
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


#include "ttaudiotreeview.h"

#include "../data/ttaudiolist.h"
#include "../data/ttavlist.h"
#include "../avstream/ttavstream.h"

#include <QHeaderView>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QAction>

/* /////////////////////////////////////////////////////////////////////////////
 * Construct a new TTAudioFileList widget.
 */
TTAudioTreeView::TTAudioTreeView(QWidget* parent)
  :QWidget(parent)
{
  setupUi( this );

  mpAVItem = 0;

  audioListView->setRootIsDecorated(false);
  QHeaderView* header = audioListView->header();
  header->resizeSection(0, 220);
  header->resizeSection(1, 220);
  header->resizeSection(2, 140);
  header->resizeSection(3, 100);
  header->resizeSection(4, 100);
  header->resizeSection(5, 100);
  header->resizeSection(6,  60);

  createActions();

  // signal and slot connections
  connect(pbAudioFileOpen,    SIGNAL(clicked()), SIGNAL(openFile()));
  connect(pbAudioEntryUp,     SIGNAL(clicked()), SLOT(onItemUp()));
  connect(pbAudioEntryDown,   SIGNAL(clicked()), SLOT(onItemDown()));
  connect(pbAudioEntryDelete, SIGNAL(clicked()), SLOT(onRemoveItem()));
  connect(audioListView,      SIGNAL(customContextMenuRequested(const QPoint&)), SLOT( onContextMenuRequest(const QPoint&)));
}

//! Set the group box title string. This method is needed by designer.
void TTAudioTreeView::setTitle (const QString&)
{
}

/* /////////////////////////////////////////////////////////////////////////////
 * setAVData
 */
void TTAudioTreeView::onAVDataChanged(const TTAVItem* avData)
{
	if (avData == 0) {
		mpAVItem = 0;
		clear();
		return;
	}

	if (mpAVItem != 0) {
	disconnect(this,   SIGNAL(removeItem(int)),             mpAVItem, SLOT(onRemoveAudioItem(int)));
  disconnect(this,   SIGNAL(swapItems(int, int)),         mpAVItem, SLOT(onSwapAudioItems(int, int)));

  disconnect(mpAVItem, SIGNAL(audioItemAppended(const TTAudioItem&)), this, SLOT(onAppendItem(const TTAudioItem&)));
  disconnect(mpAVItem, SIGNAL(audioItemRemoved(int)),                 this, SLOT(onItemRemoved(int)));
  disconnect(mpAVItem, SIGNAL(audioItemsSwapped(int, int)),           this, SLOT(onSwapItems(int, int)));
	}

  mpAVItem = avData;

  connect(mpAVItem, SIGNAL(audioItemAppended(const TTAudioItem&)), SLOT(onAppendItem(const TTAudioItem&)));
  connect(mpAVItem, SIGNAL(audioItemRemoved(int)),                 SLOT(onItemRemoved(int)));
  connect(mpAVItem, SIGNAL(audioItemsSwapped(int, int)),           SLOT(onSwapItems(int, int)));

  connect(this,   SIGNAL(removeItem(int)),             mpAVItem, SLOT(onRemoveAudioItem(int)));
  connect(this,   SIGNAL(swapItems(int, int)),         mpAVItem, SLOT(onSwapAudioItems(int, int)));

  onReloadList(mpAVItem);
}

void TTAudioTreeView::clear()
{
	audioListView->clear();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Enable or disable the widget
 */
void TTAudioTreeView::setControlEnabled( bool enabled )
{
  pbAudioFileOpen->setEnabled(enabled);
  pbAudioEntryUp->setEnabled(enabled);
  pbAudioEntryDelete->setEnabled(enabled);
  pbAudioEntryDown->setEnabled(enabled);
  audioListView->setEnabled(enabled);
}

/* /////////////////////////////////////////////////////////////////////////////
 * onClearList
 */
void TTAudioTreeView::onClearList()
{
  audioListView->clear();
}

/* /////////////////////////////////////////////////////////////////////////////
 * onAppendItem
 */
void TTAudioTreeView::onAppendItem(const TTAudioItem& item)
{
  QTreeWidgetItem* treeItem = new QTreeWidgetItem(audioListView);

  treeItem->setText(0, item.getFileName());
  treeItem->setText(1, item.getLength());
  treeItem->setText(2, item.getVersion());
  treeItem->setText(3, item.getBitrate());
  treeItem->setText(4, item.getSamplerate());
  treeItem->setText(5, item.getMode());
  treeItem->setText(6, item.getDelay());
}

/* //////////////////////////////////////////////////////////////////////////////
 * Swap two items in list
 */
void TTAudioTreeView::onSwapItems(int oldIndex, int newIndex)
{
    QTreeWidgetItem* listItem = audioListView->takeTopLevelItem(oldIndex);

    audioListView->insertTopLevelItem(newIndex, listItem);
    audioListView->setCurrentItem(listItem);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Event handler for item up button
 */
void TTAudioTreeView::onItemUp()
{
  if (audioListView->currentItem() == 0)  return;

    int index = audioListView->indexOfTopLevelItem(audioListView->currentItem());

    if (index <= 0) return;

    emit swapItems(index, index-1);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Event handler for item down button
 */
void TTAudioTreeView::onItemDown()
{
  if (audioListView->currentItem() == 0)  return;

  int index = audioListView->indexOfTopLevelItem(audioListView->currentItem());

  if (index >= audioListView->topLevelItemCount()-1) return;

  emit swapItems(index, index+1);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Event handler for remove item button
 */
void TTAudioTreeView::onRemoveItem()
{
  if (audioListView->currentItem() == 0) return;

  int index = audioListView->indexOfTopLevelItem(audioListView->currentItem());

  emit removeItem(index);
}

/* //////////////////////////////////////////////////////////////////////////////
 *
 */
void TTAudioTreeView::onItemRemoved(int index)
{
  delete audioListView->takeTopLevelItem(index);
}

/* /////////////////////////////////////////////////////////////////////////////
 * onContextMenuRequest
 * User requested a context menu
 */
void TTAudioTreeView::onContextMenuRequest(const QPoint& point)
{
  if (audioListView->currentItem() == 0)
    return;

  QMenu contextMenu(this);
  contextMenu.addAction(itemNewAction);
  contextMenu.addSeparator();
  contextMenu.addAction(itemUpAction);
  contextMenu.addAction(itemDeleteAction);
  contextMenu.addAction(itemDownAction);

  contextMenu.exec(audioListView->mapToGlobal(point));
}

void TTAudioTreeView::onReloadList(const TTAVItem* avData)
{
  onClearList();

  for (int i = 0; i < avData->audioCount(); i++) {
    onAppendItem(avData->audioListItemAt(i));
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * createAction
 * Create the actions used by the context menu.
 */
void TTAudioTreeView::createActions()
{
  itemNewAction = new QAction(tr("&Insert audiofile"), this);
  //itemNewAction->setShortcut(tr("Ctrl+I"));
  itemNewAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/fileopen_24.xpm")));
  itemNewAction->setStatusTip(tr("Open a new audiofile and insert to list"));
  connect(itemNewAction, SIGNAL(triggered()), SIGNAL(openFile()));

  itemUpAction = new QAction(tr("Move &up"), this);
  //itemUpAction->setShortcut(tr("Ctrl+U"));
  itemUpAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/uparrow_18.xpm")));
  itemUpAction->setStatusTip(tr("Move selected audiofile one position upward"));
  connect(itemUpAction, SIGNAL(triggered()), SLOT(onItemUp()));

  itemDeleteAction = new QAction(tr("&Delete"), this);
  //itemDeleteAction->setShortcut(tr("Ctrl+D"));
  itemDeleteAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/cancel_18.xpm")));
  itemDeleteAction->setStatusTip(tr("Remove selected audiofile from list"));
  connect(itemDeleteAction, SIGNAL(triggered()), SLOT(onRemoveItem()));

  itemDownAction = new QAction(tr("Move d&own"), this);
  //itemDownAction->setShortcut(tr("Ctrl+O"));
  itemDownAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/downarrow_18.xpm")));
  itemDownAction->setStatusTip(tr("Move selected audiofile one position downward"));
  connect(itemDownAction, SIGNAL(triggered()), SLOT(onItemDown()));
}

