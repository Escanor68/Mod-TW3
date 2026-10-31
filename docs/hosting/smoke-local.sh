#!/usr/bin/env bash
# Local smoke for F0 dedicated-hosting: config shape + UDP bind probe.
# Does NOT prove a TW3 client join (needs game + second peer).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CONFIG="$ROOT/witcher/bin/WitcherOnline/config.xml"
PORT="${PORT:-40000}"
FAIL=0

echo "== F0 local smoke =="
echo "repo: $ROOT"

if [[ ! -f "$CONFIG" ]]; then
  echo "FAIL: missing $CONFIG"
  FAIL=1
else
  echo "OK: config present: $CONFIG"
  if grep -q '<ServerIP>' "$CONFIG" && grep -q '<Port>' "$CONFIG"; then
    echo "OK: ServerIP and Port elements found"
  else
    echo "FAIL: config missing ServerIP or Port"
    FAIL=1
  fi
fi

if command -v javac >/dev/null 2>&1; then
  JV="$(javac -version 2>&1 | awk '{print $2}')"
  MAJOR="${JV%%.*}"
  if [[ "$MAJOR" =~ ^[0-9]+$ ]] && (( MAJOR >= 21 )); then
    echo "OK: javac $JV (>= 21)"
  else
    echo "WARN: javac $JV — need JDK 21+ to build server/ from source"
  fi
else
  echo "WARN: javac not found"
fi

python3 - "$PORT" <<'PY' || FAIL=1
import socket, sys
port = int(sys.argv[1])
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
try:
    s.bind(("0.0.0.0", port))
    print(f"OK: UDP bind 0.0.0.0:{port} succeeded (released after probe)")
except OSError as e:
    print(f"FAIL: cannot bind UDP {port}: {e}")
    sys.exit(1)
finally:
    s.close()
PY

echo "== checklist (manual / peer) =="
echo "- [ ] Start WitcherServer.jar; log shows listen on :$PORT"
echo "- [ ] Peer sets ServerIP to Tailscale or VPS IP"
echo "- [ ] Two unique usernames join from different networks"

if (( FAIL != 0 )); then
  echo "RESULT: FAIL"
  exit 1
fi
echo "RESULT: PASS (local probes only)"
