#!/usr/bin/env bash
# Mod-TW3 QA session runner (Linux / macOS bash).
# Usage: ./tools/qa-session/run-session.sh [host|client|report]
set -euo pipefail

MODE="${1:-host}"
ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
PORT="${PORT:-40000}"
SERVER_IP="${SERVER_IP:-}"
USERNAME_VALUE="${USERNAME:-}"
STAMP="$(date -u +%Y%m%dT%H%M%SZ)"
HOST_SHORT="$(hostname -s 2>/dev/null || hostname || echo host)"
SESSION_DIR="${SESSION_DIR:-$ROOT/qa-sessions/${STAMP}-${MODE}-${HOST_SHORT}}"
CONFIG="$ROOT/witcher/bin/WitcherOnline/config.xml"
SERVER_DIR="$ROOT/server"
JAR="$SERVER_DIR/out/WitcherServer.jar"
PROPS_EXAMPLE="$SERVER_DIR/server.properties.example"
PROPS_OUT="$SERVER_DIR/out/server.properties"

mkdir -p "$SESSION_DIR"
LOG="$SESSION_DIR/session.log"
REPORT="$SESSION_DIR/REPORT.md"
NOTES="$SESSION_DIR/NOTES.md"
ENVF="$SESSION_DIR/environment.txt"
SERVER_LOG="$SESSION_DIR/server.log"

log() {
  local line="[$(date -u +%H:%M:%S)] $*"
  echo "$line" | tee -a "$LOG"
}

redact_props() {
  if [[ -f "$1" ]]; then
    sed -E 's/^(authToken=).*/\1***REDACTED***/' "$1"
  fi
}

write_environment() {
  {
    echo "timestamp_utc=$STAMP"
    echo "mode=$MODE"
    echo "repo=$ROOT"
    echo "hostname=$(hostname 2>/dev/null || true)"
    echo "uname=$(uname -a 2>/dev/null || true)"
    echo "user=$(id -un 2>/dev/null || true)"
    if command -v git >/dev/null 2>&1; then
      echo "git_head=$(git -C "$ROOT" rev-parse --short HEAD 2>/dev/null || echo unknown)"
      echo "git_branch=$(git -C "$ROOT" rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)"
      echo "git_status_short<<EOF"
      git -C "$ROOT" status -sb 2>/dev/null || true
      echo "EOF"
    fi
    if [[ -n "${JAVA_HOME:-}" && -x "${JAVA_HOME}/bin/java" ]]; then
      echo "JAVA_HOME=$JAVA_HOME"
      echo "java_version=$("$JAVA_HOME/bin/java" -version 2>&1 | head -n 1)"
    elif command -v java >/dev/null 2>&1; then
      echo "java_version=$(java -version 2>&1 | head -n 1)"
    else
      echo "java_version=MISSING"
    fi
    if command -v tailscale >/dev/null 2>&1; then
      echo "tailscale_ip4=$(tailscale ip -4 2>/dev/null || true)"
    else
      echo "tailscale_ip4="
    fi
    echo "port=$PORT"
    echo "server_ip_override=$SERVER_IP"
    echo "config=$CONFIG"
  } >"$ENVF"
}

ensure_notes() {
  if [[ ! -f "$NOTES" ]]; then
    cat >"$NOTES" <<'EOF'
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


EOF
  fi
}

