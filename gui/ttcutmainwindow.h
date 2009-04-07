/*----------------------------------------------------------------------------*/
/* COPYRIGHT: TriTime (c) 2003/2010 / ttcut.tritime.org                       */
/*----------------------------------------------------------------------------*/
/* PROJEKT  : TTCUT 2008                                                      */
/* FILE     : ttcutmainwindow.h                                               */
/*----------------------------------------------------------------------------*/
/* AUTHOR  : b. altendorf (E-Mail: b.altendorf@tritime.de)   DATE: 02/26/2006 */
/*----------------------------------------------------------------------------*/

// ----------------------------------------------------------------------------
// TTCUTMAINWINDOW
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

#ifndef TTCUTMAINWINDOW_H
#define TTCUTMAINWINDOW_H

//#include "ui_mainwindow.h"
#include "ui_ttcutmainwnd_new.h"

#include <QMutexLocker>

#include "../common/ttcut.h"
#include "../common/ttmessagelogger.h"
#include "../data/ttaudiolist.h"
#include "../data/ttcutlist.h"

#include "../avstream/ttavtypes.h"
#include "../avstream/ttmpeg2videostream.h"

#include "ttcutpreview.h"
#include "ttcutsettings.h"
#include "ttcutsettingsdlg.h"

class TTAVData;
class TTAVItem;
class TTCutList;
class TTProgressBar;
class TTThreadTask;

//class TTCutMainWindow: public QMainWindow, Ui::TTCutMainWindow
class TTCutMainWindow: public QMainWindow, Ui::TTCutMainWindowForm
{
	Q_OBJECT

		public:
		TTCutMainWindow();
		~TTCutMainWindow();

		void keyPressEvent(QKeyEvent* e);

	public slots:
    void onOpenVideoFile();
    void onOpenAudioFile();
		void onFileNew();
		void onFileOpen();
		void onFileSave();
		void onFileSaveAs();
		void onFileRecent();
		void onFileExit();
		void closeEvent(QCloseEvent* event);
		void onActionSave();
		void onActionSettings();

		void onHelpAbout();

		void onReadVideoStream(QString fName);
		void onReadAudioStream(QString fName);

		void onVideoSliderChanged(int value);

		void onNewFramePos(int);

		void onAppendCutEntry(int cutIn, int cutOut);

		void onCutPreview(TTCutList* cutList);
		void onCutPreviewFinished(TTCutList* cutList);

		void onAudioVideoCut(bool cutAudioOnly, TTCutList* cutList);

		void onCutSelectionChanged(const TTCutItem&);
		void onSetSelectedCutOut(const TTCutItem&);
		void onSetCutOut(int index);

    void onGotoMarker(const TTMarkerItem& marker);

		void onNextAVData();
    void onPrevAVData();
		void onAVItemChanged(TTAVItem* avItem);

    void onOpenProjectFileFinished();

		void onStatusReport(TTThreadTask* task, int state, const QString& msg,	quint64 value);

		signals:

		private:
		void closeProject();
		void navigationEnabled(bool enabled);
		void openProjectFile(QString fName);
		void updateRecentFileActions();
		void insertRecentFile(const QString& fName);

	private:
		TTAVData*        mpAVData;
		TTAVItem*        mpCurrentAVDataItem;
		TTProgressBar*   progressBar;
		TTCutSettings*   settings;
		
    TTMessageLogger* log;
		
    bool             sliderUpdateFrame;

		// recent files menu
		enum
		{
			MaxRecentFiles = 5
		};
		QAction* recentFileAction[MaxRecentFiles];
};

#endif //TTCUTMAINWINDOW_H
