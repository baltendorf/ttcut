/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutprojectdata.cpp                                            */
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

#include "ttcutprojectdata.h"
#include "ttavdata.h"
#include "ttavdata.h"
#include "avstream/ttavstream.h"
#include "common/ttexception.h"



/* /////////////////////////////////////////////////////////////////////////////
 * Constructor
 */
TTCutProjectData::TTCutProjectData(const QFileInfo& fInfo)
{
  xmlFileInfo = new QFileInfo(fInfo);
  xmlNodeList = NULL;
  xmlDocument = NULL;
  xmlRoot     = NULL;

  createDocumentStructure();
}

/* /////////////////////////////////////////////////////////////////////////////
 * Destructor
 */
TTCutProjectData::~TTCutProjectData()
{
  if (xmlFileInfo != 0)  delete xmlFileInfo;
  if (xmlNodeList != 0)  delete xmlNodeList;
  if (xmlDocument != 0)  delete xmlDocument;
  if (xmlRoot     != 0)  delete xmlRoot;
}

/**
 * Returns the xml filename
 */
QString TTCutProjectData::fileName()
{
  return xmlFileInfo->fileName();
}

/**
 * Returns the xml filepath
 */
QString TTCutProjectData::filePath()
{
  return xmlFileInfo->absoluteFilePath();
}
/* /////////////////////////////////////////////////////////////////////////////
 * Serialize an AVDataItem to xml
 */
