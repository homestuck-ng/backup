#!/usr/bin/env bash
# Полная установка стека с этой машины на свежую Arch.
# Запуск из корня репозитория: ./install.sh
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
BIN="${HOME}/.local/bin"
mkdir -p "$BIN" "${HOME}/.config/dunst"

echo "==> 1/7 pacman (packages.txt)"
# pipewire-pulse conflicts with pulseaudio — drop PulseAudio stack first if present
if pacman -Qq pulseaudio >/dev/null 2>&1; then
  echo "    removing pulseaudio (conflicts with pipewire-pulse)…"
  # -Rdd: drop even if something still depends on it; PipeWire replaces the stack
  sudo pacman -Rdd --noconfirm \
    pulseaudio pulseaudio-bluetooth pulseaudio-equalizer pulseaudio-jack \
    pulseaudio-lirc pulseaudio-rtp pulseaudio-zeroconf 2>/dev/null || true
fi
# strip comments/blank lines for pacman stdin
grep -vE '^\s*(#|$)' "$ROOT/packages.txt" | sudo pacman -Syu --needed --noconfirm -

echo "==> 2/7 build dwm → $BIN/dwm + dwm.bin"
make -C "$ROOT/dwm" clean all
install -m 755 "$ROOT/dwm/dwm" "$BIN/dwm"
install -m 755 "$ROOT/dwm/dwm" "$BIN/dwm.bin"

echo "==> 3/7 build st → $BIN/st"
make -C "$ROOT/st" clean all
install -m 755 "$ROOT/st/st" "$BIN/st"

echo "==> 4/7 helpers → $BIN"
install -m 755 "$ROOT"/bin/* "$BIN/"

echo "==> 5/7 session configs"
install -m 755 "$ROOT/config/xinitrc" "${HOME}/.xinitrc"
install -m 644 "$ROOT/config/dunst/dunstrc" "${HOME}/.config/dunst/dunstrc"
install -m 644 "$ROOT/config/gtkrc-2.0" "${HOME}/.gtkrc-2.0"
fc-cache -fv >/dev/null 2>&1 || true

echo "==> 6/7 greetd + tuigreet-clean"
sudo install -m 755 "$ROOT/config/tuigreet-clean" /usr/local/bin/tuigreet-clean
sudo install -m 644 "$ROOT/config/greetd-config.toml" /etc/greetd/config.toml
sudo systemctl enable greetd
sudo systemctl set-default graphical.target
sudo systemctl disable sddm gdm lightdm 2>/dev/null || true

echo "==> 7/7 cpu-ac-switch (turbo off on battery)"
sudo install -m 755 "$ROOT/scripts/cpu-ac-switch" /usr/local/sbin/cpu-ac-switch
sudo install -m 644 "$ROOT/scripts/99-cpu-ac-switch.rules" /etc/udev/rules.d/99-cpu-ac-switch.rules
sudo install -m 644 "$ROOT/config/systemd/cpu-ac-switch.service" /etc/systemd/system/cpu-ac-switch.service
sudo systemctl daemon-reload
sudo systemctl enable --now cpu-ac-switch.service
sudo udevadm control --reload

echo
echo "Готово. Дальше:"
echo "  • ln -sf /path/to/wall.jpg ~/.wallpaper"
echo "  • опционально Throne/Cursor — см. README §8"
echo "  • reboot (или startx из TTY)"
echo "PATH: $BIN уже в начале ~/.xinitrc"
