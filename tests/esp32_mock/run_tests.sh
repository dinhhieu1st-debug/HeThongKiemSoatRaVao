#!/bin/bash
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"

echo "======================================================================"
echo "    [ESP32] TỰ ĐỘNG BIÊN DỊCH VÀ CHẠY UNITY TEST (CẢM BIẾN & KẾT NỐI) "
echo "======================================================================"

make clean
make
echo ""
./run_esp_tests
echo ""
python3 generate_word_report.py

