/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttcutsettingsmuxer.h                                            */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// *** TTCUTSETTINGSMUXER
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

#ifndef TTCUTSETTINGSMUXER_H 
#define TTCUTSETTINGSMUXER_H

#include "ui_ttcutsettingsmuxer.h"

#include "../common/ttcut.h"


class TTCutSettingsMuxer : public QWidget, Ui::TTCutSettingsMuxer
{
  Q_OBJECT

  public:
    TTCutSettingsMuxer(QWidget* parent=0);

    void setTitle( const QString& title );
    void setTabData();
    void getTabData();

  private:
    void initMuxProgList();
    void initMuxTargetList();

  protected slots:
    void onCreateMuxScript();
    void onCreateMuxStreams();
    void onConfigureMuxer();
    void onOpenOutputPath();
    void onStateDeleteES(int state);
    void onStatePause(int state);
};

#endif
