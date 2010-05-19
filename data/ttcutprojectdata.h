/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutprojectdata.h                                              */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 11/13/2008 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTPROJECTDATA
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

#ifndef TTCUTPROJECTDATA_H
#define TTCUTPROJECTDATA_H

class TTAVItem;
class TTAVData;
class TTVideoStream;

#include <QFileInfo>
#include <QDomElement>
#include <QTextStream>
#include <QString>
#include <QStringList>

/* /////////////////////////////////////////////////////////////////////////////
 * TTCutProjectData
 */
class TTCutProjectData
{
  public:
    TTCutProjectData(const QFileInfo& fInfo);
    ~TTCutProjectData();

    void serializeAVDataItem(TTAVItem* vitem);
    void deserializeAVDataItem(TTAVData* avData);

    QString fileName();
    QString filePath();

    void clear();
    void writeXml();
    void readXml();
    void printXml();

  private:
    void createDocumentStructure();
    QDomElement writeVideoSection(const QString& filePath, int order);
    QDomElement writeAudioSection(QDomElement& parent, const QString& filePath, int order);
    QDomElement writeCutSection(QDomElement& parent, int cutIn, int cutOut, int order);
    QDomElement writeMarkerSection(QDomElement& parent, int markerPos, int markerType, int order);
    void        parseVideoSection(QDomNodeList videoNodesList, TTAVData* avData);
    void        parseAudioSection(QDomNodeList audioNodesList, TTAVData* avData, TTAVItem* avItem);
    void        parseCutSection(QDomNodeList cutNodesList, TTAVItem* avItem);
    void        parseMarkerSection(QDomNodeList markerNodeList, TTAVItem* avItem);

  private:
    QFileInfo*    xmlFileInfo;
    QDomDocument* xmlDocument;
    QDomElement*  xmlRoot;
    QDomNodeList* xmlNodeList;
};

#endif //TTCUTPROJECTDATA_H
