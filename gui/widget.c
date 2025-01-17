/*
 * Copyright (c) 2001-2019 Julien Nadeau Carriere <vedge@csoft.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * AG_Widget: The base class of all Agar widgets.
 */

#include <agar/core/core.h>
#include <agar/gui/gui.h>
#include <agar/gui/widget.h>
#include <agar/gui/window.h>
#include <agar/gui/cursors.h>
#include <agar/gui/primitive.h>
#include <agar/gui/gui_math.h>
#include <agar/gui/opengl.h>
#include <agar/gui/text_cache.h>

#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <agar/config/ag_debug_gui.h>

/* Overly verbose focus-related debugging */
/* #define DEBUG_FOCUS */

/* Built-in style attributes */
const char *agWidgetStyleNames[] = {
	"font-family",
	"font-size",
	"font-weight",
	"font-style",
	"color",
	"text-color",
	"line-color",
	"shape-color",
	"border-color",
	NULL
};

/* Possible style states */
const char *agWidgetStateNames[] = {
	"",				/* #default */
	"#disabled",
	"#focused",
	"#hover",
	"#selected",
	NULL
};

/* Names of palette entries */
const char *agWidgetColorNames[] = {
	"color",
	"text-color",
	"line-color",
	"shape-color",
	"border-color",
	NULL
};

/* Initial color palette */
AG_WidgetPalette agDefaultPalette = {{
       /*
        *    Color (BG)         Text (FG)         Line (FG)       Shape (FG)       Border (FG->BG)
	*/
#if (AG_MODEL == AG_SMALL) || (AG_MODEL == AG_MEDIUM)
/*def*/	{{125,125,125,255}, {240,240,240,255}, {50,50,50,255}, {200,200,200,255}, {100,100,100,255}},
/*dis*/	{{160,160,160,255}, {240,240,240,255}, {70,70,70,255}, {150,150,150,255}, {100,100,100,255}},
/*foc*/	{{125,125,125,255}, {240,240,240,255}, {50,50,50,255}, {200,200,200,255}, {100,100,100,255}},
/*hov*/	{{130,130,130,255}, {240,240,240,255}, {50,50,50,255}, {220,220,220,255}, {100,100,100,255}},
/*sel*/	{{ 50, 50,120,255}, {255,255,255,255}, {50,50,60,255}, { 50, 50, 50,255}, {100,100,100,255}},
#elif AG_MODEL == AG_LARGE
        /*
	 *         Color (BG)                     Text (FG)                      Line (FG)                      Shape (FG)                   Border (FG->BG)
	 */
/*def*/	{{0x7d7d,0x7d7d,0x7d7d,0xffff}, {0xf0f0,0xf0f0,0xf0f0,0xffff}, {0x3232,0x3232,0x3232,0xffff}, {0xc8c8,0xc8c8,0xc8c8,0xffff}, {0x6464,0x6464,0x6464,0xffff}},
/*dis*/	{{0xa0a0,0xa0a0,0xa0a0,0xffff}, {0xf0f0,0xf0f0,0xf0f0,0xffff}, {0x4646,0x4646,0x4646,0xffff}, {0x9696,0x9696,0x9696,0xffff}, {0x6464,0x6464,0x6464,0xffff}},
/*foc*/	{{0x7d7d,0x7d7d,0x7d7d,0xffff}, {0xf0f0,0xf0f0,0xf0f0,0xffff}, {0x3232,0x3232,0x3232,0xffff}, {0xc8c8,0xc8c8,0xc8c8,0xffff}, {0x6464,0x6464,0x6464,0xffff}},
/*hov*/	{{0x8282,0x8282,0x8282,0xffff}, {0xf0f0,0xf0f0,0xf0f0,0xffff}, {0x3232,0x3232,0x3232,0xffff}, {0xdcdc,0xdcdc,0xdcdc,0xffff}, {0x6464,0x6464,0x6464,0xffff}},
/*sel*/	{{0x3232,0x3232,0x7878,0xffff}, {0xffff,0xffff,0xffff,0xffff}, {0x3232,0x3232,0x3c3c,0xffff}, {0x3232,0x3232,0x3232,0xffff}, {0x6464,0x6464,0x6464,0xffff}},
#endif
}};

/* Import inlinables */
#undef AG_INLINE_HEADER
#include "inline_widget.h"

static void FocusWidget(AG_Widget *_Nonnull);
static void UnfocusWidget(AG_Widget *_Nonnull);

#ifdef DEBUG_FOCUS
# define Debug_Focus AG_Debug
#else
# if defined(__GNUC__)
#  define Debug_Focus(obj, arg...) ((void)0)
# elif defined(__CC65__)
#  define Debug_Focus
# else
#  define Debug_Focus AG_Debug
# endif
#endif /* DEBUG_FOCUS */

/* Set the parent window/driver pointers on a widget and its children. */
static void
SetParentWindow(AG_Widget *_Nonnull wid, AG_Window *_Nullable win)
{
	AG_Widget *chld;
	AG_CursorArea *ca, *caNext;
	
	wid->window = win;

	if (win) {
		wid->drv = AGDRIVER( OBJECT(win)->parent );
		wid->drvOps = AGDRIVER_CLASS(wid->drv);

		if (wid->flags & AG_WIDGET_USE_TEXT)
			win->flags |= AG_WINDOW_USE_TEXT;

		/*
		 * Commit any previously deferred AG_MapStockCursor()
		 * operation.
		 */
		for (ca = TAILQ_FIRST(&wid->pvt.cursorAreas);
		     ca != TAILQ_END(&wid->pvt.cursorAreas);
		     ca = caNext) {
			caNext = TAILQ_NEXT(ca, cursorAreas);
			if (ca->stock >= 0 &&
			    ca->stock < wid->drv->nCursors) {
				AG_Cursor *ac;
				int i = 0;

				TAILQ_FOREACH(ac, &wid->drv->cursors, cursors) {
					if (i++ == ca->stock)
						break;
				}
				if (ac) {
					ca->c = ac;
					TAILQ_INSERT_TAIL(&win->pvt.cursorAreas,
					    ca, cursorAreas);
				} else {
					free(ca);
				}
			} else {
				free(ca);
			}
		}
		TAILQ_INIT(&wid->pvt.cursorAreas);
	} else {
		wid->drv = NULL;
		wid->drvOps = NULL;
	}
	OBJECT_FOREACH_CHILD(chld, wid, ag_widget)
		SetParentWindow(chld, win);
}

/* Set the parent driver pointers on a widget and its children. */
static void
SetParentDriver(AG_Widget *_Nonnull wid, AG_Driver *_Nullable drv)
{
	AG_Widget *chld;

	if (drv) {
		wid->drv = AGDRIVER(drv);
		wid->drvOps = AGDRIVER_CLASS(drv);
	} else {
		wid->drv = NULL;
		wid->drvOps = NULL;
	}
	OBJECT_FOREACH_CHILD(chld, wid, ag_widget)
		SetParentDriver(chld, drv);
}

static void
OnAttach(AG_Event *_Nonnull event)
{
	AG_Widget *widget = AG_WIDGET_SELF();
	const void *parent = AG_PTR(1);

	if (AG_OfClass(parent, "AG_Widget:AG_Window:*") &&
	    AG_OfClass(widget, "AG_Widget:*")) {
		AG_Widget *wParent = WIDGET(parent);
		Uint i;
		/*
		 * This is a widget attaching to a window.
		 */
#ifdef AG_DEBUG_GUI
		Debug(widget, "Attach to %s window (\"%s\")\n",
		    OBJECT(parent)->name, AGWINDOW(parent)->caption);
#endif
		SetParentWindow(widget, AGWINDOW(wParent));
		if (AGWINDOW(wParent)->visible) {
			widget->flags |= AG_WIDGET_UPDATE_WINDOW;
			AG_PostEvent(widget, "widget-shown", NULL);
		}
		/*
		 * Widget may have previously been detached from another
		 * driver; textures may need regenerating.
		 */
		for (i = 0; i < widget->nSurfaces; i++) {
			widget->textures[i] = 0;
		}
	} else if (AG_OfClass(parent, "AG_Widget:*") &&
	           AG_OfClass(widget, "AG_Widget:*")) {
		AG_Widget *wParent = WIDGET(parent);
		AG_Window *window = wParent->window;
		/*
		 * This is a widget attaching to another widget (not a window).
		 */
#ifdef AG_DEBUG
		if (window) { AG_OBJECT_ISA(window, "AG_Widget:AG_Window:*"); }
#endif
#ifdef AG_DEBUG_GUI
		Debug(widget, "Attach to %s in %s\n", OBJECT_CLASS(parent)->name,
		    window ? OBJECT(window)->name : "<>");
#endif
		SetParentWindow(widget, window);
		if (window && window->visible) {
			AG_PostEvent(widget, "widget-shown", NULL);
		}
	} else if (AG_OfClass(parent, "AG_Driver:*") &&
	           AG_OfClass(widget, "AG_Widget:AG_Window:*")) {
		AG_Driver *drvParent = AGDRIVER(parent);
		/*
		 * This is a Window attaching to a low-level Driver.
		 */
#ifdef AG_DEBUG_GUI
		Debug(widget, "Attach to %s (%s)\n", OBJECT_CLASS(parent)->name,
		   OBJECT(parent)->name);
#endif
		SetParentDriver(widget, drvParent);
	} else {
#ifdef AG_VERBOSITY
		AG_FatalErrorF("Cannot attach %s to %s", OBJECT(widget)->name,
		    OBJECT(parent)->name);
#else
		AG_FatalError("Cannot attach to parent");
#endif
	}
}

