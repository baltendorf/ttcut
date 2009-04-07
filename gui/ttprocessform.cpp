/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttprocessform.cpp                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 08/07/2005 */
/* MODIFIED: b. altendorf                                    DATE: 04/18/2007 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTPROCESSFORM
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

#include "ttprocessform.h"


TTProcessForm::TTProcessForm(QWidget* parent)
  : QDialog(parent)
{
  setupUi(this);

  btnCancel->setAutoDefault(false);
  btnCancel->hide();

  // signal and slot connection
  connect(btnCancel, SIGNAL(clicked(bool)), SIGNAL(btnCancelClicked()));
}

TTProcessForm::~TTProcessForm()
{
 // for (int i=0; i < procOutputList->count(); i++) {
  //  delete procOutputList->takeItem(i);
  //}
}

void TTProcessForm::setFrameCaption(QString& caption)
{
  gbProcessView->setTitle(caption);
}

void TTProcessForm::enableList(bool value)
{
  procOutputList->setEnabled(value);
}

void TTProcessForm::enableButton(bool value)
{
  btnCancel->setEnabled(value);
}

void TTProcessForm::showCancelButton(bool show)
{
  btnCancel->setText(tr("Cancel"));
  btnCancel->setIcon(QIcon(":/pixmaps/pixmaps/messagebox_critical.png"));
  btnCancel->setVisible(show);
}

void TTProcessForm::showOkButton(bool show)
{
  btnCancel->setText(tr("Ok"));
  btnCancel->setIcon(QIcon(":/pixmaps/pixmaps/button_ok.png"));
  btnCancel->setVisible(show);
}

void TTProcessForm::addLine(const QString& str_line)
{
	procOutputList->addItem(str_line);
  procOutputList->scrollToBottom();
}
