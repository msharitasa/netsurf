/*
 * Copyright 2009 Mark Benjamin <netsurf-browser.org.MarkBenjamin@dfgh.net>
 *
 * This file is part of NetSurf, http://www.netsurf-browser.org/
 *
 * NetSurf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * NetSurf is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETSURF_WINDOWS_RESOURCEID_H
#define NETSURF_WINDOWS_RESOURCEID_H

#ifndef IDC_STATIC
#define IDC_STATIC (-1)
#endif

#define IDR_NETSURF_ICON 100
#define IDR_THROBBER_AVI 101
#define IDR_TOOLBAR_BITMAP 102
#define IDR_TOOLBAR_BITMAP_GREY 103
#define IDR_TOOLBAR_BITMAP_HOT 104
#define IDR_NETSURF_BANNER 105
#define IDR_HOME_BITMAP 106
#define IDC_PAGEINFO 107
#define IDB_PAGEINFO_INSECURE 108
#define IDB_PAGEINFO_SECURE 109
#define IDB_PAGEINFO_INTERNAL 110
#define IDB_PAGEINFO_WARNING 111
#define IDB_PAGEINFO_LOCAL 112

#define IDD_ABOUT 1000
#define IDC_IMG1 1001
#define IDC_ABOUT_VERSION 1002
#define IDC_ABOUT_TEXT 1003
#define IDC_ABOUT_COPYRIGHT 1004
#define IDC_BTN_CREDITS 1005
#define IDC_BTN_LICENCE 1006

#define IDD_DOWNLOAD 1100
#define IDC_DOWNLOAD_LABEL 1101
#define IDC_DOWNLOAD_PROGRESS 1102

#define IDD_MAIN 1300
#define IDC_MAIN_TOOLBAR 1301
#define IDC_MAIN_URLBAR 1302
#define IDC_MAIN_THROBBER 1303
#define IDC_MAIN_DRAWINGAREA 1304
#define IDC_MAIN_STATUSBAR 1305
#define IDC_MAIN_LAUNCH_URL 1306

#define IDD_OPTIONS_GENERAL 1400
#define IDC_PREFS_HOMEPAGE 1401
#define IDC_PREFS_IMAGES 1402
#define IDC_PREFS_ADVERTS 1403
#define IDC_PREFS_REFERER 1404

#define IDD_OPTIONS_CONNECTIONS 1500
#define IDC_PREFS_FETCHERS 1501
#define IDC_PREFS_FETCHERS_SPIN 1502
#define IDC_PREFS_FETCH_HOST 1503
#define IDC_PREFS_FETCH_HOST_SPIN 1504
#define IDC_PREFS_FETCH_HANDLES 1505
#define IDC_PREFS_FETCH_HANDLES_SPIN 1506

#define IDD_OPTIONS_APPERANCE 1200
#define IDC_PREFS_PROXYTYPE 1206
#define IDC_PREFS_PROXYHOST 1207
#define IDC_PREFS_PROXYPORT 1208
#define IDC_PREFS_PROXYNAME 1209
#define IDC_PREFS_PROXYPASS 1210
#define IDC_PREFS_FONT_SIZE 1211
#define IDC_PREFS_FONT_MINSIZE 1212
#define IDC_PREFS_FONT_MINSIZE_SPIN 1213
#define IDC_PREFS_SANS 1214
#define IDC_PREFS_SERIF 1215
#define IDC_PREFS_FONT_SIZE_SPIN 1216
#define IDC_PREFS_MONO 1217
#define IDC_PREFS_CURSIVE 1218
#define IDC_PREFS_FANTASY 1219
#define IDC_PREFS_FONTDEF 1220
#define IDC_PREFS_NOANIMATION 1227
#define IDC_PREFS_ANIMATIONDELAY 1228
#define IDC_PREFS_ANIMATIONDELAY_SPIN 1229

#define IDD_SSLCERT 1600
#define IDC_SSLCERT_IMG1 1601
#define IDC_SSLCERT_BTN_ACCEPT 1602
#define IDC_SSLCERT_BTN_REJECT 1603

#define IDD_LOGIN 1700
#define IDC_LOGIN_USERNAME 1701
#define IDC_LOGIN_PASSWORD 1702
#define IDC_LOGIN_DESCRIPTION 1703

#define IDR_MENU_MAIN 10000
#define IDM_FILE_OPEN_WINDOW 10101
#define IDM_FILE_OPEN_LOCATION 10102
#define IDM_FILE_CLOSE_WINDOW 10103
#define IDM_FILE_SAVE_PAGE 10104
#define IDM_FILE_SAVEAS_TEXT 10105
#define IDM_FILE_SAVEAS_PDF 10106
#define IDM_FILE_SAVEAS_POSTSCRIPT 10107
#define IDM_FILE_PRINT_PREVIEW 10108
#define IDM_FILE_PRINT 10109
#define IDM_FILE_QUIT 10110
#define IDM_EDIT_CUT 10211
#define IDM_EDIT_COPY 10212
#define IDM_EDIT_PASTE 10213
#define IDM_EDIT_DELETE 10214
#define IDM_EDIT_SELECT_ALL 10215
#define IDM_EDIT_SEARCH 10216
#define IDM_NAV_STOP 10301
#define IDM_NAV_RELOAD 10302
#define IDM_VIEW_ZOOMPLUS 10303
#define IDM_VIEW_ZOOMMINUS 10304
#define IDM_VIEW_ZOOMNORMAL 10305
#define IDM_VIEW_SOURCE 10306
#define IDM_VIEW_FULLSCREEN 10307
#define IDM_VIEW_SAVE_WIN_METRICS 10308
#define IDM_NAV_BACK 10309
#define IDM_NAV_FORWARD 10310
#define IDM_NAV_HOME 10311
#define IDM_NAV_LOCALHISTORY 10312
#define IDM_NAV_GLOBALHISTORY 10313
#define IDM_NAV_BOOKMARKS 10314
#define IDM_TOOLS_DOWNLOADS 10430
#define IDM_TOOLS_COOKIES 10431
#define IDM_VIEW_TOGGLE_DEBUG_RENDERING 10432
#define IDM_VIEW_DEBUGGING_SAVE_BOXTREE 10433
#define IDM_VIEW_DEBUGGING_SAVE_DOMTREE 10434
#define IDM_EDIT_PREFERENCES 10435
#define IDM_HELP_CONTENTS 10536
#define IDM_HELP_GUIDE 10537
#define IDM_HELP_INFO 10538
#define IDM_HELP_ABOUT 10539

#define IDR_MENU_CONTEXT 11000

#endif
