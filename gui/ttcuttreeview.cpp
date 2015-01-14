/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcuttreeview.cpp                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTREEVIEW
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

#include "data/ttcutlist.h"
#include "common/ttcut.h"
#include "data/ttavdata.h"
#include "data/ttavlist.h"
#include "avstream/ttavstream.h"

#include "ttcuttreeview.h"

#include <QMenu>
#include <QPoint>
#include <QHeaderView>
#include <QModelIndex>
#include <QMessageBox>

/*!
 * TTCutTreeView
 */
TTCutTreeView::TTCutTreeView(QWidget* parent)
  :QWidget(parent)
{
  setupUi( this );

  // set list view header (column) width
  videoCutList->setRootIsDecorated( false );
  QHeaderView* header = videoCutList->header();
  header->resizeSection(0, 200);
  header->resizeSection(1, 140);
  header->resizeSection(2, 140);
  header->resizeSection(3, 150);
  header->resizeSection(4,  80);

  allowSelectionChanged = true;
  editItemIndex = -1;

  // actions for context menu
  createActions();

  // signal and slot connections
  connect(pbEntryUp,       SIGNAL(clicked()), 																							SLOT(onEntryUp()));
  connect(pbEntryDown,     SIGNAL(clicked()),                                 SLOT(onEntryDown()));
  connect(pbEntryDelete,   SIGNAL(clicked()),                                 SLOT(onEntryDelete()));
  connect(pbPreview,       SIGNAL(clicked()),                                 SLOT(onPreview()));
  connect(pbCutAudioVideo, SIGNAL(clicked()), 						  					        SLOT(onAVCut()));
  connect(videoCutList,    SIGNAL(doubleClicked(const QModelIndex)),          SLOT(onSetCutOut()));
  connect(videoCutList,    SIGNAL(itemSelectionChanged()),                    SLOT(onItemSelectionChanged()));
  connect(videoCutList,    SIGNAL(customContextMenuRequested(const QPoint&)), SLOT(onContextMenuRequest(const QPoint&)));
}

/*!
 * enableControl
 */
void TTCutTreeView::controlEnabled(bool value)
{
	pbEntryUp->setEnabled(value);
	pbEntryDown->setEnabled(value);
	pbEntryDelete->setEnabled(value);
	pbPreview->setEnabled(value);
	pbCutAudioVideo->setEnabled(value);
	videoCutList->setEnabled(value);
}

/*!
 * setAVData
 */
void TTCutTreeView::setAVData(TTAVData* avData)
{
  mAVData = avData;

  connect(mAVData, SIGNAL(cutItemAppended(const TTCutItem&)),                  SLOT(onAppendItem(const TTCutItem&)));
  connect(mAVData, SIGNAL(cutItemRemoved(int)),                                SLOT(onRemoveItem(int)));
  connect(mAVData, SIGNAL(cutItemUpdated(const TTCutItem&, const TTCutItem&)), SLOT(onUpdateItem(const TTCutItem&, const TTCutItem&)));
  connect(mAVData, SIGNAL(cutDataReloaded()),                                  SLOT(onReloadList()));
  connect(this,    SIGNAL(removeItem(const TTCutItem&)),              mAVData, SLOT(onRemoveCutItem(const TTCutItem&)));
  connect(this,    SIGNAL(itemOrderChanged(int, int)),                mAVData, SLOT(onCutOrderChanged(int , int)));
}

/*!
 * onClearList
 */
void TTCutTreeView::onClearList()
{
  videoCutList->clear();
}

/*!
 * onReloadList
 */
void TTCutTreeView::onReloadList()
{
	onClearList();

	for (int i = 0; i < mAVData->cutCount(); i++) {
	    TTCutItem cutItem = mAVData->cutItemAt(i);
	    onAppendItem(cutItem);
	  }
}

/*!
 * onAppendItem
 */
void TTCutTreeView::onAppendItem(const TTCutItem& item)
{
  QTreeWidgetItem* treeItem = new QTreeWidgetItem(videoCutList);

  treeItem->setText(0, item.fileName());
  treeItem->setText(1, item.cutInString());
  treeItem->setText(2, item.cutOutString());
  treeItem->setText(3, item.cutLengthString());
  treeItem->setText(4, QString("%1").arg(item.ID().toString())); //TODO: we need this to find cut entry in list

  //emit refreshDisplay();
}

/*!
 * onRemoveItem
 */
void TTCutTreeView::onRemoveItem(int index)
{
  delete videoCutList->takeTopLevelItem(index);

  videoCutList->indexOfTopLevelItem(videoCutList->currentItem());

  allowSelectionChanged = true;
}

/*!
 * onUpdateItem
 */
