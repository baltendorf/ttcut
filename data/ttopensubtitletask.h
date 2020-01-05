/*----------------------------------------------------------------------------*/
/* COPYRIGHT: Minei3oat (c) 2019 / github.com/Minei3oat                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2019                                                      */
/* FILE     : ttopensubtitletask.h                                            */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : Minei3oat                                       DATE: 12/30/2019 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTOPENSUBTITLETASK
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

#include "common/ttthreadtask.h"

#include <QString>
#include <QFileInfo>

class TTSubtitleType;
class TTSubtitleStream;
class TTAVItem;

//! Runable task for opening subtitle streams
class TTOpenSubtitleTask : public TTThreadTask
{
  Q_OBJECT

  public:
    TTOpenSubtitleTask(TTAVItem* avItem, QString filePath, int order);

  protected:
    void cleanUp();
    void operation();

  public slots:
    void onUserAbort();

  signals:
    void finished(TTAVItem*, TTSubtitleStream*, int);

  private:
    TTAVItem*         mpAVItem;
    int               mOrder;
    QString           mFilePath;
    TTSubtitleStream* mpSubtitleStream;
    TTSubtitleType*   mpSubtitleType;
};
