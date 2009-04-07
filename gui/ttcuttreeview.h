/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcuttreeview.h                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTTREEVIEW
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

#ifndef TTCUTTREEVIEW_H
#define TTCUTTREEVIEW_H

#include "ui_cutlistwidget.h"

class TTCutItem;
class TTCutList;
class QMenu;
class QAction;
class TTAVData;

class TTCutTreeView : public QWidget, Ui::TTCutListWidget
{
  Q_OBJECT

  public:
    TTCutTreeView(QWidget* parent=0);

    void controlEnabled(bool value);
    void setAVData(TTAVData* avData);
    void clear();

  public slots:
    void onAppendItem(const TTCutItem& item);
    void onRemoveItem(int index);
    void onUpdateItem(const TTCutItem& citem, const TTCutItem& uitem);
    void onEntryUp();
    void onEntryDown();
    void onEntryDelete();
    void onEntrySelected(QTreeWidgetItem* item, int column);
    void onItemSelectionChanged();
    void onEntryEdit();
    void onSetCutOut();
    void onGotoCutIn();
    void onGotoCutOut();
    void onEntryPreview();
    void onEntryCut();
    void onPreview();
    void onAVCut();
    void onAVSelCut();
    void onAudioCut();
    void onEditCutOut(const TTCutItem& item, int cutOut);
    void onContextMenuRequest(const QPoint& point);
    void onEntryDuplicate();
    void onClearList();
    void onReloadList();

  signals:
    void removeItem(const TTCutItem& item);
    void itemOrderChanged(int oldIndex, int newIndex);
    void itemUpdated(const TTCutItem& item);
    void selectionChanged(const TTCutItem& item);
    void entryEdit(const TTCutItem& item);
    void cutOutUpdated(const TTCutItem& item);
    void setCutOut(const TTCutItem& item);
    void gotoCutIn(int index);
    void gotoCutOut(int index);
    void refreshDisplay();
    void previewCut(TTCutList* cutList);
    void audioVideoCut(bool, TTCutList* cutList);

  private:
  	QTreeWidgetItem* findItem(const TTCutItem& cutItem);
    TTCutList* cutListFromSelection(bool ignoreSelection=false);
    void createActions();

  private:
    TTAVData*        mAVData;
    int              editItemIndex;
    bool      allowSelectionChanged;
    QTreeWidgetItem* currentEditItem;
    QMenu*           contextMenu;
    QAction*         itemUpAction;
    QAction*         itemDownAction;
    QAction*         itemDeleteAction;
    QAction*         itemEditAction;
    QAction*			   setCutOutAction;
    QAction*         gotoCutInAction;
    QAction*         gotoCutOutAction;
    QAction*         itemPreviewAction;
    QAction*         itemCutAction;
    QAction*         itemDuplicateAction;
};
#endif
