;--------------------------------
;Vibe Streamer Setup Script

  !define APP_NAME "Vibe Streamer"
  !define APP_VERSION "3.1.0"
  !define APP_MILESTONE ""
  !define COMPATIBLE_VERSION "3.1.0"
  !define DEFAULT_STARTMENU_FOLDER "Vibe Streamer"

;--------------------------------
;Includes

  !include "MUI.nsh"
  !include "WordFunc.nsh"

;--------------------------------
;Version Information

  VIProductVersion "${APP_VERSION}.0"
  VIAddVersionKey ProductName "${APP_NAME}"
  VIAddVersionKey CompanyName "Erik Nilsson"
  VIAddVersionKey LegalCopyright "Copyright Erik Nilsson"
  VIAddVersionKey FileDescription "${APP_NAME}"
  VIAddVersionKey FileVersion "${APP_VERSION}"

;--------------------------------
;MUI Settings

  !define MUI_ABORTWARNING

  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "win32-setup-small.bmp"

  !define MUI_WELCOMEFINISHPAGE_BITMAP "win32-setup-large.bmp"

  !define MUI_ICON "win32-setup.ico"
  !define MUI_UNICON "win32-setup.ico"
  
  !define MUI_STARTMENUPAGE_DEFAULTFOLDER "${DEFAULT_STARTMENU_FOLDER}"
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU"
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${APP_NAME}"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !define MUI_FINISHPAGE_RUN
  !define MUI_FINISHPAGE_RUN_TEXT "Create Desktop Shortcut"
  !define MUI_FINISHPAGE_RUN_FUNCTION CreateDesktopShortcut  
  
  ;Name and file
  Name "${APP_NAME}"
  OutFile "Vibe_Streamer_Setup_${APP_VERSION}${APP_MILESTONE}.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\${APP_NAME}"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\${APP_NAME}" "InstallPath"
  
;--------------------------------
;Functions

  Function CheckVersion
  
    ClearErrors
    
    ;Always compatible if no version is installed
    EnumRegKey $R0 HKCU "Software\${APP_NAME}" ""
    IfErrors compatible
    
    ReadRegStr $0 HKCU "Software\${APP_NAME}" "CurrentVersion"
    
    ;Check if installed version is newer than this version
    ${VersionCompare} $0 "${APP_VERSION}" $1
    ${If} $1 == 1
      Goto outdated
    ${EndIf}
    
    ;Check if this version is compatible with the installed version
    ${VersionCompare} $0 "${COMPATIBLE_VERSION}" $1
    ${If} $1 == 2
      Goto incompatible
    ${EndIf}
    
    ;Check if installed version is a milestone version
    ReadRegStr $0 HKCU "Software\${APP_NAME}" "CurrentMilestone"
    StrLen $1 $0
    ${If} $1 > 0
      Goto incompatible
    ${EndIf}
    
    Goto compatible
    
    outdated:
      MessageBox MB_YESNO|MB_ICONQUESTION "An existing installation of ${APP_NAME} was detected that is newer version than one you're installing.$\n\
        It is recommended to uninstall or move the existing installation before proceeding.$\nDo you want to continue the installation anyway?" IDYES +2 IDNO +1
          Quit
          
    Goto Compatible
          
    incompatible:
      MessageBox MB_YESNO|MB_ICONQUESTION "An existing installation of ${APP_NAME} was detected that isn't compatible with the one you're installing.$\n\
        It is recommended to uninstall or move the existing installation before proceeding.$\nDo you want to continue the installation anyway?" IDYES +2 IDNO +1
          Quit
          
    compatible:
    
  FunctionEnd

  Function CreateDesktopShortcut
  
    SetOutPath $INSTDIR
    CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\vibestreamer.exe"
    
  FunctionEnd

;--------------------------------
;Pages

  !define MUI_PAGE_CUSTOMFUNCTION_LEAVE CheckVersion
  !insertmacro MUI_PAGE_LICENSE "..\license.txt"
  !insertmacro MUI_PAGE_DIRECTORY

  Var STARTMENU_FOLDER
  !insertmacro MUI_PAGE_STARTMENU "Application" $STARTMENU_FOLDER

  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

  Section "Installation" SecInstallation

    SetOutPath "$INSTDIR"

    ;Copy files from install folder
    File /r /x .svn win32\*.*

    ;Copy files from Win32 project
    File ..\src\win32\*.dll
    File ..\src\win32\site.xml
    File ..\src\win32\Release\vibestreamer.exe
    
    ;Copy changelog and licenses
    File ..\Changelog.txt
    File ..\License.txt
    File ..\ThirdPartyLicenses.txt

    ;Write uninstaller
    WriteUninstaller "uninstall.exe"
    
    SetOutPath "$INSTDIR\db"
    
    ;Copy db scripts from Win32 project
    File ..\src\win32\db\*.sql
    
    SetOutPath "$INSTDIR\sites\default"

    ;Copy default site from client project
    File /r /x .* /x src /x build-tools /x *-debug.vibe ..\client\vibe-ext\*.*
    
    SetOutPath "$INSTDIR"

    ;Write data to registry
    WriteRegStr HKCU "Software\${APP_NAME}" "InstallPath" "$INSTDIR"
    WriteRegStr HKCU "Software\${APP_NAME}" "CurrentVersion" "${APP_VERSION}"
    WriteRegStr HKCU "Software\${APP_NAME}" "CurrentMilestone" "${APP_MILESTONE}"
    
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
      CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
      CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\${APP_NAME}.lnk" "$INSTDIR\vibestreamer.exe"
      CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"
    !insertmacro MUI_STARTMENU_WRITE_END

  SectionEnd

;--------------------------------
;Uninstaller Section

  Section "Uninstall" SecUninstall
  
    !insertmacro MUI_STARTMENU_GETFOLDER Application $R0

    ;Remove shortcuts, if any
    Delete "$SMPROGRAMS\$R0\*.*"
    Delete "$DESKTOP\${APP_NAME}.lnk"

    ;Remove directories used
    RMDir "$SMPROGRAMS\$R0"
    RMDir /r /REBOOTOK "$INSTDIR"

    DeleteRegKey HKCU "Software\${APP_NAME}"

  SectionEnd