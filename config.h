/* See LICENSE file for copyright and license details. */
#include <X11/XF86keysym.h>
#include "gaplessgrid.c"

/* appearance */
static const unsigned int borderpx  = 1;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;     /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "IBM Plex Mono:style=Regular:pixelsize=14" };
static const char dmenufont[]       = "IBM Plex Mono:style=Regular:size=10";
static const char col_darkgray[]    = "#282828";
static const char col_lightgray[]   = "#d8d8d8";
static const char col_background[]  = "#181818";
static const char col_foreground[]  = "#585858";
static const char col_highlight[]   = "#ab4642";
static const char *colors[][3]      = {
	/*                    fg             bg              border   */
	[SchemeNorm]      = { col_lightgray, col_background, col_background },
	[SchemeSel]       = { col_lightgray, col_darkgray,   col_foreground },
  [SchemeHighlight] = { col_highlight, col_darkgray,   0              },
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class             instance    title            tags mask     isfloating   monitor */
	{ "sxiv",            NULL,       NULL,            0,            1,           -1 },
	{ "mpv",             NULL,       NULL,            0,            1,           -1 },
	{ "Chromium",        NULL,       NULL,            1 << 1,       0,           -1 },
  { "TelegramDesktop", NULL,       "Media viewer",  0,            1,           -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "tile",      tile },  /* first entry is default */
	{ "grid",      gaplessgrid },
	{ "monocle",   monocle },
	{ "float",     NULL }, /* no layout function means floating behavior */
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[]  = { "dmenu_run",  "-m", dmenumon, "-fn", dmenufont, "-nb", col_background, "-nf", col_foreground, "-sb", col_highlight, "-sf", col_background, "-shb", col_highlight, NULL };
static const char *clipmenu[]  = { "clipmenu",  "-m", dmenumon, "-fn", dmenufont, "-nb", col_background, "-nf", col_foreground, "-sb", col_highlight, "-sf", col_background, "-shb", col_highlight, NULL };

static const char *browser[]  = { "chromium",  NULL };
static const char *editor[]   = { "emacsclient", "-c", NULL };
static const char *terminal[] = { "kitty", NULL };
static const char *music[]    = { "spotify-tray", NULL };
static const char *ida[]      = { "/home/mirco/bin/ida", NULL };

static const char *screenshot[]     = { "/home/mirco/bin/screenshot", NULL };
static const char *fullscreenshot[] = { "/home/mirco/bin/screenshot", "fullscreen", NULL };

/* media keys */
static const char *volup[]      = { "pactl", "set-sink-volume", "0", "+5%",    NULL };
static const char *voldown[]    = { "pactl", "set-sink-volume", "0", "-5%",    NULL };
static const char *volmute[]    = { "pactl", "set-sink-mute",   "0", "toggle", NULL };

static Key keys[] = {
	/* modifier                      key       function        argument */
	{ MODKEY,                  XK_Return,      spawn,          { .v = terminal } },

	{ MODKEY,                       XK_l,      spawn,          { .v = dmenucmd } },
	{ MODKEY|ShiftMask,             XK_l,      spawn,          { .v = clipmenu } },

	{ MODKEY,                       XK_f,      spawn,          { .v = screenshot } },
	{ MODKEY|ShiftMask,             XK_f,      spawn,          { .v = fullscreenshot } },

	{ MODKEY,                       XK_a,      spawn,          { .v = browser } },
	{ MODKEY,                       XK_e,      spawn,          { .v = editor } },
	{ MODKEY,                       XK_s,      spawn,          { .v = music } },
	{ MODKEY,                       XK_i,      spawn,          { .v = ida } },

	{ MODKEY,                       XK_b,      togglebar,      { 0 } },
	{ MODKEY,                       XK_h,      focusstack,     { .i = +1 } },
	{ MODKEY,                       XK_t,      focusstack,     { .i = -1 } },
	{ MODKEY,                       XK_d,      setmfact,       { .f = -0.025 } },
	{ MODKEY,                       XK_n,      setmfact,       { .f = +0.025 } },
	{ MODKEY|ShiftMask,             XK_h,      movestack,      { .i = +1 } },
	{ MODKEY|ShiftMask,             XK_t,      movestack,      { .i = -1 } },

	{ MODKEY,               XK_semicolon,      killclient,     {0} },
	{ MODKEY|ShiftMask,         XK_space,      togglefloating, {0} },

#if defined(noergodox)
	TAGKEYS(XK_ampersand,   0)
	TAGKEYS(XK_bracketleft, 1)
	TAGKEYS(XK_braceleft,   2)
	TAGKEYS(XK_braceright,  3)
	TAGKEYS(XK_parenleft,   4)
	TAGKEYS(XK_equal,       5)
	TAGKEYS(XK_asterisk,    6)
	TAGKEYS(XK_parenright,  7)
	TAGKEYS(XK_plus,        8)
#else
	TAGKEYS(XK_1, 0)
	TAGKEYS(XK_2, 1)
	TAGKEYS(XK_3, 2)
	TAGKEYS(XK_4, 3)
	TAGKEYS(XK_5, 4)
	TAGKEYS(XK_6, 5)
	TAGKEYS(XK_7, 6)
	TAGKEYS(XK_8, 7)
	TAGKEYS(XK_9, 8)
#endif

	{ 0,         XF86XK_AudioRaiseVolume,  spawn,          {.v = volup } },
	{ 0,         XF86XK_AudioLowerVolume,  spawn,          {.v = voldown } },
	{ 0,                XF86XK_AudioMute,  spawn,          {.v = volmute } },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = terminal } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

