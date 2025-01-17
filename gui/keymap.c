/*
 * Copyright (c) 2002-2019 Julien Nadeau Carriere <vedge@csoft.net>
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
 * Keyboard input processing for AG_Editable(3).
 */

#include <agar/core/core.h>
#include <agar/gui/widget.h>
#include <agar/gui/window.h>
#include <agar/gui/editable.h>
#include <agar/gui/keymap.h>
#include <agar/gui/text.h>
#include <agar/gui/gui_math.h>

#include <ctype.h>
#include <string.h>

/* Insert a new character at current cursor position. */
static int
Insert(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	AG_Char ins[3];
	AG_Size len;
	int i, nIns, pos;

	if (keysym == 0)
		return (0);
#ifdef __APPLE__
	if ((keymod & AG_KEYMOD_LMETA) ||
	    (keymod & AG_KEYMOD_RMETA))
		return (0);
#endif

#ifdef AG_UNICODE
	if (!(ed->flags & AG_EDITABLE_NOLATIN1)) {
		for (i = 0; ; i++) {
			const struct ag_key_mapping *km = &agKeymapLATIN1[i];

			if (keysym == km->key) {
				if (((keymod & AG_KEYMOD_ALT) &&
				     (keymod & AG_KEYMOD_SHIFT) &&
				     (km->modmask == (AG_KEYMOD_ALT|AG_KEYMOD_SHIFT)))) {
					ch = km->ch;
					break;
				} else if (keymod & AG_KEYMOD_ALT &&
				    km->modmask == AG_KEYMOD_ALT) {
					ch = km->ch;
					break;
				}
			} else if (km->key == AG_KEY_LAST) {
				break;
			}
		}
	}
#endif /* AG_UNICODE */

	if (ch == 0) { return (0); }
	if (ch == '\r') { ch = '\n'; }

	if (Strcasecmp(ed->encoding, "US-ASCII") == 0 &&
	    (ch & ~0x7f) != 0)
		return (0);

#ifdef AG_UNICODE
	if (agTextComposition) {
		if ((nIns = AG_KeyInputCompose(ed, ch, ins)) == 0)
			return (0);
	} else
#endif
	{
		ins[0] = ch;
		nIns = 1;
	}
	ins[nIns] = '\0';

	if (ed->sel != 0) {
		AG_EditableDelete(ed, buf);
	}
	if (AG_EditableGrowBuffer(ed, buf, ins, (AG_Size)nIns) == -1) {
		Verbose("Insert Failed: %s\n", AG_GetError());
		return (0);
	}

	pos = ed->pos;
	len = buf->len;
	if (pos == len) {					/* Append */
		for (i = 0; i < nIns; i++)
			buf->s[len+i] = ins[i];
	} else {						/* Insert */
		memmove(&buf->s[pos+nIns], &buf->s[pos],
		        (len-pos)*sizeof(AG_Char));
		for (i = 0; i < nIns; i++)
			buf->s[pos+i] = ins[i];
	}
	buf->len += nIns;
	buf->s[buf->len] = '\0';
	ed->pos += nIns;

#ifdef AG_UNICODE
	if (!(ed->flags & AG_EDITABLE_MULTILINE)) {	/* Optimize case */
		int wIns;
		AG_TextSizeNat(ins, &wIns, NULL);
		ed->xScrollPx += wIns;
	} else
#endif
	{
		ed->xScrollTo = &ed->xCurs;
		ed->yScrollTo = &ed->yCurs;
	}
	ed->flags |= AG_EDITABLE_BLINK_ON;
	return (1);
}

/* Delete the character at cursor, or the active selection. */
static int
Delete(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	AG_Char *c;
	AG_Size len;
#ifdef AG_UNICODE
	int wDel;
#endif
	if ((len = buf->len) == 0)
		return (0);

	if (ed->sel != 0) {
		AG_EditableDelete(ed, buf);
		return (1);
	}
	if (keysym == AG_KEY_BACKSPACE && ed->pos == 0) {
		return (0);
	}
	if (ed->pos == len) { 
		ed->pos--;
		buf->s[--len] = '\0';
		buf->len--;

		if (ed->flags & AG_EDITABLE_MULTILINE) {
			ed->xScrollTo = &ed->xCurs;
			ed->yScrollTo = &ed->yCurs;
		} else {
#ifdef AG_UNICODE
			if (len > 0) {
				AG_TextSizeNat(&buf->s[len-1], &wDel, NULL);
				if (ed->x > 0) { ed->x -= wDel; }
			}
#else
			/* TODO */
#endif
		}
		return (1);
	}
	if (keysym == AG_KEY_BACKSPACE) {
		if (ed->pos > 0)
			ed->pos--;
	}

	if (ed->flags & AG_EDITABLE_MULTILINE) {
		ed->xScrollTo = &ed->xCurs;
		ed->yScrollTo = &ed->yCurs;
	} else {
#ifdef AG_UNICODE
		AG_Char cDel[2];
	
		cDel[0] = buf->s[ed->pos];
		cDel[1] = '\0';
		AG_TextSizeNat(cDel, &wDel, NULL);
		if (ed->x > 0) { ed->x -= wDel; }
#else
		/* TODO */
#endif
	}

	for (c = &buf->s[ed->pos];
	     c < &buf->s[len+1];
	     c++) {
		*c = c[1];
		if (*c == '\0')
			break;
	}
	buf->len--;
	return (1);
}

