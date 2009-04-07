/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttmpeg2videoheader.cpp                                          */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTSEQUENCEHEADER
// TTSEQUENCEENDHEADER
// TTGOPHEADER
// TTPICTURESHEADER
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


#include "ttmpeg2videoheader.h"

const char cName[] = "MPEGVIDEOHEADER";

/* /////////////////////////////////////////////////////////////////////////////
 * TTMpeg2VideoHeader
 * Base class for all MPEG2 video header
 */
TTMpeg2VideoHeader::TTMpeg2VideoHeader()
{
  log = TTMessageLogger::getInstance();
}


/* /////////////////////////////////////////////////////////////////////////////
 * TTSequenceHeader: Sequence header [0x000001B3]
 * Default constructor, extends TTMpeg2VideoHeader
 */
TTSequenceHeader::TTSequenceHeader() : TTMpeg2VideoHeader()
{
  header_start_code = sequence_start_code;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Read sequence header from stream
 */
bool TTSequenceHeader::readHeader( TTFileBuffer* mpeg2_stream )
{
  quint8  header_data[8];

  try
  {
    // read 8 byte from stream, starting at current offset
    mpeg2_stream->readByte( header_data, 8 ) ;

    // fill sequence header
    header_offset = mpeg2_stream->position() - 12;
    parseBasicData( header_data );
  }
  catch (TTFileBufferException)
  {
    return false;
  }
  return true;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Read sequence header at given offset
 */
bool TTSequenceHeader::readHeader( TTFileBuffer* mpeg2_stream, quint64 offset )
{
  mpeg2_stream->seekAbsolute( offset+4 );

  return readHeader( mpeg2_stream );
}

/* /////////////////////////////////////////////////////////////////////////////
 * Parse basic header data
 */
void TTSequenceHeader::parseBasicData( quint8* data, int offset )
{
  horizontal_size_value        = (data[offset+0] << 4) + ((data[1] & 0xF0) >> 4);
  vertical_size_value          = ((data[1] & 0x0F) << 8) + data[2];
  aspect_ratio_information     = (data[3] & 0xF0) >> 4;
  frame_rate_code              = (data[3] & 0x0F);
  bit_rate_value               = (int)(((data[4] << 10) + (data[5] << 2)+((data[6] & 0xC0) >> 6))*400);
  vbv_buffer_size_value        = ((data[6] & 0x1F) << 5)+((data[7] & 0xF8) >> 3);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the horizontal size value
 */
int TTSequenceHeader::horizontalSize()
{
  return horizontal_size_value;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the vertical size value
 */
int TTSequenceHeader::verticalSize()
{
  return vertical_size_value;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Return the aspect ratio code
 */
int TTSequenceHeader::aspectRatio()
{
	return aspect_ratio_information;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the aspect ratio code as string value
 */
QString TTSequenceHeader::aspectRatioText()
{
  QString szTemp;

  if ( aspect_ratio_information == 1 ) szTemp = "1:1";
  if ( aspect_ratio_information == 2 ) szTemp = "4:3";
  if ( aspect_ratio_information == 3 ) szTemp = "16:9";
  if ( aspect_ratio_information == 4 ) szTemp = "2.21:1";

  return szTemp;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the frame rate as string value
 */
QString TTSequenceHeader::frameRateText()
{
  QString szTemp;

  if ( frame_rate_code == 2 ) szTemp = "24 fps";
  if ( frame_rate_code == 3 ) szTemp = "25 fps";
  if ( frame_rate_code == 5 ) szTemp = "30 fps";

  return szTemp;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the frame rate value
 */
float TTSequenceHeader::frameRateValue()
{
  float value = 25.0;

  if ( frame_rate_code == 2 ) value = 24.0;
  if ( frame_rate_code == 3 ) value = 25.0;
  if ( frame_rate_code == 5 ) value = 30.0;

  if ( frame_rate_code < 2 || frame_rate_code > 5 )
    log->errorMsg(cName, "Couldn't determine the correct frame rate: assume 25 fps!");

  return value;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the bit rate value in Kbit
 */
float TTSequenceHeader::bitRateKbit()
{
  return (float)bit_rate_value / 1000.0;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the vbv buffer size value
 */
int TTSequenceHeader::vbvBufferSize()
{
  return vbv_buffer_size_value;
}


/* /////////////////////////////////////////////////////////////////////////////
 * TTSequenceEndHeader
 * Default constructor, extends TTMpeg2VideoHeader
 */
  TTSequenceEndHeader::TTSequenceEndHeader()
: TTMpeg2VideoHeader()
{
  header_start_code = sequence_end_code;
}

bool TTSequenceEndHeader::readHeader( TTFileBuffer* mpeg2_stream )
{
  header_offset = mpeg2_stream->position() - 4;

  return true;
}

bool TTSequenceEndHeader::readHeader( TTFileBuffer* mpeg2_stream, quint64 offset )
{
  mpeg2_stream->seekAbsolute( offset+4 );

  return readHeader( mpeg2_stream );
}

void TTSequenceEndHeader::parseBasicData( __attribute__ ((unused))quint8* data, __attribute__ ((unused))int offset )
{

}

/* /////////////////////////////////////////////////////////////////////////////
 * TTGOPHeader: Group of pictures header [000001B8]
 * Default constructor, extends TTMpeg2VideoHeader
 */
  TTGOPHeader::TTGOPHeader()
:TTMpeg2VideoHeader()
{
  header_start_code = group_start_code;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Read the GOP header from given stream
 */
bool TTGOPHeader::readHeader( TTFileBuffer* mpeg2_stream )
{
  quint8 header_data[4];

  try
  {
    mpeg2_stream->readByte( header_data,4 );

    header_offset = mpeg2_stream->position() - 8;
     parseBasicData( header_data );

    return true;
  }
  catch (TTFileBufferException)
  {
    return false;
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Read the GOP header at given offset
 */
bool TTGOPHeader::readHeader( TTFileBuffer* mpeg2_stream, quint64 offset )
{
  mpeg2_stream->seekAbsolute( offset+4 );

  return readHeader( mpeg2_stream );
}

/* /////////////////////////////////////////////////////////////////////////////
 * Parse the basic GOP header data
 */
void TTGOPHeader::parseBasicData( quint8* data, int offset )
{
  time_code.drop_frame_flag = (data[offset+0] >> 7) == 1;
  time_code.hours           = (int)((data[offset+0] & 0x7C) >> 2);
  time_code.minutes         = (int)(((data[offset+0] & 0x03) << 4) + ((data[offset+1] & 0xF0) >> 4));
  time_code.marker_bit      = ((data[offset+1] & 0x08) >> 3) == 1;
  time_code.seconds         = (int)(((data[offset+1] & 0x07) << 3) + ((data[offset+2] & 0xE0) >> 5));
  time_code.pictures        = (int)(((data[offset+2] & 0x1F) << 1) + ((data[offset+3] & 0x80) >> 7));
  closed_gop                = ((data[offset+3] & 0x40) >> 6) == 1;
  broken_link               = ((data[offset+3] & 0x20) >> 5) == 1;
}

/* /////////////////////////////////////////////////////////////////////////////
 * TTPicturesHeader: Pictures header [00000100]
 * Default constructor, extends TTMpeg2VideoHeader
 */
  TTPicturesHeader::TTPicturesHeader()
:TTMpeg2VideoHeader()
{
  header_start_code = picture_start_code;

  vbv_delay = 0;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Read picture header from stream
 */
bool TTPicturesHeader::readHeader( TTFileBuffer* mpeg2_stream )
{
  quint8 header_data[5];

  try
  {
    mpeg2_stream->readByte( header_data, 4 );

    header_offset = mpeg2_stream->position() - 8;
    parseBasicData( header_data );

    return true;
  }
  catch (TTFileBufferException)
  {
    return false;
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Read picture header at given offset.
 */
bool TTPicturesHeader::readHeader( TTFileBuffer* mpeg2_stream, quint64 offset )
{
  mpeg2_stream->seekAbsolute( offset+4 );
  return readHeader( mpeg2_stream );
}

/* /////////////////////////////////////////////////////////////////////////////
 * Parse basic picture header data.
 */
void TTPicturesHeader::parseBasicData( quint8* data, int offset )
{
  picture_coding_type = (int)((data[offset+1] & 0x38) >> 3);
  temporal_reference  = (int)((data[offset+0] << 2) + ((data[offset+1] & 0xC0) >> 6));
  vbv_delay           = ((data[offset+1] & 0x07) << 13) + (data[offset+2] << 5) + ((data[offset+3] & 0xF8) >> 3);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Form an string representing the picture coding type.
 */
QString TTPicturesHeader::codingTypeString()
{
  switch (picture_coding_type)
  {
    case 1:
      return "I";

    case 2:
      return "P";

    case 3:
      return "B";

    default:
      return "-";
  }
}
