param(
    [ValidateSet("Release", "Debug")]
    [string] $Configuration = "Release",

    [ValidateSet("x64")]
    [string] $Platform = "x64",

    [string] $Preset = "windows-vs2022",

    [ValidateSet("AUTO", "ON", "OFF")]
    [string] $EnableVst2 = "AUTO",

    [ValidateSet("LEGACY", "XAYMAR")]
    [string] $Vst2Provider = "XAYMAR",

    [string] $Vst2SdkDir = "",

    [switch] $SkipBuild,

    [switch] $KeepStage
)

$ErrorActionPreference = "Stop"

$appName = "Light Host Modern"
$appVersion = "1.0.0"
$exeName = "Light Host Modern.exe"
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$outRoot = Join-Path $repoRoot "out\release"
$stageRoot = Join-Path $outRoot "payload"
$packageWorkRoot = Join-Path $outRoot "package-work"
$payloadZip = Join-Path $outRoot "LightHostModern-payload.zip"
$installerExe = Join-Path $outRoot "LightHostModern-Setup.exe"
$portableExe = Join-Path $outRoot "LightHostModern-Portable.exe"
$releaseIcon = Join-Path $repoRoot "Icon\logo.ico"

function Resolve-CMake {
    $cmake = Get-Command cmake -ErrorAction SilentlyContinue
    if ($null -ne $cmake) {
        return $cmake.Source
    }

    $defaultCMake = Join-Path $env:ProgramFiles "CMake\bin\cmake.exe"
    if (Test-Path -LiteralPath $defaultCMake) {
        return $defaultCMake
    }

    throw "CMake 3.22+ was not found. Install current CMake and Visual Studio Build Tools 2022."
}

function Resolve-MSBuild {
    $candidates = @()
    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"

    if (Test-Path -LiteralPath $vswhere) {
        $candidates += & $vswhere -latest -products * -requires Microsoft.Component.MSBuild -find "MSBuild\**\Bin\MSBuild.exe"
    }

    $candidates += @(
        "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\18\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\18\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe"
    )

    foreach ($candidate in $candidates | Where-Object { -not [string]::IsNullOrWhiteSpace($_) } | Select-Object -Unique) {
        if (Test-Path -LiteralPath $candidate) {
            return $candidate
        }
    }

    throw "MSBuild was not found. Install Visual Studio 2022/2026 with Desktop development with C++ and Windows App SDK tooling."
}

function Resolve-VCVars64 {
    $candidateRoots = @(
        "C:\Program Files\Microsoft Visual Studio\18\Community",
        "C:\Program Files\Microsoft Visual Studio\18\Professional",
        "C:\Program Files\Microsoft Visual Studio\18\Enterprise",
        "C:\Program Files\Microsoft Visual Studio\2022\Community",
        "C:\Program Files\Microsoft Visual Studio\2022\Professional",
        "C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
        "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools"
    )

    $vswhere = Join-Path ${env:ProgramFiles(x86)} "Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path -LiteralPath $vswhere) {
        $installations = & $vswhere -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
        $candidateRoots = @($installations) + $candidateRoots
    }

    foreach ($root in $candidateRoots | Where-Object { -not [string]::IsNullOrWhiteSpace($_) } | Select-Object -Unique) {
        $vcvars = Join-Path $root "VC\Auxiliary\Build\vcvars64.bat"
        if (Test-Path -LiteralPath $vcvars) {
            return $vcvars
        }
    }

    throw "vcvars64.bat was not found. Install Visual Studio Build Tools with the MSVC x64 toolchain."
}

