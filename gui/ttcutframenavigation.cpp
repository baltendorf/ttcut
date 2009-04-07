/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                     */
/* FILE     : ttnavigation.cpp                                                */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTFRAMENAVIGATION
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

#include "ttcutframenavigation.h"
#include "../common/ttcut.h"
#include "../data/ttavlist.h"

#include <QKeyEvent>

TTCutFrameNavigation::TTCutFrameNavigation(QWidget* parent) :
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
	//connect(pbNextBFrame, SIGNAL(clicked()), SLOT(onNextBFrame()));
	//connect(pbPrevBFrame, SIGNAL(clicked()), SLOT(onPrevBFrame()));
	connect(pbSetCutIn, SIGNAL(clicked()), SLOT(onSetCutIn()));
	connect(pbSetCutOut, SIGNAL(clicked()), SLOT(onSetCutOut()));
	connect(pbGotoCutIn, SIGNAL(clicked()), SLOT(onGotoCutIn()));
	connect(pbGotoCutOut, SIGNAL(clicked()), SLOT(onGotoCutOut()));
	connect(pbAddCut, SIGNAL(clicked()), SLOT(onAddCutRange()));
}

//void TTCutFrameNavigation::setTitle(const QString & title)
//{
//}

void TTCutFrameNavigation::controlEnabled(bool enabled)
{
	isControlEnabled = enabled;
	pbNextIFrame->setEnabled(enabled);
	pbPrevIFrame->setEnabled(enabled);
	pbNextPFrame->setEnabled(enabled);
	pbPrevPFrame->setEnabled(enabled);
	//pbNextBFrame->setEnabled(enabled);
	//pbPrevBFrame->setEnabled(enabled);
	pbSetCutIn->setEnabled(enabled);
	pbSetCutOut->setEnabled(enabled);
	pbGotoCutIn->setEnabled(enabled);
	pbGotoCutOut->setEnabled(enabled);
	pbAddCut->setEnabled(enabled);
}

void TTCutFrameNavigation::checkCutPosition(TTAVItem* avData)
{
	TTVideoStream* vs = avData->videoStream();
	currentPosition = vs->currentIndex();
	//currentTime = vs->currentFrameTime().toString("hh:mm:ss.zzz");
	currentTime = vs->currentFrameTime().toString("hh:mm:ss");
	currentFrameType = vs->currentFrameType();

	pbSetCutIn->setEnabled(vs->isCutInPoint(-1));
	pbSetCutOut->setEnabled(vs->isCutOutPoint(-1));
}

void TTCutFrameNavigation::keyPressEvent(QKeyEvent* e)
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
/*	case Qt::Key_B:
		// previous B-Frame
		if (e->modifiers() == Qt::ControlModifier)
			emit prevBFrame();
		// next B-frame
		else
			emit nextBFrame();
		break;*/

	default:
		break;
	}
}

void TTCutFrameNavigation::onPrevIFrame()
{
	emit prevIFrame();
}

void TTCutFrameNavigation::onNextIFrame()
{
	emit nextIFrame();
}

void TTCutFrameNavigation::onPrevPFrame()
{
	emit prevPFrame();
}

void TTCutFrameNavigation::onNextPFrame()
{
	emit nextPFrame();
}

void TTCutFrameNavigation::onPrevBFrame()
{
	emit prevBFrame();
}

void TTCutFrameNavigation::onNextBFrame()
{
	emit nextBFrame();
}

void TTCutFrameNavigation::onSetCutIn()
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

void TTCutFrameNavigation::onSetCutOut()
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

void TTCutFrameNavigation::onGotoCutIn()
{
	if (isCutInPosition)
		emit gotoCutIn(cutInPosition);
}

void TTCutFrameNavigation::onGotoCutOut()
{
	if (isCutOutPosition)
		emit gotoCutOut(cutOutPosition);
}

void TTCutFrameNavigation::onAddCutRange()
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

void TTCutFrameNavigation::onEditCut(const TTCutItem& cutData)
{
	QString szTemp1, szTemp2;

	isCutInPosition = true;
	isCutOutPosition = true;
	cutInPosition = cutData.cutInIndex();
	cutOutPosition = cutData.cutOutIndex();

	//szTemp1 = cutData.getCutInTime().toString("hh:mm:ss.zzz");
	szTemp1 = cutData.cutInTime().toString("hh:mm:ss");
		szTemp2.sprintf(" (%d)", cutInPosition);

	if (cutData.cutInFrameType() == 1)
		szTemp2 += " [I]";
	if (cutData.cutInFrameType() == 2)
		szTemp2 += " [P]";
	if (cutData.cutInFrameType() == 3)
		szTemp2 += " [B]";

	szTemp1 += szTemp2;
	laCutInPosition->setText(szTemp1);

	//szTemp1 = cutData.getCutOutTime().toString("hh:mm:ss.zzz");
	szTemp1 = cutData.cutOutTime().toString("hh:mm:ss");
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