static void
OnDetach(AG_Event *_Nonnull event)
{
	AG_Widget *widget = AG_WIDGET_SELF();
	const void *parent = AG_PTR(1);
	
	if (AG_OfClass(parent, "AG_Widget:*") &&
	    AG_OfClass(widget, "AG_Widget:*")) {
		if (widget->window) {
			AG_UnmapAllCursors(widget->window, widget);
		}
		SetParentWindow(widget, NULL);
	} else if (AG_OfClass(parent, "AG_Driver:*") &&
	           AG_OfClass(widget, "AG_Widget:AG_Window:*")) {
		SetParentDriver(widget, NULL);
	} else {
#ifdef AG_VERBOSITY
		AG_FatalErrorF("Unexpected parent on detach. "
		               "Why is %s attached to %s?",
			       OBJECT(widget)->name, OBJECT(parent)->name);
#else
		AG_FatalError("Unexpected parent on detach");
#endif
	}
}

#ifdef AG_TIMERS
/* Timer callback for AG_RedrawOnTick(). */
static Uint32
RedrawOnTickTimeout(AG_Timer *_Nonnull to, AG_Event *_Nonnull event)
{
	AG_Widget *wid = AG_WIDGET_SELF();

	if (wid->window) {
		AG_OBJECT_ISA(wid->window, "AG_Widget:AG_Window:*");
		wid->window->dirty = 1;
	}
	return (to->ival);
}

/* Timer callback for AG_RedrawOnChange(). */
static Uint32
RedrawOnChangeTimeout(AG_Timer *_Nonnull to, AG_Event *_Nonnull event)
{
	AG_Widget *wid = AG_WIDGET_SELF();
	AG_RedrawTie *rt = AG_PTR(1);
	AG_Variable *V, Vd;
	void *p;

	V = AG_GetVariable(wid, rt->name, &p);
	AG_DerefVariable(&Vd, V);
	if (!rt->VlastInited || AG_CompareVariables(&Vd, &rt->Vlast) != 0) {
		if (wid->window) {
			AG_OBJECT_ISA(wid->window, "AG_Widget:AG_Window:*");
			wid->window->dirty = 1;
		}
		AG_CopyVariable(&rt->Vlast, &Vd);
		rt->VlastInited = 1;
	}
	AG_UnlockVariable(V);
	return (to->ival);
}
#endif /* AG_TIMERS */

/* "widget-shown" handler */
static void
OnShow(AG_Event *_Nonnull event)
{
	AG_Widget *wid = AG_WIDGET_SELF();
	AG_Widget *chld;
#ifdef AG_TIMERS
	AG_RedrawTie *rt;
#endif
#ifdef AG_DEBUG
	if (!wid->font) { AG_FatalError("!font"); }
	AG_OBJECT_ISA(wid->font, "AG_Font:*");
#endif
	OBJECT_FOREACH_CHILD(chld, wid, ag_widget) {
		AG_ForwardEvent(chld, event);
	}
	wid->flags |= AG_WIDGET_VISIBLE;

#ifdef AG_TIMERS
	TAILQ_FOREACH(rt, &wid->pvt.redrawTies, redrawTies) {
		switch (rt->type) {
		case AG_REDRAW_ON_TICK:
			AG_AddTimer(wid, &rt->to, rt->ival,
			    RedrawOnTickTimeout, NULL);
			break;
		case AG_REDRAW_ON_CHANGE:
			AG_AddTimer(wid, &rt->to, rt->ival,
			    RedrawOnChangeTimeout, "%p", rt);
			break;
		}
	}
#endif /* AG_TIMERS */
}

/* "widget-hidden" handler */
static void
OnHide(AG_Event *_Nonnull event)
{
	AG_Widget *wid = AG_WIDGET_SELF();
	AG_Widget *chld;
#ifdef AG_TIMERS
	AG_RedrawTie *rt;
#endif
	OBJECT_FOREACH_CHILD(chld, wid, ag_widget) {
		AG_ForwardEvent(chld, event);
	}
	wid->flags &= ~(AG_WIDGET_VISIBLE);

#ifdef AG_TIMERS
	TAILQ_FOREACH(rt, &wid->pvt.redrawTies, redrawTies) {
		switch (rt->type) {
		case AG_REDRAW_ON_TICK:
		case AG_REDRAW_ON_CHANGE:
			AG_DelTimer(wid, &rt->to);
			break;
		}
	}
#endif
}

static void
Init(void *_Nonnull obj)
{
	AG_Widget *wid = obj;

	OBJECT(wid)->flags |= AG_OBJECT_NAME_ONATTACH;

	wid->flags = 0;
	wid->rView.x1 = -1;
	wid->rView.y1 = -1;
	wid->rView.w = -1;
	wid->rView.h = -1;
	wid->rView.x2 = -1;
	wid->rView.y2 = -1;
	wid->rSens.x1 = 0;
	wid->rSens.y1 = 0;
	wid->rSens.w = 0;
	wid->rSens.h = 0;
	wid->rSens.x2 = 0;
	wid->rSens.y2 = 0;
	wid->x = -1;
	wid->y = -1;
	wid->w = -1;
	wid->h = -1;
	wid->focusFwd = NULL;
	wid->window = NULL;
	wid->drv = NULL;
	wid->drvOps = NULL;
	wid->nSurfaces = 0;
	wid->surfaces = NULL;
	wid->surfaceFlags = NULL;
	wid->textures = NULL;
	wid->texcoords = NULL;
	AG_TblInit(&wid->pvt.actions, 32, 0);
	TAILQ_INIT(&wid->pvt.mouseActions);
	TAILQ_INIT(&wid->pvt.keyActions);
	
	wid->state = AG_DEFAULT_STATE;
	wid->css = NULL;
	wid->font = agDefaultFont;
	wid->pal = agDefaultPalette;
#ifdef HAVE_OPENGL
	wid->gl = NULL;
#endif

	AG_SetEvent(wid, "attached", OnAttach, NULL);
	AG_SetEvent(wid, "detached", OnDetach, NULL);
	AG_SetEvent(wid, "widget-shown", OnShow, NULL);
	AG_SetEvent(wid, "widget-hidden", OnHide, NULL);
#ifdef AG_TIMERS
	TAILQ_INIT(&wid->pvt.redrawTies);
#endif
	TAILQ_INIT(&wid->pvt.cursorAreas);
}

/* Arrange for a redraw whenever a given binding value changes. */
void
AG_RedrawOnChange(void *obj, int refresh_ms, const char *name)
{
#ifdef AG_TIMERS
	AG_Widget *wid = obj;
	AG_RedrawTie *rt;

	AG_OBJECT_ISA(wid, "AG_Widget:*");

	TAILQ_FOREACH(rt, &wid->pvt.redrawTies, redrawTies) {
		if (rt->type == AG_REDRAW_ON_CHANGE &&
		    strcmp(rt->name, name) == 0 &&
		    rt->ival == refresh_ms)
			break;
	}
	if (rt) {
		AG_ResetTimer(wid, &rt->to, refresh_ms);
		return;
	}
	
	rt = Malloc(sizeof(AG_RedrawTie));
	rt->type = AG_REDRAW_ON_CHANGE;
	rt->ival = refresh_ms;
	rt->VlastInited = 0;
	Strlcpy(rt->name, name, sizeof(rt->name));
	AG_InitTimer(&rt->to, "redrawTie-", 0);
#ifdef AG_DEBUG
	Strlcat(rt->to.name, name, sizeof(rt->to.name));
#endif
	TAILQ_INSERT_TAIL(&wid->pvt.redrawTies, rt, redrawTies);
	
	if (wid->flags & AG_WIDGET_VISIBLE) {
		AG_AddTimer(wid, &rt->to, rt->ival, RedrawOnChangeTimeout, "%p", rt);
	} else {
		/* Fire from OnShow() */
	}
#endif /* AG_TIMERS */
}

/* Arrange for an unconditional redraw at a periodic interval. */
void
AG_RedrawOnTick(void *obj, int refresh_ms)
{
#ifdef AG_TIMERS
	AG_Widget *wid = obj;
	AG_RedrawTie *rt;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");

	if (refresh_ms == -1) {
		TAILQ_FOREACH(rt, &wid->pvt.redrawTies, redrawTies) {
			if (rt->type == AG_REDRAW_ON_TICK)
				break;
		}
		if (rt) {
			TAILQ_REMOVE(&wid->pvt.redrawTies, rt, redrawTies);
			AG_DelTimer(wid, &rt->to);
			free(rt);
		}
		return;
	}

	rt = Malloc(sizeof(AG_RedrawTie));
	rt->type = AG_REDRAW_ON_TICK;
	rt->ival = refresh_ms;
	rt->name[0] = '\0';
	AG_InitTimer(&rt->to, "redrawTick", 0);
	TAILQ_INSERT_TAIL(&wid->pvt.redrawTies, rt, redrawTies);
	
	if (wid->flags & AG_WIDGET_VISIBLE) {
		AG_AddTimer(wid, &rt->to, rt->ival, RedrawOnTickTimeout, NULL);
	} else {
		/* Fire from OnShow() */
	}
#endif /* AG_TIMERS */
}

/* Default event handler for "key-down" (for widgets using Actions). */
void
AG_WidgetStdKeyDown(AG_Event *event)
{
	AG_Widget *wid = AG_WIDGET_SELF();
	const int sym = AG_INT(1);
	const int mod = AG_INT(2);

	AG_ExecKeyAction(wid, AG_ACTION_ON_KEYDOWN, sym, mod);
}

/* Default event handler for "key-up" (for widgets using Actions). */
void
AG_WidgetStdKeyUp(AG_Event *event)
{
	AG_Widget *wid = AG_WIDGET_SELF();
	const int sym = AG_INT(1);
	const int mod = AG_INT(2);

	AG_ExecKeyAction(wid, AG_ACTION_ON_KEYUP, sym, mod);
}

