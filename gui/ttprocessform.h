/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttprocessform.h                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 08/07/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/18/2006 */
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


#ifndef TTPROCESSFORM_H
#define TTPROCESSFORM_H

#include "ui_processviewwidget.h"
#include "../common/ttcut.h"

#include <QtGui/QApplication>

class TTProcessForm : public QDialog, Ui::ProcessViewWidget
{
  Q_OBJECT

  public:
    TTProcessForm(QWidget* parent);
    ~TTProcessForm();

  public:
    void setFrameCaption(QString& caption);
    void enableList(bool value);
    void enableButton(bool value);
    void addLine(const QString& str_line);
    void showCancelButton(bool show);
    void showOkButton(bool show);

  signals:
    void btnCancelClicked();
};

#endif // TTPROCESSFORM_H
