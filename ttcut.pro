#/*.............................................................................*/
#/*  (c) 2003/2008 TriTime  (ttcut.tritime.org)                                 */
#/*-----------------------------------------------------------------------------*/
#/*  FILE:   QMake Projektfile for ttcut                                        */
#/*-----------------------------------------------------------------------------*/
#/*  ABOUT:                                                                     */
#/*-----------------------------------------------------------------------------*/
#/*  AUTHOR:    b. altendorf (ttcut@tritime.org)              DATE: 02/18/2006  */
#/*-----------------------------------------------------------------------------*/
#
#
PROJECT     = TTCut
CONFIG      += qt warn_on
QT          += core gui xml multimedia multimediawidgets widgets
DEFINES     += _FILE_OFFSET_BITS=64
DIST        = ttcut
TARGET      = ttcut
ICON        = ui/pixmaps/ttcut_logo_001.icns
INCLUDEPATH += /opt/local/include
LIBS        += -L /opt/local/lib -lmpeg2 -lmpeg2convert

unix {
  DEFINES += UNIX
  DEFINES -= MACX
}

macx {
  DEFINES += MACX
  DEFINES -= UNIX
}

OBJECTS_DIR = obj
MOC_DIR     = moc
UI_DIR      = ui_h 
RCC_DIR     = res
TRANSLATIONS = trans/ttcut_de.ts\
               trans/ttcut_en.ts
RESOURCES   = ui/ttcutvideoinfowidget.qrc\
              ui/audiofilelistwidget.qrc\
              ui/videofilelistwidget.qrc\
              ui/cutoutframewidget.qrc\
              ui/currentframewidget.qrc\
              ui/ttcutframenavigationwidget.qrc \
              ui/cutlistwidget.qrc\
              ui/ttcutsettingscommon.qrc\
              ui/ttcutsettingsmuxer.qrc\
              ui/ttsettingsdialog.qrc\
              ui/previewwidget.qrc\
              ui/avcutdialog.qrc\
              ui/processviewwidget.qrc\
              ui/mainwindow.qrc
FORMS       = ui/audiofilelistwidget.ui\
              ui/videofilelistwidget.ui\
              ui/markerlistwidget.ui\
              ui/cutoutframewidget.ui\
              ui/currentframewidget.ui\
              ui/navigatordisplaywidget.ui\
              ui/streamnavigationwidget.ui\
              ui/cutlistwidget.ui\
              ui/ttcutsettingschapter.ui\
              ui/ttcutsettingsmuxer.ui\
              ui/ttcutsettingsencoder.ui\
              ui/ttcutsettingsfiles.ui\
              ui/ttcutsettingscommon.ui\
              ui/ttsettingsdialog.ui\
              ui/mplexparamdlg.ui\
              ui/previewwidget.ui\
              ui/processviewwidget.ui\
              ui/avcutdialog.ui\
              ui/aboutdlg.ui\
#             ui/mainwindow.ui\
              ui/ttcutmainwnd_new.ui\
              ui/ttcutvideoinfowidget.ui\
              ui/ttcutframenavigationwidget.ui\
              ui/ttprogressform.ui\
              ui/tttaskprogresswidget.ui
HEADERS     = common/ttcut.h\
              common/ttthreadtask.h\
              common/ttthreadtaskpool.h\
              common/ttmessagelogger.h\
              common/ttmessagebox.h\
              common/ttexception.h\
              common/istatusreporter.h\
              data/ttaudiolist.h\
              data/ttcutlist.h\
              data/ttmarkerlist.h\
              data/ttcutparameter.h\
              data/ttmuxlistdata.h\
              data/ttavdata.h\
              data/ttavlist.h\
              data/ttcutprojectdata.h\
              data/ttopenvideotask.h\
              data/ttopenaudiotask.h\
              data/ttcutvideotask.h\
              data/ttcutaudiotask.h\
              data/ttcutpreviewtask.h\
              data/ttframesearchtask.h\