/* Copy the selection to clipboard. */
static int
Copy(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	AG_EditableCopy(ed, buf, &agEditableClipbrd);
	return (0);
}

/* Copy selection to clipboard and subsequently delete it. */
static int
Cut(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	return AG_EditableCut(ed, buf, &agEditableClipbrd);
}

/* Paste clipboard contents to current cursor position. */
static int
Paste(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	return AG_EditablePaste(ed, buf, &agEditableClipbrd);
}

/*
 * Kill the current selection; if there is no selection, cut the
 * characters up to the end of the line (Emacs-style).
 */
static int
Kill(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	if (ed->sel != 0) {
		AG_EditableValidateSelection(ed, buf);
		if (ed->sel < 0) {
			ed->pos += ed->sel;
			ed->sel = -(ed->sel);
		}
	} else {
		AG_Char *c;

		for (c = &buf->s[ed->pos]; c < &buf->s[buf->len]; c++) {
			if (*c == '\n') {
				break;
			}
			ed->sel++;
		}
		if (ed->sel == 0)
			return (0);
	}
	
	AG_EditableCopyChunk(ed, &agEditableKillring, &buf->s[ed->pos], ed->sel);
	AG_EditableDelete(ed, buf);
	return (1);
}

/* Paste the contents of the Emacs-style kill ring at cursor position. */
static int
Yank(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	return AG_EditablePaste(ed, buf, &agEditableKillring);
}

/* Seek one word backwards. */
static int
WordBack(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	int newPos = ed->pos;
	AG_Char *c;

	/* XXX: handle other types of spaces */
	if (ed->pos > 1 && buf->s[newPos-1] == ' ') {
		newPos -= 2;
	}

	for (c = &buf->s[newPos];
	     c > &buf->s[0] && *c != ' ';
	     c--, newPos--)
		;;
	if (*c == ' ')
		newPos++;

	if (keymod & AG_KEYMOD_SHIFT) {
		ed->sel += (ed->pos - newPos);
	} else {
		ed->sel = 0;
	}
	ed->pos = newPos;

	ed->flags |= AG_EDITABLE_MARKPREF;
	ed->flags |= AG_EDITABLE_BLINK_ON;
	ed->xScrollTo = &ed->xCurs;
	ed->yScrollTo = &ed->yCurs;
	AG_Redraw(ed);
	return (0);
}

/* Seek one word forward. */
static int
WordForw(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	int newPos = ed->pos;
	AG_Char *c;
	
	if (newPos == buf->len) {
		return (0);
	}
	if (buf->len > 1 && buf->s[newPos] == ' ')
		newPos++;
	
	for (c = &buf->s[newPos];
	     *c != '\0' && *c != ' ';
	     c++, newPos++)
		;;

	if (keymod & AG_KEYMOD_SHIFT) {
		ed->sel += (ed->pos - newPos);
	} else {
		ed->sel = 0;
	}
	ed->pos = newPos;

	ed->flags |= AG_EDITABLE_MARKPREF;
	ed->flags |= AG_EDITABLE_BLINK_ON;
	ed->xScrollTo = &ed->xCurs;
	ed->yScrollTo = &ed->yCurs;
	AG_Redraw(ed);
	return (0);
}

/* Select all. */
static int
SelectAll(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
   AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	AG_EditableSelectAll(ed, buf);
	return (0);
}

/* Move cursor to beginning of line. */
static int
CursorHome(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	int newPos = ed->pos;
	AG_Char *c;

	if (ed->flags & AG_EDITABLE_MULTILINE) {
		if (newPos == 0) {
			return (0);
		}
		for (c = &buf->s[newPos - 1];
		     c >= &buf->s[0] && newPos >= 0;
		     c--, newPos--) {
			if (*c == '\n')
				break;
		}
	} else {
		newPos = 0;
	}

	if (keymod & AG_KEYMOD_SHIFT) {
		ed->sel += (ed->pos - newPos);
	} else {
		ed->sel = 0;
	}
	ed->pos = newPos;

	ed->x = 0;
	ed->flags |= AG_EDITABLE_MARKPREF;
	AG_Redraw(ed);
	return (0);
}

