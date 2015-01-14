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

#ifndef TTVIDEOINDEXLIST_H
#define TTVIDEOINDEXLIST_H

#include <QVector>

#include "ttmpeg2videoheader.h"
#include "common/ttmessagelogger.h"

// -----------------------------------------------------------------------------
// TTVideoIndexList: List of pointers to TTFrameIndex 
// -----------------------------------------------------------------------------
class TTVideoIndexList : public QVector<TTVideoIndex*>
{
 public:
  TTVideoIndexList();
  virtual ~TTVideoIndexList();

  void add(TTVideoIndex* index);
  void deleteAll();

  TTVideoIndex* videoIndexAt(int index);

  void sortDisplayOrder();
  bool isStreamOrder();
  bool isDisplayOrder();

  int moveToNextIndexPos(int start_pos, int frame_type=0);
  int moveToPrevIndexPos(int start_pos, int frame_type=0);
  int moveToIndexPos(int index, int frame_type=0);

  int  displayOrder(int index);
  int  streamOrder(int index);
  int  headerListIndex(int index);
  int  pictureCodingType(int index);
 
 protected:
  virtual void sort();
  void checkIndexRange(int index);

 protected:
  TTMessageLogger* log;
  int current_order;
};
#endif //TTVIDEOINDEXLIST_H


