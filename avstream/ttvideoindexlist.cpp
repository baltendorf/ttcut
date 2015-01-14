/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2005/2010 / www.tritime.org                    */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttvideoindexlist.h                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/01/2008 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTVIDEOINDEXLIST
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


#include "ttheaderlist.h"
#include "ttvideoindexlist.h"
#include "common/ttexception.h"

const char c_name[] = "TTVIDEOINDEX  : ";

/*! ////////////////////////////////////////////////////////////////////////////
 * Compare function for sorting the video index list by display order
 */
bool compareFunc( TTVideoIndex* index_1, TTVideoIndex* index_2 )
{
  return (index_1->getDisplayOrder() < index_2->getDisplayOrder());
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Construct a index list object
 */
TTVideoIndexList::TTVideoIndexList()
{
  log = TTMessageLogger::getInstance();
  current_order = 0;  //0: stream order; 1: display order
}

/*! /////////////////////////////////////////////////////////////////////////////
 * Destructor
 */
TTVideoIndexList::~TTVideoIndexList()
{
}

/*! /////////////////////////////////////////////////////////////////////////////
 * Add index to video index list
 */
void TTVideoIndexList::add(TTVideoIndex* index)
{
 append(index); 
}

void TTVideoIndexList::deleteAll()
{
  for (int i = 0; i < size(); i++)
  {
    TTVideoIndex* v_index = at(i);
    if (v_index != NULL) {
      delete v_index;
    }
  }
  clear();
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the TTVideoIndex object at index list position index
 */
TTVideoIndex* TTVideoIndexList::videoIndexAt( int index )
{
  checkIndexRange( index );
  return (TTVideoIndex*)at( index );
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Sort list items in display order
 */
void TTVideoIndexList::sortDisplayOrder()
{
  sort();
  current_order = 1;
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns true if the index list is sorted in stream order
 */
bool TTVideoIndexList::isStreamOrder()
{
	return (current_order == 0);
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns true if the index list is sorted in display order
 */
bool TTVideoIndexList::isDisplayOrder()
{
	return (current_order == 1);
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the display order of the picture at index positon
 */
int TTVideoIndexList::displayOrder( int index )
{
  return videoIndexAt(index)->getDisplayOrder();
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the index in video header list of picture at given index
 */
int TTVideoIndexList::headerListIndex( int index )
{
  return videoIndexAt(index)->getHeaderListIndex();
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Returns the picture_coding_type (I=1,P=2,B=3) at index position
 */
int  TTVideoIndexList::pictureCodingType( int index )
{
  return videoIndexAt(index)->getPictureCodingType();
}

/*! ////////////////////////////////////////////////////////////////////////////
 * Sort the video index list by display order
 */
void TTVideoIndexList::sort()
{
  qSort( begin(), end(), compareFunc );
}

/*! ///////////////////////////////////////////////////////////////////////////// 
 * Move to the next position after start_pos and frame_type in index list
 */
int TTVideoIndexList::moveToNextIndexPos(int start_pos, int frame_type)
{  
  TTVideoIndex* v_index;
  int index = start_pos+1;

  if (index < 0 || index >= count())
    return -1;

  if (frame_type <= 0) {
    return index;
  }

  do {
	  v_index = videoIndexAt(index);

    if (v_index->getPictureCodingType() == frame_type)
      break;

	  index++;
  } while (index < count());

  return (v_index->getPictureCodingType() == frame_type)
        ? index
        : -1;
}

/*! ///////////////////////////////////////////////////////////////////////////// 
 * Move to the previous position before start_pos and frame_type in index list
 */
int TTVideoIndexList::moveToPrevIndexPos(int start_pos, int frame_type)
{
  TTVideoIndex* v_index;
  int index = start_pos-1;

  if (index < 0 || index >= count())
    return -1;

  if (frame_type <= 0) {
    return index;
  }

  do {
	  v_index = videoIndexAt(index);

    if (v_index->getPictureCodingType() == frame_type)
      break;

	  index--;
  } while (index >= 0);

  return (v_index->getPictureCodingType() == frame_type)
        ? index
        : -1;
}

/*! ///////////////////////////////////////////////////////////////////////////// 
 * move to specified index position (index and frame coding type)
 */
int TTVideoIndexList::moveToIndexPos(int index, int frame_type)
{
  return moveToNextIndexPos(index-1, frame_type);
}

/*! /////////////////////////////////////////////////////////////////////////////
 * Check if given index is in list range and throw an exception if index isn't
 * in range
 */
void TTVideoIndexList::checkIndexRange( int index )
{
  if (index < 0 || index >= size()) {
    QString msg = QString("Index %1 exceeds list bounds: %2").arg(index).arg(count());
    throw TTIndexOutOfRangeException(msg);
  }
}


