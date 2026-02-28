#!/usr/bin/env bash
set -euo pipefail

SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/unihd_flipper_app"
DEST_DIR="${1:-../unihd-card-flipper-app}"

rm -rf "$DEST_DIR"
mkdir -p "$DEST_DIR"
cp -R "$SRC_DIR"/. "$DEST_DIR"/

cd "$DEST_DIR"
git init -q

git add .
git commit -m "Initial UniHD Flipper app import" >/dev/null

echo "Created new repo at: $DEST_DIR"