void TTCutProjectData::serializeAVDataItem(TTAVItem* vItem)
{
  QDomElement video = writeVideoSection(vItem->videoStream()->filePath(), 0);

  for (int i = 0; i < vItem->audioCount(); i++) {
    TTAudioItem aItem   = vItem->audioListItemAt(i);
    TTAudioStream*      aStream = aItem.getAudioStream();
    writeAudioSection(video, aStream->filePath(), aItem.order());
  }

  for (int i = 0; i < vItem->cutCount(); i++) {
    TTCutItem cItem = vItem->cutListItemAt(i);
    writeCutSection(video, cItem.cutInIndex(), cItem.cutOutIndex(), cItem.order());
  }

  for (int i = 0; i < vItem->markerCount(); i++) {
  	TTMarkerItem mItem = vItem->markerAt(i);
  	writeMarkerSection(video, mItem.markerPos(), 1, mItem.order());
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Deserialize an AVDataItem from xml
 */
void TTCutProjectData::deserializeAVDataItem(TTAVData* avData)
{
  for (int i = 1; i < xmlNodeList->size(); i++) {
    parseVideoSection(xmlNodeList->at(i).childNodes(), avData);
  }
}

/* /////////////////////////////////////////////////////////////////////////////
 * Create document structure
 */
void TTCutProjectData::createDocumentStructure()
{
  xmlDocument = new QDomDocument("TTCut-Projectfile");
  xmlRoot     = new QDomElement(xmlDocument->createElement("TTCut-Projectfile"));

  xmlDocument->appendChild((*xmlRoot));

  QDomElement version = xmlDocument->createElement("Version");
  xmlRoot->appendChild(version);

  version.appendChild(xmlDocument->createTextNode("1.0"));
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTCutProjectData::parseVideoSection(QDomNodeList videoNodesList, TTAVData* avData)
{
  int     order = videoNodesList.at(0).toElement().text().toInt();
  QString name  = videoNodesList.at(1).toElement().text();

  qDebug("TTCutProjectData::parseVideoSection -> doOpenVideoStream...");
  TTAVItem* avItem = avData->doOpenVideoStream(name, order);

  qDebug("after doOpenVideoStream");
  //create the data item;
  for (int i = 2; i < videoNodesList.size(); i++) {

    if (videoNodesList.at(i).nodeName() == "Audio") {
      parseAudioSection(videoNodesList.at(i).childNodes(), avData, avItem);
    }
    else if (videoNodesList.at(i).nodeName() == "Cut") {
      parseCutSection(videoNodesList.at(i).childNodes(), avItem);
    }
    else if (videoNodesList.at(i).nodeName() == "Marker") {
    	parseMarkerSection(videoNodesList.at(i).childNodes(), avItem);
    }
    else {
      qDebug("unkown node!");
    }
  }

  avData->sortCutItemsByOrder();
  avData->sortMarkerByOrder();
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTCutProjectData::parseAudioSection(QDomNodeList audioNodesList, TTAVData* avData, TTAVItem* avItem)
{
  int     order = audioNodesList.at(0).toElement().text().toInt();
  QString name  = audioNodesList.at(1).toElement().text();

  QFileInfo fInfo(name);
  qDebug("TTCutProjectData::parseAudioSection -> before doOpenAudioStream...");
  avData->doOpenAudioStream(avItem, name, order);
  qDebug("after doOpenAudioStream...");
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTCutProjectData::parseCutSection(QDomNodeList cutNodesList, TTAVItem* avItem)
{
  int order       = cutNodesList.at(0).toElement().text().toInt();
  int cutIn       = cutNodesList.at(1).toElement().text().toInt();
  int cutOut      = cutNodesList.at(2).toElement().text().toInt();

  avItem->appendCutEntry(cutIn, cutOut, order);
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTCutProjectData::parseMarkerSection(QDomNodeList markerNodesList, TTAVItem* avItem)
{
  int order = markerNodesList.at(0).toElement().text().toInt();
  int pos   = markerNodesList.at(1).toElement().text().toInt();
  //int type  = markerNodesList.at(2).toElement().text().toInt();

  avItem->appendMarker(pos, order);
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
QDomElement TTCutProjectData::writeVideoSection(const QString& filePath, int order)
{
  QDomElement video = xmlDocument->createElement("Video");
  xmlRoot->appendChild(video);

  QDomElement xmlOrder = xmlDocument->createElement("Order");
  video.appendChild(xmlOrder);
  xmlOrder.appendChild(xmlDocument->createTextNode(QString("%1").arg(order)));

  QDomElement name = xmlDocument->createElement("Name");
  video.appendChild(name);
  name.appendChild(xmlDocument->createTextNode(filePath));

 return video;
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
QDomElement TTCutProjectData::writeAudioSection(QDomElement& parent, const QString& filePath, int order)
{
  QDomElement audio = xmlDocument->createElement("Audio");
  parent.appendChild(audio);

  QDomElement xmlOrder = xmlDocument->createElement("Order");
  audio.appendChild(xmlOrder);
  xmlOrder.appendChild(xmlDocument->createTextNode(QString("%1").arg(order)));

  QDomElement name = xmlDocument->createElement("Name");
  audio.appendChild(name);
  name.appendChild(xmlDocument->createTextNode(filePath));

  return audio;
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
QDomElement TTCutProjectData::writeCutSection(QDomElement& parent, int cutIn, int cutOut, int order)
{
  QDomElement cut = xmlDocument->createElement("Cut");
  parent.appendChild(cut);

  QDomElement xmlOrder = xmlDocument->createElement("Order");
  cut.appendChild(xmlOrder);
  xmlOrder.appendChild(xmlDocument->createTextNode(QString("%1").arg(order)));

  QDomElement xmlCutIn = xmlDocument->createElement("CutIn");
  cut.appendChild(xmlCutIn);
  xmlCutIn.appendChild(xmlDocument->createTextNode(QString("%1").arg(cutIn)));

  QDomElement xmlCutOut = xmlDocument->createElement("CutOut");
  cut.appendChild(xmlCutOut);
  xmlCutOut.appendChild(xmlDocument->createTextNode(QString("%1").arg(cutOut)));

  return cut;
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
QDomElement TTCutProjectData::writeMarkerSection(QDomElement& parent, int markerPos, int markerType, int order)
{
  QDomElement marker = xmlDocument->createElement("Marker");
  parent.appendChild(marker);

  QDomElement xmlOrder = xmlDocument->createElement("Order");
  marker.appendChild(xmlOrder);
  xmlOrder.appendChild(xmlDocument->createTextNode(QString("%1").arg(order)));

  QDomElement xmlMarkerPos = xmlDocument->createElement("MarkerPos");
  marker.appendChild(xmlMarkerPos);
  xmlMarkerPos.appendChild(xmlDocument->createTextNode(QString("%1").arg(markerPos)));

  QDomElement xmlMarkerType = xmlDocument->createElement("MarkerType");
  marker.appendChild(xmlMarkerType);
  xmlMarkerType.appendChild(xmlDocument->createTextNode(QString("%1").arg(markerType)));

  return marker;
}

/* /////////////////////////////////////////////////////////////////////////////
 * Clear the xml structure
 */
void TTCutProjectData::clear()
{

}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTCutProjectData::printXml()
{
  qDebug("xml: %s", qPrintable(xmlDocument->toString()));
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTCutProjectData::writeXml()
{
  QFile xmlFile(xmlFileInfo->absoluteFilePath());

  xmlFile.resize(0);
  xmlFile.open(QIODevice::WriteOnly);
  xmlFile.write(xmlDocument->toByteArray());

  xmlFile.flush();
  xmlFile.close();
}

/* /////////////////////////////////////////////////////////////////////////////
 *
 */
void TTCutProjectData::readXml()
{
  QFile xmlFile(xmlFileInfo->absoluteFilePath());

  if (!xmlFile.open(QIODevice::ReadOnly)) {
    throw new TTDataFormatException(QString("Error opening project file %1!").arg(xmlFileInfo->filePath()));
  }

  if (!xmlDocument->setContent(&xmlFile)) {
    throw new TTDataFormatException(QString("Error parsing xml project file %1!").arg(xmlFileInfo->filePath()));
  }

  xmlFile.close();

  if (xmlRoot != NULL)
    delete xmlRoot;

  if (xmlNodeList != NULL)
    delete xmlNodeList;

  xmlRoot     = new QDomElement(xmlDocument->documentElement());
  xmlNodeList = new QDomNodeList(xmlRoot->childNodes());

  //check file version
  if (!xmlNodeList->at(0).isElement()) {
    qDebug("wrong project file format!");
    return;
  }

  QDomElement version = xmlNodeList->at(0).toElement();

  float ver = version.text().toFloat();

  if (ver != 1.0) {
    qDebug("wrong project file version!");
    return;
  }
}