/* Default event handler for "mouse-button-down" (for widgets using Actions). */
void
AG_WidgetStdMouseButtonDown(AG_Event *event)
{
	AG_Widget *wid = AG_WIDGET_SELF();
	const int btn = AG_INT(1);
	const int x = AG_INT(2);
	const int y = AG_INT(3);

	if (!AG_WidgetIsFocused(wid)) {
		AG_WidgetFocus(wid);
	}
	AG_ExecMouseAction(wid, AG_ACTION_ON_BUTTONDOWN, btn, x,y);
}

/* Default event handler for "mouse-button-up" (for widgets using Actions). */
void
AG_WidgetStdMouseButtonUp(AG_Event *event)
{
	AG_Widget *wid = AG_WIDGET_SELF();
	const int btn = AG_INT(1);
	const int x = AG_INT(2);
	const int y = AG_INT(3);

	AG_ExecMouseAction(wid, AG_ACTION_ON_BUTTONUP, btn, x,y);
}

/* Tie an action to a mouse-button-down event. */
void
AG_ActionOnButtonDown(void *obj, int button, const char *action)
{
	AG_Widget *wid = obj;
	AG_ActionTie *at;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");
	
	at = Malloc(sizeof(AG_ActionTie));
	at->type = AG_ACTION_ON_BUTTONDOWN;
	Strlcpy(at->action, action, sizeof(at->action));
	at->data.button = (AG_MouseButton)button;

	AG_ObjectLock(wid);
	TAILQ_INSERT_TAIL(&wid->pvt.mouseActions, at, ties);
	if (AG_FindEventHandler(wid, "mouse-button-down") == NULL) {
		AG_SetEvent(wid, "mouse-button-down", AG_WidgetStdMouseButtonDown, NULL);
	}
	AG_ObjectUnlock(wid);
}

void
AG_ActionOnButton(void *obj, int button, const char *action)
{
	AG_ActionOnButtonDown(obj, button, action);
}

/* Tie an action to a mouse-button-up event. */
void
AG_ActionOnButtonUp(void *obj, int button, const char *action)
{
	AG_Widget *wid = obj;
	AG_ActionTie *at;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	
	at = Malloc(sizeof(AG_ActionTie));
	at->type = AG_ACTION_ON_BUTTONUP;
	Strlcpy(at->action, action, sizeof(at->action));
	at->data.button = (AG_MouseButton)button;

	AG_ObjectLock(wid);
	TAILQ_INSERT_TAIL(&wid->pvt.mouseActions, at, ties);
	if (AG_FindEventHandler(wid, "mouse-button-up") == NULL) {
		AG_SetEvent(wid, "mouse-button-up", AG_WidgetStdMouseButtonUp, NULL);
	}
	AG_ObjectUnlock(wid);
}

/* Tie an action to a key-down event. */
void
AG_ActionOnKeyDown(void *obj, AG_KeySym sym, AG_KeyMod mod, const char *action)
{
	AG_Widget *wid = obj;
	AG_ActionTie *at;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");
	
	at = Malloc(sizeof(AG_ActionTie));
	at->type = AG_ACTION_ON_KEYDOWN;
	Strlcpy(at->action, action, sizeof(at->action));
	at->data.key.sym = sym;
	at->data.key.mod = mod;

	AG_ObjectLock(wid);
	TAILQ_INSERT_TAIL(&wid->pvt.keyActions, at, ties);
	if (AG_FindEventHandler(wid, "key-down") == NULL) {
		AG_SetEvent(wid, "key-down", AG_WidgetStdKeyDown, NULL);
	}
	AG_ObjectUnlock(wid);
}

/* Tie an action to a key-up event. */
void
AG_ActionOnKeyUp(void *obj, AG_KeySym sym, AG_KeyMod mod, const char *action)
{
	AG_Widget *wid = obj;
	AG_ActionTie *at;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");
	
	at = Malloc(sizeof(AG_ActionTie));
	at->type = AG_ACTION_ON_KEYUP;
	Strlcpy(at->action, action, sizeof(at->action));
	at->data.key.sym = sym;
	at->data.key.mod = mod;

	AG_ObjectLock(wid);
	TAILQ_INSERT_TAIL(&wid->pvt.keyActions, at, ties);
	if (AG_FindEventHandler(wid, "key-up") == NULL) {
		AG_SetEvent(wid, "key-up", AG_WidgetStdKeyUp, NULL);
	}
	AG_ObjectUnlock(wid);
}

#ifdef AG_TIMERS
/* Timer callback for AG_ACTION_ON_KEYREPEAT actions. */
static Uint32
ActionKeyRepeatTimeout(AG_Timer *_Nonnull to, AG_Event *_Nonnull event)
{
	AG_Widget *wid = AG_WIDGET_SELF();
	AG_ActionTie *at = AG_PTR(1);
	AG_Action *a;

	if (AG_TblLookupPointer(&wid->pvt.actions, at->action, (void *)&a) == -1 ||
	    a == NULL) {
		return (0);
	}
	AG_ExecAction(wid, a);
	return (agKbdRepeat);
}
#endif /* AG_TIMERS */

/* Tie an action to a key-down event, with key repeat. */
void
AG_ActionOnKey(void *obj, AG_KeySym sym, AG_KeyMod mod, const char *action)
{
#ifdef AG_TIMERS
	AG_Widget *wid = obj;
	AG_ActionTie *at;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");
	
	at = Malloc(sizeof(AG_ActionTie));
	at->type = AG_ACTION_ON_KEYREPEAT;
	Strlcpy(at->action, action, sizeof(at->action));
	at->data.key.sym = sym;
	at->data.key.mod = mod;
	AG_InitTimer(&at->data.key.toRepeat, "actKey-", 0);
# ifdef AG_DEBUG
	Strlcat(at->data.key.toRepeat.name, action, sizeof(at->data.key.toRepeat.name));
# endif
	AG_ObjectLock(wid);
	TAILQ_INSERT_TAIL(&wid->pvt.keyActions, at, ties);
	if (AG_FindEventHandler(wid, "key-up") == NULL &&
	    AG_FindEventHandler(wid, "key-down") == NULL) {
		AG_SetEvent(wid, "key-up", AG_WidgetStdKeyUp, NULL);
		AG_SetEvent(wid, "key-down", AG_WidgetStdKeyDown, NULL);
	}
	AG_ObjectUnlock(wid);
#else /* AG_TIMERS */

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ActionOnKeyDown(obj, sym, mod, action);

#endif /* AG_TIMERS */
}

/* Configure a widget action. */
AG_Action *
AG_ActionFn(void *obj, const char *name, AG_EventFn fn, const char *fnArgs,...)
{
	AG_Widget *wid = obj;
	AG_Action *a;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");

	a = Malloc(sizeof(AG_Action));
	a->type = AG_ACTION_FN;
	a->widget = wid;

	AG_ObjectLock(wid);
	a->fn = AG_SetEvent(wid, NULL, fn, NULL);
	if (fnArgs) {
		va_list ap;
		
		va_start(ap, fnArgs);
		AG_EventGetArgs(a->fn, fnArgs, ap);
		va_end(ap);
	}
	AG_TblInsertPointer(&wid->pvt.actions, name, a);
	AG_ObjectUnlock(wid);
	return (a);
}

/* Configure a widget action for setting an integer flag. */
AG_Action *
AG_ActionSetInt(void *obj, const char *name, int *p, int val)
{
	AG_Widget *wid = obj;
	AG_Action *a;

	AG_OBJECT_ISA(wid, "AG_Widget:*");

	a = Malloc(sizeof(AG_Action));
	a->type = AG_ACTION_SET_INT;
	a->widget = wid;
	a->fn = NULL;
	a->p = (void *)p;
	a->val = val;

	AG_ObjectLock(wid);
	AG_TblInsertPointer(&wid->pvt.actions, name, a);
	AG_ObjectUnlock(wid);
	return (a);
}

/* Configure a widget action for toggling an integer flag. */
AG_Action *
AG_ActionToggleInt(void *obj, const char *name, int *p)
{
	AG_Widget *wid = obj;
	AG_Action *a;

	AG_OBJECT_ISA(wid, "AG_Widget:*");

	a = Malloc(sizeof(AG_Action));
	a->type = AG_ACTION_TOGGLE_INT;
	a->widget = wid;
	a->fn = NULL;
	a->p = (void *)p;

	AG_ObjectLock(wid);
	AG_TblInsertPointer(&wid->pvt.actions, name, a);
	AG_ObjectUnlock(wid);
	return (a);
}

/* Configure a widget action for setting bitwise flags. */
AG_Action *
AG_ActionSetFlag(void *obj, const char *name, Uint *p, Uint bitmask, int val)
{
	AG_Widget *wid = obj;
	AG_Action *a;

	AG_OBJECT_ISA(wid, "AG_Widget:*");

	a = Malloc(sizeof(AG_Action));
	a->type = AG_ACTION_SET_INT;
	a->widget = wid;
	a->fn = NULL;
	a->p = (void *)p;
	a->bitmask = bitmask;
	a->val = val;

	AG_ObjectLock(wid);
	AG_TblInsertPointer(&wid->pvt.actions, name, a);
	AG_ObjectUnlock(wid);
	return (a);
}

/* Configure a widget action for toggling bitwise flags. */
AG_Action *
AG_ActionToggleFlag(void *obj, const char *name, Uint *p, Uint bitmask)
{
	AG_Widget *wid = obj;
	AG_Action *a;

	AG_OBJECT_ISA(wid, "AG_Widget:*");

	a = Malloc(sizeof(AG_Action));
	a->type = AG_ACTION_TOGGLE_FLAG;
	a->widget = wid;
	a->fn = NULL;
	a->p = (void *)p;
	a->bitmask = bitmask;

	AG_ObjectLock(wid);
	AG_TblInsertPointer(&wid->pvt.actions, name, a);
	AG_ObjectUnlock(wid);
	return (a);
}

