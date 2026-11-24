# arch-dwm-desktop

Portable Arch + **dwm** / **st** desktop stack (Ilon’s working setup).

Clone → install packages → build dwm/st → install configs/scripts → greetd + startx.

An agent can follow this README end-to-end on a fresh Arch laptop.

## What’s in the repo

| Path | Purpose |
|------|---------|
| `dwm/` | Patched dwm sources + `config.h` (systray, tabs, movestack, floatterm centering, Shift/Super+wheel focusstack, atomic float restack) |
| `st/` | Patched st + `config.h` (scrollback, clipboard, boxdraw/ligatures patches under `st/patches/`) |
| `bin/` | Session helpers: `floatterm`, `floatagent`, `clipd`, `clipmenu-dmenu`, `dwm-status`, `dunst-tg-raise`, `set-75hz`, … |
| `config/xinitrc` | X session: keyboard, dunst, snixembed, nm/blueman, Throne tray, clipd, status, dwm loop |
| `config/dunst/` | Notification daemon config |
| `config/greetd-config.toml` | `tuigreet` → `startx` |
| `config/grub-theme-simple.txt` | Minimal GRUB theme |
| `scripts/cpu-ac-switch` + udev/systemd | Disable CPU turbo on battery, restore on AC |
| `packages.txt` / `packages-aur.txt` | Pacman / AUR lists |

## Quick install (agent checklist)

### 1. Packages

```bash
sudo pacman -Syu --needed - < packages.txt
# AUR (yay or paru), if missing from repos:
#   snixembed  materia-gtk-theme  papirus-icon-theme
# Optional VPN GUI: throne / throne-bin (setcap on ThroneCore only; do NOT setcap the GUI)
```

Also useful (often already pulled in):

- `xorg-xrandr`, `xorg-xset`, `xorg-xsetroot`, `xclip`, `maim`, `brightnessctl`
- fonts: `ttf-jetbrains-mono-nerd` (or change `st`/`dwm` font in `config.h`)
- audio: `pipewire` `pipewire-pulse` `wireplumber`

### 2. Build & install dwm + st

```bash
# dwm → ~/.local/bin/dwm and dwm.bin (xinitrc/restart expects dwm on PATH)
mkdir -p ~/.local/bin
make -C dwm clean all
cp -f dwm/dwm ~/.local/bin/dwm
cp -f dwm/dwm ~/.local/bin/dwm.bin

# st → ~/.local/bin/st  (floatterm hardcodes this path; adjust script if needed)
make -C st clean all
cp -f st/st ~/.local/bin/st
```

Ensure `~/.local/bin` is early on `PATH` (see `config/xinitrc`).

### 3. Helper scripts

```bash
cp -a bin/* ~/.local/bin/
chmod +x ~/.local/bin/*
# floatagent launches:  ~/.local/bin/agent  (Cursor Agent CLI — install separately if desired)
```

### 4. Session configs

```bash
cp config/xinitrc ~/.xinitrc
chmod +x ~/.xinitrc
mkdir -p ~/.config/dunst
cp config/dunst/dunstrc ~/.config/dunst/dunstrc
# optional:
# cp config/Xresources ~/.Xresources
```

Wallpaper: `~/.wallpaper` (feh). Point a symlink at any image, or edit the `feh` line in `xinitrc`.

### 5. Login (greetd + tuigreet)

```bash
sudo cp config/greetd-config.toml /etc/greetd/config.toml
sudo systemctl enable --now greetd
sudo systemctl set-default graphical.target
```

`tuigreet` runs `startx` → `~/.xinitrc` → dwm.

### 6. Battery CPU turbo switch

```bash
sudo cp scripts/cpu-ac-switch /usr/local/sbin/cpu-ac-switch
sudo chmod 755 /usr/local/sbin/cpu-ac-switch
sudo cp scripts/99-cpu-ac-switch.rules /etc/udev/rules.d/
sudo cp config/systemd/cpu-ac-switch.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now cpu-ac-switch.service
sudo udevadm control --reload
```

No background daemon — udev/oneshot only.

### 7. GRUB simple theme (optional)

```bash
sudo mkdir -p /boot/grub/themes/simple
sudo cp config/grub-theme-simple.txt /boot/grub/themes/simple/theme.txt
# In /etc/default/grub set:
#   GRUB_THEME="/boot/grub/themes/simple/theme.txt"
#   GRUB_TIMEOUT=3
#   GRUB_COLOR_NORMAL="light-gray/black"
#   GRUB_COLOR_HIGHLIGHT="white/black"
sudo grub-mkconfig -o /boot/grub/grub.cfg
```

### 8. Lock screen

`slock` is in `packages.txt`. Bind in dwm: **Super+Shift+L**. No `xss-lock` required (manual lock only).

## Key bindings (high level)

| Bind | Action |
|------|--------|
| Super+Enter | `st` |
| Super+Shift+Enter / Super+\` | floating `floatterm` |
| Super+Shift+= / Super++ | floating Cursor `agent` (`floatagent`) |
| Super+j / k | focus stack |
| **Shift+wheel** or **Super+wheel** | same as j/k (cyclic) |
| Super+Shift+L | `slock` |
| Super+Shift+q | soft-restart dwm (`while dwm; do :; done` in xinitrc) |
| Super+d / p | dmenu |
| Super+q | kill client |

## Tray (Throne / Telegram)

dwm has an XEmbed systray. Qt/SNI apps need **`snixembed`** (started in `xinitrc` **before** Throne).

Throne notes (if used):

- Run as user: `throne -tray` (wrapper should exec `/opt/Throne/Throne -appdata`)
- **setuid or caps on `ThroneCore` only**, never on the GUI binary
- Do not kill live TUN carelessly when copying this setup

## Soft dwm restart

`~/.xinitrc` ends with:

```sh
while dwm; do :; done
```

Install new builds to **`~/.local/bin/dwm.bin`** if your restart helper uses that name, and/or `~/.local/bin/dwm`. Then **Super+Shift+q**.

## Agent prompt (copy-paste)

```
On this Arch machine, clone https://github.com/homestuck-ng/arch-dwm-desktop
and follow README.md to reproduce the dwm/st desktop: install packages.txt (+ AUR),
build dwm & st into ~/.local/bin, install bin/ scripts and config/xinitrc + dunst,
enable greetd/tuigreet, install cpu-ac-switch udev/systemd, optional GRUB theme.
Do not force-push; do not kill an existing Throne TUN without asking.
```

## Layout / patches notes

- **dwm**: systray left of status; float window restack via `XRestackWindows`; `floatterm` WM_CLASS centered; Shift/Mod + mouse wheel `focusstack` with EnterNotify skip so focus does not stick.
- **st**: see `st/patches/` for applied diffs; rebuild after editing `config.h`.

## Not included (install yourself)

- Cursor IDE / `cursor-agent` CLI (`~/.local/bin/agent`)
- Throne VPN app + polkit/setcap
- Personal wallpapers, Firefox profile, SSH keys, secrets