void TTCutTreeView::onUpdateItem(const TTCutItem& cItem, const TTCutItem& uitem)
{
  QTreeWidgetItem* treeItem = (editItemIndex < 0)
      ? findItem(cItem) //videoCutList->currentItem()
      : videoCutList->topLevelItem(editItemIndex);

  if (treeItem == 0) {
    qDebug("TTCutTreeView::item not found!");
   	return;
  }

  treeItem->setText(0, uitem.fileName());
  treeItem->setText(1, uitem.cutInString());
  treeItem->setText(2, uitem.cutOutString());
  treeItem->setText(3, uitem.cutLengthString());

  if (editItemIndex >= 0) {
    editItemIndex = -1;
    treeItem->setBackgroundColor(0, Qt::white);
    treeItem->setBackgroundColor(1, Qt::white);
    treeItem->setBackgroundColor(2, Qt::white);
    treeItem->setBackgroundColor(3, Qt::white);
    treeItem->setBackgroundColor(4, Qt::white);
  }

  emit itemUpdated(cItem);
  emit refreshDisplay();
}

/*!
 * findItem
 */
QTreeWidgetItem* TTCutTreeView::findItem(const TTCutItem& cutItem)
{
	for (int i = 0; i < videoCutList->topLevelItemCount(); i++) {
		QTreeWidgetItem* item = videoCutList->topLevelItem(i);
		if (item->text(4) == QString("%1").arg(cutItem.ID().toString()))
			return item;
	}
	return 0;
}

/*!
 * onEntryUp
 */
void TTCutTreeView::onEntryUp()
{
  if (videoCutList->topLevelItemCount() == 0)
    return;

  if (videoCutList->topLevelItem(0)->isSelected() && editItemIndex < 0)
    return;

  QTreeWidgetItem* pCurItem = videoCutList->currentItem();
  QList<QTreeWidgetItem*> SelectedItems = videoCutList->selectedItems();
  for ( int i=0; i<videoCutList->topLevelItemCount(); ++i ) {
    if ( videoCutList->topLevelItem(i)->isSelected() ) {
      QTreeWidgetItem* pTmpItem = videoCutList->takeTopLevelItem( i );
      videoCutList->insertTopLevelItem( i-1, pTmpItem );

      emit itemOrderChanged(i, i-1);
    }
  }

  // restore current item, and item selection
  videoCutList->setCurrentItem( pCurItem );
  for ( int i=0; i<SelectedItems.count(); ++i )
    SelectedItems[i]->setSelected( true );
}

/*!
 * onEntryDown
 */
void TTCutTreeView::onEntryDown()
{
  if (videoCutList->topLevelItemCount() == 0)
    return;

  if (videoCutList->topLevelItem(videoCutList->topLevelItemCount()-1)->isSelected() && editItemIndex < 0)
    return;

  QTreeWidgetItem* pCurItem = videoCutList->currentItem();
  QList<QTreeWidgetItem*> SelectedItems = videoCutList->selectedItems();
  for ( int i=videoCutList->topLevelItemCount()-1; i>=0; --i ) {
    if ( videoCutList->topLevelItem(i)->isSelected() ) {
      QTreeWidgetItem* pTmpItem = videoCutList->takeTopLevelItem( i );
      videoCutList->insertTopLevelItem( i+1, pTmpItem );

      emit itemOrderChanged(i, i+1);
    }
  }

  // restore current item, and item selection
  videoCutList->setCurrentItem( pCurItem );
  for ( int i=0; i<SelectedItems.count(); ++i )
    SelectedItems[i]->setSelected( true );
}

/*!
 * onEntryDelete
 */
void TTCutTreeView::onEntryDelete()
{
  if (videoCutList->topLevelItemCount() == 0) return;
  if (editItemIndex >= 0)  return;

  allowSelectionChanged = false;

  for (int i = videoCutList->selectedItems().count()-1; i >= 0; i--) {
    int index = videoCutList->indexOfTopLevelItem(videoCutList->selectedItems()[i]);
    TTCutItem cutItem = mAVData->cutItemAt(index);
    emit removeItem(cutItem);
  }

  emit refreshDisplay();
}

/*!
 * onEntryDuplicate
 */
void TTCutTreeView::onEntryDuplicate()
{
  //if (videoCutList->topLevelItemCount() == 0) return;
  if (mAVData == 0 || videoCutList->currentItem() == 0) return;
  if (editItemIndex >= 0) return;

   for ( int i=0; i<videoCutList->topLevelItemCount(); ++i ) {
    if ( videoCutList->topLevelItem(i)->isSelected() ) {
      TTCutItem cutItem = mAVData->cutItemAt(i);
    	mAVData->copyCutEntry(cutItem);
    }
  }
}

/*!
 * onEntrySelected
 */
