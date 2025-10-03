# Witcher3-MP Installation Script
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "   Installing Mod in The Witcher 3..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if executable exists
if (-not (Test-Path "build\Release\Witcher3-MP.exe")) {
    Write-Host "ERROR: Server executable not found!" -ForegroundColor Red
    Write-Host "Please run 'witcher3_mp.bat build' first." -ForegroundColor Red
    Read-Host "Press Enter to continue"
    exit 1
}

# Common Steam installation paths
$steamPaths = @(
    "C:\Program Files (x86)\Steam\steamapps\common\The Witcher 3",
    "E:\SteamLibrary\steamapps\common\The Witcher 3",
    "D:\SteamLibrary\steamapps\common\The Witcher 3",
    "F:\SteamLibrary\steamapps\common\The Witcher 3"
)

$tw3Path = $null
foreach ($path in $steamPaths) {
    if (Test-Path $path) {
        $tw3Path = $path
        Write-Host "Found The Witcher 3 at: $path" -ForegroundColor Green
        break
    }
}

if (-not $tw3Path) {
    Write-Host "ERROR: The Witcher 3 installation not found!" -ForegroundColor Red
    Write-Host "Searched locations:" -ForegroundColor Yellow
    foreach ($path in $steamPaths) {
        Write-Host "  - $path" -ForegroundColor Yellow
    }
    Read-Host "Press Enter to continue"
    exit 1
}

$modInstallDir = Join-Path $tw3Path "mods\witcher3_mp"
$configDir = Join-Path $tw3Path "bin\config\r4game\user_config_matrix\pc"

# Create directories
Write-Host "Creating directories..." -ForegroundColor Yellow
if (-not (Test-Path (Join-Path $tw3Path "mods"))) {
    New-Item -ItemType Directory -Path (Join-Path $tw3Path "mods") -Force | Out-Null
}
if (-not (Test-Path $modInstallDir)) {
    New-Item -ItemType Directory -Path $modInstallDir -Force | Out-Null
}
if (-not (Test-Path (Join-Path $modInstallDir "config"))) {
    New-Item -ItemType Directory -Path (Join-Path $modInstallDir "config") -Force | Out-Null
}
if (-not (Test-Path (Join-Path $modInstallDir "scripts\game\witcher3_mp"))) {
    New-Item -ItemType Directory -Path (Join-Path $modInstallDir "scripts\game\witcher3_mp") -Force | Out-Null
}
if (-not (Test-Path $configDir)) {
    New-Item -ItemType Directory -Path $configDir -Force | Out-Null
}

# Copy mod files
Write-Host "Copying mod files..." -ForegroundColor Yellow
Copy-Item "mod\config\mp_config.json" (Join-Path $modInstallDir "config\") -Force
Copy-Item "mod\scripts\game\witcher3_mp\*.ws" (Join-Path $modInstallDir "scripts\game\witcher3_mp\") -Force
Copy-Item "mod\modInfo.ws" $modInstallDir -Force
Copy-Item "build\Release\Witcher3-MP.exe" $modInstallDir -Force

# Copy XML configuration files
Write-Host "Copying XML configuration files..." -ForegroundColor Yellow
Copy-Item "mod\config\witcher3_mp_config.xml" $configDir -Force
Copy-Item "mod\config\filelist.txt" $configDir -Force

# Update game filelists
Write-Host "Updating game filelists..." -ForegroundColor Yellow
$dx11Filelist = Join-Path $configDir "dx11filelist.txt"
$dx12Filelist = Join-Path $configDir "dx12filelist.txt"

# Add to dx11filelist.txt
if (Test-Path $dx11Filelist) {
    Add-Content $dx11Filelist "witcher3_mp_config.xml;"
    Write-Host "Added to dx11filelist.txt" -ForegroundColor Green
} else {
    "witcher3_mp_config.xml;" | Out-File $dx11Filelist -Encoding ASCII
    Write-Host "Created dx11filelist.txt" -ForegroundColor Green
}

# Add to dx12filelist.txt
if (Test-Path $dx12Filelist) {
    Add-Content $dx12Filelist "witcher3_mp_config.xml;"
    Write-Host "Added to dx12filelist.txt" -ForegroundColor Green
} else {
    "witcher3_mp_config.xml;" | Out-File $dx12Filelist -Encoding ASCII
    Write-Host "Created dx12filelist.txt" -ForegroundColor Green
}

Write-Host ""
Write-Host "Installation successful!" -ForegroundColor Green
Write-Host "Mod installed to: $modInstallDir" -ForegroundColor Green
Write-Host "XML config copied to: $configDir" -ForegroundColor Green
Write-Host ""
Write-Host "You can now run: witcher3_mp.bat start" -ForegroundColor Cyan
Write-Host ""
Read-Host "Press Enter to continue"
