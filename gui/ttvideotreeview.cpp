/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttvideotreeview.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/28/2005 */
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


#include "ttvideotreeview.h"

#include "../data/ttavdata.h"
#include "../data/ttavlist.h"
#include "../avstream/ttmpeg2videostream.h"

#include <QHeaderView>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenu>
#include <QAction>

/* /////////////////////////////////////////////////////////////////////////////
 * Construct a new TTVideoFileList widget.
 */
TTVideoTreeView::TTVideoTreeView(QWidget* parent)
  :QWidget(parent)
{
  setupUi( this );

  videoListView->setRootIsDecorated(false);
  QHeaderView* header = videoListView->header();
  header->resizeSection(0, 320);
  header->resizeSection(1, 220);
  header->resizeSection(2, 140);

  mAVData = 0;
  allowSelectionChanged = true;

  createActions();

  // signal and slot connections
  connect(pbVideoFileOpen,  SIGNAL(clicked()),              SIGNAL(openFile()));
  connect(pbEntryUp,        SIGNAL(clicked()),              SLOT(onItemUp()));
  connect(pbEntryDown,      SIGNAL(clicked()),              SLOT(onItemDown()));
  connect(pbEntryDelete,    SIGNAL(clicked()),              SLOT(onRemoveItem()));
  connect(videoListView,    SIGNAL(itemSelectionChanged()), SLOT(onItemSelectionChanged()));
  connect(videoListView,    SIGNAL(customContextMenuRequested(const QPoint&)), SLOT( onContextMenuRequest(const QPoint&)));
}

//! Set the group box title string. This method is needed by designer.
void TTVideoTreeView::setTitle ( __attribute__((unused))const QString& title )
{
}

/* /////////////////////////////////////////////////////////////////////////////
 * setAVData
 */
void TTVideoTreeView::setAVData(TTAVData* avData)
{
  mAVData = avData;

  connect(mAVData, SIGNAL(avItemAppended(const TTAVItem&)), SLOT(onAppendItem(const TTAVItem&)));
  connect(mAVData, SIGNAL(avItemRemoved(int)),              SLOT(onItemRemoved(int)));
  connect(mAVData, SIGNAL(avItemsSwapped(int, int)),        SLOT(onItemsSwapped(int, int)));
  connect(mAVData, SIGNAL(avDataReloaded()),                SLOT(onReloadList()));

  connect(this,    SIGNAL(removeItem(int)),       mAVData, SLOT(onRemoveAVItem(int)));
  connect(this,    SIGNAL(swapItems(int, int)),   mAVData, SLOT(onSwapAVItems(int, int)));
  connect(this,    SIGNAL(selectionChanged(int)), mAVData, SLOT(onChangeCurrentAVItem(int)));
}

