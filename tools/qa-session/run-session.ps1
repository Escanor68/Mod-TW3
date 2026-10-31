# Mod-TW3 QA session runner (Windows PowerShell).
# Usage: powershell -ExecutionPolicy Bypass -File tools\qa-session\run-session.ps1 [host|client|report]
param(
    [ValidateSet("host", "client", "report")]
    [string]$Mode = "host"
)

$ErrorActionPreference = "Stop"
$Root = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
$Port = if ($env:PORT) { $env:PORT } else { "40000" }
$ServerIp = if ($env:SERVER_IP) { $env:SERVER_IP } else { "" }
$UsernameValue = if ($env:USERNAME_OVERRIDE) { $env:USERNAME_OVERRIDE } elseif ($env:WO_USERNAME) { $env:WO_USERNAME } else { $env:USERNAME }
$Stamp = (Get-Date).ToUniversalTime().ToString("yyyyMMddTHHmmssZ")
$HostShort = $env:COMPUTERNAME
if (-not $HostShort) { $HostShort = "host" }
$SessionDir = if ($env:SESSION_DIR) { $env:SESSION_DIR } else { Join-Path $Root "qa-sessions\$Stamp-$Mode-$HostShort" }
$Config = Join-Path $Root "witcher\bin\WitcherOnline\config.xml"
$ServerDir = Join-Path $Root "server"
$Jar = Join-Path $ServerDir "out\WitcherServer.jar"
$PropsExample = Join-Path $ServerDir "server.properties.example"
$PropsOut = Join-Path $ServerDir "out\server.properties"
$Log = Join-Path $SessionDir "session.log"
$Report = Join-Path $SessionDir "REPORT.md"
$Notes = Join-Path $SessionDir "NOTES.md"
$EnvFile = Join-Path $SessionDir "environment.txt"
$ServerLog = Join-Path $SessionDir "server.log"

New-Item -ItemType Directory -Force -Path $SessionDir | Out-Null

function Write-Log([string]$Message) {
    $line = "[{0:HH:mm:ss}] {1}" -f (Get-Date).ToUniversalTime(), $Message
    Add-Content -Path $Log -Value $line
    Write-Host $line
}

function Get-JavaBin {
    if ($env:JAVA_HOME -and (Test-Path (Join-Path $env:JAVA_HOME "bin\java.exe"))) {
        return (Join-Path $env:JAVA_HOME "bin\java.exe")
    }
    $cmd = Get-Command java -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    return $null
}

function Get-JavacBin {
    if ($env:JAVA_HOME -and (Test-Path (Join-Path $env:JAVA_HOME "bin\javac.exe"))) {
        return (Join-Path $env:JAVA_HOME "bin\javac.exe")
    }
    $cmd = Get-Command javac -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    return $null
}

function Write-EnvironmentFile {
    $gitHead = ""
    $gitBranch = ""
    $gitStatus = ""
    if (Get-Command git -ErrorAction SilentlyContinue) {
        Push-Location $Root
        try {
            $gitHead = (git rev-parse --short HEAD 2>$null)
            $gitBranch = (git rev-parse --abbrev-ref HEAD 2>$null)
            $gitStatus = (git status -sb 2>$null | Out-String)
        } finally { Pop-Location }
    }
    $javaBin = Get-JavaBin
    $javaVer = if ($javaBin) { & $javaBin -version 2>&1 | Select-Object -First 1 | Out-String } else { "MISSING" }
    $tsIp = ""
    if (Get-Command tailscale -ErrorAction SilentlyContinue) {
        $tsIp = (tailscale ip -4 2>$null | Out-String).Trim()
    }
    @"
timestamp_utc=$Stamp
mode=$Mode
repo=$Root
hostname=$HostShort
os=$([System.Environment]::OSVersion.VersionString)
user=$UsernameValue
git_head=$gitHead
git_branch=$gitBranch
git_status_short<<EOF
$gitStatus
EOF
java_version=$($javaVer.Trim())
JAVA_HOME=$($env:JAVA_HOME)
tailscale_ip4=$tsIp
port=$Port
server_ip_override=$ServerIp
config=$Config
"@ | Set-Content -Path $EnvFile -Encoding UTF8
}