/* Execute an action (usually called internally from AG_ExecFooAction()) */
int
AG_ExecAction(void *obj, AG_Action *a)
{
	AG_OBJECT_ISA(obj, "AG_Widget:*");

	switch (a->type) {
	case AG_ACTION_FN:
		if (a->fn) {
			a->fn->fn(a->fn);
			return (1);
		}
		return (0);
	case AG_ACTION_SET_INT:
		*(int *)a->p = a->val;
		return (1);
	case AG_ACTION_TOGGLE_INT:
		*(int *)a->p = !(*(int *)a->p);
		return (1);
	case AG_ACTION_SET_FLAG:
		if (a->val) {
			*(Uint *)a->p |= a->bitmask;
		} else {
			*(Uint *)a->p &= ~(a->bitmask);
		}
		return (1);
	case AG_ACTION_TOGGLE_FLAG:
		if (*(Uint *)a->p & a->bitmask) {
			*(Uint *)a->p &= ~(a->bitmask);
		} else {
			*(Uint *)a->p |= a->bitmask;
		}
		return (1);
	}
	return (0);
}

/*
 * Run any action tied to a mouse-button event. Exceptionally, we pass
 * mouse event arguments to the function.
 */
int
AG_ExecMouseAction(void *obj, AG_ActionEventType et, int button,
    int xCurs, int yCurs)
{
	AG_Widget *wid = obj;
	AG_ActionTie *at;
	AG_Action *a;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");
#ifdef AG_DEBUG
	if (et != AG_ACTION_ON_BUTTONDOWN &&
	    et != AG_ACTION_ON_BUTTONUP)
		AG_FatalError("Invalid type argument");
#endif
	TAILQ_FOREACH(at, &wid->pvt.mouseActions, ties) {
		if (at->type == et &&
		    ((button == at->data.button) ||
		     (at->data.button == AG_MOUSE_ANY)))
			break;
	}
	if (at == NULL) {
		return (0);
	}
	if (AG_TblLookupPointer(&wid->pvt.actions, at->action, (void *)&a) == -1 ||
	    a == NULL) {
		return (0);
	}
	if (a->fn) {
		AG_PostEventByPtr(wid, a->fn, "%i,%i,%i", button, xCurs,yCurs);
		return (1);
	}
	return (0);
}

/* Run any action tied to a key-down event. */
int
AG_ExecKeyAction(void *obj, AG_ActionEventType et, AG_KeySym sym, AG_KeyMod mod)
{
	AG_Widget *wid = obj;
	AG_ActionTie *at;
	AG_Action *a;
	int rv;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
#ifdef AG_DEBUG
	if (et != AG_ACTION_ON_KEYDOWN &&
	    et != AG_ACTION_ON_KEYUP)
		AG_FatalError("Invalid type argument");
#endif
	TAILQ_FOREACH(at, &wid->pvt.keyActions, ties) {
#ifdef AG_TIMERS
		if (at->type != et &&
		    at->type != AG_ACTION_ON_KEYREPEAT)
			continue;
#else
		if (at->type != et)
			continue;
#endif
		if ((at->data.key.mod == AG_KEYMOD_ANY ||
		     at->data.key.mod & mod) &&
		    (at->data.key.sym == AG_KEY_ANY ||
		     at->data.key.sym == sym))
			break;
	}
	if (at == NULL) {
		return (0);
	}
	if (AG_TblLookupPointer(&wid->pvt.actions, at->action, (void *)&a) == -1 ||
	    a == NULL)
		return (0);

	rv = AG_ExecAction(wid, a);
#ifdef AG_TIMERS
	if (at->type == AG_ACTION_ON_KEYREPEAT) {
		if (et == AG_ACTION_ON_KEYDOWN) {
			AG_AddTimer(wid, &at->data.key.toRepeat, agKbdDelay,
			    ActionKeyRepeatTimeout, "%p", at);
		} else {
			AG_DelTimer(wid, &at->data.key.toRepeat);
		}
	}
#endif
	return (rv);
}

static void *_Nullable
WidgetFindPath(const AG_Object *_Nonnull parent, const char *_Nonnull name)
{
	char node_name[AG_OBJECT_PATH_MAX];
	void *rv;
	char *s;
	AG_Object *chld;

	Strlcpy(node_name, name, sizeof(node_name));
	if ((s = strchr(node_name, '/')) != NULL) {
		*s = '\0';
	}
	if (AG_OfClass(parent, "AG_Driver:*")) {
		AG_Driver *drv = AGDRIVER(parent);
		AG_Window *win;

		AG_FOREACH_WINDOW(win, drv) {
			AG_OBJECT_ISA(win, "AG_Widget:AG_Window:*");
			if (strcmp(AGOBJECT(win)->name, node_name) != 0) {
				continue;
			}
			if ((s = strchr(name, '/')) != NULL) {
				rv = WidgetFindPath(AGOBJECT(win), &s[1]);
				if (rv) {
					return (rv);
				} else {
					return (NULL);
				}
			}
			return (win);
		}
	} else {
		TAILQ_FOREACH(chld, &parent->children, cobjs) {
			AG_OBJECT_ISA(chld, "AG_Widget:*");
			if (strcmp(chld->name, node_name) != 0) {
				continue;
			}
			if ((s = strchr(name, '/')) != NULL) {
				rv = WidgetFindPath(chld, &s[1]);
				if (rv) {
					return (rv);
				} else {
					return (NULL);
				}
			}
			return (chld);
		}
	}
	return (NULL);
}

/*
 * Find a widget by name (e.g., "Window/Widget1/Widget2"). This works
 * similarly to the more general AG_ObjectFind(3). Return value is only
 * valid as long as the Driver VFS is locked.
 *
 * XXX how does this differ from AG_ObjectFind() now?
 */
void *
AG_WidgetFind(void *obj, const char *name)
{
	AG_Driver *drv = obj;
	void *rv;

#ifdef AG_DEBUG
	if (name[0] != '/') { AG_FatalError("Not an absolute path"); }
#endif
	AG_OBJECT_ISA(drv, "AG_Driver:*");
	AG_LockVFS(drv);
	rv = WidgetFindPath(OBJECT(drv), &name[1]);
	AG_UnlockVFS(drv);
	if (rv == NULL) {
		AG_SetError(_("The widget `%s' does not exist."), name);
	}
	return (rv);
}

/* Set the FOCUSABLE flag on a widget. */
int
AG_WidgetSetFocusable(void *obj, int enable)
{
	AG_Widget *wid = obj;
	int prev;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	prev = (wid->flags & AG_WIDGET_FOCUSABLE);
	AG_SETFLAGS(wid->flags, AG_WIDGET_FOCUSABLE, enable);
	Debug_Focus(wid, "SetFocusable: %d => %d\n", prev, enable);
	AG_ObjectUnlock(wid);
	return (prev);
}

/* Set widget to "enabled" state for input. */
void
AG_WidgetEnable(void *obj)
{
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	/* TODO make the "disabled" flag a (locklessly settable) int. */
	if (wid->flags & AG_WIDGET_DISABLED) {
		wid->flags &= ~(AG_WIDGET_DISABLED);
		AG_PostEvent(wid, "widget-enabled", NULL);
		AG_Redraw(wid);
	}
	AG_ObjectUnlock(wid);
}

/* Set widget to "disabled" state for input. */
void
AG_WidgetDisable(void *obj)
{
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	/* TODO make the "disabled" flag a (locklessly settable) int. */
	if (!(wid->flags & AG_WIDGET_DISABLED)) {
		wid->flags |= AG_WIDGET_DISABLED;
		AG_PostEvent(wid, "widget-disabled", NULL);
		AG_Redraw(wid);
	}
	AG_ObjectUnlock(wid);
}

/* Arrange for a widget to automatically forward focus to another widget. */
void
AG_WidgetForwardFocus(void *obj, void *objFwd)
{
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	if (objFwd) {
		AG_OBJECT_ISA(objFwd, "AG_Widget:*");
		wid->flags |= AG_WIDGET_FOCUSABLE;
		wid->focusFwd = WIDGET(objFwd);
		Debug_Focus(wid, "ForwardFocus(%s)\n", OBJECT(objFwd)->name);
	} else {
		wid->flags &= ~(AG_WIDGET_FOCUSABLE);
		wid->focusFwd = NULL;
		Debug_Focus(wid, "ForwardFocus(NULL)\n");
	}
	AG_ObjectUnlock(wid);
}

static void
Destroy(void *_Nonnull obj)
{
	AG_Widget *wid = obj;
	AG_CursorArea *ca, *caNext;
#ifdef AG_TIMERS
	AG_RedrawTie *rt, *rtNext;
#endif
	AG_ActionTie *at, *atNext;
	AG_Variable *V;
	Uint i, j;

	for (ca = TAILQ_FIRST(&wid->pvt.cursorAreas);
	     ca != TAILQ_END(&wid->pvt.cursorAreas);
	     ca = caNext) {
		caNext = TAILQ_NEXT(ca, cursorAreas);
		free(ca);
	}
#ifdef AG_TIMERS
	for (rt = TAILQ_FIRST(&wid->pvt.redrawTies);
	     rt != TAILQ_END(&wid->pvt.redrawTies);
	     rt = rtNext) {
		rtNext = TAILQ_NEXT(rt, redrawTies);
		free(rt);
	}
#endif
	for (at = TAILQ_FIRST(&wid->pvt.mouseActions);
	     at != TAILQ_END(&wid->pvt.mouseActions);
	     at = atNext) {
		atNext = TAILQ_NEXT(at, ties);
		free(at);
	}
	for (at = TAILQ_FIRST(&wid->pvt.keyActions);
	     at != TAILQ_END(&wid->pvt.keyActions);
	     at = atNext) {
		atNext = TAILQ_NEXT(at, ties);
		free(at);
	}

	/* Free the action tables. */
	AG_TBL_FOREACH(V, i,j, &wid->pvt.actions) {
		Free(V->data.p);
	}
	AG_TblDestroy(&wid->pvt.actions);

	/*
	 * Free surfaces. We can assume that drivers have already deleted
	 * any associated resources.
	 */
	for (i = 0; i < wid->nSurfaces; i++) {
		AG_Surface *S;

		if ((S = wid->surfaces[i]) && !WSURFACE_NODUP(wid,i)) {
			S->flags &= ~(AG_SURFACE_MAPPED);
			AG_SurfaceFree(S);
		}
	}
	Free(wid->surfaces);
	Free(wid->surfaceFlags);
	Free(wid->textures);
	Free(wid->texcoords);
}