write_report() {
  local end_stamp
  end_stamp="$(date -u +%Y%m%dT%H%M%SZ)"
  {
    echo "# QA session REPORT"
    echo
    echo "- Started (UTC): \`$STAMP\`"
    echo "- Ended (UTC): \`$end_stamp\`"
    echo "- Mode: \`$MODE\`"
    echo "- Session dir: \`$SESSION_DIR\`"
    echo
    echo "## Environment"
    echo
    echo '```'
    cat "$ENVF"
    echo '```'
    echo
    echo "## Server properties (redacted)"
    echo
    echo '```'
    if [[ -f "$PROPS_OUT" ]]; then
      redact_props "$PROPS_OUT"
    else
      echo "(no server.properties in server/out)"
    fi
    echo '```'
    echo
    echo "## Client config snapshot"
    echo
    echo '```xml'
    if [[ -f "$CONFIG" ]]; then
      # strip AuthToken value if present
      sed -E 's#(<AuthToken>)[^<]*(</AuthToken>)#\1***REDACTED***\2#' "$CONFIG"
    else
      echo "(missing config.xml)"
    fi
    echo '```'
    echo
    echo "## Server log (last 80 lines)"
    echo
    echo '```'
    if [[ -f "$SERVER_LOG" ]]; then
      tail -n 80 "$SERVER_LOG" || true
    else
      echo "(no server.log — client/report mode or server not started)"
    fi
    echo '```'
    echo
    echo "## Runner log (last 40 lines)"
    echo
    echo '```'
    tail -n 40 "$LOG" || true
    echo '```'
    echo
    echo "## Operator notes"
    echo
    if [[ -f "$NOTES" ]]; then
      cat "$NOTES"
    fi
    echo
    echo "## Bring-back"
    echo
    echo "Zip this folder and share it (do not commit \`qa-sessions/\`)."
  } >"$REPORT"
  log "Wrote $REPORT"
}

