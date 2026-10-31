#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=require-jdk21.sh
source "$(dirname "$0")/require-jdk21.sh"
OUT="$ROOT/out-test"
mkdir -p "$OUT"
"$JAVA_HOME/bin/javac" --release 21 -d "$OUT" \
  "$ROOT/src/PacketAuthRules.java" \
  "$ROOT/test/PacketAuthRulesTest.java"
"$JAVA_HOME/bin/java" -cp "$OUT" PacketAuthRulesTest
