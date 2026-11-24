# backup — Arch dwm/st desktop

Рабочий стек с исходной машины: **X11 + dwm + st + greetd/tuigreet**, трей, PipeWire, тёмная Materia, turbo off на батарее.

Агенту: иди по шагам ниже сверху вниз на свежей Arch. Не убивай живой Throne/TUN без спроса. Не `push --force`.

Репозиторий **public** — SSH-ключ не нужен:

```bash
git clone https://github.com/homestuck-ng/backup.git
cd backup
./install.sh          # пакеты + сборка + конфиги + greetd + cpu-ac-switch
# потом: обои, опционально Throne/Cursor, reboot
```

Или руками — тот же порядок, что в `install.sh`.

---

## Цель: что должно получиться

| Слой | Как на исходной системе |
|------|-------------------------|
| Логин | `greetd` → `tuigreet-clean` → `startx` |
| WM | `~/.local/bin/dwm` в цикле из `~/.xinitrc` (soft restart: Super+Shift+q) |
| Терминал | `~/.local/bin/st` (шрифт JetBrainsMono Nerd Font Mono) |
| Трей | dwm systray + `snixembed` + `nm-applet` + `blueman-applet` (`dwm-tray`) |
| Уведомления | `dunst` + `dunst-tg-raise` |
| Клипборд | `clipd` (clipnotify + xclip) |
| Тема | Materia-dark / Papirus-Dark |
| Питание | `cpu-ac-switch`: turbo off на батарее, on на AC |
| Лок | `slock` (Super+Shift+L), без xss-lock |

---

## 0. Предусловия

- Arch Linux, интернет, пользователь с `sudo`
- Для сборки suckless: `base-devel` (есть в `packages.txt`)
- Желательно сразу NetworkManager:

```bash
sudo systemctl enable --now NetworkManager
```

---

## 1. Пакеты из официальных репозиториев

Список полный и актуальный: **`packages.txt`**.

```bash
# конфликты PipeWire (оставляем pipewire-*, снимаем старый стек):
sudo pacman -Rdd --noconfirm pulseaudio pulseaudio-bluetooth jack2 jack 2>/dev/null || true

sudo pacman -Syu --needed - < packages.txt
# или: ./install.sh  (сам снимает pulseaudio / jack2)
```

Типовые конфликты Arch: `pipewire-pulse` ↔ `pulseaudio`, `pipewire-jack` ↔ `jack2`/`jack`. Удаляем старое (`-Rdd`), ставим PipeWire.
Критичное, без чего «не как тут»:

| Пакет | Зачем |
|-------|--------|
| `xorg-server` `xorg-xinit` + xorg-утилиты | сессия X11 |
| `libx11` `libxft` `libxinerama` `freetype2` `harfbuzz` | сборка dwm/st |
| `dmenu` `dunst` `feh` `slock` | меню / тосты / обои / лок |
| `snixembed` | SNI → XEmbed (Telegram/Throne в трее dwm) |
| `clipnotify` `xclip` | журнал буфера (`clipd`) |
| `pipewire` `pipewire-pulse` `wireplumber` | звук |
| `network-manager-applet` `blueman` | иконки Wi‑Fi / BT |
| `materia-gtk-theme` `papirus-icon-theme` | GTK как в xinitrc |
| `ttf-jetbrains-mono-nerd` | шрифт в `dwm/config.h` и `st/config.h` |
| `greetd` `greetd-tuigreet` | логин без SDDM/GDM |
| `gnome-keyring` | секреты GTK-приложений |
| `brightnessctl` | яркость + `set-75hz` не сбрасывает её |
| `xdotool` | клик по тосту Telegram → raise окна |

AUR почти не нужен. См. `packages-aur.txt` только для **Throne** / **Cursor**.

После шрифтов:

```bash
fc-cache -fv
```

---

## 2. Сборка dwm и st → `~/.local/bin`

Исходники уже с патчами и готовым `config.h` — просто `make`.

```bash
mkdir -p ~/.local/bin
make -C dwm clean all
install -m 755 dwm/dwm ~/.local/bin/dwm
install -m 755 dwm/dwm ~/.local/bin/dwm.bin   # на всякий случай (restart helpers)

make -C st clean all
install -m 755 st/st ~/.local/bin/st
```

`floatterm` / `floatagent` вызывают именно `~/.local/bin/st`.

Патчи лежат в `dwm/patches/` и `st/patches/` (уже применены в дереве; заново патчить не надо).

---

## 3. Хелперы из `bin/`

```bash
install -m 755 bin/* ~/.local/bin/
```

| Скрипт | Роль |
|--------|------|
| `floatterm` | плавающий центрированный st |
| `floatagent` | то же + Cursor `agent` CLI |
| `dwm-tray` | после старта dwm: snixembed → nm/blueman |
| `dwm-status` | строка статуса |
| `clipd` / `clipmenu-dmenu` | история буфера |
| `dunst-tg-raise` | клик по TG-уведомлению |
| `set-75hz` | 1920×1080@75 + TearFree (AMD; на другом железе может no-op / правка) |
| `dwm-restart` | soft restart helper |

