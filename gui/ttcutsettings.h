/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2008 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttcutsettings.h                                                 */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/01/2005 */
/* MODIFIED: b. altendorf                                    DATE: 03/05/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTSETTINGS
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


#ifndef TTCUTSETTINGS_H
#define TTCUTSETTINGS_H

#include <QDir>
#include <QString>
#include <QSettings>

#include "../common/ttcut.h"

// -----------------------------------------------------------------------------
// TTCut settings object; Save and restore settings from configuration file
// -----------------------------------------------------------------------------
class TTCutSettings : public QSettings
{
 public:
   TTCutSettings();
   ~TTCutSettings();

   void readSettings();
   void writeSettings();

   private:
   QString searchPath;
   QDir    userHomeDir;
};

#endif //TTCUTSETTINGS_H

