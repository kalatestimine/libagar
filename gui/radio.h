/*	Public domain	*/

#ifndef _AGAR_WIDGET_RADIO_H_
#define _AGAR_WIDGET_RADIO_H_

#include <agar/gui/widget.h>

#include <agar/gui/begin.h>

#ifndef AG_RADIO_TEXT_MAX
#define AG_RADIO_TEXT_MAX (AG_MODEL*2)
#endif

typedef struct ag_radio_item {
	char text[AG_RADIO_TEXT_MAX];	/* Display text */
	int  surface;			/* Cached surface */
	AG_KeySym hotkey;		/* Associated key (or AG_KEY_NONE) */
} AG_RadioItem;

typedef struct ag_radio {
	struct ag_widget wid;		/* AG_Widget -> AG_Radio */
	Uint flags;
#define AG_RADIO_HFILL	0x01
#define AG_RADIO_VFILL	0x02
#define AG_RADIO_EXPAND (AG_RADIO_HFILL|AG_RADIO_VFILL)

	int value;			/* Default "value" binding */

	AG_RadioItem *_Nullable items;	/* Array of items */
	int                    nItems;
	int                  selitem;	/* Selected item index */

	int max_w;			/* Width of widest label */
	int oversel;			/* Overlapping selection */
	int xPadding, yPadding;
	int xSpacing, ySpacing;
	int radius;			/* Control radius in pixels */
	int itemHeight;
	AG_Rect r;			/* Display area */
	int wPre, hPre;			/* Size hint */
} AG_Radio;

#define AGRADIO(obj)            ((AG_Radio *)(obj))
#define AGCRADIO(obj)           ((const AG_Radio *)(obj))
#define AG_RADIO_SELF()          AGRADIO( AG_OBJECT(0,"AG_Widget:AG_Radio:*") )
#define AG_RADIO_PTR(n)          AGRADIO( AG_OBJECT((n),"AG_Widget:AG_Radio:*") )
#define AG_RADIO_NAMED(n)        AGRADIO( AG_OBJECT_NAMED((n),"AG_Widget:AG_Radio:*") )
#define AG_CONST_RADIO_SELF()   AGCRADIO( AG_CONST_OBJECT(0,"AG_Widget:AG_Radio:*") )
#define AG_CONST_RADIO_PTR(n)   AGCRADIO( AG_CONST_OBJECT((n),"AG_Widget:AG_Radio:*") )
#define AG_CONST_RADIO_NAMED(n) AGCRADIO( AG_CONST_OBJECT_NAMED((n),"AG_Widget:AG_Radio:*") )

__BEGIN_DECLS
extern AG_WidgetClass agRadioClass;

AG_Radio *_Nonnull AG_RadioNewInt(void *_Nullable, Uint,
                                  const char *_Nullable *_Nullable,
                                  int *_Nonnull);
AG_Radio *_Nonnull AG_RadioNewUint(void *_Nullable, Uint,
                                   const char *_Nullable *_Nullable,
                                   Uint *_Nonnull);
AG_Radio *_Nonnull AG_RadioNew(void *_Nullable, Uint,
                               const char *_Nullable *_Nullable);

void AG_RadioItemsFromArray(AG_Radio *_Nonnull, const char *_Nonnull *_Nullable);

int AG_RadioAddItemS(AG_Radio *_Nonnull, const char *_Nonnull);
int AG_RadioAddItem(AG_Radio *_Nonnull, const char *_Nonnull, ...)
                   FORMAT_ATTRIBUTE(printf,2,3);
int AG_RadioAddItemHKS(AG_Radio *_Nonnull, AG_KeySym, const char *_Nonnull);
int AG_RadioAddItemHK(AG_Radio *_Nonnull, AG_KeySym, const char *_Nonnull, ...)
                     FORMAT_ATTRIBUTE(printf,3,4);

void AG_RadioClearItems(AG_Radio *_Nonnull);
void AG_RadioSizeHint(AG_Radio *_Nonnull, int, const char *);

#ifdef AG_LEGACY
AG_Radio *_Nonnull AG_RadioNewFn(void *_Nullable, Uint,
                                 const char *_Nullable *_Nullable,
                                 _Nonnull AG_EventFn, const char *_Nullable, ...)
                                DEPRECATED_ATTRIBUTE;
#endif
__END_DECLS

#include <agar/gui/close.h>
#endif /* _AGAR_WIDGET_RADIO_H_ */
