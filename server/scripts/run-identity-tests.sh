#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
JAVA_HOME="${JAVA_HOME:-$HOME/.local/jdk/jdk-21.0.13+9}"
if [[ ! -x "$JAVA_HOME/bin/javac" ]]; then
  echo "Need JDK 21+. Set JAVA_HOME (tried $JAVA_HOME)" >&2
  exit 1
fi
OUT="$ROOT/out-test"
rm -rf "$OUT"
mkdir -p "$OUT"
"$JAVA_HOME/bin/javac" --release 21 -d "$OUT" \
  "$ROOT/src/SessionIdentityRules.java" \
  "$ROOT/test/SessionIdentityRulesTest.java"
"$JAVA_HOME/bin/java" -cp "$OUT" SessionIdentityRulesTest