function Invoke-Checked {
    param(
        [Parameter(Mandatory)]
        [string] $FilePath,

        [Parameter(Mandatory)]
        [string[]] $Arguments
    )

    & $FilePath @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code $LASTEXITCODE`: $FilePath $($Arguments -join ' ')"
    }
}

function Copy-VCRuntime {
    param(
        [Parameter(Mandatory)]
        [string] $Destination
    )

    $redistRoots = @(
        "C:\Program Files\Microsoft Visual Studio\18",
        "C:\Program Files\Microsoft Visual Studio\2022",
        "C:\Program Files (x86)\Microsoft Visual Studio\2022"
    )

    $crtDirs = @()
    foreach ($root in $redistRoots) {
        if (Test-Path -LiteralPath $root) {
            $crtDirs += Get-ChildItem -LiteralPath $root -Recurse -Directory -ErrorAction SilentlyContinue |
                Where-Object { $_.FullName -match "\\VC\\Redist\\MSVC\\[^\\]+\\$Platform\\Microsoft\.VC\d+\.CRT$" }
        }
    }

    $crtDir = $crtDirs | Sort-Object FullName -Descending | Select-Object -First 1
    if ($null -eq $crtDir) {
        Write-Warning "MSVC runtime redist folder was not found. The release may require the Microsoft Visual C++ Redistributable on target machines."
        return
    }

    Copy-Item -Path (Join-Path $crtDir.FullName "*.dll") -Destination $Destination -Force
}

function New-Directory {
    param([Parameter(Mandatory)][string] $Path)

    if (Test-Path -LiteralPath $Path) {
        Remove-Item -LiteralPath $Path -Recurse -Force
    }

    New-Item -ItemType Directory -Force -Path $Path | Out-Null
}

function New-IExpressPackage {
    param(
        [Parameter(Mandatory)]
        [string] $Name,

        [Parameter(Mandatory)]
        [string] $SourceDir,

        [Parameter(Mandatory)]
        [string] $Launcher,

        [Parameter(Mandatory)]
        [string] $TargetExe
    )

    $iexpress = Join-Path $env:windir "System32\iexpress.exe"
    if (!(Test-Path -LiteralPath $iexpress)) {
        throw "iexpress.exe was not found. It is required to generate the single-file release executables."
    }

    $files = Get-ChildItem -LiteralPath $SourceDir -File | Sort-Object Name
    if ($files.Count -eq 0) {
        throw "No files found for IExpress package: $SourceDir"
    }

    $sedPath = Join-Path $SourceDir "$Name.sed"
    $fileStrings = New-Object System.Collections.Generic.List[string]
    $sourceFileEntries = New-Object System.Collections.Generic.List[string]

    for ($i = 0; $i -lt $files.Count; $i++) {
        $fileStrings.Add("FILE$i=$($files[$i].Name)")
        $sourceFileEntries.Add("%FILE$i%=")
    }

    $sed = @"
[Version]
Class=IEXPRESS
SEDVersion=3

[Options]
PackagePurpose=InstallApp
ShowInstallProgramWindow=0
HideExtractAnimation=1
UseLongFileName=1
InsideCompressed=1
CAB_FixedSize=0
CAB_ResvCodeSigning=0
RebootMode=N
InstallPrompt=%InstallPrompt%
DisplayLicense=%DisplayLicense%
FinishMessage=%FinishMessage%
TargetName=%TargetName%
FriendlyName=%FriendlyName%
AppLaunched=%AppLaunched%
PostInstallCmd=<None>
AdminQuietInstCmd=%AppLaunched%
UserQuietInstCmd=%AppLaunched%
SourceFiles=SourceFiles

[Strings]
InstallPrompt=
DisplayLicense=
FinishMessage=
TargetName=$TargetExe
FriendlyName=$Name
AppLaunched=$Launcher
$($fileStrings -join "`r`n")

[SourceFiles]
SourceFiles0=$SourceDir

[SourceFiles0]
$($sourceFileEntries -join "`r`n")
"@

    Set-Content -LiteralPath $sedPath -Value $sed -Encoding ASCII
    Invoke-Checked -FilePath $iexpress -Arguments @("/N", "/Q", $sedPath)

    if (!(Test-Path -LiteralPath $TargetExe)) {
        throw "IExpress did not create the expected package: $TargetExe"
    }
}

function New-NativeSelfExtractPackage {
    param(
        [Parameter(Mandatory)]
        [string] $Name,

        [Parameter(Mandatory)]
        [string] $WorkDir,

        [Parameter(Mandatory)]
        [string] $PayloadZipPath,

        [Parameter(Mandatory)]
        [string] $EntryScriptPath,

        [Parameter(Mandatory)]
        [string] $TargetExe,

        [string] $IconPath = "",

        [switch] $InstallerUi
    )

    $vcvars = Resolve-VCVars64
    $safeName = ($Name -replace "[^A-Za-z0-9_]", "_")
    $cppPath = Join-Path $WorkDir "$safeName.cpp"
    $rcPath = Join-Path $WorkDir "$safeName.rc"
    $resPath = Join-Path $WorkDir "$safeName.res"
    $objPath = Join-Path $WorkDir "$safeName.obj"

    $cpp = @'
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <windows.h>
#include <commctrl.h>
#include <string>

static bool writeResource(int id, const std::wstring& path)
{
    HRSRC resource = FindResourceW(nullptr, MAKEINTRESOURCEW(id), RT_RCDATA);
    if (resource == nullptr) return false;

    HGLOBAL loaded = LoadResource(nullptr, resource);
    if (loaded == nullptr) return false;

    DWORD size = SizeofResource(nullptr, resource);
    const void* data = LockResource(loaded);
    if (data == nullptr || size == 0) return false;

    HANDLE file = CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;

    DWORD written = 0;
    BOOL ok = WriteFile(file, data, size, &written, nullptr);
    CloseHandle(file);
    return ok && written == size;
}

static std::wstring quote(const std::wstring& value)
{
    return L"\"" + value + L"\"";
}

static bool showInstallPrompt()
{
    HMODULE comctl = LoadLibraryW(L"comctl32.dll");
    if (comctl != nullptr) {
        using TaskDialogIndirectFn = HRESULT (WINAPI *)(const TASKDIALOGCONFIG*, int*, int*, BOOL*);
        auto taskDialogIndirect = reinterpret_cast<TaskDialogIndirectFn>(GetProcAddress(comctl, "TaskDialogIndirect"));

        if (taskDialogIndirect != nullptr) {
            const TASKDIALOG_BUTTON buttons[] = {
                { 100, L"Install Light Host Modern\nInstall for the current Windows user." },
                { IDCANCEL, L"Cancel" }
            };

            TASKDIALOGCONFIG config = {};
            config.cbSize = sizeof(config);
            config.dwFlags = TDF_USE_COMMAND_LINKS | TDF_ALLOW_DIALOG_CANCELLATION;
            config.pszWindowTitle = L"Light Host Modern Setup";
            config.pszMainInstruction = L"Install Light Host Modern";
            config.pszContent = L"Destination: %LOCALAPPDATA%\\Programs\\Light Host Modern\n\nThe installer will add Start Menu and Desktop shortcuts.";
            config.cButtons = ARRAYSIZE(buttons);
            config.pButtons = buttons;
            config.nDefaultButton = 100;
            config.pszMainIcon = TD_INFORMATION_ICON;

            int pressedButton = IDCANCEL;
            if (SUCCEEDED(taskDialogIndirect(&config, &pressedButton, nullptr, nullptr))) {
                FreeLibrary(comctl);
                return pressedButton == 100;
            }
        }

        FreeLibrary(comctl);
    }

    int result = MessageBoxW(
        nullptr,
        L"Install Light Host Modern for the current Windows user?\n\nDestination: %LOCALAPPDATA%\\Programs\\Light Host Modern",
        L"Light Host Modern Setup",
        MB_OKCANCEL | MB_ICONINFORMATION);

    return result == IDOK;
}

static void showInstallComplete()
{
    HMODULE comctl = LoadLibraryW(L"comctl32.dll");
    if (comctl != nullptr) {
        using TaskDialogFn = HRESULT (WINAPI *)(HWND, HINSTANCE, PCWSTR, PCWSTR, PCWSTR, TASKDIALOG_COMMON_BUTTON_FLAGS, PCWSTR, int*);
        auto taskDialog = reinterpret_cast<TaskDialogFn>(GetProcAddress(comctl, "TaskDialog"));

        if (taskDialog != nullptr) {
            int pressedButton = IDOK;
            taskDialog(
                nullptr,
                nullptr,
                L"Light Host Modern Setup",
                L"Installation completed",
                L"Light Host Modern was installed successfully.",
                TDCBF_OK_BUTTON,
                TD_INFORMATION_ICON,
                &pressedButton);
            FreeLibrary(comctl);
            return;
        }

        FreeLibrary(comctl);
    }

    MessageBoxW(nullptr, L"Light Host Modern was installed successfully.", L"Light Host Modern Setup", MB_OK | MB_ICONINFORMATION);
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
#ifdef SHOW_INSTALLER_UI
    if (!showInstallPrompt()) {
        return 0;
    }
#endif

    wchar_t tempPath[MAX_PATH] = {};
    DWORD tempLength = GetTempPathW(MAX_PATH, tempPath);
    if (tempLength == 0 || tempLength >= MAX_PATH) {
        MessageBoxW(nullptr, L"Unable to resolve the temporary folder.", L"Light Host Modern", MB_ICONERROR);
        return 1;
    }

    std::wstring workDir = std::wstring(tempPath) + L"LightHostModernRelease-" + std::to_wstring(GetCurrentProcessId());
    CreateDirectoryW(workDir.c_str(), nullptr);

    std::wstring payloadPath = workDir + L"\\payload.zip";
    std::wstring scriptPath = workDir + L"\\entry.ps1";

    if (!writeResource(101, payloadPath) || !writeResource(102, scriptPath)) {
        MessageBoxW(nullptr, L"Unable to extract release payload.", L"Light Host Modern", MB_ICONERROR);
        return 1;
    }

    std::wstring command = L"powershell.exe -NoProfile -ExecutionPolicy Bypass -File " + quote(scriptPath);
    STARTUPINFOW startupInfo = {};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION processInfo = {};
    std::wstring mutableCommand = command;
    if (!CreateProcessW(nullptr, &mutableCommand[0], nullptr, nullptr, FALSE, CREATE_NO_WINDOW, nullptr, workDir.c_str(), &startupInfo, &processInfo)) {
        MessageBoxW(nullptr, L"Unable to run the embedded release script.", L"Light Host Modern", MB_ICONERROR);
        return 1;
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(processInfo.hProcess, &exitCode);
    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);

    if (exitCode != 0) {
        MessageBoxW(nullptr, L"The release script failed. Run the app with --debug or rebuild the release package for diagnostics.", L"Light Host Modern", MB_ICONERROR);
        return static_cast<int>(exitCode);
    }

#ifdef SHOW_INSTALLER_UI
    showInstallComplete();
#endif

    return 0;
}
'@

    Set-Content -LiteralPath $cppPath -Value $cpp -Encoding ASCII

    $payloadResourcePath = $PayloadZipPath.Replace("\", "\\")
    $scriptResourcePath = $EntryScriptPath.Replace("\", "\\")
    $iconResourceLine = ""
    if (![string]::IsNullOrWhiteSpace($IconPath) -and (Test-Path -LiteralPath $IconPath)) {
        $iconResourcePath = $IconPath.Replace("\", "\\")
        $iconResourceLine = "1 ICON `"$iconResourcePath`"`r`n"
    }

    $rc = @"
