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

#include "ttaudioheaderlist.h"

bool audioHeaderListCompareItems( TTAVHeader* head_1, TTAVHeader* head_2 );

TTAudioHeaderList::TTAudioHeaderList( int size )
  : TTHeaderList( size )
{

}

TTAudioHeader* TTAudioHeaderList::audioHeaderAt( int index )
{
  checkIndexRange(index);
    
  return (TTAudioHeader*)at( index );
}


int TTAudioHeaderList::searchTimeIndex( double s_time )
{
  int           abs_time = 0;
  int           search_time = (int)s_time*1000;
  TTAudioHeader* audio_header;

  int index = 0;

  do
  {
    audio_header = (TTAudioHeader*)at(index);
    abs_time = (int)(audio_header->abs_frame_time*1000);
    index++;
  }
  while ( abs_time <= search_time );

  return index-2;
}

void TTAudioHeaderList::sort()
{
  qSort( begin(), end(), audioHeaderListCompareItems );
}

bool audioHeaderListCompareItems( TTAVHeader* head_1, TTAVHeader* head_2 )
{
  // the values for the display order of two items are compared
  int time1 = (int)((TTAudioHeader*)head_1)->abs_frame_time*1000;
  int time2 = (int)((TTAudioHeader*)head_2)->abs_frame_time*1000;

  return (time1 < time2);
}
