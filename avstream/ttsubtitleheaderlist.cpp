/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttaudioheaderlist.cpp                                           */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// *** TTAUDIOHEADERLIST
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
//               |
//               +- TTSubtitleHeaderList
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

#include "ttsubtitleheaderlist.h"

bool subtitleHeaderListCompareItems( TTAVHeader* head_1, TTAVHeader* head_2 );

TTSubtitleHeaderList::TTSubtitleHeaderList( int size )
  : TTHeaderList( size )
{

}

TTSubtitleHeader* TTSubtitleHeaderList::subtitleHeaderAt( int index )
{
  checkIndexRange(index);
    
  return (TTSubtitleHeader*)at( index );
}

int TTSubtitleHeaderList::searchTimeIndex( int search_time )
{
  int abs_time = 0;
  TTSubtitleHeader* subtitle_header;

  int index = 0;

  do
  {
    subtitle_header = (TTSubtitleHeader*)at(index);
    abs_time = (int)(subtitle_header->startMSec());
    index++;
  }
  while ( abs_time < search_time && index < size());

  // return index of next subtitle, if search_time is after end of found subtitle
  return subtitle_header->endMSec() < search_time ? index : index-1;
}

void TTSubtitleHeaderList::sort()
{
  qSort( begin(), end(), subtitleHeaderListCompareItems );
}

bool subtitleHeaderListCompareItems( TTAVHeader* head_1, TTAVHeader* head_2 )
{
  // the values for the display order of two items are compared
  int time1 = (int)((TTSubtitleHeader*)head_1)->startMSec();
  int time2 = (int)((TTSubtitleHeader*)head_2)->startMSec();

  return (time1 < time2);
}