void TTCutTreeView::onEntrySelected(QTreeWidgetItem*, int)
{
  if (mAVData == 0 || videoCutList->currentItem() == 0) return;

  int index = videoCutList->indexOfTopLevelItem(videoCutList->currentItem());
  TTCutItem cutItem = mAVData->cutItemAt(index);

  emit selectionChanged(cutItem);
}

/*!
 * onItemSelectionChanged
 */
void TTCutTreeView::onItemSelectionChanged()
{
	if (!allowSelectionChanged) return;
  if (mAVData == 0 || videoCutList->currentItem() == 0)  return;

  int index = videoCutList->indexOfTopLevelItem(videoCutList->currentItem());
  TTCutItem cutItem = mAVData->cutItemAt(index);

  emit selectionChanged(cutItem);
}

/*!
 * onEntryEdit
 */
void TTCutTreeView::onEntryEdit()
{
  //if (videoCutList->topLevelItemCount() == 0) return;
  if (mAVData == 0 || videoCutList->currentItem() == 0) return;

  QTreeWidgetItem* curItem = videoCutList->currentItem();
  editItemIndex = videoCutList->indexOfTopLevelItem(curItem);

  curItem->setBackgroundColor(0, Qt::lightGray);
  curItem->setBackgroundColor(1, Qt::lightGray);
  curItem->setBackgroundColor(2, Qt::lightGray);
  curItem->setBackgroundColor(3, Qt::lightGray);
  curItem->setBackgroundColor(4,Qt::lightGray);

  // deselect item
  videoCutList->setItemSelected(curItem, false);

  TTCutItem cutItem = mAVData->cutItemAt(editItemIndex);
  emit entryEdit(cutItem);
}

/*!
 * onSetCutOut
 */
void TTCutTreeView::onSetCutOut()
{
  if (mAVData == 0 || videoCutList->currentItem() == 0) return;

  int index = videoCutList->indexOfTopLevelItem(videoCutList->currentItem());

  TTCutItem cutItem = mAVData->cutItemAt(index);

  emit setCutOut(cutItem);
}

/*!
 * onGotoCutIn
 */
void TTCutTreeView::onGotoCutIn()
{
  if (mAVData == 0 || videoCutList->currentItem() == 0) return;

  int index = videoCutList->indexOfTopLevelItem(videoCutList->currentItem());
  TTCutItem cutItem = mAVData->cutItemAt(index);

  emit gotoCutIn(cutItem.cutInIndex());
}

/*!
 * onGotoCutOut
 */
void TTCutTreeView::onGotoCutOut()
{
  if (mAVData == 0 || videoCutList->currentItem() == 0) return;

  int index = videoCutList->indexOfTopLevelItem(videoCutList->currentItem());
  TTCutItem cutItem = mAVData->cutItemAt(index);

  emit gotoCutOut(cutItem.cutOutIndex());
}

//! Creates the cut list from current selection
TTCutList* TTCutTreeView::cutListFromSelection(bool ignoreSelection)
{
  TTCutList* cutList = new TTCutList();

  for (int i = 0; i < videoCutList->topLevelItemCount(); i++) {
    TTCutItem cutItem = mAVData->cutItemAt(i);

    if (ignoreSelection) {
      cutList->append(cutItem.avDataItem(), cutItem.cutInIndex(), cutItem.cutOutIndex());
      continue;
    }

    if (videoCutList->topLevelItem(i)->isSelected())
      cutList->append(cutItem.avDataItem(), cutItem.cutInIndex(), cutItem.cutOutIndex());
  }

  return cutList;
}

/*!
 * onEntryPreview
 */
void TTCutTreeView::onEntryPreview()
{
  if (mAVData == 0 || videoCutList->currentItem() == 0)  return;

  emit previewCut(cutListFromSelection());
}

/*!
 * onEntryCut
 */
void TTCutTreeView::onEntryCut()
{
  if (mAVData == 0 || videoCutList->currentItem() == 0)
    return;

  emit audioVideoCut(false, cutListFromSelection());
}

/*!
 * onPreview
 */
void TTCutTreeView::onPreview()
{
  if (mAVData == 0) return;

  emit previewCut(cutListFromSelection(true));
}

/*!
 * onAVCut
 */
void TTCutTreeView::onAVCut()
{
  if (mAVData == 0) return;

  emit audioVideoCut(false, cutListFromSelection(true));
}

/*!
 * onAVSelCut
 */
void TTCutTreeView::onAVSelCut()
{
  if (mAVData == 0) return;

  emit audioVideoCut(false, cutListFromSelection());
}

/*!
 * onAudioCut
 */
void TTCutTreeView::onAudioCut()
{
  if (mAVData == 0) return;

  emit audioVideoCut(true, cutListFromSelection());
}

