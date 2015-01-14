/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : tmpegaudioheader.h                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/11/2005 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TMPEGAUDIOHEADER
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Overview
// -----------------------------------------------------------------------------
//
//                               +- TTMpegAudioHeader
//             +- TTAudioHeader -|                  
//             |                 +- TTAC3AudioHeader 
// TTAVHeader -|                 
//             |
//             |                                     +- TTSequenceHeader
//             |                                     |
//             |                                     +- TTSequenceEndHeader
//             +- TTVideoHeader -TTMpeg2VideoHeader -|
//             |                                     +- TTPicturesHeader
//             |                                     |
//             |                                     +- TTGOPHeader
//             |
//             +- TTVideoIndex
//             |
//             +- TTBreakObject
//
// -----------------------------------------------------------------------------

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

// Aufbau der Mpeg AudioHeader:  (kein Anspruch auf Vollstaendigkeit)
// -----------------------------------------------------------------------------
// SyncWort: FF E    (11111111 111)   (4 Byte)
//     2 Bit Version   (0, 2, 3)
//     2 Bit Layer;    (1, 2, 3)
//     1 Bit Protection
//     4 Bit Bitrate   (1 - 14)
//     2 Samplerate    (0, 1, 2)
//     1 Bit Padding
//     1 Bit Privat
//     2 Bit Mode
//     2 Bit ModeErweiterung
//     1 Bit Copyright
//     1 Bit Orginal
//     2 Bit Emphasis
// -----------------------------------------------------------------------------

#ifndef TTMPEGAUDIOHEADER_H
#define TTMPEGAUDIOHEADER_H

#include "common/ttmessagelogger.h"
#include "ttavheader.h"

class QString;

__attribute__ ((unused))static int  mpeg_sample_raten[4][4] =
  {
    {11025, 12000, 8000, 0}, // version 2.5
    {0, 0, 0, 0},            // unknown
    {22050, 24000, 16000, 0},// version 2.0
    {44100, 48000, 32000, 0} // version 1.0
  };

__attribute__ ((unused))static int mpeg_bit_raten[4][4][16] =
  {
    //Version 2.5, Layer III, II, I
    {{0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,0},
     {0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0},
     {0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0},
     {0,32,48,56, 64, 80, 96,112,128,144,160,176,192,224,256,0}},
    //Version Unbekannt
    {{0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    //Version 2, Layer III, II, I
    {{0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,0},
     {0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0},
     {0, 8,16,24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,0},
     {0,32,48,56, 64, 80, 96,112,128,144,160,176,192,224,256,0}},
    //Version 1, Layer III, II, I
    {{0, 0, 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,0},
     {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,0},
     {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,0},
     {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,0}}
  };


class TTMpegAudioHeader : public TTAudioHeader
{
public:
  TTMpegAudioHeader();

  QString& descString();
  QString& modeString();
  int      bitRate();
  QString& bitRateString();
  int      sampleRate();
  QString& sampleRateString();

  //private:
  quint8 version;
  quint8 layer;
  bool   protection_bit;
  quint8 bitrate_index;
  quint8 bit_rate;
  quint8 sampling_frequency;
  quint8 sample_rate;
  bool   padding_bit;
  bool   private_bit;
  quint8 mode;
  quint8 mode_extension;
  bool   copyright;
  bool   original_home;
  quint8 emphasis;

  TTMessageLogger* log;
};

#endif //TTMPEGAUDIOHEADER_H

