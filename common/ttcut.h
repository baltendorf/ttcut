/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttcut.h                                                         */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/01/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/19/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/23/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/31/2005 */
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


#include "avstream/ttcommon.h"

#ifndef TTCUT_H
#define TTCUT_H

class QPixmap;
class QString;
class QWidget;

extern "C" const char* toAscii(const QString& string);

class TTCut
{
 public:
   TTCut();
   ~TTCut();

 public:
  // icons
   static QPixmap* imgDownArrow;
   static QPixmap* imgUpArrow;
   static QPixmap* imgDelete;
   static QPixmap* imgFileOpen24;
   static QPixmap* imgFileNew;
   static QPixmap* imgFileOpen;
   static QPixmap* imgFileSave;
   static QPixmap* imgFileSaveAs;
   static QPixmap* imgSaveImage;
   static QPixmap* imgSettings;
   static QPixmap* imgSettings18;
   static QPixmap* imgExit;
   static QPixmap* imgPlay;
   static QPixmap* imgStop;
   static QPixmap* imgSearch;
   static QPixmap* imgChapter;
   static QPixmap* imgPreview;
   static QPixmap* imgCutAV;
   static QPixmap* imgCutAudio;
   static QPixmap* imgGoTo;
   static QPixmap* imgMarker;
   static QPixmap* imgClock;
   static QPixmap* imgApply;
   static QPixmap* imgAddToList;
   static QPixmap* imgFileClose;

   // for TTMpeg2
   static QPixmap* imgIFrame;
   static QPixmap* imgPFrame;
   static QPixmap* imgBFrame;


   // --------------------------------------------------------------
   // common settings
   // --------------------------------------------------------------
   // Version
   static QString versionString;

   // Options
   static bool    fastSlider;
   static QString tempDirPath;
   static QString lastDirPath;
   static QString projectFileName;

   // Preview
   static int cutPreviewSeconds;
   static int playSkipFrames;

   // Frame search
   static int searchLength;
   static int searchAccuracy;

   // Navigation
   static int stepSliderClick;
   static int stepPgUpDown;
   static int stepArrowKeys;
   static int stepPlusAlt;
   static int stepPlusCtrl;
   static int stepPlusShift;
   static int stepQuickJump;
   static int stepMouseWheel;

   // Index files
   static bool createVideoIDD;
   static bool createAudioIDD;
   static bool createPrevIDD;
   static bool createD2V;
   static bool readVideoIDD;
   static bool readAudioIDD;
   static bool readPrevIDD;

   // Logfile
   static bool createLogFile;
   static bool logModeConsole;
   static bool logModeExtended;
   static bool logVideoIndexInfo;
   static bool logAudioIndexInfo;

   // Recent files
   static QStringList recentFileList;

   // --------------------------------------------------------------
   // encoder settings
   // --------------------------------------------------------------
   // Options
   static bool    encoderMode;

   // --------------------------------------------------------------
   // muxer settings
   // --------------------------------------------------------------
   // Options
   static int     muxMode;
   static int     mpeg2Target;
   static QString muxProg;
   static QString muxProgPath;
   static QString muxProgCmd;
   static QString muxOutputPath;
   static bool    muxDeleteES;
   static bool    muxPause;

   // --------------------------------------------------------------
   // chapter settings
   // --------------------------------------------------------------
   // Options
   static bool    spumuxChapter;


   // --------------------------------------------------------------
   // Status variables
   // --------------------------------------------------------------
   static bool isVideoOpen;
   static int  numAudioTracks;
   static bool isProjektModified;
   static bool isPlaying;
   static bool isWorking;

   // --------------------------------------------------------------
   // Cut settings
   // --------------------------------------------------------------
   static QString muxFileName;
   static QString cutDirPath;
   static QString cutVideoName;
   static bool    cutWriteMaxBitrate;
   static bool    cutWriteSeqEnd;
   static bool    correctCutTimeCode;
   static bool    correctCutBitRate;
   static bool    createCutIDD;
   static bool    readCutIDD;

   // --------------------------------------------------------------
   // Global properties
   // --------------------------------------------------------------
   static float        frameRate;
   static QWidget*     mainWindow;
};


#endif