void TTVideoTreeView::clear()
{
  disconnect(mAVData, SIGNAL(avItemAppended(const TTAVItem&)), this, SLOT(onAppendItem(const TTAVItem&)));
  disconnect(mAVData, SIGNAL(avItemRemoved(int)),              this, SLOT(onItemRemoved(int)));
  disconnect(mAVData, SIGNAL(avItemsSwapped(int, int)),        this, SLOT(onItemsSwapped(int, int)));
  disconnect(mAVData, SIGNAL(avDataReloaded()),                this, SLOT(onReloadList()));

  disconnect(this,    SIGNAL(removeItem(int)),       mAVData, SLOT(onRemoveAVItem(int)));
  disconnect(this,    SIGNAL(swapItems(int, int)),   mAVData, SLOT(onSwapAVItems(int, int)));
  disconnect(this,    SIGNAL(selectionChanged(int)), mAVData, SLOT(onChangeCurrentAVItem(int)));

	videoListView->clear();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Enable or disable the widget
 */
void TTVideoTreeView::setControlEnabled( bool enabled )
{
  pbVideoFileOpen->setEnabled(enabled);
  pbEntryUp->setEnabled(enabled);
  pbEntryDelete->setEnabled(enabled);
  pbEntryDown->setEnabled(enabled);
  videoListView->setEnabled(enabled);
}

/* /////////////////////////////////////////////////////////////////////////////
 * onClearList
 */
void TTVideoTreeView::onClearList()
{
  videoListView->clear();
}

/* /////////////////////////////////////////////////////////////////////////////
 * onAppendItem
 */
void TTVideoTreeView::onAppendItem(const TTAVItem& item)
{
  QTreeWidgetItem* treeItem = new QTreeWidgetItem(videoListView);
  TTVideoStream*   mpeg2    = item.videoStream();

  treeItem->setText(0, (mpeg2 != 0) ? mpeg2->fileName() : "");
  treeItem->setText(1, (mpeg2 != 0)
				? QString("%1 (%2)").arg(mpeg2->streamLengthTime().toString("hh:mm:ss.zzz")).arg(mpeg2->frameCount())
				: "");
  treeItem->setText(2, (mpeg2 != 0)
				? QString("%1").arg(mpeg2->frameRate())
				: "");
}

void TTVideoTreeView::onItemSelectionChanged()
{
	if (!allowSelectionChanged)
		return;

	QList<QTreeWidgetItem*> selectedItems = videoListView->selectedItems();

	if (selectedItems.count() == 0)
		return;

	int currentIndex = videoListView->indexOfTopLevelItem(selectedItems[0]);

  emit selectionChanged(currentIndex);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Event handler for item up button
 */
void TTVideoTreeView::onItemUp()
{
  if (mAVData == 0 || videoListView->currentItem() == 0)
    return;

    int index = videoListView->indexOfTopLevelItem(videoListView->currentItem());

    if (index <= 0)
      return;

    emit swapItems(index, index-1);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Event handler for item down button
 */
void TTVideoTreeView::onItemDown()
{
  if (mAVData == 0 || videoListView->currentItem() == 0)
    return;

  int index = videoListView->indexOfTopLevelItem(videoListView->currentItem());

  if (index >= videoListView->topLevelItemCount()-1)
    return;

  emit swapItems(index, index+1);
}

void TTVideoTreeView::onItemRemoved(int index)
{
	delete videoListView->takeTopLevelItem(index);

	int indexNew = videoListView->indexOfTopLevelItem(videoListView->currentItem());

  emit selectionChanged(indexNew);
  allowSelectionChanged = true;
}

void TTVideoTreeView::onItemsSwapped(int oldIndex, int newIndex)
{
  QTreeWidgetItem* listItem = videoListView->takeTopLevelItem(oldIndex);

  videoListView->insertTopLevelItem(newIndex, listItem);
  videoListView->setCurrentItem(listItem);
}


/* /////////////////////////////////////////////////////////////////////////////
 * Event handler for delete item button
 */
void TTVideoTreeView::onRemoveItem()
{
 if (mAVData == 0 || videoListView->currentItem() == 0)
    return;

  allowSelectionChanged = false;
  int index = videoListView->indexOfTopLevelItem(videoListView->currentItem());

  emit removeItem(index);
}


/* /////////////////////////////////////////////////////////////////////////////
 * onContextMenuRequest
 * User requested a context menu
 */
void TTVideoTreeView::onContextMenuRequest(const QPoint& point)
{
  if (videoListView->currentItem() == 0)
    return;

  QMenu contextMenu(this);
  contextMenu.addAction(itemNewAction);
  contextMenu.addSeparator();
  contextMenu.addAction(itemUpAction);
  contextMenu.addAction(itemDeleteAction);
  contextMenu.addAction(itemDownAction);

  contextMenu.exec(videoListView->mapToGlobal(point));
}

void TTVideoTreeView::onReloadList()
{
  onClearList();

  for (int i = 0; i < mAVData->avCount(); i++) {
    TTAVItem* videoItem = mAVData->avItemAt(i);
    onAppendItem(*videoItem);
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * createAction
 * Create the actions used by the context menu.
 */
void TTVideoTreeView::createActions()
{
  itemNewAction = new QAction(tr("&Insert videofile"), this);
  //itemNewAction->setShortcut(tr("Ctrl+I"));
  itemNewAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/fileopen_24.xpm")));
  itemNewAction->setStatusTip(tr("Open a new videofile and insert to list"));
  connect(itemNewAction, SIGNAL(triggered()), SIGNAL(openFile()));

  itemUpAction = new QAction(tr("Move &up"), this);
  //itemUpAction->setShortcut(tr("Ctrl+U"));
  itemUpAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/uparrow_18.xpm")));
  itemUpAction->setStatusTip(tr("Move selected file one position upward"));
  connect(itemUpAction, SIGNAL(triggered()), SLOT(onItemUp()));

  itemDeleteAction = new QAction(tr("&Delete"), this);
  //itemDeleteAction->setShortcut(tr("Ctrl+D"));
  itemDeleteAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/cancel_18.xpm")));
  itemDeleteAction->setStatusTip(tr("Remove selected file from list"));
  connect(itemDeleteAction, SIGNAL(triggered()), SLOT(onRemoveItem()));

  itemDownAction = new QAction(tr("Move d&own"), this);
  //itemDownAction->setShortcut(tr("Ctrl+O"));
  itemDownAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/downarrow_18.xpm")));
  itemDownAction->setStatusTip(tr("Move selected file one position downward"));
  connect(itemDownAction, SIGNAL(triggered()), SLOT(onItemDown()));
}

