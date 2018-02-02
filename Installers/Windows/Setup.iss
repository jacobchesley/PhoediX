; Script generated by the Inno Script Studio Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "PhoediX"
#define MyAppVersion "0.1.0"
#define MyAppPublisher "Jacob Chesley"
#define MyAppURL "http://www.phoedix.com/"
#define MyAppExeName "PhoediX.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{8EC19654-9A6F-4753-ABDC-7BB8D8576989}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputDir=C:\Development\PhoediX\Installers\Windows
OutputBaseFilename=setup
SetupIconFile=C:\Development\PhoediX\Icon\px.ico
Compression=lzma
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Setup]
LicenseFile=AllLicenses.txt

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "..\..\Build\Release_x64\PhoediX.exe"; DestDir: "{app}"; Flags: ignoreversion; Check: Is64BitInstallMode
Source: "..\..\Build\Release_x64\libraw.dll"; DestDir: "{app}"; Flags: ignoreversion; Check: Is64BitInstallMode
Source: "..\..\Build\Release_x64\libtiff.dll"; DestDir: "{app}"; Flags: ignoreversion; Check: Is64BitInstallMode
Source: "..\..\Build\Release_x64\settings.ini"; DestDir: "{app}"; Flags: ignoreversion; Check: Is64BitInstallMode

Source: "..\..\Build\Release_x86\PhoediX.exe"; DestDir: "{app}"; Flags: ignoreversion; Check: not Is64BitInstallMode
Source: "..\..\Build\Release_x86\libraw.dll"; DestDir: "{app}"; Flags: ignoreversion; Check: not Is64BitInstallMode
Source: "..\..\Build\Release_x86\libtiff.dll"; DestDir: "{app}"; Flags: ignoreversion; Check: not Is64BitInstallMode
Source: "..\..\Build\Release_x86\settings.ini"; DestDir: "{app}"; Flags: ignoreversion; Check: not Is64BitInstallMode

Source: "..\..\3rd Party Licenses\*"; DestDir: "{app}\3rd Party Licenses"; Flags: ignoreversion recursesubdirs

; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Dirs]
Name: "{app}"; Permissions: everyone-full

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

;[Code]
;var 
;  licensePage: TOutputMsgMemoWizardPage;
;  acceptButton: TButton
;  cancelButton: TButton

;  procedure InitializeWizard();
;  begin
;    licensePage := CreateOutputMsgMemoPage(wpLicense, SetupMessage(msgWizardLicense), SetupMessage(msgLicenseLabel),'test 2', '');