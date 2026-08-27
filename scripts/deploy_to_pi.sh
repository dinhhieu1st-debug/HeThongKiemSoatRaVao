#!/usr/bin/env bash

set -euo pipefail

THU_MUC_SCRIPT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
THU_MUC_PROJECT="$(cd "$THU_MUC_SCRIPT/.." && pwd)"
FILE_CHAY="$THU_MUC_PROJECT/build-pi/Hethongkiemsoatravao"

PI_USER="pi"
PI_IP="192.168.137.227"
THU_MUC_PI="/home/pi/he-thong-kiem-soat-ra-vao"

echo "Dang bien dich chuong trinh ARM64..."

cmake --build "$THU_MUC_PROJECT/build-pi" --parallel

echo "Dang kiem tra kien truc file..."

THONG_TIN_FILE="$(file "$FILE_CHAY")"
echo "$THONG_TIN_FILE"

if [[ "$THONG_TIN_FILE" != *"ARM aarch64"* ]]; then
    echo "Dung trien khai: file khong phai ARM aarch64." >&2
    exit 1
fi

echo "Dang chep chuong trinh sang Raspberry Pi..."

scp "$FILE_CHAY" \
    "$PI_USER@$PI_IP:$THU_MUC_PI/Hethongkiemsoatravao.new"

echo "Dang thay ban moi va khoi dong lai tren Raspberry Pi..."

ssh "$PI_USER@$PI_IP" \
    "pkill -f '^$THU_MUC_PI/Hethongkiemsoatravao$' || true; \
     sleep 1; \
     install -m 755 '$THU_MUC_PI/Hethongkiemsoatravao.new' '$THU_MUC_PI/Hethongkiemsoatravao'; \
     rm -f '$THU_MUC_PI/Hethongkiemsoatravao.new'; \
     setsid -f env \
       XDG_SESSION_TYPE=wayland \
       WAYLAND_DISPLAY=wayland-0 \
       XDG_RUNTIME_DIR=/run/user/1000 \
       DISPLAY=:0 \
       XAUTHORITY=/home/pi/.Xauthority \
       DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus \
       QT_QPA_PLATFORM=xcb \
       QT_PLUGIN_PATH=/usr/local/qt6/plugins \
       LD_LIBRARY_PATH=/usr/local/qt6/lib:/usr/local/lib \
       '$THU_MUC_PI/Hethongkiemsoatravao' \
       </dev/null >/home/pi/hethongkiemsoatravao.log 2>&1"

echo "Trien khai sang Raspberry Pi hoan tat."
