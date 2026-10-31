#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
# shellcheck source=require-jdk21.sh
source "$(dirname "$0")/require-jdk21.sh"
OUT="$ROOT/out-test"
rm -rf "$OUT"
mkdir -p "$OUT"
"$JAVA_HOME/bin/javac" --release 21 -d "$OUT" \
  "$ROOT/src/SessionIdentityRules.java" \
  "$ROOT/test/SessionIdentityRulesTest.java"
"$JAVA_HOME/bin/java" -cp "$OUT" SessionIdentityRulesTest
