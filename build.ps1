[CmdletBinding()]
param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",
    [switch]$Clean,
    [switch]$Run,
    [switch]$Shared,
    [switch]$Install,
    [string]$Prefix = "",
    [ValidateRange(1, 64)]
    [int]$Jobs = 4
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $MyInvocation.MyCommand.Path
$build = Join-Path $root "build"

if ([string]::IsNullOrWhiteSpace($Prefix)) {
    $Prefix = [Environment]::GetEnvironmentVariable("RIBBONLIB_INSTALL_PREFIX")
}
if ([string]::IsNullOrWhiteSpace($Prefix)) {
    $Prefix = Join-Path $build "install"
}

if ($Clean -and (Test-Path -LiteralPath $build)) {
    Remove-Item -LiteralPath $build -Recurse -Force
}

$configureArgs = @(
    "-S", $root,
    "-B", $build,
    "-A", "x64",
    "-DRIBBONLIB_BUILD_EXAMPLE=ON",
    "-DRIBBONLIB_BUILD_SHARED=$(if ($Shared) { 'ON' } else { 'OFF' })",
    "-DRIBBONLIB_ENABLE_INSTALL=$(if ($Install) { 'ON' } else { 'OFF' })"
)

foreach ($name in @("Qt5_DIR", "Qt6_DIR", "CMAKE_PREFIX_PATH")) {
    $value = [Environment]::GetEnvironmentVariable($name)
    if (-not [string]::IsNullOrWhiteSpace($value)) {
        $configureArgs += "-D${name}=$value"
    }
}

& cmake @configureArgs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

& cmake --build $build --config $Configuration --parallel $Jobs
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if ($Install) {
    & cmake --install $build --config $Configuration --prefix $Prefix
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

if ($Run) {
    $exe = Join-Path $build "example\RibbonExample.exe"
    if (-not (Test-Path -LiteralPath $exe)) {
        $exe = Join-Path $build ("example\" + $Configuration + "\RibbonExample.exe")
    }
    if (-not (Test-Path -LiteralPath $exe)) {
        Write-Error "Example not found: $exe"
        exit 1
    }

    $qtBinDir = [Environment]::GetEnvironmentVariable("QT_BIN_DIR")
    if (-not [string]::IsNullOrWhiteSpace($qtBinDir)) {
        $env:Path = "${qtBinDir};$env:Path"
    }

    & $exe
    exit $LASTEXITCODE
}
