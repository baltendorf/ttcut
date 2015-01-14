/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttcutsettingsdlg.cpp                                            */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// *** TTCUTSETTINGSDLG
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

#include "ttcutsettingsdlg.h"

#include "common/ttcut.h"

  
TTCutSettingsDlg::TTCutSettingsDlg(QWidget* parent)
:QDialog(parent)
{
  setupUi(this);

  // not implemented yet
  settingsTab->removeTab(4);

  // set the tabs data
  // ------------------------------------------------------------------
  commonPage->setTabData();
  filesPage->setTabData();
  encodingPage->setTabData();
  muxingPage->setTabData();
  //chaptersPage->setTabData();

  // signal and slot connections
  connect(okButton,     SIGNAL(clicked()), SLOT(onDlgOk()));
  connect(cancelButton, SIGNAL(clicked()), SLOT(onDlgCancel()));
}

// save the tabs data
void TTCutSettingsDlg::setGlobalData()
{
    commonPage->getTabData();
    filesPage->getTabData();
    encodingPage->getTabData();
    muxingPage->getTabData();
    //chaptersPage->getTabData();
}


// exit, saving changes
void TTCutSettingsDlg::onDlgOk()
{
   setGlobalData();

   done( 0 );
}


// exit, discard changes
void TTCutSettingsDlg::onDlgCancel()
{
   done( 1 );
}




