/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttmpeg2videoheader.h                                            */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 01/01/2008 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// *** TTSEQUENCEHEADER
// *** TTSEQUENCEENDHEADER
// *** TTGOPHEADER
// *** TTPICTURESHEADER
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


#ifndef TTMPEG2VIDEOHEADER_H
#define TTMPEG2VIDEOHEADER_H

#include "ttcommon.h"
#include "ttfilebuffer.h"
#include "ttavheader.h"


// -----------------------------------------------------------------------------
// Abstract class TTMpeg2VideoHeader
// -----------------------------------------------------------------------------
class TTMpeg2VideoHeader : public TTVideoHeader
{
public:
  TTMpeg2VideoHeader();

  virtual bool readHeader( TTFileBuffer* mpeg2_stream ) = 0;
  virtual bool readHeader( TTFileBuffer* mpeg2_stream, quint64 offset ) = 0;
  virtual void parseBasicData( quint8* data, int offset=0 ) = 0;

enum mpeg2StartCodes
  {
    picture_start_code            = 0x00,
    userdata_start_code           = 0xb2,
    sequence_start_code           = 0xb3,
    sequence_error_code           = 0xb4,
    sequence_extension_code       = 0xb5,
    sequence_end_code             = 0xb7,
    group_start_code              = 0xb8,
    sequence_extension_id         = 0x01,
    sequence_display_extension_id = 0x02,
    ndef                          = 0xFF
  };
};

// -----------------------------------------------------------------------------
// Sequence header [0x000001B3]
// -----------------------------------------------------------------------------
class TTSequenceHeader : public TTMpeg2VideoHeader
{
 public:
  TTSequenceHeader();

  bool readHeader( TTFileBuffer* mpeg2_stream );
  bool readHeader( TTFileBuffer* mpeg2_stream, quint64 offset );
  void parseBasicData( quint8* data, int offset=0);

  int     horizontalSize();
  int     verticalSize();
  int     aspectRatio();
  QString aspectRatioText();
  QString frameRateText();
  float   frameRateValue();
  float   bitRateKbit();
  int     vbvBufferSize();

  // from sequence [B3]
  int      horizontal_size_value;
  int      vertical_size_value;
  int      aspect_ratio_information;
  int      frame_rate_code;
  int      bit_rate_value;
  int      vbv_buffer_size_value;
};

/*! \brief SequenceEndHeader
 *
 */
class TTSequenceEndHeader : public TTMpeg2VideoHeader
{
 public:
  TTSequenceEndHeader();

  bool readHeader( TTFileBuffer* mpeg2_stream );
  bool readHeader( TTFileBuffer* mpeg2_stream, quint64 offset );
  void parseBasicData( quint8* data, int offset=0);
};

// -----------------------------------------------------------------------------
// Group of pictures header [000001B8]
// -----------------------------------------------------------------------------
class TTGOPHeader : public TTMpeg2VideoHeader
{
public:
   TTGOPHeader();

  bool readHeader( TTFileBuffer* mpeg2_stream );
  bool readHeader( TTFileBuffer* mpeg2_stream, quint64 offset );
  void parseBasicData( quint8* data, int offset=0 );

   // from group_of_pictures_header [B8]
   TTimeCode time_code;
   bool      closed_gop;
   bool      broken_link;
};

// -----------------------------------------------------------------------------
// Pictures header [00000100]
// -----------------------------------------------------------------------------
class TTPicturesHeader : public TTMpeg2VideoHeader
{
 public:
  TTPicturesHeader();

  bool    readHeader( TTFileBuffer* mpeg2_stream );
  bool    readHeader( TTFileBuffer* mpeg2_stream, quint64 offset );
  void    parseBasicData( quint8* data, int offset=0 );
  QString codingTypeString();

  // from picture_header [00]
  int     temporal_reference;
  int     picture_coding_type;
  int     vbv_delay;
  bool    progressive_frame;
};
#endif //TTMPEG2VIDEOHEADER_H
