/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttcutsettings.cpp                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/01/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/05/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// *** TTCUTSETTINGS
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


#include "ttcutsettings.h"


// /////////////////////////////////////////////////////////////////////////////
// -----------------------------------------------------------------------------
// TTCut settings object
// -----------------------------------------------------------------------------
// /////////////////////////////////////////////////////////////////////////////
TTCutSettings::TTCutSettings()
  : QSettings("TriTime", "TTCut")
{

}


TTCutSettings::~TTCutSettings()
{

}


void TTCutSettings::readSettings()
{
  // read application settings
  // ---------------------------------------------------------------------------
  // Navigation settings
  // ---------------------------------------------------------------------------
  beginGroup( "/Settings" );

  beginGroup( "/Navigation" );
  TTCut::fastSlider      = value( "FastSlider/", TTCut::fastSlider ).toBool();
  TTCut::stepSliderClick = value( "StepSliderClick/", TTCut::stepSliderClick ).toInt();
  TTCut::stepPgUpDown    = value( "StepPgUpDown/",TTCut::stepPgUpDown ).toInt();
  TTCut::stepArrowKeys   = value( "StepArrowKeys/",TTCut::stepArrowKeys ).toInt();
  TTCut::stepPlusAlt     = value( "StepPlusAlt/", TTCut::stepPlusAlt ).toInt();
  TTCut::stepPlusCtrl    = value( "StepPlusCtrl/", TTCut::stepPlusCtrl ).toInt();
  TTCut::stepQuickJump   = value( "StepQuickJump/", TTCut::stepQuickJump ).toInt();
  TTCut::stepMouseWheel  = value( "StepMouseWheel/", TTCut::stepMouseWheel ).toInt();
  endGroup();

  // Common options
  // ---------------------------------------------------------------------------
  beginGroup( "/Common" );
  TTCut::tempDirPath = value( "TempDirPath/", TTCut::tempDirPath ).toString();
  TTCut::lastDirPath = value( "LastDirPath/", TTCut::lastDirPath ).toString();
  endGroup();

  // Preview
  // ---------------------------------------------------------------------------
  beginGroup( "/Preview" );
  TTCut::cutPreviewSeconds = value( "PreviewSeconds/", TTCut::cutPreviewSeconds ).toInt();
  TTCut::playSkipFrames    = value( "SkipFrames/", TTCut::playSkipFrames ).toInt();
  endGroup();

  // Search
  // ---------------------------------------------------------------------------
  beginGroup( "/Search" );
  TTCut::searchLength   = value( "Length/", TTCut::searchLength ).toInt();
  TTCut::searchAccuracy = value( "Accuracy/", TTCut::searchAccuracy ).toInt();
  endGroup();

  // Index files
  // ---------------------------------------------------------------------------
  beginGroup( "/IndexFiles" );
  TTCut::createVideoIDD = value( "CreateVideoIDD/", TTCut::createVideoIDD ).toBool();
  TTCut::createAudioIDD = value( "CreateAudioIDD/", TTCut::createAudioIDD ).toBool();
  TTCut::createPrevIDD  = value( "CreatePrevIDD/", TTCut::createPrevIDD ).toBool();
  TTCut::createD2V      = value( "CreateD2V/", TTCut::createD2V ).toBool();
  TTCut::readVideoIDD   = value( "ReadVideoIDD/", TTCut::readVideoIDD ).toBool();
  TTCut::readAudioIDD   = value( "ReadAudioIDD", TTCut::readAudioIDD ).toBool();
  TTCut::readPrevIDD    = value( "ReadPrevIDD/", TTCut::readPrevIDD ).toBool();
  endGroup();

  // Log file
  // --------------------------------------------------------------------------
  beginGroup( "/LogFile" );
  TTCut::createLogFile     = value( "CreateLogFile/",     TTCut::createLogFile ).toBool();
  TTCut::logModeConsole    = value( "LogModeConsole/",    TTCut::logModeConsole ).toBool();
  TTCut::logModeExtended   = value( "LogModeExtended/",   TTCut::logModeExtended ).toBool();
  TTCut::logVideoIndexInfo = value( "LogVideoIndexInfo/", TTCut::logVideoIndexInfo ).toBool();
  TTCut::logAudioIndexInfo = value( "LogAudioIndexInfo/", TTCut::logAudioIndexInfo ).toBool();
  endGroup();
  
  // Encoder settings
  // ---------------------------------------------------------------------------
  beginGroup( "/Encoder" );
  TTCut::encoderMode = value( "EncoderMode/", TTCut::encoderMode ).toBool();
  endGroup();

  // Muxer settings
  // ---------------------------------------------------------------------------
  beginGroup( "/Muxer" );
  TTCut::muxMode       = value( "MuxMode/",      TTCut::muxMode ).toInt();
  TTCut::mpeg2Target   = value( "Mpeg2Target/",  TTCut::mpeg2Target ).toInt();
  TTCut::muxProg       = value( "MuxProg/",      TTCut::muxProg ).toString();
  TTCut::muxProgPath   = value( "MuxProgPath/",  TTCut::muxProgPath ).toString();
  TTCut::muxProgCmd    = value( "MuxProgCmd/",   TTCut::muxProgCmd ).toString();
  TTCut::muxOutputPath = value( "MuxOutputDir/", TTCut::muxOutputPath ).toString();
  TTCut::muxDeleteES   = value( "MuxDeleteES/",  TTCut::muxDeleteES ).toBool();
  TTCut::muxPause      = value( "MuxPause/",     TTCut::muxPause ).toBool();
  endGroup();

  // Chapter settings
  // ---------------------------------------------------------------------------
  beginGroup( "/Chapter" );
  TTCut::spumuxChapter = value( "SpumuxChapter/", TTCut::spumuxChapter ).toBool();
  endGroup();

  // Cut option
  // ---------------------------------------------------------------------------
  beginGroup( "/CutOptions" );
  TTCut::cutDirPath         = value( "DirPath/", TTCut::cutDirPath ).toString();
  TTCut::cutVideoName       = value( "VideoName/", TTCut::cutVideoName ).toString();
  TTCut::cutWriteMaxBitrate = value( "WriteMaxBitrate/", TTCut::cutWriteMaxBitrate ).toBool();
  TTCut::cutWriteSeqEnd     = value( "WriteSeqEnd/", TTCut::cutWriteSeqEnd ).toBool();
  TTCut::correctCutTimeCode = value( "CorrectTimeCode/", TTCut::correctCutTimeCode ).toBool();
  TTCut::correctCutBitRate  = value( "CorrectBitrate/", TTCut::correctCutBitRate ).toBool();
  TTCut::createCutIDD       = value( "CreateIDD/", TTCut::createCutIDD ).toBool();
  TTCut::readCutIDD         = value( "ReadIDD/", TTCut::readCutIDD ).toBool();
  endGroup();

  // Recent files
  // --------------------------------------------------------------------------
  beginGroup( "/RecentFiles" );
  TTCut::recentFileList    = value( "RecentFiles/", TTCut::recentFileList ).toStringList();
  endGroup();
  
  endGroup(); // settings

  // check temporary path; we must ensure taht we have a temporary directory
  // the temporary directory is used for the preview clips and for
  // the temporary avi-clips
  if ( !QDir( TTCut::tempDirPath ).exists() )
    TTCut::tempDirPath = QDir::tempPath();

  // check the cut directory path
  if ( !QDir( TTCut::cutDirPath ).exists() )
    TTCut::cutDirPath = QDir::currentPath();
}


