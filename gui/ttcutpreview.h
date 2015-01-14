/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / www.tritime.org                         */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutpreview.h                                                  */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 03/13/2005 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTPREVIEW
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

#ifndef TTCUTPREVIEW_H
#define TTCUTPREVIEW_H

#include "ui_previewwidget.h"

#include <QThread>
#include <QProcess>
#include <QResizeEvent>
#include <QCloseEvent>

#include "common/ttcut.h"
#include "data/ttcutlist.h"

class TTAVData;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QComboBox;
class QPushButton;
class QFrame;
class TTVideoPlayer;

/* /////////////////////////////////////////////////////////////////////////////
 * Class TTCutPreview
 */
class TTCutPreview: public QDialog, Ui::TTPreviewWidget
{
Q_OBJECT

public:
	TTCutPreview(QWidget* parent = 0, int prevW = 640, int prevH = 480);
	~TTCutPreview();

	void resizeEvent(QResizeEvent* event);
	void initPreview(TTCutList* cutList);
	void createPreview();

protected:
	void closeEvent(QCloseEvent* event);
	void cleanUp();

protected slots:
  void onOptimalSizeChanged();
  void onPlayerPlaying();
	void onPlayerFinished();
	void onCutSelectionChanged(int iCut);
	void onPlayPreview();
	void onExitPreview();

private:
  TTVideoPlayer* videoPlayer;
	int            previewWidth;
	int            previewHeight;
	QString        current_video_file;
	QString        current_audio_file;
};

#endif // TTCUTPREVIEW_H
