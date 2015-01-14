/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttheaderlist.h                                                  */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/20/2008 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTHEADERLIST
// ----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// Overview
// -----------------------------------------------------------------------------
//
//               +- TTAudioHeaderList 
//               | 
// TTHeaderList -+
//               |
//               +- TTVideoHeaderList
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

#ifndef TTHEADERLIST_H
#define TTHEADERLIST_H

#include "ttavheader.h"
#include "common/ttmessagelogger.h"

#include <QVector>

// -----------------------------------------------------------------------------
// TTHeaderList: Pointer list for TTAVHeader objects
// -----------------------------------------------------------------------------
class TTHeaderList : public QVector<TTAVHeader*>
{
 public:
  virtual ~TTHeaderList();

  virtual void add( TTAVHeader* header );
  virtual void deleteAll();

 protected:
  TTHeaderList(int size);
  virtual void sort() = 0;
  virtual void checkIndexRange(int index);

 protected:
  TTMessageLogger* log;
  int              initial_size;
};

#endif //TTHEADERLIST
