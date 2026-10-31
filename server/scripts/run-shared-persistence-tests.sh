#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
JAVA_HOME="${JAVA_HOME:-$HOME/.local/jdk/jdk-21.0.13+9}"
OUT="$ROOT/out-test"
mkdir -p "$OUT"
"$JAVA_HOME/bin/javac" --release 21 -d "$OUT" \
  "$ROOT/src/SharedPersistenceRules.java" \
  "$ROOT/test/SharedPersistenceRulesTest.java"
"$JAVA_HOME/bin/java" -cp "$OUT" SharedPersistenceRulesTest
