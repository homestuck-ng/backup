#!/usr/bin/env bash
# Полная установка стека с этой машины на свежую Arch.
# Запуск из корня репозитория: ./install.sh
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
BIN="${HOME}/.local/bin"
mkdir -p "$BIN" "${HOME}/.config/dunst"

echo "==> 1/7 pacman (packages.txt)"
# PipeWire replaces PulseAudio + JACK — drop conflicting packages first
conflict_rm=()
pacman -Qq pulseaudio >/dev/null 2>&1 && conflict_rm+=(pulseaudio pulseaudio-bluetooth pulseaudio-equalizer pulseaudio-jack pulseaudio-lirc pulseaudio-rtp pulseaudio-zeroconf)
pacman -Qq jack2 >/dev/null 2>&1 && conflict_rm+=(jack2)
pacman -Qq jack >/dev/null 2>&1 && conflict_rm+=(jack)
if ((${#conflict_rm[@]})); then
  echo "    removing conflicts: ${conflict_rm[*]}"
  sudo pacman -Rdd --noconfirm "${conflict_rm[@]}" 2>/dev/null || true
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

echo "==> 5/7 session configs + wallpaper"
install -m 755 "$ROOT/config/xinitrc" "${HOME}/.xinitrc"
install -m 644 "$ROOT/config/dunst/dunstrc" "${HOME}/.config/dunst/dunstrc"
install -m 644 "$ROOT/config/gtkrc-2.0" "${HOME}/.gtkrc-2.0"
mkdir -p "${HOME}/Pictures"
install -m 644 "$ROOT/wallpaper/cat_black_white_artwork.png" \
  "${HOME}/Pictures/cat_black_white_artwork.png"
ln -sfn "${HOME}/Pictures/cat_black_white_artwork.png" "${HOME}/.wallpaper"
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
echo "Готово. Обои → ~/.wallpaper. Опционально Throne/Cursor — README §8."
echo "reboot или startx из TTY. PATH: $BIN уже в ~/.xinitrc"