void TTCutSettings::writeSettings()
{
  beginGroup( "/Settings" );

  // Navigation settings
  // ---------------------------------------------------------------------------
  beginGroup( "/Navigation" );
  setValue( "FastSlider/",      TTCut::fastSlider );
  setValue( "StepSliderClick/", TTCut::stepSliderClick );
  setValue( "StepPgUpDown/",    TTCut::stepPgUpDown );
  setValue( "StepArrowKeys/",   TTCut::stepArrowKeys );
  setValue( "StepPlusAlt/",     TTCut::stepPlusAlt );
  setValue( "StepPlusCtrl/",    TTCut::stepPlusCtrl );
  setValue( "StepQuickJump/",   TTCut::stepQuickJump );
  setValue( "StepMouseWheel/",  TTCut::stepMouseWheel );
  endGroup();

  // Common options
  // ---------------------------------------------------------------------------
  beginGroup( "/Common" );
  setValue( "TempDirPath/" , TTCut::tempDirPath );
  setValue( "LastDirPath/" , TTCut::lastDirPath );
  endGroup();

  // Preview
  // ---------------------------------------------------------------------------
  beginGroup( "/Preview" );
  setValue( "PreviewSeconds/",  TTCut::cutPreviewSeconds );
  setValue( "SkipFrames/",      TTCut::playSkipFrames );
  endGroup();

  // Search
  // ---------------------------------------------------------------------------
  beginGroup( "/Search" );
  setValue( "Length/",          TTCut::searchLength );
  setValue( "Accuracy/",        TTCut::searchAccuracy );
  endGroup();

  // Index files
  // ---------------------------------------------------------------------------
  beginGroup( "/IndexFiles" );
  setValue( "CreateVideoIDD/",  TTCut::createVideoIDD );
  setValue( "CreateAudioIDD/",  TTCut::createAudioIDD );
  setValue( "CreatePrevIDD/",   TTCut::createPrevIDD );
  setValue( "CreateD2V/",       TTCut::createD2V );
  setValue( "ReadVideoIDD/",    TTCut::readVideoIDD );
  setValue( "ReadAudioIDD",     TTCut::readAudioIDD );
  setValue( "ReadPrevIDD/",     TTCut::readPrevIDD );
  endGroup();

  // Log file
  // --------------------------------------------------------------------------
  beginGroup( "/LogFile" );
  setValue( "CreateLogFile/",     TTCut::createLogFile );
  setValue( "LogModeConsole/",    TTCut::logModeConsole );
  setValue( "LogModeExtended/",   TTCut::logModeExtended );
  setValue( "LogVideoIndexInfo/", TTCut::logVideoIndexInfo );
  setValue( "LogAudioIndexInfo/", TTCut::logAudioIndexInfo );
  endGroup();
  
  // Encoder settings
  // ---------------------------------------------------------------------------
  beginGroup( "/Encoder" );
  setValue( "EncoderMode/",     TTCut::encoderMode );
  endGroup();

  // Muxer settings
  // ---------------------------------------------------------------------------
  beginGroup( "/Muxer" );
  setValue( "MuxMode/",         TTCut::muxMode );
  setValue( "Mpeg2Target/",     TTCut::mpeg2Target );
  setValue( "MuxProg/",         TTCut::muxProg );
  setValue( "MuxProgPath/",     TTCut::muxProgPath );
  setValue( "MuxProgCmd/",      TTCut::muxProgCmd );
  setValue( "MuxOutputDir/",    TTCut::muxOutputPath );
  setValue( "MuxDeleteES/",     TTCut::muxDeleteES );
  setValue( "MuxPause/",        TTCut::muxPause );
  endGroup();

  // Chapter settings
  // ---------------------------------------------------------------------------
  beginGroup( "/Chapter" );
  setValue( "SpumuxChapter/",   TTCut::spumuxChapter );
  endGroup();

  // Cut option
  // ---------------------------------------------------------------------------
  beginGroup( "/CutOptions" );
  setValue( "DirPath/",         TTCut::cutDirPath );
  setValue( "VideoName/",       TTCut::cutVideoName );
  setValue( "WriteMaxBitrate/", TTCut::cutWriteMaxBitrate );
  setValue( "WriteSeqEnd/",     TTCut::cutWriteSeqEnd );
  setValue( "CorrectTimeCode/", TTCut::correctCutTimeCode );
  setValue( "CorrectBitrate/",  TTCut::correctCutBitRate );
  setValue( "CreateIDD/",       TTCut::createCutIDD );
  setValue( "ReadIDD/",         TTCut::readCutIDD );
  endGroup();

  // Recent files
  // --------------------------------------------------------------------------
  beginGroup( "/RecentFiles" );
  setValue( "RecentFiles/", TTCut::recentFileList );
  endGroup();
 
  endGroup(); // settings

  sync();
}
