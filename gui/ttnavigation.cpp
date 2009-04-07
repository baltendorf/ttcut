/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2005                                                      */
/* FILE     : ttnavigation.cpp                                                */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTNAVIGATION
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

#include "ttnavigation.h"
#include "../data/ttvideodatalist.h"

#include <QKeyEvent>

const char oName[] = "TTNavigation";

TTNavigation::TTNavigation(QWidget* parent) :
	QWidget(parent)
{
	setupUi(this);

	// message logger instance
	log = TTMessageLogger::getInstance();

	isControlEnabled = true;
	isEditCut = false;
	isCutInPosition = false;
	isCutOutPosition = false;

	cutInPosition = -1;
	cutOutPosition = -1;
	markerPosition = -1;

	connect(pbNextIFrame, SIGNAL(clicked()), SLOT(onNextIFrame()));
	connect(pbPrevIFrame, SIGNAL(clicked()), SLOT(onPrevIFrame()));
	connect(pbNextPFrame, SIGNAL(clicked()), SLOT(onNextPFrame()));
	connect(pbPrevPFrame, SIGNAL(clicked()), SLOT(onPrevPFrame()));
	connect(pbNextBFrame, SIGNAL(clicked()), SLOT(onNextBFrame()));
	connect(pbPrevBFrame, SIGNAL(clicked()), SLOT(onPrevBFrame()));
	connect(pbSetCutIn, SIGNAL(clicked()), SLOT(onSetCutIn()));
	connect(pbSetCutOut, SIGNAL(clicked()), SLOT(onSetCutOut()));
	connect(pbGotoCutIn, SIGNAL(clicked()), SLOT(onGotoCutIn()));
	connect(pbGotoCutOut, SIGNAL(clicked()), SLOT(onGotoCutOut()));
	connect(pbAddCut, SIGNAL(clicked()), SLOT(onAddCutRange()));
	connect(pbQuickJump, SIGNAL(clicked()), SLOT(onQuickJump()));
	connect(pbStreamPoints, SIGNAL(clicked()), SLOT(onStreamPoints()));
	connect(pbSetMarker, SIGNAL(clicked()), SLOT(onSetMarker()));
	connect(pbGotoMarker, SIGNAL(clicked()), SLOT(onGotoMarker()));
}

void TTNavigation::setTitle(const QString & title)
{
	gbNavigation->setTitle(title);
}

void TTNavigation::controlEnabled(bool enabled)
{
	isControlEnabled = enabled;
	pbNextIFrame->setEnabled(enabled);
	pbPrevIFrame->setEnabled(enabled);
	pbNextPFrame->setEnabled(enabled);
	pbPrevPFrame->setEnabled(enabled);
	pbNextBFrame->setEnabled(enabled);
	pbPrevBFrame->setEnabled(enabled);
	pbSetCutIn->setEnabled(enabled);
	pbSetCutOut->setEnabled(enabled);
	pbGotoCutIn->setEnabled(enabled);
	pbGotoCutOut->setEnabled(enabled);
	pbAddCut->setEnabled(enabled);
	pbSetMarker->setEnabled(enabled);
	pbGotoMarker->setEnabled(false);
}

void TTNavigation::checkCutPosition(TTVideoDataItem* avData)
{
	TTVideoStream* vs = avData->videoStream();
	currentPosition = vs->currentIndex();
	currentTime = vs->currentFrameTime().toString("hh:mm:ss.zzz");
	currentFrameType = vs->currentFrameType();

	pbSetCutIn->setEnabled(vs->isCutInPoint(-1));
	pbSetCutOut->setEnabled(vs->isCutOutPoint(-1));
}

void TTNavigation::keyPressEvent(QKeyEvent* e)
{
	int steps = 0;

	if (!isControlEnabled)
		return;

	//log->infoMsg(oName, "key press event");

	switch (e->key()) {

	// left arrow key
	case Qt::Key_Left:

		switch (e->modifiers()) {

		// backward TTCut::stepPlusAlt
		case Qt::AltModifier:
			steps -= TTCut::stepPlusAlt;
			break;
			// backward TTCut::stepPlusCtrl
		case Qt::ControlModifier:
			steps -= TTCut::stepPlusCtrl;
			break;
			// backward TTCut::stepPlusShift
		case Qt::ShiftModifier:
			steps -= TTCut::stepPlusShift;
			break;
			// backward one frame
		default:
			steps -= 1;
			break;
		}

		emit moveNumSteps(steps);
		break;

		// right arrow key
	case Qt::Key_Right:

		switch (e->modifiers()) {

		// forward TTCut::stepPlusAlt
		case Qt::AltModifier:
			steps += TTCut::stepPlusAlt;
			break;
			// forward TTCut::stepPlusCtrl
		case Qt::ControlModifier:
			steps += TTCut::stepPlusCtrl;
			break;
			// forward TTCut::stepPlusShift
		case Qt::ShiftModifier:
			steps += TTCut::stepPlusShift;
			break;
			// forward one frame
		default:
			steps += 1;
			break;
		}

		emit moveNumSteps(steps);
		break;
		// home key: show first frame
	case Qt::Key_Home:
		emit moveToHome();
		break;
		// end key: show last frame
	case Qt::Key_End:
		emit moveToEnd();
		break;
		// page down
	case Qt::Key_PageUp:
		steps -= TTCut::stepPgUpDown;
		emit moveNumSteps(steps);
		break;
		// page up
	case Qt::Key_PageDown:
		steps += TTCut::stepPgUpDown;
		emit moveNumSteps(steps);
		break;
		// I-frame
	case Qt::Key_I:
		// previous I-Frame
		if (e->modifiers() == Qt::ControlModifier)
			emit prevIFrame();
		// next I-frame
		else
			emit nextIFrame();
		break;
		// ---------------------------------------------------------------------------
		// P-frame
		// ---------------------------------------------------------------------------
	case Qt::Key_P:
		// previous P-Frame
		if (e->modifiers() == Qt::ControlModifier)
			emit prevPFrame();
		// next P-frame
		else
			emit nextPFrame();
		break;
		// ---------------------------------------------------------------------------
		// B-frame
		// ---------------------------------------------------------------------------
	case Qt::Key_B:
		// previous B-Frame
		if (e->modifiers() == Qt::ControlModifier)
			emit prevBFrame();
		// next B-frame
		else
			emit nextBFrame();
		break;

	default:
		break;
	}
}

