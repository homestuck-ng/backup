#!/usr/bin/env bash
# Install this desktop stack on Arch. Run from the repo root.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")" && pwd)"
BIN="${HOME}/.local/bin"
mkdir -p "$BIN" "${HOME}/.config/dunst"

echo "==> pacman packages"
sudo pacman -Syu --needed --noconfirm - < "$ROOT/packages.txt" || {
  echo "Note: some names may be AUR-only (e.g. snixembed). Install those with yay/paru."
  true
}

if command -v yay >/dev/null 2>&1; then
  echo "==> AUR (yay)"
  # strip comments/blank for yay
  mapfile -t aur < <(grep -vE '^\s*(#|$)' "$ROOT/packages-aur.txt" | tr -s ' ' '\n' | grep -vE '^\s*(#|$)' || true)
  [[ ${#aur[@]} -gt 0 ]] && yay -S --needed --noconfirm "${aur[@]}" || true
elif command -v paru >/dev/null 2>&1; then
  echo "==> AUR (paru)"
  mapfile -t aur < <(grep -vE '^\s*(#|$)' "$ROOT/packages-aur.txt" | tr -s ' ' '\n' | grep -vE '^\s*(#|$)' || true)
  [[ ${#aur[@]} -gt 0 ]] && paru -S --needed --noconfirm "${aur[@]}" || true
else
  echo "No yay/paru — install AUR pkgs from packages-aur.txt manually (esp. snixembed)."
fi

echo "==> build dwm → $BIN/dwm + dwm.bin"
make -C "$ROOT/dwm" clean all
install -m 755 "$ROOT/dwm/dwm" "$BIN/dwm"
install -m 755 "$ROOT/dwm/dwm" "$BIN/dwm.bin"

echo "==> build st → $BIN/st"
make -C "$ROOT/st" clean all
install -m 755 "$ROOT/st/st" "$BIN/st"

echo "==> helper scripts"
install -m 755 "$ROOT"/bin/* "$BIN/"

echo "==> xinitrc + dunst"
install -m 755 "$ROOT/config/xinitrc" "${HOME}/.xinitrc"
install -m 644 "$ROOT/config/dunst/dunstrc" "${HOME}/.config/dunst/dunstrc"

echo "==> greetd (tuigreet → startx)"
sudo install -m 644 "$ROOT/config/greetd-config.toml" /etc/greetd/config.toml
sudo systemctl enable greetd
sudo systemctl set-default graphical.target

echo "==> cpu-ac-switch (turbo off on battery)"
sudo install -m 755 "$ROOT/scripts/cpu-ac-switch" /usr/local/sbin/cpu-ac-switch
sudo install -m 644 "$ROOT/scripts/99-cpu-ac-switch.rules" /etc/udev/rules.d/99-cpu-ac-switch.rules
sudo install -m 644 "$ROOT/config/systemd/cpu-ac-switch.service" /etc/systemd/system/cpu-ac-switch.service
sudo systemctl daemon-reload
sudo systemctl enable --now cpu-ac-switch.service
sudo udevadm control --reload

echo "==> done. PATH needs $BIN (xinitrc already prepends it)."
echo "Optional: GRUB theme — see README. Reboot into greetd, or: startx"
