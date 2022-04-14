; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CAboutDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "MPEG4Play.h"
LastPage=0

ClassCount=9
Class1=CMPEG4PlayApp
Class2=CMPEG4PlayDoc
Class3=CMPEG4PlayView
Class4=CMainFrame
Class9=CAboutDlg

ResourceCount=2
Resource1=IDR_MAINFRAME
Resource2=IDD_ABOUTBOX

[CLS:CMPEG4PlayApp]
Type=0
HeaderFile=MPEG4Play.h
ImplementationFile=MPEG4Play.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CMPEG4PlayDoc]
Type=0
HeaderFile=MPEG4PlayDoc.h
ImplementationFile=MPEG4PlayDoc.cpp
Filter=N
BaseClass=CDocument
VirtualFilter=DC

[CLS:CMPEG4PlayView]
Type=0
HeaderFile=MPEG4PlayView.h
ImplementationFile=MPEG4PlayView.cpp
Filter=C
BaseClass=CView
VirtualFilter=VWC


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T




[CLS:CAboutDlg]
Type=0
HeaderFile=MPEG4Play.cpp
ImplementationFile=MPEG4Play.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_OPEN
Command2=ID_FILE_MRU_FILE1
Command3=ID_APP_EXIT
Command4=ID_APP_ABOUT
CommandCount=4

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