$iconResourceLine
101 RCDATA "$payloadResourcePath"
102 RCDATA "$scriptResourcePath"
"@
    Set-Content -LiteralPath $rcPath -Value $rc -Encoding ASCII

    if (Test-Path -LiteralPath $TargetExe) {
        Remove-Item -LiteralPath $TargetExe -Force
    }

    $installerDefine = ""
    if ($InstallerUi) {
        $installerDefine = "/DSHOW_INSTALLER_UI "
    }

    $compileCommand = "`"$vcvars`" >nul && rc.exe /nologo /fo `"$resPath`" `"$rcPath`" && cl.exe /nologo /O2 /MT /EHsc /DUNICODE /D_UNICODE /D_WIN32_WINNT=0x0600 $installerDefine/Fo`"$objPath`" `"$cppPath`" `"$resPath`" user32.lib /link /SUBSYSTEM:WINDOWS /OUT:`"$TargetExe`""
    & cmd.exe /d /s /c $compileCommand
    if ($LASTEXITCODE -ne 0) {
        throw "Native self-extracting package build failed with exit code $LASTEXITCODE`: $TargetExe"
    }

    if (!(Test-Path -LiteralPath $TargetExe)) {
        throw "Native self-extracting package was not created: $TargetExe"
    }
}

function Write-InstallerPayload {
    param([Parameter(Mandatory)][string] $TargetDir)

    New-Directory -Path $TargetDir
    Copy-Item -LiteralPath $payloadZip -Destination (Join-Path $TargetDir "payload.zip") -Force

    @'
@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0install.ps1"
exit /b %ERRORLEVEL%
'@ | Set-Content -LiteralPath (Join-Path $TargetDir "install.cmd") -Encoding ASCII

    @'
$ErrorActionPreference = "Stop"

$appName = "Light Host Modern"
$exeName = "Light Host Modern.exe"
$installRoot = Join-Path $env:LOCALAPPDATA "Programs\Light Host Modern"
$payloadZip = Join-Path $PSScriptRoot "payload.zip"
$startMenuDir = Join-Path $env:APPDATA "Microsoft\Windows\Start Menu\Programs\Light Host Modern"
$desktopShortcut = Join-Path ([Environment]::GetFolderPath("DesktopDirectory")) "Light Host Modern.lnk"
$uninstallScript = Join-Path $installRoot "Uninstall-LightHostModern.ps1"

if (Test-Path -LiteralPath $installRoot) {
    Remove-Item -LiteralPath $installRoot -Recurse -Force
}

New-Item -ItemType Directory -Force -Path $installRoot | Out-Null
Expand-Archive -LiteralPath $payloadZip -DestinationPath $installRoot -Force

$exePath = Join-Path $installRoot $exeName
if (!(Test-Path -LiteralPath $exePath)) {
    throw "Installed executable was not found: $exePath"
}

$uninstallBody = @"
`$ErrorActionPreference = "Stop"
`$installRoot = "$installRoot"
`$startMenuDir = "$startMenuDir"
`$desktopShortcut = "$desktopShortcut"
`$uninstallKey = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall\LightHostModern"
Get-Process "Light Host Modern" -ErrorAction SilentlyContinue | Stop-Process -Force -ErrorAction SilentlyContinue
if (Test-Path -LiteralPath `$desktopShortcut) { Remove-Item -LiteralPath `$desktopShortcut -Force }
if (Test-Path -LiteralPath `$startMenuDir) { Remove-Item -LiteralPath `$startMenuDir -Recurse -Force }
if (Test-Path -LiteralPath `$uninstallKey) { Remove-Item -LiteralPath `$uninstallKey -Recurse -Force }
if (Test-Path -LiteralPath `$installRoot) { Remove-Item -LiteralPath `$installRoot -Recurse -Force }
"@

Set-Content -LiteralPath $uninstallScript -Value $uninstallBody -Encoding UTF8

$shell = New-Object -ComObject WScript.Shell
New-Item -ItemType Directory -Force -Path $startMenuDir | Out-Null

$startShortcut = $shell.CreateShortcut((Join-Path $startMenuDir "Light Host Modern.lnk"))
$startShortcut.TargetPath = $exePath
$startShortcut.WorkingDirectory = $installRoot
$startShortcut.IconLocation = "$exePath,0"
$startShortcut.Save()

$desktop = $shell.CreateShortcut($desktopShortcut)
$desktop.TargetPath = $exePath
$desktop.WorkingDirectory = $installRoot
$desktop.IconLocation = "$exePath,0"
$desktop.Save()

$uninstallKey = "HKCU:\Software\Microsoft\Windows\CurrentVersion\Uninstall\LightHostModern"
New-Item -Path $uninstallKey -Force | Out-Null
Set-ItemProperty -Path $uninstallKey -Name DisplayName -Value $appName
Set-ItemProperty -Path $uninstallKey -Name DisplayVersion -Value "1.0.0"
Set-ItemProperty -Path $uninstallKey -Name Publisher -Value "Light Host Modern"
Set-ItemProperty -Path $uninstallKey -Name InstallLocation -Value $installRoot
Set-ItemProperty -Path $uninstallKey -Name DisplayIcon -Value $exePath
Set-ItemProperty -Path $uninstallKey -Name UninstallString -Value "powershell.exe -NoProfile -ExecutionPolicy Bypass -File `"$uninstallScript`""
Set-ItemProperty -Path $uninstallKey -Name NoModify -Value 1 -Type DWord
Set-ItemProperty -Path $uninstallKey -Name NoRepair -Value 1 -Type DWord

Write-Host "Light Host Modern installed to $installRoot"
'@ | Set-Content -LiteralPath (Join-Path $TargetDir "install.ps1") -Encoding UTF8
}

