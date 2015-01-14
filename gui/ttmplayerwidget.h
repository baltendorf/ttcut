/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttmplayerwidget.cpp                                             */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/06/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTMPLAYERWIDGET
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

#include <QObject>
#include <QProcess>

#include "common/ttcut.h"
#include "common/ttmessagelogger.h"
#include "ttvideoplayer.h"

#ifndef TTMPLAYERWIDGET_H
#define TTMPLAYERWIDGET_H

class TTMplayerWidget : public TTVideoPlayer
{
    Q_OBJECT
    Q_PROPERTY(bool getControlsVisible READ getControlsVisible WRITE setControlsVisible)

public:
    TTMplayerWidget(QWidget *parent);
    ~TTMplayerWidget();
    
    QSize sizeHint() const;

    bool getControlsVisible() const;
    void setControlsVisible(bool visible);

    void cleanUp();
    void load(const QString& value);
    void play();
    void stop();

protected:
    bool playMplayer(QString videoFile);
    bool stopMplayer();

protected slots:
    void mplayerStarted();
    void readFromStdout();
    void exitMplayer(int e_code, QProcess::ExitStatus e_status);
    void errorMplayer(QProcess::ProcessError);
    void stateChangedMplayer(QProcess::ProcessState newState);

private:
    TTMessageLogger*   log;
    QProcess*          mplayerProc;      
};

#endif