function Ensure-Notes {
    if (-not (Test-Path $Notes)) {
        @"
# Session notes (fill while playing)

- [ ] Two unique usernames joined
- [ ] Could see each other / move sync
- [ ] Party time/weather/dialog (if tested)
- [ ] Combat HIT/HITACK (if tested)
- [ ] Persistence crowns/bag (if tested)
- [ ] F7 ROLECLAIM / STORYDONE (if tested)

## What worked


## What failed (exact error / screenshot note)


## Steps to reproduce


"@ | Set-Content -Path $Notes -Encoding UTF8
    }
}

function Redact-Props([string]$Path) {
    if (-not (Test-Path $Path)) { return "(missing)" }
    (Get-Content $Path) -replace '^(authToken=).*', '$1***REDACTED***' | Out-String
}

function Write-ReportFile {
    $endStamp = (Get-Date).ToUniversalTime().ToString("yyyyMMddTHHmmssZ")
    $serverTail = if (Test-Path $ServerLog) { Get-Content $ServerLog -Tail 80 | Out-String } else { "(no server.log)" }
    $runnerTail = if (Test-Path $Log) { Get-Content $Log -Tail 40 | Out-String } else { "" }
    $cfg = if (Test-Path $Config) {
        ((Get-Content $Config -Raw) -replace '(?s)(<AuthToken>)[^<]*(</AuthToken>)', '$1***REDACTED***$2')
    } else { "(missing config.xml)" }
    $notesBody = if (Test-Path $Notes) { Get-Content $Notes -Raw } else { "" }
    $envBody = Get-Content $EnvFile -Raw
    $propsBody = Redact-Props $PropsOut

    @"
# QA session REPORT

- Started (UTC): ``$Stamp``
- Ended (UTC): ``$endStamp``
- Mode: ``$Mode``
- Session dir: ``$SessionDir``

## Environment

``````
$envBody
``````

## Server properties (redacted)

``````
$propsBody
``````

## Client config snapshot

``````xml
$cfg
``````

## Server log (last 80 lines)

``````
$serverTail
``````

## Runner log (last 40 lines)

``````
$runnerTail
``````

## Operator notes

$notesBody

## Bring-back

Zip this folder and share it (do not commit ``qa-sessions/``).
"@ | Set-Content -Path $Report -Encoding UTF8
    Write-Log "Wrote $Report"
}

function Build-ServerIfNeeded {
    $javaBin = Get-JavaBin
    if (-not $javaBin) {
        Write-Log "ERROR: Java not found. Install JDK 21+ and retry."
        exit 1
    }
    $outDir = Join-Path $ServerDir "out"
    New-Item -ItemType Directory -Force -Path $outDir | Out-Null
    if (-not (Test-Path $Jar)) {
        $javac = Get-JavacBin
        if (-not $javac) {
            Write-Log "ERROR: javac not found (need JDK 21+ to build)."
            exit 1
        }
        Write-Log "Building WitcherServer.jar …"
        $sources = Get-ChildItem (Join-Path $ServerDir "src\*.java") | ForEach-Object { $_.FullName }
        & $javac --release 21 -d $outDir @sources
        $jarCmd = Get-Command jar -ErrorAction SilentlyContinue
        if (-not $jarCmd -and $env:JAVA_HOME) {
            $jarExe = Join-Path $env:JAVA_HOME "bin\jar.exe"
        } else {
            $jarExe = $jarCmd.Source
        }
        & $jarExe cfe $Jar WitcherServer -C $outDir .
        Write-Log "Built $Jar"
    } else {
        Write-Log "Using existing $Jar"
    }
    if (-not (Test-Path $PropsOut) -and (Test-Path $PropsExample)) {
        Copy-Item $PropsExample $PropsOut
        Write-Log "Copied server.properties.example → out\server.properties"
    }
}

