/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttvideoheaderlist.cpp                                           */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 08/29/2007 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTVIDEOHEADERLIST
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Overview
// -----------------------------------------------------------------------------
//
//               +- TTAudioHeaderList
//               |
//               +- TTAudioIndexList
// TTHeaderList -|
//               +- TTVideoHeaderList
//               |
//               +- TTVideoIndexList
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

#include "ttvideoheaderlist.h"

#include "../common/ttexception.h"

bool videoHeaderListCompareItems( TTAVHeader* head_1, TTAVHeader* head_2 );

const char c_name[] = "TTVideoHeaderList";

/*! ////////////////////////////////////////////////////////////////////////////
 * Constructor
 */
TTVideoHeaderList::TTVideoHeaderList(int size)
  :TTHeaderList( size )
{
}

TTVideoHeaderList::~TTVideoHeaderList()
{
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the header type at header list index position
 */
quint8 TTVideoHeaderList::headerTypeAt(int index)
{
  checkIndexRange(index);
  return at(index)->headerType();
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the TTVideoHeader at header list index position
 */
TTVideoHeader* TTVideoHeaderList::headerAt(int index)
{
  checkIndexRange(index);
  return (TTVideoHeader*)at( index );
}

/*! //////////////////////////////////////////////////////////////////////////////////////
 *
 */
TTVideoHeader* TTVideoHeaderList::getPrevHeader(int startPos, TTMpeg2VideoHeader::mpeg2StartCodes type)
{
  int prevIndex = startPos-1;

  if (prevIndex < 0 || prevIndex >= count())
    return NULL;

  if (prevIndex >= 0 && type == TTMpeg2VideoHeader::ndef)
    return (TTVideoHeader*)at(prevIndex);

  while (prevIndex >= 0)
  {
    if (at(prevIndex)->headerType() == type)
      return (TTVideoHeader*)at(prevIndex);

    prevIndex--;
  }

  return NULL;
}

/*! //////////////////////////////////////////////////////////////////////////////////////
 *
 */
TTVideoHeader* TTVideoHeaderList::getNextHeader(int startPos, TTMpeg2VideoHeader::mpeg2StartCodes type)
{
  int nextIndex = startPos+1;

  if (nextIndex < 0 || nextIndex >= count())
    return NULL;

  if (nextIndex < count() && type == TTMpeg2VideoHeader::ndef) {
    TTAVHeader* next = at(nextIndex);

    if (next == at(startPos))
      qDebug("ksfjgkdfjgkdjfgkldjf");

    return (TTVideoHeader*)at(nextIndex);
  }

  while (nextIndex < count())
  {
    if (at(nextIndex)->headerType() == type)
      return (TTVideoHeader*)at(nextIndex);

    nextIndex++;
  }

  return NULL;
}

TTVideoHeader* TTVideoHeaderList::getNextHeader(TTVideoHeader* current, TTMpeg2VideoHeader::mpeg2StartCodes type)
{
  return (current != NULL)
      ? getNextHeader(indexOf((TTAVHeader*)current), type)
      : NULL;
}

TTVideoHeader* TTVideoHeaderList::getPrevHeader(TTVideoHeader* current, TTMpeg2VideoHeader::mpeg2StartCodes type)
{
  return (current != NULL)
      ? getPrevHeader(indexOf((TTAVHeader*)current), type)
      : NULL;
}


int TTVideoHeaderList::findIndexOf(TTVideoHeader* current)
{
  for (int i = 0; i < size(); i++) {
    TTAVHeader* check = at(i);

    if (check == NULL)
      qDebug("check is null!");

    if (check->headerOffset() == ((TTAVHeader*)current)->headerOffset())
      return i;
  }
  return -2;
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the TTSequenceHeader at header list index position
 */
TTSequenceHeader* TTVideoHeaderList::sequenceHeaderAt(int index)
{
  checkIndexRange(index);
  return (TTSequenceHeader*)at( index );
}

/* /////////////////////////////////////////////////////////////////////////////
 * Returns the first TTSequenceHeader in index list
 */
TTSequenceHeader* TTVideoHeaderList::firstSequenceHeader()
{
  int    index = -1;
  quint8 type  = 0xFF;

  if (size() == 0)
  	throw new TTInvalidOperationException("Invalid");

  do
  {
    index++;
    type = at(index)->headerType();
  } while (index < count() && type != TTMpeg2VideoHeader::sequence_start_code);

  return (at(index)->headerType() == TTMpeg2VideoHeader::sequence_start_code)
    ? (TTSequenceHeader*)at( index )
    : NULL;
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the TTPicturesHeader at header list index position
 */
TTPicturesHeader* TTVideoHeaderList::pictureHeaderAt( int index )
{
  checkIndexRange( index );
  return (TTPicturesHeader*)at( index );
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the TTGOPHeader at header list index position
 */
TTGOPHeader* TTVideoHeaderList::gopHeaderAt( int index )
{
  checkIndexRange( index );
  return (TTGOPHeader*)at( index );
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the index position of the current TTVideoHeader
 */
int TTVideoHeaderList::headerIndex( TTVideoHeader* current )
{
  if (size() == 0) {
    QString msg = QString("No items in list!");
    throw TTInvalidOperationException(msg);
  }

  return indexOf( (TTAVHeader*)current );
}

// -----------------------------------------------------------------------------
// IDD-Index file operations
// -----------------------------------------------------------------------------
// Aufbau der Indexdatei Video:
// -----------------------------------------------------------------------------
//
//   3 Byte Zeichenkette 'idd' (Indexdatei)
//   1 Byte Versionsnummer
//
//   Wiederholen bis Dateiende
//   -------------------------------------------------------------
//     1 Byte Headertype       ($B3-Sequenzheader,
//                              $B8-Gruppenheader,
//                              $00-Bildheader)
//     8 Byte (Int64) Adresse des Headers in der Datei
//                    (inclusive 4 Byte Startcode $00 00 01 xx)
//     >> Wenn Bildheader dann
//        2 Byte (Wort) temporaere Referenz
//        1 Byte        Bildtype (1-IFrame, 2-PFrame, 3-BFrame)
//   -------------------------------------------------------------
//   Wiederholen Ende
//
//   1 Byte HeaderType         ($B7-Sequenzendcode)
//   8 Byte Adresse            (wird zum kopieren des letzten Bildes gebraucht)
// -----------------------------------------------------------------------------

/*! ///////////////////////////////////////////////////////////////////////////
 * Sort the header list by header offset
 */
void TTVideoHeaderList::sort()
{
  qSort( begin(), end(), videoHeaderListCompareItems );
}

/*! ////////////////////////////////////////////////////////////////////////////
 * compare routine for sort
 */
bool videoHeaderListCompareItems( TTAVHeader* head_1, TTAVHeader* head_2 )
{
  return (head_1->headerOffset() < head_2->headerOffset());
}
