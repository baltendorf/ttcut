/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttmessagewindow.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/24/2007 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMESSAGEWINDOW
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

#include "ttmessagewindow.h"

/* /////////////////////////////////////////////////////////////////////////////
 * Constructor
 */
TTMessageWindow::TTMessageWindow(QWidget* parent)
    : QDialog(parent)
{

  setupUi( this );
  
  // signals and slot connection
  connect( acceptBtn, SIGNAL( clicked() ),  SLOT( onDlgClose() ) );
}

/* /////////////////////////////////////////////////////////////////////////////
 * Destructor
 */
TTMessageWindow::~TTMessageWindow()
{

}

/* /////////////////////////////////////////////////////////////////////////////
 * Dialog close event
 */
void TTMessageWindow::onDlgClose()
{
  done( 0 );
}

/* /////////////////////////////////////////////////////////////////////////////
 * Set caller label text property
 */
void TTMessageWindow::setCallerText(QString text)
{
  laCallerText->setText(text);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Set message label text property
 */
void TTMessageWindow::setMessageText(QString text)
{
  laMessageText->setText(text);
}