function Patch-ClientConfig {
    $ip = $ServerIp
    if (-not $ip -and (Get-Command tailscale -ErrorAction SilentlyContinue)) {
        $ip = (tailscale ip -4 2>$null | Out-String).Trim()
    }
    if (-not $ip) {
        Write-Log "ERROR: set SERVER_IP=... (host Tailscale or VPS IP) for client mode"
        exit 1
    }
    $user = $UsernameValue
    if (-not $user) { $user = "Player" }
    $cfgDir = Split-Path $Config -Parent
    New-Item -ItemType Directory -Force -Path $cfgDir | Out-Null
    if (-not (Test-Path $Config)) {
        @"
<?xml version="1.0"?>
<Config>
	<Username>$user</Username>
	<ServerIP>$ip</ServerIP>
	<Port>$Port</Port>
</Config>
"@ | Set-Content -Path $Config -Encoding UTF8
        Write-Log "Created $Config"
    } else {
        Copy-Item $Config (Join-Path $SessionDir "config.xml.bak") -Force
        [xml]$xml = Get-Content $Config
        if (-not $xml.Config) { throw "Invalid config.xml root" }
        $xml.Config.Username = $user
        $xml.Config.ServerIP = $ip
        $xml.Config.Port = "$Port"
        $xml.Save($Config)
        Write-Log "Patched config Username=$user ServerIP=$ip Port=$Port"
    }
}

function Invoke-Host {
    Write-EnvironmentFile
    Ensure-Notes
    Build-ServerIfNeeded
    $javaBin = Get-JavaBin
    Write-Log "Starting WitcherServer on UDP $Port …"
    $outDir = Join-Path $ServerDir "out"
    $proc = Start-Process -FilePath $javaBin -ArgumentList @("-jar", "WitcherServer.jar", "$Port") `
        -WorkingDirectory $outDir -RedirectStandardOutput $ServerLog -RedirectStandardError $ServerLog `
        -PassThru -NoNewWindow
    Write-Log "Server pid=$($proc.Id) — console: $ServerLog"
    $tipIp = $ServerIp
    if (-not $tipIp -and (Get-Command tailscale -ErrorAction SilentlyContinue)) {
        $tipIp = (tailscale ip -4 2>$null | Out-String).Trim()
    }
    if (-not $tipIp) { $tipIp = "<your Tailscale or VPS IP>" }

    Write-Host ""
    Write-Host "======== HOST READY ========"
    Write-Host "Session: $SessionDir"
    Write-Host "Friend should set ServerIP to: $tipIp"
    Write-Host "Port: $Port"
    Write-Host "Friend (PowerShell):"
    Write-Host "  `$env:SERVER_IP='$tipIp'; `$env:USERNAME_OVERRIDE='FriendName'"
    Write-Host "  powershell -ExecutionPolicy Bypass -File tools\qa-session\run-session.ps1 client"
    Write-Host "Fill NOTES.md while playing. Press Enter to stop server + write REPORT.md"
    Write-Host "============================"
    Write-Host ""
    try {
        [void](Read-Host)
    } finally {
        if (-not $proc.HasExited) {
            Write-Log "Stopping server pid=$($proc.Id)"
            Stop-Process -Id $proc.Id -Force -ErrorAction SilentlyContinue
        }
        Write-ReportFile
    }
}

function Invoke-Client {
    Write-EnvironmentFile
    Ensure-Notes
    Patch-ClientConfig
    @"
# Client checklist

1. Confirm config.xml ServerIP/Port/Username (see REPORT).
2. Launch TW3 Next Gen 4.04+ with Witcher Online mods.
3. Load a save; watch join on host server.log.
4. Fill NOTES.md; re-run report:
   `$env:SESSION_DIR='$SessionDir'; powershell -ExecutionPolicy Bypass -File tools\qa-session\run-session.ps1 report`
"@ | Set-Content -Path (Join-Path $SessionDir "CLIENT_CHECKLIST.md") -Encoding UTF8
    Write-ReportFile
    Write-Host ""
    Write-Host "======== CLIENT READY ========"
    Write-Host "Session: $SessionDir"
    Write-Host "Start The Witcher 3, then update NOTES.md and zip the session folder."
    Write-Host "=============================="
}

function Invoke-ReportOnly {
    if (-not (Test-Path $SessionDir)) {
        throw "SESSION_DIR does not exist: $SessionDir"
    }
    Write-EnvironmentFile
    Ensure-Notes
    Write-ReportFile
}

switch ($Mode) {
    "host" { Invoke-Host }
    "client" { Invoke-Client }
    "report" { Invoke-ReportOnly }
}