function Write-PortablePayload {
    param([Parameter(Mandatory)][string] $TargetDir)

    New-Directory -Path $TargetDir
    Copy-Item -LiteralPath $payloadZip -Destination (Join-Path $TargetDir "payload.zip") -Force

    @'
@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0run-portable.ps1"
exit /b %ERRORLEVEL%
'@ | Set-Content -LiteralPath (Join-Path $TargetDir "run-portable.cmd") -Encoding ASCII

    @'
$ErrorActionPreference = "Stop"

$exeName = "Light Host Modern.exe"
$payloadZip = Join-Path $PSScriptRoot "payload.zip"
$payloadHash = (Get-FileHash -LiteralPath $payloadZip -Algorithm SHA256).Hash.Substring(0, 16)
$portableRoot = Join-Path $env:TEMP "LightHostModern-Portable\$payloadHash"
$exePath = Join-Path $portableRoot $exeName

if (!(Test-Path -LiteralPath $exePath)) {
    if (Test-Path -LiteralPath $portableRoot) {
        Remove-Item -LiteralPath $portableRoot -Recurse -Force
    }

    New-Item -ItemType Directory -Force -Path $portableRoot | Out-Null
    Expand-Archive -LiteralPath $payloadZip -DestinationPath $portableRoot -Force
}

Start-Process -FilePath $exePath -WorkingDirectory $portableRoot
'@ | Set-Content -LiteralPath (Join-Path $TargetDir "run-portable.ps1") -Encoding UTF8
}