#ifdef HAVE_OPENGL
/*
 * Coordinate-free variants of AG_WidgetBlit*() for OpenGL-only widgets.
 * Rely on GL transformations instead of coordinates.
 */
void
AG_WidgetBlitGL(void *obj, AG_Surface *S, float w, float h)
{
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	wid->drvOps->blitSurfaceGL(wid->drv, wid, S, w,h);
}
void
AG_WidgetBlitSurfaceGL(void *obj, int name, float w, float h)
{
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	wid->drvOps->blitSurfaceFromGL(wid->drv, wid, name, w,h);
}
void
AG_WidgetBlitSurfaceFlippedGL(void *obj, int name, float w, float h)
{
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	wid->drvOps->blitSurfaceFlippedGL(wid->drv, wid, name, w,h);
}

/*
 * Backup and regenerate all GL resources associated with a widget
 * (and any of its children).
 *
 * If some textures exist without a corresponding surface, allocate a
 * software surface and copy their contents to be later restored. These
 * routines are necessary for dealing with GL context loss.
 */
void
AG_WidgetFreeResourcesGL(void *obj)
{
	AG_Widget *wid = obj, *cwid;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");

	if (wid->drvOps->backupSurfaces != NULL) {
		wid->drvOps->backupSurfaces(wid->drv, wid);
	}
	OBJECT_FOREACH_CHILD(cwid, wid, ag_widget)
		AG_WidgetFreeResourcesGL(cwid);
}
void
AG_WidgetRegenResourcesGL(void *obj)
{
	AG_Widget *wid = obj, *cwid;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");

	if (wid->drvOps->restoreSurfaces != NULL) {
		wid->drvOps->restoreSurfaces(wid->drv, wid);
	}
	OBJECT_FOREACH_CHILD(cwid, wid, ag_widget)
		AG_WidgetRegenResourcesGL(cwid);
}
#endif /* HAVE_OPENGL */

/* Remove focus from a widget and its children. */
void
AG_WidgetUnfocus(void *p)
{
	AG_Widget *wid = p, *cwid, *focusFwd;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	if ((focusFwd = wid->focusFwd) != NULL) {
		AG_OBJECT_ISA(focusFwd, "AG_Widget:*");
		AG_ObjectLock(focusFwd);
		if (focusFwd->flags & AG_WIDGET_FOCUSED) {
			UnfocusWidget(focusFwd);
		}
		AG_ObjectUnlock(focusFwd);
	}
	if (wid->flags & AG_WIDGET_FOCUSED) {
		UnfocusWidget(wid);
	}
	OBJECT_FOREACH_CHILD(cwid, wid, ag_widget) {
		AG_WidgetUnfocus(cwid);
	}
	AG_ObjectUnlock(wid);
}

static void
UnfocusWidget(AG_Widget *_Nonnull wid)
{
	AG_Window *win;

	AG_OBJECT_ISA(wid, "AG_Widget:*");

	wid->flags &= ~(AG_WIDGET_FOCUSED);
	if ((win = wid->window)) {
		AG_OBJECT_ISA(win, "AG_Widget:AG_Window:*");
		AG_PostEvent(wid, "widget-lostfocus", NULL);
		win->nFocused--;
		win->dirty = 1;
	}
}

/* Move the focus over a widget (and its parents). */
int
AG_WidgetFocus(void *obj)
{
	AG_Widget *wid = obj, *wParent = wid, *focusFwd;
	AG_Window *win;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");
	win = wid->window;
	AG_OBJECT_ISA(win, "AG_Widget:AG_Window:*");

	AG_LockVFS(wid);
	AG_ObjectLock(wid);
	
	if (AG_WidgetIsFocused(wid)) {
		Debug_Focus(wid, "Already focused\n");
		goto out;
	}
	if (!(wid->flags & AG_WIDGET_FOCUSABLE)) {
		if ((focusFwd = wid->focusFwd) &&
		   !(focusFwd->flags & AG_WIDGET_FOCUSED)) {
			AG_OBJECT_ISA(focusFwd, "AG_Widget:*");
			AG_ObjectLock(focusFwd);
			Debug_Focus(wid, "Forward focus to %s\n", OBJECT(focusFwd)->name);
			FocusWidget(focusFwd);
			AG_ObjectUnlock(focusFwd);
			goto out;
		}
		Debug_Focus(wid, "Reject focus\n");
		goto fail;
	}
	Debug_Focus(wid, "Focusing\n");

	/* Remove any existing focus. XXX inefficient */
	if (win && win->nFocused > 0)
		AG_WidgetUnfocus(win);

	/*
	 * Set the focus flag on the widget and its parents, up
	 * to the parent window.
	 */
	do {
		if (AG_OfClass(wParent, "AG_Widget:AG_Window:*")) {
			Debug_Focus(wid, "Imply window %s focus\n", OBJECT(wParent)->name);
			AG_WindowFocus(AGWINDOW(wParent));
			break;
		}
		AG_ObjectLock(wParent);
		if ((wParent->flags & AG_WIDGET_FOCUSED) == 0) {
			if ((focusFwd = wParent->focusFwd) &&
			    !(focusFwd->flags & AG_WIDGET_FOCUSED)) {
				Debug_Focus(wid, "Imply %s focus: fwd to %s\n",
				    OBJECT(wParent)->name, OBJECT(focusFwd)->name);
				FocusWidget(focusFwd);
			}
			Debug_Focus(wid, "Imply %s focus\n", OBJECT(wParent)->name);
			FocusWidget(wParent);
		}
		AG_ObjectUnlock(wParent);
	} while ((wParent = OBJECT(wParent)->parent) != NULL);
out:
	AG_ObjectUnlock(wid);
	AG_UnlockVFS(wid);
	return (1);
fail:
	AG_ObjectUnlock(wid);
	AG_UnlockVFS(wid);
	return (0);
}

/* Acquire widget focus */
static void
FocusWidget(AG_Widget *_Nonnull wid)
{
	AG_Window *win;

	AG_OBJECT_ISA(wid, "AG_Widget:*");

	wid->flags |= AG_WIDGET_FOCUSED;
	if ((win = wid->window) != NULL) {
		AG_OBJECT_ISA(win, "AG_Widget:AG_Window:*");
		AG_PostEvent(wid, "widget-gainfocus", NULL);
		win->nFocused++;
		win->dirty = 1;
	} else {
		Debug_Focus(wid, "Gained focus, but no parent window\n");
	}
}

#ifdef HAVE_OPENGL

static void
DrawPrologueGL_Reshape(AG_Widget *_Nonnull wid)
{
	glMatrixMode(GL_PROJECTION); glPushMatrix();
	glMatrixMode(GL_MODELVIEW);  glPushMatrix();

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_PostEvent(wid, "widget-reshape", NULL);
	wid->flags &= ~(AG_WIDGET_GL_RESHAPE);

	if (wid->gl == NULL) {
		wid->gl = Malloc(sizeof(AG_WidgetGL));
	}
	glGetFloatv(GL_PROJECTION, wid->gl->mProjection);
	glGetFloatv(GL_MODELVIEW, wid->gl->mModelview);
		
	glMatrixMode(GL_PROJECTION); glPopMatrix();
	glMatrixMode(GL_MODELVIEW);  glPopMatrix();
}

static void
DrawPrologueGL(AG_Widget *_Nonnull wid)
{
	Uint hView;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_PostEvent(wid, "widget-underlay", NULL);

	glPushAttrib(GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_TEXTURE_BIT);

	if (wid->flags & AG_WIDGET_GL_RESHAPE)
		DrawPrologueGL_Reshape(wid);

	hView = AGDRIVER_SINGLE(wid->drv) ? AGDRIVER_SW(wid->drv)->h :
	                                    HEIGHT(wid->window);
	glViewport(wid->rView.x1, (hView - wid->rView.y2),
	           WIDTH(wid), HEIGHT(wid));

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

#ifdef AG_DEBUG
	if (wid->gl == NULL) { AG_FatalError("!wid->gl"); }
#endif
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(wid->gl->mProjection);
		
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadMatrixf(wid->gl->mModelview);

	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);
}

static void
DrawEpilogueGL(AG_Widget *_Nonnull wid)
{
	glMatrixMode(GL_MODELVIEW);	glPopMatrix();
	glMatrixMode(GL_PROJECTION);	glPopMatrix();
	glMatrixMode(GL_TEXTURE);	glPopMatrix();

	glPopAttrib(); /* GL_TRANSFORM_BIT | GL_VIEWPORT_BIT | GL_TEXTURE_BIT */
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_PostEvent(wid, "widget-overlay", NULL);
}
#endif /* HAVE_OPENGL */

/*
 * Render a widget to the display.
 * Must be invoked from GUI rendering context.
 */
