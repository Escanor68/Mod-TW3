#!/usr/bin/env bash
# Shared JDK 21+ gate for server scripts (R2).
JAVA_HOME="${JAVA_HOME:-$HOME/.local/jdk/jdk-21.0.13+9}"
export JAVA_HOME
if [[ ! -x "$JAVA_HOME/bin/javac" ]]; then
  echo "Need JDK 21+ (javac missing). Set JAVA_HOME (tried: $JAVA_HOME)" >&2
  echo "See server/README.md and docs/hosting/dedicated-hosting.md" >&2
  exit 1
fi
ver="$("$JAVA_HOME/bin/javac" -version 2>&1 | sed -n 's/.*javac \([0-9][0-9]*\).*/\1/p')"
if [[ -z "$ver" || "$ver" -lt 21 ]]; then
  echo "Need JDK 21+ (found: ${ver:-unknown} at $JAVA_HOME)" >&2
  exit 1
fi