if (!$SkipBuild) {
    $msbuild = Resolve-MSBuild
    $winUIProject = Join-Path $repoRoot "WinUI\LightHost.WinUI\LightHost.WinUI.vcxproj"

    Invoke-Checked -FilePath $msbuild -Arguments @(
        $winUIProject,
        "/restore",
        "/m",
        "/p:Configuration=$Configuration",
        "/p:Platform=$Platform"
    )

    $cmakePath = Resolve-CMake
    $configureArgs = @(
        "--preset", $Preset,
        "-DLIGHTHOST_ENABLE_VST2=$EnableVst2",
        "-DLIGHTHOST_VST2_PROVIDER=$Vst2Provider"
    )

    if (![string]::IsNullOrWhiteSpace($Vst2SdkDir)) {
        $configureArgs += "-DLIGHTHOST_VST2_SDK_DIR=$Vst2SdkDir"
    } elseif (![string]::IsNullOrWhiteSpace($env:LIGHTHOST_VST2_SDK_DIR)) {
        $configureArgs += "-DLIGHTHOST_VST2_SDK_DIR=$env:LIGHTHOST_VST2_SDK_DIR"
    }

    Invoke-Checked -FilePath $cmakePath -Arguments $configureArgs
    Invoke-Checked -FilePath $cmakePath -Arguments @("--build", "--preset", "$Preset-$($Configuration.ToLowerInvariant())", "--config", $Configuration)
}

