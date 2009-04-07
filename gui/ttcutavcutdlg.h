/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttcutavcutdlg.h                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 04/01/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/05/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTAVCUTDLG
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

#ifndef TTCUTAVCUTDLG_H
#define TTCUTAVCUTDLG_H

#include "ui_avcutdialog.h"

#include "../common/ttmessagelogger.h"
#include "../common/ttcut.h"

class QString;

class DfInfo {
	 public:
		 DfInfo() {};
		 QString path;
		 double  size;
		 double  free;
		 double  used;
		 double  percentUsed;
 };

// -----------------------------------------------------------------------------
// TTCut A/V cut dialog;
// -----------------------------------------------------------------------------
class TTCutAVCutDlg : public QDialog, Ui::TTCutAVCutDlg
{
  Q_OBJECT

  public:
    TTCutAVCutDlg( QWidget* parent=0);
    ~TTCutAVCutDlg();

    void setGlobalData();
    void setCommonData();
    void getCommonData();

  protected slots:
    void onDlgStart();
    void onDlgCancel();
    void onDirectoryOpen();

  private:
    void   getFreeDiskSpace();
    DfInfo getDiskSpaceInfo(QString path);

 private:
    TTMessageLogger* log;

};

#endif // TTCUTAVCUTDLG_H