void
AG_WidgetDraw(void *p)
{
	AG_Widget *wid = p;
	int flags;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	flags = wid->flags;

	if (!(wid->flags & AG_WIDGET_VISIBLE) ||
	     (wid->flags & AG_WIDGET_UNDERSIZE) ||
	     WIDGET_OPS(wid)->draw == NULL)
		goto out;

	if (flags & AG_WIDGET_DISABLED) {       wid->state = AG_DISABLED_STATE; }
	else if (flags & AG_WIDGET_MOUSEOVER) { wid->state = AG_HOVER_STATE; }
	else if (flags & AG_WIDGET_FOCUSED) {   wid->state = AG_FOCUSED_STATE; }
	else {                                  wid->state = AG_DEFAULT_STATE; }

	if (flags & AG_WIDGET_USE_TEXT) {
		AG_PushTextState();
		AG_TextFont(wid->font);
		AG_TextColor(&wid->pal.c[wid->state][AG_TEXT_COLOR]);
	}
#ifdef HAVE_OPENGL
	if (flags & AG_WIDGET_USE_OPENGL)
		DrawPrologueGL(wid);
#endif

	WIDGET_OPS(wid)->draw(wid);
	
#ifdef HAVE_OPENGL
	if (flags & AG_WIDGET_USE_OPENGL)
		DrawEpilogueGL(wid);
#endif
	if (flags & AG_WIDGET_USE_TEXT)
		AG_PopTextState();
out:
	AG_ObjectUnlock(wid);
}

static void
SizeRequest(void *_Nonnull p, AG_SizeReq *_Nonnull r)
{
	r->w = 0;
	r->h = 0;
}

static int
SizeAllocate(void *_Nonnull p, const AG_SizeAlloc *_Nonnull a)
{
	return (0);
}

void
AG_WidgetSizeReq(void *obj, AG_SizeReq *r)
{
	AG_Widget *wid = obj;
	int useText;

	r->w = 0;
	r->h = 0;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	useText = (wid->flags & AG_WIDGET_USE_TEXT);
	if (useText) {
		AG_PushTextState();
		AG_TextFont(wid->font);
	}
	if (WIDGET_OPS(wid)->size_request != NULL) {
		WIDGET_OPS(wid)->size_request(wid, r);
	}
	if (useText) {
		AG_PopTextState();
	}
	AG_ObjectUnlock(wid);
}

void
AG_WidgetSizeAlloc(void *obj, AG_SizeAlloc *a)
{
	AG_Widget *wid = obj;
	int useText;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	useText = (wid->flags & AG_WIDGET_USE_TEXT);
	if (useText) {
		AG_PushTextState();
		AG_TextFont(wid->font);
	}
	if (a->w <= 0 || a->h <= 0) {
		a->w = 0;
		a->h = 0;
		wid->flags |= AG_WIDGET_UNDERSIZE;
	} else {
		wid->flags &= ~(AG_WIDGET_UNDERSIZE);
	}
	wid->x = a->x;
	wid->y = a->y;
	wid->w = a->w;
	wid->h = a->h;
	if (WIDGET_OPS(wid)->size_allocate != NULL) {
		if (WIDGET_OPS(wid)->size_allocate(wid, a) == -1) {
			wid->flags |= AG_WIDGET_UNDERSIZE;
		} else {
			wid->flags &= ~(AG_WIDGET_UNDERSIZE);
		}
	}
	if (useText) {
		AG_PopTextState();
	}
#ifdef HAVE_OPENGL
	wid->flags |= AG_WIDGET_GL_RESHAPE;
#endif
	AG_ObjectUnlock(wid);
}

/*
 * Test whether view coordinates x,y lie in widget's sensitivity rectangle
 * (intersected against those of all parent widgets).
 */
int
AG_WidgetSensitive(void *obj, int x, int y)
{
	AG_Widget *wid = WIDGET(obj);
	AG_Widget *widParent = wid;
	AG_Rect2 rx;

	AG_OBJECT_ISA(wid, "AG_Widget:*");

	memcpy(&rx, &wid->rSens, sizeof(AG_Rect2));

	/* XXX why not use widget's window pointer? */
	while ((widParent = OBJECT(widParent)->parent) != NULL) {
		if (AG_OfClass(widParent, "AG_Widget:AG_Window:*")) {
			break;
		}
		AG_RectIntersect2(&rx, &rx, &widParent->rSens);
	}
	return AG_RectInside2(&rx, x,y);
}

/*
 * Search for a focused widget inside a window. Return value is only valid
 * as long as the Driver VFS is locked.
 */
AG_Widget *
AG_WidgetFindFocused(void *p)
{
	AG_Widget *wid = p;
	AG_Widget *cwid, *fwid;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_LockVFS(wid);
	AG_ObjectLock(wid);

	if (!AG_OfClass(wid, "AG_Widget:AG_Window:*")) {
		if ((wid->flags & AG_WIDGET_FOCUSED) == 0 ||
		    (wid->flags & AG_WIDGET_VISIBLE) == 0 ||
		    (wid->flags & AG_WIDGET_DISABLED)) {
			goto fail;
		}
	}
	/* Search for a better match. */
	OBJECT_FOREACH_CHILD(cwid, wid, ag_widget) {
		if ((fwid = AG_WidgetFindFocused(cwid)) != NULL) {
			AG_ObjectUnlock(wid);
			AG_UnlockVFS(wid);
			return (fwid);
		}
	}

	AG_ObjectUnlock(wid);
	AG_UnlockVFS(wid);
	return (wid);
fail:
	AG_ObjectUnlock(wid);
	AG_UnlockVFS(wid);
	return (NULL);
}

/* Compute the absolute view coordinates of a widget and its descendents. */
void
AG_WidgetUpdateCoords(void *obj, int x, int y)
{
	AG_Widget *wid = obj, *chld;
	AG_Rect2 rPrev;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_LockVFS(wid);
	AG_ObjectLock(wid);
	wid->flags &= ~(AG_WIDGET_UPDATE_WINDOW);

	if (wid->drv && AGDRIVER_MULTIPLE(wid->drv) &&
	    AG_OfClass(wid, "AG_Widget:AG_Window:*")) {
		/* Multiple-window drivers use window coordinate systems */
		x = 0;
		y = 0;
	}

	rPrev = wid->rView;
	wid->rView.x1 = x;
	wid->rView.y1 = y;
	wid->rView.w = wid->w;
	wid->rView.h = wid->h;
	wid->rView.x2 = x + wid->w;
	wid->rView.y2 = y + wid->h;
	
	wid->rSens.x1 = x;
	wid->rSens.y1 = y;
	wid->rSens.w = wid->w;
	wid->rSens.h = wid->h;
	wid->rSens.x2 = x + wid->w;
	wid->rSens.y2 = y + wid->h;

	if (AG_RectCompare2(&wid->rView, &rPrev) != 0) {
		AG_PostEvent(wid, "widget-moved", NULL);
#ifdef HAVE_OPENGL
		wid->flags |= AG_WIDGET_GL_RESHAPE;
#endif
	}
	OBJECT_FOREACH_CHILD(chld, wid, ag_widget) {
		AG_WidgetUpdateCoords(chld,
		    wid->rView.x1 + chld->x,
		    wid->rView.y1 + chld->y);
	}

	AG_ObjectUnlock(wid);
	AG_UnlockVFS(wid);
}

/* Parse a generic size specification. */
enum ag_widget_sizespec
AG_WidgetParseSizeSpec(const char *s, int *w)
{
	const char *p;
	size_t len;

	len = strlen(s);
	if (len == 0) { goto syntax; }
	p = &s[len-1];

	switch (*p) {
	case '-':
		*w = 0;
		return (AG_WIDGET_FILL);
	case '%':
		*w = (int)strtol(s, NULL, 10);
		return (AG_WIDGET_PERCENT);
	case '>':
		if (s[0] != '<') { goto syntax; }
		AG_TextSize(&s[1], w, NULL);
		return (AG_WIDGET_STRINGLEN);
	case 'x':
		if (p > &s[0] && p[-1] != 'p') { goto syntax; }
		*w = (int)strtol(s, NULL, 10);
		return (AG_WIDGET_PIXELS);
	}
syntax:
	*w = 0;
	return (AG_WIDGET_BAD_SPEC);
}

/* Show a widget */
void
AG_WidgetShow(void *obj)
{
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	wid->flags &= ~(AG_WIDGET_HIDE);
	AG_PostEvent(wid, "widget-shown", NULL);
	if (wid->window) {
		AG_WindowUpdate(wid->window);
	}
	AG_ObjectUnlock(wid);
}

/* Hide a widget */
void
AG_WidgetHide(void *obj)
{
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
	wid->flags |= AG_WIDGET_HIDE;
	AG_PostEvent(wid, "widget-hidden", NULL);
	if (wid->window) {
		AG_WindowUpdate(wid->window);
	}
	AG_ObjectUnlock(wid);
}

/* Make a widget and all of its children visible. */
void
AG_WidgetShowAll(void *p)
{
	AG_Widget *wid = p;
	AG_Widget *chld;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_LockVFS(wid);
	AG_ObjectLock(wid);

	OBJECT_FOREACH_CHILD(chld, wid, ag_widget)
		AG_WidgetShowAll(chld);

	AG_PostEvent(wid, "widget-shown", NULL);

	AG_ObjectUnlock(wid);
	AG_UnlockVFS(wid);
}

/* Make a widget and all of its children invisible. */
void
AG_WidgetHideAll(void *p)
{
	AG_Widget *wid = p;
	AG_Widget *chld;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_LockVFS(wid);
	AG_ObjectLock(wid);
	
	OBJECT_FOREACH_CHILD(chld, wid, ag_widget)
		AG_WidgetHideAll(chld);

	AG_PostEvent(wid, "widget-hidden", NULL);
	
	AG_ObjectUnlock(wid);
	AG_UnlockVFS(wid);
}

