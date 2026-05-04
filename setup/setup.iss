#define BuildDir "build"

; Info: http://www.jrsoftware.org/ishelp/

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{4A5942D0-1C97-4FB2-AA4E-955032EEC89E}
AppName=Qt Commander
AppVersion=0.1.5
AppPublisher=Dirk Stolle
AppPublisherURL=https://github.com/striezel/qt-commander
AppSupportURL=https://github.com/striezel/qt-commander
AppUpdatesURL=https://github.com/striezel/qt-commander/releases
DefaultDirName={autopf}\Qt Commander
; UninstallDisplayIcon={app}\{#MyAppExeName}
ArchitecturesAllowed=x64compatible
; "ArchitecturesInstallIn64BitMode=x64compatible" requests that the
; install be done in "64-bit mode" on x64 or Windows 11 on Arm,
; meaning it should use the native 64-bit Program Files directory and
; the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64compatible
DefaultGroupName=Qt Commander
DisableProgramGroupPage=no
; Uncomment the following line to run in non administrative install mode (install for current user only).
;PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=commandline dialog
OutputBaseFilename=qt-commander-setup
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "ca"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "co"; MessagesFile: "compiler:Languages\Corsican.isl"
Name: "cs"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "da"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "nl"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "fi"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "fr"; MessagesFile: "compiler:Languages\French.isl"
Name: "he"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "it"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "jp"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "no"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "pl"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "pt"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "ru"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "sl"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "es"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "tr"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "uk"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; main executable
Source: "..\{#BuildDir}\qt-commander.exe"; DestDir: "{app}"; Flags: ignoreversion
; DLL files in main directory
Source: "..\{#BuildDir}\*.dll"; DestDir: "{app}"; Flags: ignoreversion
; DLL files from various subdirectories
Source: "..\{#BuildDir}\generic\*.dll"; DestDir: "{app}\generic"; Flags: ignoreversion
Source: "..\{#BuildDir}\iconengines\*.dll"; DestDir: "{app}\iconengines"; Flags: ignoreversion
Source: "..\{#BuildDir}\imageformats\*.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion
Source: "..\{#BuildDir}\multimedia\*.dll"; DestDir: "{app}\multimedia"; Flags: ignoreversion
Source: "..\{#BuildDir}\networkinformation\*.dll"; DestDir: "{app}\networkinformation"; Flags: ignoreversion
Source: "..\{#BuildDir}\platforms\*.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "..\{#BuildDir}\styles\*.dll"; DestDir: "{app}\styles"; Flags: ignoreversion
Source: "..\{#BuildDir}\tls\*.dll"; DestDir: "{app}\tls"; Flags: ignoreversion
; GPL 3 license text
Source: "..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion
; changelog
Source: "..\changelog.md"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Qt Commander"; Filename: "{app}\qt-commander.exe"
Name: "{autodesktop}\Qt Commander"; Filename: "{app}\qt-commander.exe"; Tasks: desktopicon
Name: "{group}\Uninstall Qt Commander"; Filename: "{app}\unins000.exe"