/*!
 * onEditCutOut
 */
void TTCutTreeView::onEditCutOut(const TTCutItem& cutItem, int cutOut)
{
  if (mAVData == 0) return

  cutItem.avDataItem()->updateCutEntry(cutItem, cutItem.cutInIndex(), cutOut);
  emit cutOutUpdated(cutItem);
}

/*!
 * onContextMenuRequest
 */
void TTCutTreeView::onContextMenuRequest( const QPoint& point)
{
  if (mAVData == 0 || videoCutList->currentItem() == 0)
    return;

  QMenu contextMenu(this);
  bool bMultipleSelected = ( videoCutList->selectedItems().count() > 1 );

  if ( !bMultipleSelected ) {
  	contextMenu.addAction(setCutOutAction);
    contextMenu.addAction(gotoCutInAction);
    contextMenu.addAction(gotoCutOutAction);
    contextMenu.addSeparator();
  }
  contextMenu.addAction(itemUpAction);
  contextMenu.addAction(itemDeleteAction);
  contextMenu.addAction(itemDuplicateAction);
  contextMenu.addAction(itemDownAction);
  contextMenu.addSeparator();
  contextMenu.addAction(itemCutAction);
  contextMenu.addSeparator();
  contextMenu.addAction(itemPreviewAction);
  if ( !bMultipleSelected ) {
    contextMenu.addSeparator();
    contextMenu.addAction(itemEditAction);
  }

  contextMenu.exec(videoCutList->mapToGlobal(point));
}

/*!
 * createActions
 */
void TTCutTreeView::createActions()
{
  itemUpAction = new QAction(tr("Move &up"), this);
  itemUpAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/uparrow_18.xpm")));
  itemUpAction->setStatusTip(tr("Move selected cut one position upward"));
  connect(itemUpAction, SIGNAL(triggered()), this, SLOT(onEntryUp()));

  itemDeleteAction = new QAction(tr("&Delete"), this);
  itemDeleteAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/cancel_18.xpm")));
  itemDeleteAction->setStatusTip(tr("Remove selected cut from list"));
  connect(itemDeleteAction, SIGNAL(triggered()), this, SLOT(onEntryDelete()));

  itemDuplicateAction = new QAction(tr("Duplicate Cut"), this);
  itemDuplicateAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/editcopy.png")));
  itemDuplicateAction->setStatusTip(tr("Duplicate the selected cut"));
  connect(itemDuplicateAction, SIGNAL(triggered()), this, SLOT(onEntryDuplicate()));

  itemDownAction = new QAction(tr("Move d&own"), this);
  itemDownAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/downarrow_18.xpm")));
  itemDownAction->setStatusTip(tr("Move selected cut one position downward"));
  connect(itemDownAction, SIGNAL(triggered()), this, SLOT(onEntryDown()));

  itemEditAction = new QAction(tr("Edit &cut"), this);
  //itemEditAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/downarrow_18.xpm")));
  itemEditAction->setStatusTip(tr("Edit selected cut"));
  connect(itemEditAction, SIGNAL(triggered()), this, SLOT(onEntryEdit()));

  itemPreviewAction = new QAction(tr("Preview cut"), this);
  itemPreviewAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/preview_18.xpm")));
  itemPreviewAction->setStatusTip(tr("Preview selected cut"));
  connect(itemPreviewAction, SIGNAL(triggered()), this, SLOT(onEntryPreview()));

  itemCutAction = new QAction(tr("Cut selected entries"), this);
  itemCutAction->setStatusTip(tr("Cut the selected entries"));
  connect(itemCutAction, SIGNAL(triggered()), this, SLOT(onEntryCut()));

  gotoCutInAction = new QAction(tr("Goto Cut-In"), this);
  //gotoCutInAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/downarrow_18.xpm")));
  gotoCutInAction->setStatusTip(tr("Goto selected cut-in position"));
  connect(gotoCutInAction, SIGNAL(triggered()), this, SLOT(onGotoCutIn()));

  gotoCutOutAction = new QAction(tr("Goto Cut-Out"), this);
  //gotoCutOutAction->setIcon(QIcon(QString::fromUtf8(":/pixmaps/pixmaps/downarrow_18.xpm")));
  gotoCutOutAction->setStatusTip(tr("Goto selected cut-out position"));
  connect(gotoCutOutAction, SIGNAL(triggered()), this, SLOT(onGotoCutOut()));

  setCutOutAction = new QAction(tr("Set Cut-Out"), this);
  setCutOutAction->setStatusTip(tr("Show selected cut-out frame in cut-out window"));
  connect(setCutOutAction, SIGNAL(triggered()), this, SLOT(onSetCutOut()));
}