---

## 4. Конфиги сессии

```bash
install -m 755 config/xinitrc ~/.xinitrc
mkdir -p ~/.config/dunst
install -m 644 config/dunst/dunstrc ~/.config/dunst/dunstrc
install -m 644 config/gtkrc-2.0 ~/.gtkrc-2.0

# обои (feh в xinitrc ждёт этот путь)
# ln -sf /path/to/image.jpg ~/.wallpaper
```

`~/.xinitrc` поднимает: тему, dunst, clipd, status, цикл `dwm` + `dwm-tray`.

Throne в xinitrc **закомментирован** — включи строку `throne -tray &` после установки VPN.

---

## 5. Логин: greetd + tuigreet

```bash
sudo install -m 755 config/tuigreet-clean /usr/local/bin/tuigreet-clean
sudo install -m 644 config/greetd-config.toml /etc/greetd/config.toml
sudo systemctl enable greetd
sudo systemctl set-default graphical.target
# отключи конфликтующие дисплей-менеджеры, если были:
# sudo systemctl disable sddm gdm lightdm 2>/dev/null || true
```

После reboot: tuigreet → `startx` → `~/.xinitrc` → dwm.

Быстрый тест без reboot: из TTY `startx` (если уже залогинен консольно).

---

## 6. Батарея: turbo off (как на исходнике)

```bash
sudo install -m 755 scripts/cpu-ac-switch /usr/local/sbin/cpu-ac-switch
sudo install -m 644 scripts/99-cpu-ac-switch.rules /etc/udev/rules.d/
sudo install -m 644 config/systemd/cpu-ac-switch.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now cpu-ac-switch.service
sudo udevadm control --reload
```

Один oneshot + udev, без демона. Проверка: `cat /sys/devices/system/cpu/intel_pstate/no_turbo` (или amd аналог — скрипт сам выбирает).

---

## 7. GRUB (опционально)

```bash
sudo mkdir -p /boot/grub/themes/simple
sudo cp config/grub-theme-simple.txt /boot/grub/themes/simple/theme.txt
```

В `/etc/default/grub`:

```
GRUB_THEME="/boot/grub/themes/simple/theme.txt"
GRUB_TIMEOUT=3
GRUB_COLOR_NORMAL="light-gray/black"
GRUB_COLOR_HIGHLIGHT="white/black"
```

```bash
sudo grub-mkconfig -o /boot/grub/grub.cfg
```

---

## 8. Опционально: как «добить до полной копии»

### Throne (VPN, TUN без root GUI)

1. AUR: `yay -S throne-bin` (или поставь `.AppImage`/пакет как на исходнике в `/opt/Throne`).
2. Caps/setuid **только** на Core, **не** на GUI:
   ```bash
   # пример:
   sudo setcap 'cap_net_admin,cap_net_bind_service=+ep' /opt/Throne/ThroneCore
   ```
3. Раскомментируй в `~/.xinitrc` блок `throne -tray`.
4. `snixembed` уже стартует из `dwm-tray` — нужен для иконки.

### Cursor / agent

- Cursor IDE по желанию.
- CLI: бинарь `agent` → `~/.local/bin/agent` (для Super+Shift+= / `floatagent`).

### Telegram

- Клиент любой; для raise из dunst удобен враппер `~/.local/bin/telegram` (иначе поправь `dunst-tg-raise`).

### Обои / 75 Hz

- `~/.wallpaper` — любой jpg/png.
- `set-75hz` заточен под eDP 1920×1080 + AMD TearFree — на другом ноуте правь под свой `xrandr`.

---

## Хоткеи (кратко)

| Клавиши | Действие |
|---------|----------|
| Super+Enter | st |
| Super+Shift+Enter / Super+\` | floatterm |
| Super+Shift+= | floatagent |
| Super+j / k | фокус в стеке |
| Shift+колёсико или Super+колёсико | то же, циклически |
| Super+d / p | dmenu |
| Super+q | закрыть окно |
| Super+Shift+L | slock |
| Super+Shift+q | soft-restart dwm |

После правок dwm: пересобери → `install` в `~/.local/bin/dwm` (+ `dwm.bin`) → Super+Shift+q.

---

## Промпт для агента (copy-paste)

```
Клонируй https://github.com/homestuck-ng/backup.git (public, без SSH) и воспроизведи desktop как в README.md:
1) pacman -Syu --needed - < packages.txt
2) собери dwm и st в ~/.local/bin (dwm, dwm.bin, st)
3) поставь bin/* в ~/.local/bin
4) ~/.xinitrc, dunst, ~/.gtkrc-2.0 из config/
5) tuigreet-clean в /usr/local/bin + greetd config, enable greetd, graphical.target
6) cpu-ac-switch (script + udev + systemd)
7) fc-cache; обои ~/.wallpaper по желанию
Не force-push. Throne/Cursor — только если попросят. Не рви живой VPN.
```

---

## Не кладётся в git

SSH-ключи, секреты, профиль Firefox, бинарники Cursor/Throne, личные обои, история clipd.
