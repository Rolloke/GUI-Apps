/*
   Registry-Keys für die Bugfixes in ETS3DGL
   Stand 15.10.1999
*/
#if !defined(ETS3DGL_REG_KEYS_H_INCLUDED_)
#define ETS3DGL_REG_KEYS_H_INCLUDED_

#define ETS3DGL_DEBUG_FILE       "ETS3DGL.DBG"
#define GLSETUP_INFO_PROPERTIES  "Your 3D-Graphic Properties"
#define GLSETUP_INFO_ABILITIES   "Your Graphic Card Capabilities"

#define SOFTWARE_REGKEY    "Software"
#define ETSKIEL_REGKEY        "ETSKIEL"

#define CETS3DGL_REGKEY          "ETS3DGL"
#define REPORT_KEY                  "Report"
#define REGKEY_PIXELFORMAT          "PixelFormat"
#define REGKEY_BUGFIX               "BugFix"
#define REGKEY_BUGFIX_BUFFERSIZE       "BufferSize"
#define REGKEY_BUGFIX_MOVEWINDOW       "MoveWindow"
#define REGKEY_BUGFIX_NORMALVECTOR     "NormalVector"
#define REGKEY_BUGFIX_LIGHTTEXTURE     "LightTexture"
#define REGKEY_BUGFIX_POLYGON_OFFSET   "PolygonOffset"
#define REGKEY_BUGFIX_DISTANCE_FACTOR  "DistanceFactor"
#define REGKEY_BUGFIX_BUFFER           "Buffer"
#define REGKEY_BUGFIX_NOGRAPHIC        "NoGraphic"

#define CARASDB_REGKEY           "CARASDB"
#define CARABOX_REGKEY           "CARABOX"
#define AC_COM_REGKEY            "AC_COM"
#define ETSCDPL_REGKEY           "ETSCDPL"

#define CARAWALK_MODULEFILENAME  "CARAWALK.Dll"
#define CARAWALK_REGKEY          "CARAWALK"
#define CARAWALK_GL_SETTINGS        "GLSettings"
#define CARAWALK_GENERIC               "Generic"
#define CARAWALK_GENERIC_ACC           "GenericAccelerated"
#define CARAWALK_COLOR_BITS            "ColorBits"
#define CARAWALK_DEPTH_BITS            "DepthBits"
#define CARAWALK_STENCIL_BITS          "StencilBits"
#define CARAWALK_ALPHA_BITS            "AlphaBits"
#define CARAWALK_ACCUM_BITS            "AccumBits"
#define CARAWALK_REFRESH_TIME          "RefreshTime"
#define CARAWALK_PERSPECTIVE_ANGLE     "PerspectiveAngle"
#define CARAWALK_BOX_SHADOWS           "Shadows"
#define CARAWALK_TEX_AMBIENT           "TexAmbient"
#define CARAWALK_TEX_DIFFUSE           "TexDiffuse"
#define CARAWALK_TEX_SPECULAR          "TexSpecular"
#define CARAWALK_INIT_3D_ALWAYS        "InitAlways"
#define CARAWALK_TEX_AVG_B             "TexAvgB"
#define CARAWALK_TEX_AVG_M             "TexAvgM"
#define CARAWALK_TEX_AMBIENT_FACTOR    "TexAmbFac"

#define ETS3DV_MODULEFILENAME    "Ets3dV.exe"
#define ETS3DV_REGKEY            "ETS3DVIEW"
#define REGKEY_SETTINGS              "Settings"
#define REGKEY_SETTINGS_GLGENERIC      "GLGeneric"
#define REGKEY_SETTINGS_MENU           "Menu"
#define REGKEY_SETTINGS_FILLPATH       "FillPath"

#define REGKEY_DEFAULT_GLDIRECT        "GLDirect"
#define REGKEY_DEFAULT_GLDOUBLE        "GLDouble"

#define ETS2DV_MODULEFILENAME    "Ets2dV.exe"
#define ETS2DV_REGKEY            "ETS2DVIEW"

#define REGKEY_DEFAULT           "DefaultSettings"
#define REGKEY_DEFAULT_MAX_MRU_FILES   "Max MRU Files"

#define PRINTER                  "Printer"
#define OFFSET_CORRECTION              "Offset"

#define ETSPRTD_REGKEY           "ETSPRTD"