/* Move cursor to end of line. */
static int
CursorEnd(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	int newPos = ed->pos;
	AG_Char *c;

	if (ed->flags & AG_EDITABLE_MULTILINE) {
		if (newPos == buf->len || buf->s[newPos] == '\n') {
			return (0);
		}
		for (c = &buf->s[newPos + 1];
		     c <= &buf->s[buf->len] && newPos <= buf->len;
		     c++, newPos++) {
			if (*c == '\n') {
				newPos++;
				break;
			}
		}
		if (newPos > buf->len)
			newPos = buf->len;
	} else {
		newPos = buf->len;
	}

	if (keymod & AG_KEYMOD_SHIFT) {
		ed->sel += (ed->pos - newPos);
	} else {
		ed->sel = 0;
	}
	ed->pos = newPos;

	ed->flags |= AG_EDITABLE_MARKPREF;
	ed->xScrollTo = &ed->xCurs;
	ed->yScrollTo = &ed->yCurs;
	AG_Redraw(ed);
	return (0);
}

/* Move cursor left. */
static int
CursorLeft(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	if ((ed->pos - 1) >= 0) {
		ed->pos--;
		if (keymod & AG_KEYMOD_SHIFT) {
			ed->sel++;
		} else {
			ed->sel = 0;
		}
	} else {
		ed->pos = 0;
	}
	ed->flags |= AG_EDITABLE_MARKPREF;
	ed->flags |= AG_EDITABLE_BLINK_ON;
	ed->xScrollTo = &ed->xCurs;
	ed->yScrollTo = &ed->yCurs;
	AG_Redraw(ed);
	return (0);
}

/* Move cursor right. */
static int
CursorRight(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	if (ed->pos < buf->len) {
		ed->pos++;
		if (keymod & AG_KEYMOD_SHIFT) {
			ed->sel--;
		} else {
			ed->sel = 0;
		}
	}
	ed->flags |= AG_EDITABLE_MARKPREF;
	ed->flags |= AG_EDITABLE_BLINK_ON;
	ed->xScrollTo = &ed->xCurs;
	ed->yScrollTo = &ed->yCurs;
	AG_Redraw(ed);
	return (0);
}

/* Move cursor up in a multi-line string. */
static int
CursorUp(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	int prevPos = ed->pos;
	int prevSel = ed->sel;

	if (!(ed->flags & AG_EDITABLE_MULTILINE))
		return (0);

	AG_EditableMoveCursor(ed, buf, ed->xCursPref,
	    (ed->yCurs - ed->y - 1)*agTextFontLineSkip + 1);

	if (keymod & AG_KEYMOD_SHIFT) {
		ed->sel = prevSel - (ed->pos - prevPos);
	} else {
		ed->sel = 0;
	}

	ed->flags |= AG_EDITABLE_BLINK_ON;
	ed->xScrollTo = &ed->xCurs;
	ed->yScrollTo = &ed->yCurs;
	AG_Redraw(ed);
	return (0);
}

/* Move cursor down in a multi-line string. */
static int
CursorDown(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	int prevPos = ed->pos;
	int prevSel = ed->sel;

	if (!(ed->flags & AG_EDITABLE_MULTILINE))
		return (0);

	AG_EditableMoveCursor(ed, buf, ed->xCursPref,
	    (ed->yCurs - ed->y + 1)*agTextFontLineSkip + 1);

	if (keymod & AG_KEYMOD_SHIFT) {
		ed->sel = prevSel - (ed->pos - prevPos);
	} else {
		ed->sel = 0;
	}

	ed->flags |= AG_EDITABLE_BLINK_ON;
	ed->xScrollTo = &ed->xCurs;
	ed->yScrollTo = &ed->yCurs;
	AG_Redraw(ed);
	return (0);
}

/* Move cursor one page up in a multi-line string. */
static int
PageUp(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	int prevPos = ed->pos;
	int prevSel = ed->sel;

	if (!(ed->flags & AG_EDITABLE_MULTILINE))
		return (0);

	AG_EditableMoveCursor(ed, buf, ed->xCurs,
	    (ed->yCurs - ed->y - ed->yVis)*agTextFontLineSkip + 1);
	
	if (keymod & AG_KEYMOD_SHIFT) {
		ed->sel = prevSel - (ed->pos - prevPos);
	} else {
		ed->sel = 0;
	}

	ed->xScrollTo = &ed->xCurs;
	ed->yScrollTo = &ed->yCurs;
	AG_Redraw(ed);
	return (0);
}

