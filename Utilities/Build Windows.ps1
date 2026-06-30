param(
    [ValidateSet("Debug", "Release")]
    [string] $Configuration = "Release",

    [string] $Preset = "windows-vs2022",

    [ValidateSet("AUTO", "ON", "OFF")]
    [string] $EnableVst2 = "AUTO",

[ValidateSet("LEGACY", "XAYMAR")]
[string] $Vst2Provider = "XAYMAR",

    [string] $Vst2SdkDir = ""
)

$ErrorActionPreference = "Stop"

$cmake = Get-Command cmake -ErrorAction SilentlyContinue
$cmakePath = if ($null -ne $cmake) { $cmake.Source } else { Join-Path $env:ProgramFiles "CMake\bin\cmake.exe" }

if (!(Test-Path $cmakePath)) {
    throw "CMake 3.22+ was not found. Install current CMake and Visual Studio Build Tools 2022."
}

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

& $cmakePath @configureArgs
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $cmakePath --build --preset "$Preset-$($Configuration.ToLowerInvariant())"
exit $LASTEXITCODE