build_server_if_needed() {
  local java_bin javac_bin
  java_bin="java"
  javac_bin="javac"
  if [[ -n "${JAVA_HOME:-}" ]]; then
    [[ -x "$JAVA_HOME/bin/java" ]] && java_bin="$JAVA_HOME/bin/java"
    [[ -x "$JAVA_HOME/bin/javac" ]] && javac_bin="$JAVA_HOME/bin/javac"
  fi

  if ! command -v "$java_bin" >/dev/null 2>&1 && [[ ! -x "$java_bin" ]]; then
    log "ERROR: Java not found. Install JDK 21+ and retry."
    exit 1
  fi

  mkdir -p "$SERVER_DIR/out"
  if [[ ! -f "$JAR" ]]; then
    log "Building WitcherServer.jar …"
    if [[ ! -x "$javac_bin" ]] && ! command -v "$javac_bin" >/dev/null 2>&1; then
      log "ERROR: javac not found (need JDK 21+ to build)."
      exit 1
    fi
    "$javac_bin" --release 21 -d "$SERVER_DIR/out" "$SERVER_DIR"/src/*.java
    jar cfe "$JAR" WitcherServer -C "$SERVER_DIR/out" .
    log "Built $JAR"
  else
    log "Using existing $JAR"
  fi

  if [[ ! -f "$PROPS_OUT" && -f "$PROPS_EXAMPLE" ]]; then
    cp "$PROPS_EXAMPLE" "$PROPS_OUT"
    log "Copied server.properties.example → out/server.properties"
  fi
}

patch_client_config() {
  local ip user port
  ip="${SERVER_IP}"
  user="${USERNAME_VALUE}"
  port="$PORT"

  if [[ -z "$ip" ]]; then
    if command -v tailscale >/dev/null 2>&1; then
      ip="$(tailscale ip -4 2>/dev/null || true)"
    fi
  fi
  if [[ -z "$ip" ]]; then
    log "ERROR: set SERVER_IP=... (host Tailscale or VPS IP) for client mode"
    exit 1
  fi
  if [[ -z "$user" ]]; then
    user="$(id -un 2>/dev/null || echo Player)"
  fi
  if [[ ! -f "$CONFIG" ]]; then
    mkdir -p "$(dirname "$CONFIG")"
    cat >"$CONFIG" <<EOF
<?xml version="1.0"?>
<Config>
	<Username>${user}</Username>
	<ServerIP>${ip}</ServerIP>
	<Port>${port}</Port>
</Config>
EOF
    log "Created $CONFIG"
  else
    cp "$CONFIG" "$SESSION_DIR/config.xml.bak"
    # Portable-ish XML field replace
    python3 - "$CONFIG" "$user" "$ip" "$port" <<'PY'
import re, sys
path, user, ip, port = sys.argv[1:5]
text = open(path, encoding="utf-8").read()
def repl(tag, val, s):
    if re.search(rf"<{tag}>.*?</{tag}>", s, flags=re.S):
        return re.sub(rf"<{tag}>.*?</{tag}>", f"<{tag}>{val}</{tag}>", s, count=1, flags=re.S)
    return s.replace("</Config>", f"\t<{tag}>{val}</{tag}>\n</Config>")
text = repl("Username", user, text)
text = repl("ServerIP", ip, text)
text = repl("Port", port, text)
open(path, "w", encoding="utf-8").write(text)
print("patched", path)
PY
    log "Patched config Username=$user ServerIP=$ip Port=$port (backup in session dir)"
  fi
}

SERVER_PID=""
cleanup_host() {
  if [[ -n "$SERVER_PID" ]] && kill -0 "$SERVER_PID" 2>/dev/null; then
    log "Stopping server pid=$SERVER_PID"
    kill "$SERVER_PID" 2>/dev/null || true
    wait "$SERVER_PID" 2>/dev/null || true
  fi
  write_report
}

run_host() {
  write_environment
  ensure_notes
  build_server_if_needed
  local java_bin="java"
  [[ -n "${JAVA_HOME:-}" && -x "${JAVA_HOME}/bin/java" ]] && java_bin="$JAVA_HOME/bin/java"

  log "Starting WitcherServer on UDP $PORT …"
  (
    cd "$SERVER_DIR/out"
    "$java_bin" -jar WitcherServer.jar "$PORT"
  ) >>"$SERVER_LOG" 2>&1 &
  SERVER_PID=$!
  log "Server pid=$SERVER_PID — console: $SERVER_LOG"
  trap cleanup_host EXIT INT TERM

  local tip_ip="${SERVER_IP}"
  if [[ -z "$tip_ip" ]] && command -v tailscale >/dev/null 2>&1; then
    tip_ip="$(tailscale ip -4 2>/dev/null || true)"
  fi
  echo
  echo "======== HOST READY ========"
  echo "Session: $SESSION_DIR"
  echo "Friend should set ServerIP to: ${tip_ip:-<your Tailscale or VPS IP>}"
  echo "Port: $PORT"
  echo "Friend command:"
  echo "  SERVER_IP=${tip_ip:-100.x.y.z} USERNAME=FriendName $0 client"
  echo "Fill NOTES.md while playing. Press Enter here to stop server + write REPORT.md"
  echo "============================"
  echo
  # Wait for Enter even if stdin is a pipe
  if [[ -t 0 ]]; then
    read -r _
  else
    log "stdin not a TTY — waiting on server until signal"
    wait "$SERVER_PID" || true
  fi
}

run_client() {
  write_environment
  ensure_notes
  patch_client_config
  {
    echo "# Client checklist"
    echo
    echo "1. Confirm \`config.xml\` ServerIP/Port/Username (see REPORT)."
    echo "2. Launch TW3 Next Gen 4.04+ with Witcher Online mods."
    echo "3. Load a save; watch join on host \`server.log\`."
    echo "4. Fill NOTES.md; re-run: \`SESSION_DIR=$SESSION_DIR $0 report\`"
  } >"$SESSION_DIR/CLIENT_CHECKLIST.md"
  log "Client prep done. Checklist: $SESSION_DIR/CLIENT_CHECKLIST.md"
  write_report
  echo
  echo "======== CLIENT READY ========"
  echo "Session: $SESSION_DIR"
  echo "Start The Witcher 3, then update NOTES.md and zip the session folder."
  echo "=============================="
}

run_report_only() {
  if [[ ! -d "$SESSION_DIR" ]]; then
    echo "SESSION_DIR does not exist: $SESSION_DIR" >&2
    exit 1
  fi
  write_environment
  ensure_notes
  write_report
}

case "$MODE" in
  host) run_host ;;
  client) run_client ;;
  report) run_report_only ;;
  *)
    echo "Usage: $0 [host|client|report]" >&2
    exit 2
    ;;
esac