void TTNavigation::onPrevIFrame()
{
	emit prevIFrame();
}

void TTNavigation::onNextIFrame()
{
	emit nextIFrame();
}

void TTNavigation::onPrevPFrame()
{
	emit prevPFrame();
}

void TTNavigation::onNextPFrame()
{
	emit nextPFrame();
}

void TTNavigation::onPrevBFrame()
{
	emit prevBFrame();
}

void TTNavigation::onNextBFrame()
{
	emit nextBFrame();
}

void TTNavigation::onSetCutIn()
{
	QString szTemp1, szTemp2;

	isCutInPosition = true;
	cutInPosition = currentPosition;

	szTemp1 = currentTime;
	szTemp2.sprintf(" (%d)", cutInPosition);

	if (currentFrameType == 1)
		szTemp2 += " [I]";
	if (currentFrameType == 2)
		szTemp2 += " [P]";
	if (currentFrameType == 3)
		szTemp2 += " [B]";

	szTemp1 += szTemp2;
	laCutInPosition->setText(szTemp1);

	emit setCutIn(cutInPosition);
}

void TTNavigation::onSetCutOut()
{
	QString szTemp1, szTemp2;

	isCutOutPosition = true;
	cutOutPosition = currentPosition;

	szTemp1 = currentTime;
	szTemp2.sprintf(" (%d)", cutOutPosition);

	if (currentFrameType == 1)
		szTemp2 += " [I]";
	if (currentFrameType == 2)
		szTemp2 += " [P]";
	if (currentFrameType == 3)
		szTemp2 += " [B]";

	szTemp1 += szTemp2;
	laCutOutPosition->setText(szTemp1);

	emit setCutOut(cutOutPosition);
}

void TTNavigation::onGotoCutIn()
{
	if (isCutInPosition)
		emit gotoCutIn(cutInPosition);
}

void TTNavigation::onGotoCutOut()
{
	if (isCutOutPosition)
		emit gotoCutOut(cutOutPosition);
}

void TTNavigation::onAddCutRange()
{
	if (isCutInPosition && isCutOutPosition) {
		isCutInPosition = false;
		isCutOutPosition = false;
		laCutInPosition->setText("...");
		laCutOutPosition->setText("...");

		if (isEditCut) {
			editCutData->avDataItem()->updateCutEntry(*editCutData,
					cutInPosition, cutOutPosition);
			pbAddCut->setText(tr("Add range to cut list"));
			isEditCut = false;
			delete editCutData;
			return;
		}

		emit addCutRange(cutInPosition, cutOutPosition);
	}
}

void TTNavigation::onSetMarker()
{
	QString szTemp1, szTemp2;

	markerPosition = currentPosition;
	pbGotoMarker->setEnabled(true);

	szTemp1 = currentTime;
	szTemp2.sprintf(" (%d)", markerPosition);

	if (currentFrameType == 1)
		szTemp2 += " [I]";
	if (currentFrameType == 2)
		szTemp2 += " [P]";
	if (currentFrameType == 3)
		szTemp2 += " [B]";

	szTemp1 += szTemp2;
	laMarkerPosition->setText(szTemp1);
}

void TTNavigation::onGotoMarker()
{
	if (markerPosition >= 0)
		emit gotoMarker(markerPosition);
}

void TTNavigation::onStreamPoints()
{
}

void TTNavigation::onQuickJump()
{
}

void TTNavigation::onEditCut(const TTCutItem& cutData)
{
	QString szTemp1, szTemp2;

	isCutInPosition = true;
	isCutOutPosition = true;
	cutInPosition = cutData.cutInIndex();
	cutOutPosition = cutData.cutOutIndex();

	szTemp1 = cutData.cutInTime().toString("hh:mm:ss.zzz");
	szTemp2.sprintf(" (%d)", cutInPosition);

	if (cutData.cutInFrameType() == 1)
		szTemp2 += " [I]";
	if (cutData.cutInFrameType() == 2)
		szTemp2 += " [P]";
	if (cutData.cutInFrameType() == 3)
		szTemp2 += " [B]";

	szTemp1 += szTemp2;
	laCutInPosition->setText(szTemp1);

	szTemp1 = cutData.cutOutTime().toString("hh:mm:ss.zzz");
	szTemp2.sprintf(" (%d)", cutOutPosition);

	if (cutData.cutOutFrameType() == 1)
		szTemp2 += " [I]";
	if (cutData.cutOutFrameType() == 2)
		szTemp2 += " [P]";
	if (cutData.cutOutFrameType() == 3)
		szTemp2 += " [B]";

	szTemp1 += szTemp2;
	laCutOutPosition->setText(szTemp1);

	isEditCut = true;
	editCutData = new TTCutItem(cutData);

	pbAddCut->setText(tr("Update range in cut list"));

	emit gotoCutIn(cutInPosition);
}
