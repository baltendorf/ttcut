/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttheaderlist.cpp                                                */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 05/12/2005 */
/* MODIFIED: b. altendorf                                    DATE: 06/20/2007 */
/* MODIFIED:                                                 DATE:            */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTHEADERLIST
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

#include "../common/ttexception.h"

#include <QString>

const char c_name[] = "TTHEADERLIST: ";

/* /////////////////////////////////////////////////////////////////////////////
 * Create header list instance
 */
TTHeaderList::TTHeaderList(int size)
{
  log = TTMessageLogger::getInstance();
  initial_size = size;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Destructor: delete all header objects in list
 */
TTHeaderList::~TTHeaderList()
{
}

/* /////////////////////////////////////////////////////////////////////////////
 * add an header to the header list
 */
void TTHeaderList::add(TTAVHeader* header)
{
  if (header == NULL)
    qDebug("try to insert NULL header in list!");

  append(header);
}

/* /////////////////////////////////////////////////////////////////////////////
 * Remove all items from the header list
 * The objects in the list were also deleted!
 */
void TTHeaderList::deleteAll()
{
  for (int i = 0; i < size(); i++)
  {
    TTAVHeader* av_header = at(i);
    if (av_header != NULL) {
      //remove(i);
      delete av_header;
    }
  }
  clear();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Check if given index is in list range
 */
void TTHeaderList::checkIndexRange(int index)
{
  if (index < 0 || index >= size())
  {
    QString msg = QString("Index %1 exceeds array bounds: %2").arg(index).arg(count());
    throw TTIndexOutOfRangeException(msg);
  }
}