$hostOutput = Join-Path $repoRoot "out\build\windows-vs2022\LightHost_artefacts\$Configuration"
$hostExe = Join-Path $hostOutput $exeName
$winUIOutput = Join-Path $hostOutput "WinUI\$Platform\$Configuration\LightHost.WinUI\LightHostWinUI.exe"

if (!(Test-Path -LiteralPath $hostExe)) {
    throw "Host executable was not found: $hostExe"
}

if (!(Test-Path -LiteralPath $winUIOutput)) {
    throw "WinUI executable was not found inside the host output. Build the WinUI project before building the host: $winUIOutput"
}

New-Directory -Path $stageRoot
New-Item -ItemType Directory -Force -Path $outRoot | Out-Null
Copy-Item -Path (Join-Path $hostOutput "*") -Destination $stageRoot -Recurse -Force

Get-ChildItem -LiteralPath $stageRoot -Recurse -File |
    Where-Object { $_.Extension -in @(".pdb", ".ilk", ".exp", ".lib", ".appxsym") } |
    Remove-Item -Force

Copy-VCRuntime -Destination $stageRoot

$releaseInfo = [ordered]@{
    name = $appName
    version = $appVersion
    configuration = $Configuration
    platform = $Platform
    builtAtUtc = (Get-Date).ToUniversalTime().ToString("o")
    entryPoint = $exeName
}

$releaseInfo | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $stageRoot "release-info.json") -Encoding UTF8

if (Test-Path -LiteralPath $payloadZip) {
    Remove-Item -LiteralPath $payloadZip -Force
}

Compress-Archive -Path (Join-Path $stageRoot "*") -DestinationPath $payloadZip -Force

$installerWork = Join-Path $packageWorkRoot "installer"
$portableWork = Join-Path $packageWorkRoot "portable"
Write-InstallerPayload -TargetDir $installerWork
Write-PortablePayload -TargetDir $portableWork

New-NativeSelfExtractPackage -Name "Light Host Modern Setup" -WorkDir $installerWork -PayloadZipPath (Join-Path $installerWork "payload.zip") -EntryScriptPath (Join-Path $installerWork "install.ps1") -TargetExe $installerExe -IconPath $releaseIcon -InstallerUi
New-NativeSelfExtractPackage -Name "Light Host Modern Portable" -WorkDir $portableWork -PayloadZipPath (Join-Path $portableWork "payload.zip") -EntryScriptPath (Join-Path $portableWork "run-portable.ps1") -TargetExe $portableExe -IconPath $releaseIcon

if (!$KeepStage) {
    Remove-Item -LiteralPath $stageRoot -Recurse -Force
    Remove-Item -LiteralPath $packageWorkRoot -Recurse -Force
    Remove-Item -LiteralPath $payloadZip -Force
}

Write-Host ""
Write-Host "Release artifacts created:"
Write-Host "  Installer: $installerExe"
Write-Host "  Portable:  $portableExe"
