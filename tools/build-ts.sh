#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
npx tsc -p tsconfig.json
echo "Built TypeScript presentation/bridge modules into web/v2."
