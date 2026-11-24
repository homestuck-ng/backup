/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;
static const unsigned int systrayonleft = 1; /* tray left of status text */
static const unsigned int systrayspacing = 1;
static const int systraypinningfailfirst = 1;
static const int showsystray        = 1;
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int showtab            = 0;        /* separate tab bar off — tabs live in main bar */
static const int toptab             = 1;
static const char *fonts[]          = {
	"JetBrainsMono Nerd Font Mono:size=10",
	"DwmFlags:size=11",
};
static const char dmenufont[]       = "JetBrainsMono Nerd Font Mono:size=10";
/* AMOLED: pure black bar, soft white focus */
static const char col_gray1[]       = "#000000";
static const char col_gray2[]       = "#121212";
static const char col_gray3[]       = "#9a9a9a";
static const char col_gray4[]       = "#f2f2f2";
static const char col_accent[]      = "#161616"; /* bar sel bg */
static const char col_border[]      = "#d0d0d0"; /* focused window edge */
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
	[SchemeSel]  = { col_gray4, col_accent, col_border },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* class            instance  title  tags mask  isfloating  monitor */
	{ "Mullvad Browser",        NULL,     NULL,  0,         0,          -1 },
	{ "stalonetray",    NULL,     NULL,  0,         1,          -1 },
	{ "Nm-connection-editor", NULL, NULL, 0,        1,          -1 },
	{ "Pavucontrol",    NULL,     NULL,  0,         1,          -1 },
	{ "st-256color",    "floatterm", NULL, 0,       1,          -1 },
	{ "st",             "floatterm", NULL, 0,       1,          -1 },
	{ "TelegramDesktop", NULL,       NULL, 0,       1,          -1 },
};

/* layout(s) */
static const float mfact     = 0.55;
static const int nmaster     = 1;
static const int resizehints = 0;    /* don't respect terminal size hints */
static const int lockfullscreen = 1;
static const int refreshrate = 120;

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },
	{ "><>",      NULL },
	{ "[M]",      monocle },
};

/* Super as mod — Win+Space left free for keyboard layout */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

#include "movestack.c"

/* commands */
static char dmenumon[8];
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_accent, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };
/* floating terminal — instance matched by rules[] */
static const char *floattermcmd[] = { "/home/ryuzaki/.local/bin/floatterm", NULL };
static const char *floatagentcmd[] = { "/home/ryuzaki/.local/bin/floatagent", NULL };
static const char *lockcmd[] = { "slock", NULL };
static const char *filecmd[]  = { "thunar", NULL };
static const char *browsercmd[] = { "mullvad-browser", NULL };
static const char *firefoxcmd[] = { "firefox", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	/* apps */
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ShiftMask,             XK_Return, spawn,          {.v = floattermcmd } },
	{ MODKEY,                       XK_grave,  spawn,          {.v = floattermcmd } },
	/* Mod+Plus: Cursor agent in floating terminal (main + / numpad +) */
	{ MODKEY|ShiftMask,             XK_equal,  spawn,          {.v = floatagentcmd } },
	{ MODKEY,                       XK_plus,   spawn,          {.v = floatagentcmd } },
	{ MODKEY,                       XK_KP_Add, spawn,          {.v = floatagentcmd } },
	{ MODKEY,                       XK_d,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_p,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_e,      spawn,          {.v = filecmd } },
	{ MODKEY,                       XK_w,      spawn,          {.v = firefoxcmd } },
	{ MODKEY|ShiftMask,             XK_w,      spawn,          {.v = browsercmd } },
	{ MODKEY,                       XK_v,      spawn,          SHCMD("clipmenu-dmenu") },

	/* window management */
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_j,      movestack,      {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_k,      movestack,      {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_i,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_h,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_l,      setmfact,       {.f = +0.05} },
	{ MODKEY,                       XK_Left,   setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_Right,  setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_Left,   setmfact,       {.f = -0.025} },
	{ MODKEY|ShiftMask,             XK_Right,  setmfact,       {.f = +0.025} },
	{ MODKEY|ControlMask,           XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY|ShiftMask,             XK_c,      killclient,     {0} },

	/* layouts — not Super+Space (that switches keyboard) */
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY,                       XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY,                       XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ShiftMask,             XK_space,  setlayout,      {0} },
	{ MODKEY|ShiftMask,             XK_f,      togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },

	/* monitors */
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },

	/* media / system */
	{ 0, XF86XK_AudioRaiseVolume,   spawn, SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%+ -l 1.0; pkill -USR1 -F \"${XDG_RUNTIME_DIR:-/tmp}/dwm-status.pid\" 2>/dev/null") },
	{ 0, XF86XK_AudioLowerVolume,   spawn, SHCMD("wpctl set-volume @DEFAULT_AUDIO_SINK@ 5%-; pkill -USR1 -F \"${XDG_RUNTIME_DIR:-/tmp}/dwm-status.pid\" 2>/dev/null") },
	{ 0, XF86XK_AudioMute,          spawn, SHCMD("wpctl set-mute @DEFAULT_AUDIO_SINK@ toggle; pkill -USR1 -F \"${XDG_RUNTIME_DIR:-/tmp}/dwm-status.pid\" 2>/dev/null") },
	{ 0, XF86XK_AudioMicMute,       spawn, SHCMD("wpctl set-mute @DEFAULT_AUDIO_SOURCE@ toggle; pkill -USR1 -F \"${XDG_RUNTIME_DIR:-/tmp}/dwm-status.pid\" 2>/dev/null") },
	{ 0, XF86XK_MonBrightnessUp,    spawn, SHCMD("brightnessctl set +10%") },
	{ 0, XF86XK_MonBrightnessDown,  spawn, SHCMD("brightnessctl set 10%-") },
	{ 0, XK_Print,                  spawn, SHCMD("maim -s | xclip -selection clipboard -t image/png") },
	{ MODKEY, XK_Print,             spawn, SHCMD("maim | xclip -selection clipboard -t image/png") },

	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_l,      spawn,          {.v = lockcmd } },
	{ MODKEY|ShiftMask,             XK_q,      quit,           {0} },
};

/* button definitions */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button1,        focuswin,       {0} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	/* wheel cycle = same as Mod+k / Mod+j (Shift or Super) */
	{ ClkClientWin,         ShiftMask,      Button4,        focusstack,     {.i = -1 } },
	{ ClkClientWin,         ShiftMask,      Button5,        focusstack,     {.i = +1 } },
	{ ClkClientWin,         MODKEY,         Button4,        focusstack,     {.i = -1 } },
	{ ClkClientWin,         MODKEY,         Button5,        focusstack,     {.i = +1 } },
	{ ClkRootWin,           ShiftMask,      Button4,        focusstack,     {.i = -1 } },
	{ ClkRootWin,           ShiftMask,      Button5,        focusstack,     {.i = +1 } },
	{ ClkRootWin,           MODKEY,         Button4,        focusstack,     {.i = -1 } },
	{ ClkRootWin,           MODKEY,         Button5,        focusstack,     {.i = +1 } },
	{ ClkWinTitle,          ShiftMask,      Button4,        focusstack,     {.i = -1 } },
	{ ClkWinTitle,          ShiftMask,      Button5,        focusstack,     {.i = +1 } },
	{ ClkWinTitle,          MODKEY,         Button4,        focusstack,     {.i = -1 } },
	{ ClkWinTitle,          MODKEY,         Button5,        focusstack,     {.i = +1 } },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