static void *_Nullable
FindAtPoint(AG_Widget *_Nonnull parent, const char *_Nonnull type, int x, int y)
{
	AG_Widget *chld;
	void *p;

	OBJECT_FOREACH_CHILD(chld, parent, ag_widget) {
		if ((p = FindAtPoint(chld, type, x,y)) != NULL)
			return (p);
	}
	if ((parent->flags & AG_WIDGET_VISIBLE) &&
	    AG_OfClass(parent, type) &&
	    AG_WidgetArea(parent, x,y)) {
		return (parent);
	}
	return (NULL);
}

/* Search for widgets of the specified class enclosing the given point. */
void *_Nullable
AG_WidgetFindPoint(const char *_Nonnull type, int x, int y)
{
	AG_Driver *drv;
	AG_Window *win;
	void *p;

	AG_LockVFS(&agDrivers);
	OBJECT_FOREACH_CHILD(drv, &agDrivers, ag_driver) {
		AG_FOREACH_WINDOW_REVERSE(win, drv) {
			if ((p = FindAtPoint(WIDGET(win), type, x,y)) != NULL) {
				AG_UnlockVFS(&agDrivers);
				return (p);
			}
		}
	}
	AG_UnlockVFS(&agDrivers);
	return (NULL);
}

static void *_Nullable
FindRectOverlap(AG_Widget *_Nonnull parent, const char *_Nonnull type,
    int x, int y, int w, int h)
{
	AG_Widget *chld;
	void *p;

	OBJECT_FOREACH_CHILD(chld, parent, ag_widget) {
		if ((p = FindRectOverlap(chld, type, x,y,w,h)) != NULL)
			return (p);
	}
	if (AG_OfClass(parent, type) &&
	    !(x+w < parent->rView.x1 || x > parent->rView.x2 ||
	      y+w < parent->rView.y1 || y > parent->rView.y2)) {
		return (parent);
	}
	return (NULL);
}

/*
 * Search for widgets of the specified class enclosing the given rectangle.
 * Result is only accurate as long as the Driver VFS is locked.
 */
void *
AG_WidgetFindRect(const char *type, int x, int y, int w, int h)
{
	AG_Driver *drv;
	AG_Window *win;
	void *p;
	
	AG_LockVFS(&agDrivers);
	OBJECT_FOREACH_CHILD(drv, &agDrivers, ag_driver) {
		AG_FOREACH_WINDOW_REVERSE(win, drv) {
			if ((p = FindRectOverlap(WIDGET(win), type, x,y,w,h)) != NULL) {
				AG_UnlockVFS(&agDrivers);
				return (p);
			}
		}
	}
	AG_UnlockVFS(&agDrivers);
	return (NULL);
}

/* Generic inherited draw() routine. */
void
AG_WidgetInheritDraw(void *obj)
{
	AG_OBJECT_ISA(obj, "AG_Widget:*");
	WIDGET_SUPER_OPS(obj)->draw(obj);
}

/* Generic inherited size_request() routine. */
void
AG_WidgetInheritSizeRequest(void *obj, AG_SizeReq *r)
{
	AG_OBJECT_ISA(obj, "AG_Widget:*");
	WIDGET_SUPER_OPS(obj)->size_request(obj, r);
}

/* Generic inherited size_allocate() routine. */
int
AG_WidgetInheritSizeAllocate(void *obj, const AG_SizeAlloc *a)
{
	AG_OBJECT_ISA(obj, "AG_Widget:*");
	return WIDGET_SUPER_OPS(obj)->size_allocate(obj, a);
}

/* Render a widget to an AG_Surface(3). */
AG_Surface *
AG_WidgetSurface(void *obj)
{
	AG_Widget *wid = obj;
	AG_Surface *S;
	int rv;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_LockVFS(wid);
	if (wid->drvOps->renderToSurface == NULL) {
		AG_SetError("renderToSurface not supported by driver");
		rv = -1;
	} else {
		rv = wid->drvOps->renderToSurface(wid->drv, wid, &S);
	}
	AG_UnlockVFS(wid);
	return (rv == 0) ? S : NULL;
}

/*
 * Attach a surface to a Widget and return an integer surface handle.
 *
 * The returned handle is unique to the Widget, and is also an index into its
 * surfaces[] and surfaceFlags[]. If textures are supported, it is also an
 * index into textures[] and texcoords[].
 *
 * The surface will be freed automatically when the widget is destroyed
 * (unless the NODUP bit is set in surfaceFlags).
 */
int
AG_WidgetMapSurface(void *obj, AG_Surface *S)
{
	AG_Widget *wid = obj;
	int i, n, s = -1;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);

	n = wid->nSurfaces;
	for (i = 0; i < n; i++) {
		if (wid->surfaces[i] == NULL) {
			s = i;
			break;
		}
	}
	if (i == n) {
		++n;
		wid->surfaces= Realloc(wid->surfaces, n*sizeof(AG_Surface *));
		wid->surfaceFlags = Realloc(wid->surfaceFlags, n*sizeof(Uint));
		wid->textures = Realloc(wid->textures, n*sizeof(Uint));
		wid->texcoords = Realloc(wid->texcoords, n*sizeof(AG_TexCoord));
		s = wid->nSurfaces++;
	}
	wid->surfaces[s] = S;
	wid->surfaceFlags[s] = 0;
	wid->textures[s] = 0;

	if (S) {
		S->flags |= AG_SURFACE_MAPPED;
	}
	AG_ObjectUnlock(wid);
	return (s);
}

/*
 * Request update of any hardware copy of a given mapped surface.
 * If hardware textures are not used, this is a no-op.
 */
void
AG_WidgetUpdateSurface(void *obj, int name)
{
#ifdef HAVE_OPENGL
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
# ifdef AG_DEBUG
	if (name < 0 || name >= wid->nSurfaces)
		AG_FatalError("No such surface");
# endif
	wid->surfaceFlags[name] |= AG_WIDGET_SURFACE_REGEN;
#endif
}

/*
 * Delete and free all resources associated with a mapped surface.
 * If a hardware texture was created, it is deleted asynchronously.
 */
void
AG_WidgetUnmapSurface(void *obj, int name)
{
	AG_OBJECT_ISA(obj, "AG_Widget:*");
	AG_WidgetReplaceSurface(obj, name, NULL);
}

/*
 * Perform an image transfer from a mapped surface (by name) to specified
 * target coordinates x,y (relative to the widget). If a texture unit is
 * available, this is done entirely in hardware.
 *
 * This must be called from rendering context (the Widget draw() operation).
 */
void
AG_WidgetBlitSurface(void *obj, int name, int x, int y)
{
	AG_OBJECT_ISA(obj, "AG_Widget:*");
	AG_WidgetBlitFrom(obj, name, NULL, x,y);
}

/*
 * Replace the contents of a mapped surface. Passing S => NULL is equivalent
 * to calling AG_WidgetUnmapSurface().
 */
void
AG_WidgetReplaceSurface(void *obj, int s, AG_Surface *S)
{
	AG_Widget *wid = obj;
	AG_Surface *Sprev;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_ObjectLock(wid);
#ifdef AG_DEBUG
	if (s < 0 || s >= wid->nSurfaces)
		AG_FatalError("No such surface");
#endif
	if ((Sprev = wid->surfaces[s]) && !WSURFACE_NODUP(wid,s)) {
		Sprev->flags &= ~(AG_SURFACE_MAPPED);
		AG_SurfaceFree(Sprev);
	}
	if (S) {
		S->flags |= AG_SURFACE_MAPPED;
	}
	wid->surfaces[s] = S;
	wid->surfaceFlags[s] &= ~(AG_WIDGET_SURFACE_NODUP);

	/*
	 * Queue the previous texture for deletion and set the texture handle
	 * to 0 so the texture will be regenerated at the next blit.
	 */
	if (wid->textures[s] != 0 && wid->drv) {
		AG_OBJECT_ISA(wid->drv, "AG_Driver:*");
		if (wid->drvOps->deleteTexture != NULL) {
			wid->drvOps->deleteTexture(wid->drv, wid->textures[s]);
			wid->textures[s] = 0;
		}
	}
	AG_ObjectUnlock(wid);
}

static void
Apply_Font_Size(AG_FontPts *fontSize, AG_FontPts parentFontSize, const char *spec)
{
	char *ep;
#ifdef HAVE_FLOAT
	double v;

	v = strtod(spec, &ep);
	*fontSize = (*ep == '%') ? parentFontSize * (v / 100.0) :  v;
#else
	*fontSize = (int)strtol(spec, &ep, 10);
	if (*ep == '%')
		*fontSize = parentFontSize * (*fontSize) / 100;
#endif
}

static void
Apply_Font_Weight(Uint *fontFlags, Uint parentFontFlags, const char *spec)
{
	if (AG_Strcasecmp(spec, "bold") == 0) {
		*fontFlags |= AG_FONT_BOLD;
	} else if (AG_Strcasecmp(spec, "normal") == 0) {
		*fontFlags &= ~(AG_FONT_BOLD);
	} else if (AG_Strcasecmp(spec, "!parent") == 0) {
		if (parentFontFlags & AG_FONT_BOLD) {
			*fontFlags &= ~(AG_FONT_BOLD);
		} else {
			*fontFlags |= AG_FONT_BOLD;
		}
	}
}
	
static void
Apply_Font_Style(Uint *fontFlags, Uint parentFontFlags, const char *spec)
{
	if (AG_Strcasecmp(spec, "italic") == 0) {
		*fontFlags |= AG_FONT_ITALIC;
	} else if (AG_Strcasecmp(spec, "normal") == 0) {
		*fontFlags &= ~(AG_FONT_ITALIC);
	} else if (AG_Strcasecmp(spec, "!parent") == 0) {
		if (parentFontFlags & AG_FONT_ITALIC) {
			*fontFlags &= ~(AG_FONT_ITALIC);
		} else {
			*fontFlags |= AG_FONT_ITALIC;
		}
	}
}