#unused              data/ttthreadstatusargs.h\
              avstream/ttcommon.h\
              avstream/ttac3audioheader.h\
              avstream/ttac3audiostream.h\
              avstream/ttaudioheaderlist.h\
              avstream/ttavheader.h\
              avstream/ttavstream.h\
              avstream/ttavtypes.h\
              avstream/ttfilebuffer.h\ 
              avstream/ttheaderlist.h\ 
              avstream/ttmpeg2videoheader.h\
              avstream/ttmpeg2videostream.h\   
              avstream/ttmpegaudioheader.h\    
              avstream/ttmpegaudiostream.h\    
              avstream/ttvideoheaderlist.h\    
              avstream/ttvideoindexlist.h\
              avstream/ttaviwriter.h\
              mpeg2decoder/ttmpeg2decoder.h\
              mpeg2window/ttmpeg2window2.h\
              avilib/avilib.h\
              extern/imuxprovider.h\
              extern/tttranscode.h\
              extern/ttmplexprovider.h\
              gui/ttcutsettings.h\
              gui/ttcutsettingschapter.h\
              gui/ttcutsettingsmuxer.h\
              gui/ttcutsettingsencoder.h\
              gui/ttcutsettingsfiles.h\
              gui/ttcutsettingscommon.h\
              gui/ttcutsettingsdlg.h\
              gui/ttcutvideoinfo.h\
              gui/ttaudiotreeview.h\
              gui/ttvideotreeview.h\
              gui/ttmarkertreeview.h\
              gui/ttcutoutframe.h\
              gui/ttcurrentframe.h\
              gui/ttcutframenavigation.h\
              gui/ttnavigatordisplay.h \
              gui/ttstreamnavigator.h \
              gui/ttcuttreeview.h\
              gui/ttprogressbar.h\
              gui/ttprocessform.h\
              gui/tttaskprogress.h\
              gui/ttcutpreview.h\
              gui/ttvideoplayer.h\
              gui/ttmediaplayerwidget.h\
              gui/ttmplayerwidget.h\
              gui/ttcutaboutdlg.h\
              gui/ttcutavcutdlg.h\
              gui/ttcutmainwindow.h
SOURCES     = common/ttcut.cpp\
              common/ttthreadtask.cpp\
              common/ttthreadtaskpool.cpp\  
              common/ttmessagelogger.cpp\
              common/ttmessagebox.cpp\
              common/ttexception.cpp\
              common/istatusreporter.cpp\
              data/ttaudiolist.cpp \
              data/ttcutlist.cpp\
              data/ttmarkerlist.cpp\
              data/ttcutparameter.cpp\
              data/ttmuxlistdata.cpp\
              data/ttavdata.cpp\
              data/ttavlist.cpp\
              data/ttcutprojectdata.cpp\              
              data/ttopenvideotask.cpp\
              data/ttopenaudiotask.cpp\
              data/ttcutvideotask.cpp\
              data/ttcutaudiotask.cpp\
              data/ttcutpreviewtask.cpp\
              data/ttframesearchtask.cpp\
              avstream/ttcommon.cpp\
              avstream/ttac3audioheader.cpp\
              avstream/ttac3audiostream.cpp\
              avstream/ttaudioheaderlist.cpp\
              avstream/ttavheader.cpp\
              avstream/ttavstream.cpp\
              avstream/ttavtypes.cpp\
              avstream/ttfilebuffer.cpp\ 
              avstream/ttheaderlist.cpp\ 
              avstream/ttmpeg2videoheader.cpp\
              avstream/ttmpeg2videostream.cpp\   
              avstream/ttmpegaudioheader.cpp\    
              avstream/ttmpegaudiostream.cpp\    
              avstream/ttvideoheaderlist.cpp\    
              avstream/ttvideoindexlist.cpp\
              avstream/ttaviwriter.cpp\
              mpeg2decoder/ttmpeg2decoder.cpp\
              mpeg2window/ttmpeg2window2.cpp\
              avilib/avilib.c\
              extern/tttranscode.cpp\
              extern/ttmplexprovider.cpp\
              gui/ttcutsettings.cpp\
              gui/ttcutsettingschapter.cpp\
              gui/ttcutsettingsmuxer.cpp\
              gui/ttcutsettingsencoder.cpp\
              gui/ttcutsettingsfiles.cpp\
              gui/ttcutsettingscommon.cpp\
              gui/ttcutsettingsdlg.cpp\
              gui/ttcutvideoinfo.cpp\
              gui/ttaudiotreeview.cpp\
              gui/ttvideotreeview.cpp\
              gui/ttmarkertreeview.cpp\
              gui/ttcutoutframe.cpp\
              gui/ttcurrentframe.cpp\
              gui/ttcutframenavigation.cpp\
              gui/ttnavigatordisplay.cpp\
              gui/ttstreamnavigator.cpp\
              gui/ttcuttreeview.cpp\
              gui/ttprogressbar.cpp\
              gui/tttaskprogress.cpp\
              gui/ttprocessform.cpp\
              gui/ttcutpreview.cpp\
              gui/ttvideoplayer.cpp\
              gui/ttmediaplayerwidget.cpp\
              gui/ttmplayerwidget.cpp\
              gui/ttcutaboutdlg.cpp\
              gui/ttcutavcutdlg.cpp\
              gui/ttcutmainwindow.cpp\
              gui/ttcutmain.cpp
#
# -------------------------------------------------------------------------------
# TTCUT.PRO ENDE
# -------------------------------------------------------------------------------