/* Move cursor one page down in a multi-line string. */
static int
PageDown(AG_Editable *_Nonnull ed, AG_EditableBuffer *_Nonnull buf,
    AG_KeySym keysym, Uint keymod, AG_Char ch)
{
	int prevPos = ed->pos;
	int prevSel = ed->sel;

	if (!(ed->flags & AG_EDITABLE_MULTILINE))
		return (0);

	AG_EditableMoveCursor(ed, buf, ed->xCurs,
	    (ed->yCurs - ed->y + ed->yVis)*agTextFontLineSkip + 1);

	if (keymod & AG_KEYMOD_SHIFT) {
		ed->sel = prevSel - (ed->pos - prevPos);
	} else {
		ed->sel = 0;
	}

	ed->xScrollTo = &ed->xCurs;
	ed->yScrollTo = &ed->yCurs;
	AG_Redraw(ed);
	return (0);
}

/*
 * Map AG_KeySym(3) and AG_KeyMod(3) values to AG_Editable functions.
 * Variants with modifier keys must appear first in the list. The
 * modifiers string is passed to AG_CompareKeyMods(3).
 *
 * Available flags:
 *	"w" = Require a writeable buffer
 *	"e" = AG_EDITABLE_NOEMACS must be unset
 */
const struct ag_keycode agKeymap[] = {
#ifdef __APPLE__
	{ AG_KEY_LEFT,	AG_KEYMOD_CTRL | AG_KEYMOD_META, CursorHome, "" },
	{ AG_KEY_RIGHT,	AG_KEYMOD_CTRL | AG_KEYMOD_META, CursorEnd,  "" },
	{ AG_KEY_LEFT,	AG_KEYMOD_ALT,  WordBack,  "" },
	{ AG_KEY_RIGHT,	AG_KEYMOD_ALT,  WordForw,  "" },
	{ AG_KEY_A,	AG_KEYMOD_META,	SelectAll, "" },
	{ AG_KEY_C,	AG_KEYMOD_META,	Copy,      "" },
	{ AG_KEY_X,	AG_KEYMOD_META,	Cut,       "w" },
	{ AG_KEY_V,	AG_KEYMOD_META,	Paste,     "w" },
	{ AG_KEY_K,	AG_KEYMOD_META,	Kill,      "we" },
	{ AG_KEY_Y,	AG_KEYMOD_META,	Yank,      "we" },
#else /* __APPLE__ */
	{ AG_KEY_LEFT,	AG_KEYMOD_CTRL,	WordBack,  "" },
	{ AG_KEY_LEFT,	AG_KEYMOD_ALT,	WordBack,  "" },
	{ AG_KEY_RIGHT,	AG_KEYMOD_CTRL,	WordForw,  "" },
	{ AG_KEY_RIGHT,	AG_KEYMOD_ALT,	WordForw,  "" },
	{ AG_KEY_A,	AG_KEYMOD_CTRL,	SelectAll, "" },
	{ AG_KEY_C,	AG_KEYMOD_CTRL,	Copy,      "" },
	{ AG_KEY_X,	AG_KEYMOD_CTRL,	Cut,       "w" },
	{ AG_KEY_V,	AG_KEYMOD_CTRL,	Paste,     "w" },
	{ AG_KEY_K,	AG_KEYMOD_CTRL,	Kill,      "we" },
	{ AG_KEY_Y,	AG_KEYMOD_CTRL,	Yank,      "we" },
#endif /* !__APPLE__ */
	{ AG_KEY_HOME,      0,	CursorHome,  "" },
	{ AG_KEY_END,       0,	CursorEnd,   "" },
	{ AG_KEY_LEFT,      0,	CursorLeft,  "" },
	{ AG_KEY_RIGHT,     0,	CursorRight, "" },
	{ AG_KEY_UP,        0,	CursorUp,    "" },
	{ AG_KEY_DOWN,      0,	CursorDown,  "" },
	{ AG_KEY_PAGEUP,    0,	PageUp,      "" },
	{ AG_KEY_PAGEDOWN,  0,	PageDown,    "" },
	{ AG_KEY_BACKSPACE, 0,	Delete,      "w" },
	{ AG_KEY_DELETE,    0,	Delete,      "w" },
	{ AG_KEY_LAST,      0,	Insert,      "w" },
};