/*
 * Compile style attributes of a widget and its children. Generate color
 * palette and load any required fonts in the process.
 *
 * Per-instance (AG_SetStyle()-set) attributes have precedence over those
 * of the AG_Stylesheet(3). By default, attributes are inherited from parent.
 */
static void
CompileStyleRecursive(AG_Widget *_Nonnull wid, const char *_Nonnull parentFace,
    AG_FontPts parentFontSize, Uint parentFontFlags,
    const AG_WidgetPalette *parentPalette)
{
	AG_StyleSheet *css = &agDefaultCSS;
	char *fontFace, *cssData;
	AG_Widget *chld;
	AG_Variable *V;
	AG_Object *po;
	AG_FontPts fontSize;
	Uint fontFlags = parentFontFlags;
	int i, j;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");

	/* Select the effective style sheet for this widget. */
	for (po = OBJECT(wid);
	     po->parent && AG_OfClass(po->parent, "AG_Widget:*");
	     po = po->parent) {
		if (WIDGET(po)->css) {
			css = WIDGET(po)->css;
			break;
		}
	}

	/*
	 * Apply the font attributes.
	 */
	if ((V = AG_AccessVariable(wid, "font-family")) != NULL) {
		fontFace = Strdup(V->data.s);
		AG_UnlockVariable(V);
	} else if (AG_LookupStyleSheet(css, wid, "font-family", &cssData)) {
		fontFace = Strdup(cssData);
	} else {
		fontFace = Strdup(parentFace);
	}
	if ((V = AG_AccessVariable(wid, "font-size")) != NULL) {
		Apply_Font_Size(&fontSize, parentFontSize, V->data.s);
		AG_UnlockVariable(V);
	} else if (AG_LookupStyleSheet(css, wid, "font-size", &cssData)) {
		Apply_Font_Size(&fontSize, parentFontSize, cssData);
	} else {
		fontSize = parentFontSize;
	}
	if ((V = AG_AccessVariable(wid, "font-weight")) != NULL) {
		Apply_Font_Weight(&fontFlags, parentFontFlags, V->data.s);
		AG_UnlockVariable(V);
	} else if (AG_LookupStyleSheet(css, wid, "font-weight", &cssData)) {
		Apply_Font_Weight(&fontFlags, parentFontFlags, cssData);
	} else {
		fontFlags &= ~(AG_FONT_BOLD);
		fontFlags |= (parentFontFlags & AG_FONT_BOLD);
	}
	if ((V = AG_AccessVariable(wid, "font-style")) != NULL) {
		Apply_Font_Style(&fontFlags, parentFontFlags, V->data.s);
		AG_UnlockVariable(V);
	} else if (AG_LookupStyleSheet(css, wid, "font-style", &cssData)) {
		Apply_Font_Style(&fontFlags, parentFontFlags, cssData);
	} else {
		fontFlags &= ~(AG_FONT_ITALIC);
		fontFlags |= (parentFontFlags & AG_FONT_ITALIC);
	}
	
	/*
	 * Compile the color palette.
	 */
	for (i = 0; i < AG_WIDGET_NSTATES; i++) {
		for (j = 0; j < AG_WIDGET_NCOLORS; j++) {
			const AG_Color *parentColor = &parentPalette->c[i][j];
			char name[AG_VARIABLE_NAME_MAX];

			Strlcpy(name, agWidgetColorNames[j], sizeof(name));
			Strlcat(name, agWidgetStateNames[i], sizeof(name));

			if ((V = AG_AccessVariable(wid, name)) != NULL) {
				AG_ColorFromString(&wid->pal.c[i][j],
				    V->data.s, parentColor);
				AG_UnlockVariable(V);
			} else if (AG_LookupStyleSheet(css, wid, name, &cssData)) {
				AG_ColorFromString(&wid->pal.c[i][j],
				    cssData, parentColor);
			} else {
				Strlcpy(name, agWidgetColorNames[j], sizeof(name));
				if (AG_LookupStyleSheet(css, wid, name, &cssData)) {
					AG_ColorFromString(&wid->pal.c[i][j],
					    cssData, parentColor);
				} else {
					wid->pal.c[i][j] = *parentColor;
				}
			}
		}
	}

	/* Load any font required */
	if (wid->flags & AG_WIDGET_USE_TEXT) {
		char *pFace = fontFace, *tok;
		AG_Font *fontNew = NULL;

		while ((tok = AG_Strsep(&pFace, ",")) != NULL) {
			fontNew = AG_FetchFont(fontFace, &fontSize, fontFlags);
			if (fontNew != NULL)
				break;
		}
		if (fontNew == NULL) {
			fontNew = AG_FetchFont(NULL, &fontSize, fontFlags);
			AG_OBJECT_ISA(fontNew, "AG_Font:*");
		}
		if (fontNew != NULL && wid->font != fontNew) {
			if (wid->font != NULL) {
				AG_OBJECT_ISA(wid->font, "AG_Font:*");
				AG_UnusedFont(wid->font);
			}
			wid->font = fontNew;
			AG_PushTextState();
			AG_TextFont(wid->font);
			AG_PostEvent(wid, "font-changed", NULL);
			AG_PopTextState();
			AG_Redraw(wid);
		}
	}

	OBJECT_FOREACH_CHILD(chld, wid, ag_widget) {
		CompileStyleRecursive(chld,
		    fontFace, fontSize, fontFlags,
		    &wid->pal);
	}
	
	free(fontFace);
}
void
AG_WidgetCompileStyle(void *obj)
{
	AG_Widget *wid = obj;
	AG_Widget *parent;
	AG_Font *parentFont;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_LockVFS(wid);
	AG_MutexLock(&agTextLock);

	if ((parent = OBJECT(wid)->parent) != NULL &&
	    AG_OfClass(parent, "AG_Widget:*") &&
	    (parentFont = parent->font) != NULL) {
		CompileStyleRecursive(wid,	/* Inheritable attributes: */
		    OBJECT(parentFont)->name,	/* "font-family" */
		    parentFont->spec.size,	/* "font-size" */
		    parentFont->flags,		/* "font-{style,weight}" */
		    &parent->pal);		/* and the color palette */
	} else {
		CompileStyleRecursive(wid,
		    OBJECT(agDefaultFont)->name,
		    agDefaultFont->spec.size,
		    agDefaultFont->flags,
		    &agDefaultPalette);
	}

	AG_MutexUnlock(&agTextLock);
	AG_UnlockVFS(wid);
}

/*
 * Clear the style parameters of a widget and its descendants. Called
 * on detach. The widget's VFS must be locked.
 */
void
AG_WidgetFreeStyle(void *obj)
{
	AG_Widget *wid = obj;
	AG_Widget *chld;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");
	if (wid->font) {
		AG_OBJECT_ISA(wid->font, "AG_Font:*");
		AG_UnusedFont(wid->font);
		wid->font = NULL;
	}
	OBJECT_FOREACH_CHILD(chld, wid, ag_widget)
		AG_WidgetFreeStyle(chld);
}

/* Copy all style properties from one widget to another. */
void
AG_WidgetCopyStyle(void *objDst, void *objSrc)
{
	AG_Widget *widSrc = objSrc;
	AG_Widget *widDst = objDst;
	AG_Variable *V;
	const char **s;

	AG_OBJECT_ISA(widSrc, "AG_Widget:*");
	AG_OBJECT_ISA(widDst, "AG_Widget:*");
	AG_ObjectLock(widSrc);
	AG_ObjectLock(widDst);
	for (s = &agWidgetStyleNames[0]; *s != NULL; s++) {
		if ((V = AG_AccessVariable(widSrc, *s)) != NULL) {
			AG_SetString(widDst, *s, V->data.s);
			AG_UnlockVariable(V);
		}

	}
	AG_ObjectUnlock(widDst);
	AG_ObjectUnlock(widSrc);

	AG_WidgetCompileStyle(widDst);
	AG_Redraw(widDst);
}

/*
 * Set the default font parameters for a widget.
 * If a NULL argument is provided, the parameter is inherited from parent.
 */
void
AG_SetFont(void *obj, const AG_Font *font)
{
	AG_Widget *wid = obj;

	AG_OBJECT_ISA(wid, "AG_Widget:*");
	AG_OBJECT_ISA(font, "Font:*");

	AG_SetString(wid, "font-family", OBJECT(font)->name);
	AG_SetStringF(wid, "font-size", "%.2fpts", font->spec.size);
	AG_SetString(wid, "font-weight", (font->flags & AG_FONT_BOLD) ? "bold" : "normal");
	AG_SetString(wid, "font-style", (font->flags & AG_FONT_ITALIC) ? "italic" : "normal");

	AG_WidgetCompileStyle(wid);
	AG_Redraw(wid);
}

/* Set the value of a CSS parameter to the given formatted string. */
void
AG_SetStyleF(void *obj, const char *which, const char *fmt, ...)
{
	AG_Widget *wid = obj;
	va_list ap;
	char *val;

	va_start(ap, fmt);
	Vasprintf(&val, fmt, ap);
	va_end(ap);

	AG_SetStyle(wid, which, val);
	free(val);
}

/* Set the value of a CSS parameter to the given string (or NULL = unset it). */
void
AG_SetStyle(void *obj, const char *which, const char *value)
{
	AG_Widget *wid = obj;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");

	if (value) {
		AG_SetString(wid, which, value);
	} else {
		AG_Unset(wid, which);				/* inherit */
	}
	AG_WidgetCompileStyle(wid);
	AG_Redraw(wid);
}

AG_WidgetClass agWidgetClass = {
	{
		"Agar(Widget)",
		sizeof(AG_Widget),
		{ 0,0 },
		Init,
		NULL,		/* reset */
		Destroy,
		NULL,		/* load */
		NULL,		/* save */
		NULL		/* edit */
	},
	NULL,		/* draw */
	SizeRequest,
	SizeAllocate
};
