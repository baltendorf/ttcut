
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttcut.cpp                                                       */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/01/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/19/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/23/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/31/2005 */
/* MODIFIED: b. altendorf                                    DATE: 04/06/2007 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUT
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

#include "ttcut.h"

#include <QWidget>
#include <QDir>
#include <QCoreApplication>
#include <QString>

// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// Initialize static TTCut class members
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////

// -----------------------------------------------------------------------------
// Pixmaps
// -----------------------------------------------------------------------------
QPixmap* TTCut::imgDownArrow  = NULL;
QPixmap* TTCut::imgUpArrow    = NULL;
QPixmap* TTCut::imgDelete     = NULL;
QPixmap* TTCut::imgFileOpen24 = NULL;
QPixmap* TTCut::imgFileNew    = NULL;
QPixmap* TTCut::imgFileOpen   = NULL;
QPixmap* TTCut::imgFileSave   = NULL;
QPixmap* TTCut::imgFileSaveAs = NULL;
QPixmap* TTCut::imgSaveImage  = NULL;
QPixmap* TTCut::imgSettings   = NULL;
QPixmap* TTCut::imgSettings18 = NULL;
QPixmap* TTCut::imgExit       = NULL;
QPixmap* TTCut::imgPlay       = NULL;
QPixmap* TTCut::imgStop       = NULL;
QPixmap* TTCut::imgSearch     = NULL;
QPixmap* TTCut::imgChapter    = NULL;
QPixmap* TTCut::imgPreview    = NULL;
QPixmap* TTCut::imgCutAV      = NULL;
QPixmap* TTCut::imgCutAudio   = NULL;
QPixmap* TTCut::imgGoTo       = NULL;
QPixmap* TTCut::imgMarker     = NULL;
QPixmap* TTCut::imgClock      = NULL;
QPixmap* TTCut::imgApply      = NULL;
QPixmap* TTCut::imgAddToList  = NULL;
QPixmap* TTCut::imgFileClose  = NULL;


QPixmap* TTCut::imgIFrame     = NULL;
QPixmap* TTCut::imgPFrame     = NULL;
QPixmap* TTCut::imgBFrame     = NULL;


// --------------------------------------------------------------
// common settings
// --------------------------------------------------------------

// Options
bool    TTCut::fastSlider      = false;
QString TTCut::tempDirPath     = QDir::tempPath();
QString TTCut::lastDirPath     = QDir::homePath();
QString TTCut::projectFileName = "";

// Preview
int TTCut::cutPreviewSeconds   = 25;
int TTCut::playSkipFrames      = 0;

// Frame search
int TTCut::searchLength   = 45;
int TTCut::searchAccuracy = 1;

// Navigation
int TTCut::stepSliderClick =  40;
int TTCut::stepPgUpDown    =  80;
int TTCut::stepArrowKeys   =   1;
int TTCut::stepPlusAlt     = 100;
int TTCut::stepPlusCtrl    = 200;
int TTCut::stepPlusShift   = 200;
int TTCut::stepQuickJump   =  25;
int TTCut::stepMouseWheel  = 120;

// Index files
bool TTCut::createVideoIDD = false;
bool TTCut::createAudioIDD = false;
bool TTCut::createPrevIDD  = false;
bool TTCut::createD2V      = false;
bool TTCut::readVideoIDD   = false;
bool TTCut::readAudioIDD   = false;
bool TTCut::readPrevIDD    = false;

 // Logfile
bool TTCut::createLogFile     = true;
bool TTCut::logModeConsole    = false;
bool TTCut::logModeExtended   = true;
bool TTCut::logVideoIndexInfo = false;
bool TTCut::logAudioIndexInfo = false;

// Recent files
QStringList TTCut::recentFileList;

// --------------------------------------------------------------
// encoder settings
// --------------------------------------------------------------
// Version
QString TTCut::versionString = "TTCut - 1.0.1";

// Options
bool TTCut::encoderMode = false;

// --------------------------------------------------------------
// muxer settings
// --------------------------------------------------------------
// Options
int     TTCut::muxMode       = 1;
int     TTCut::mpeg2Target   = 7;
QString TTCut::muxProg       = "mplex";
QString TTCut::muxProgPath   = "/usr/local/bin/";
QString TTCut::muxProgCmd    = "-f 8";
QString TTCut::muxOutputPath = QDir::homePath();
bool    TTCut::muxDeleteES   = false;
bool    TTCut::muxPause      = true;

// --------------------------------------------------------------
// chapter settings
// --------------------------------------------------------------
// Options
bool TTCut::spumuxChapter = false;

// -----------------------------------------------------------------------------
// Status
// -----------------------------------------------------------------------------
bool TTCut::isVideoOpen       = false;
int  TTCut::numAudioTracks    = 0;
bool TTCut::isProjektModified = false;
bool TTCut::isPlaying         = false;
bool TTCut::isWorking         = false;

// --------------------------------------------------------------
// Cut settings
// --------------------------------------------------------------
// cut option
QString  TTCut::muxFileName        = "muxscript.sh";
QString  TTCut::cutDirPath         = QDir::currentPath();
QString  TTCut::cutVideoName       = "_cut.m2v";
bool     TTCut::cutWriteMaxBitrate = false;
bool     TTCut::cutWriteSeqEnd     = false;
bool     TTCut::correctCutTimeCode = false;
bool     TTCut::correctCutBitRate  = false;
bool     TTCut::createCutIDD       = false;
bool     TTCut::readCutIDD         = false;

// --------------------------------------------------------------
// Global properties
// --------------------------------------------------------------
float    TTCut::frameRate          = 25.0;
QWidget* TTCut::mainWindow         = NULL;

TTCut::TTCut()
{

}

TTCut::~TTCut()
{

}


const char* toAscii(const QString& string)
{
	//qDebug(qPrintable(QString("converting string %1").arg(string)));

	char* result = string.toLatin1().data();

	//qDebug(qPrintable(QString("result is %1").arg(result)));

	return result;
}