#define ETSBIND_REGKEY           "ETSBIND"
#define REGKEY_DLLPATH           "Path"
#define REGKEY_DLLPATH_DEBUG     "PathD"
#define REGKEY_DLLPATH_RELEASE   "PathR"
#define REGKEY_DLLPATH_RELEASE_E "PathRe"
#define REGKEY_USEDEBUGDLL       "UseDebugDll"
#define REGKEY_USELOCALDLL       "UseLocalDll"

// AC-COM-Schnittstelle
#define COM_PORT           "Port"
#define CD_DRIVE           "CD-Drive"
#define TAB_BUTTONS        "Button"
#define REGKEY_CD_DISPLAY  "CD-Display"
#define CD_DISPLAY_BKGND   "BkGndColor"
#define CD_DISPLAY_TEXT    "TextColor"
#define CD_DISPLAY_FONT    "TextFont"
#define CD_DISPLAY_DIV     "xDivision"
#define REGKEY_FORMATS     "Format"
#define CARA_PATH          "CaraPath"
#define CD_INFO_PATH       "CD_Info"
#define FR_RANGE           "FR-Range"
#define FR_DIVISION        "FR-Division"
#define POLL_TIME_M        "PollTimeM"
#define PROFI_MODE         "Professional"
#define EDIT_CURVES        "EditCurves"
#define PORT_TRIALS        "Trials"
#define CLOSE_PORT         "ClosePort"
#define FILE_PATH          "FilePath"
#define TRACK9             "Track9"
#define TRACK10            "Track10"
#define PORT_EVENT_TIME    "EventTime"
#define AUDIO_EXTENSION    "AudioExt"
#define WAVE_DEV_NAME      "WaveDevName"
#define OTHER_DEV_NAME     "OtherDevName"
#define NO_PORT_ENUM       "NoPortEnum"
#define MB_FLAGS           "MBFlags"

#define REPORT_KEY_DEFAULT             false
#define BUGFIX_BUFFERSIZE_DEFAULT      false
#define BUGFIX_MOVEWINDOW_DEFAULT      false
#define BUGFIX_NORMALVECTOR_DEFAULT    false
#define BUGFIX_LIGHTTEXTURE_DEFAULT    false
#define BUGFIX_POLYGON_OFFSET_DEFAULT  0
#define BUGFIX_DISTANCE_FACTOR_DEFAULT 0.05f
#define PIXELFORMAT_DEFAULT            0
#define BUGFIX_BUFFER_DEFAULT          1

#define CARAWALK_GENERIC_DEFAULT       true
#define CARAWALK_GENERIC_ACC_DEFAULT   false
#define CARAWALK_COLOR_BITS_DEFAULT    0
#define CARAWALK_DEPTH_BITS_DEFAULT    32
#define CARAWALK_STENCIL_BITS_DEFAULT  0
#define CARAWALK_ALPHA_BITS_DEFAULT    0
#define CARAWALK_ACCUM_BITS_DEFAULT    0
#define CARAWALK_REFRESH_TIME_DEFAULT  42
#define CARAWALK_PERSPECTIVE_ANGLE_DEFAULT  60
#define CARAWALK_GLDIRECT_DEFAULT      0
#define CARAWALK_GLDOUBLE_DEFAULT      0
#define CARAWALK_BOX_SHADOWS_DEFAULT   1
#define CARAWALK_TEX_AMBIENT_DEFAULT   0x00b2b2b2
#define CARAWALK_TEX_DIFFUSE_DEFAULT   0x00b2b2b2
#define CARAWALK_TEX_SPECULAR_DEFAULT  0x00000000
#define CARAWALK_INIT_3D_ALWAYS_DEFAULT false
#define CARAWALK_TEX_AVG_B_DEFAULT     200
#define CARAWALK_TEX_AVG_M_DEFAULT     500
#define CARAWALK_TEX_AMBIENTF_DEFAULT  500

#define SETTINGS_GLGENERIC_DEFAULT     true
#define SETTINGS_MENU_DEFAULT          100
#define SETTINGS_FILLPATH_DEFAULT      true
#define SETTINGS_GLDIRECT_DEFAULT      0
#define SETTINGS_GLDOUBLE_DEFAULT      0

#endif // ETS3DGL_REG_KEYS_H_INCLUDED_