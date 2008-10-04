/*	Public domain	*/

#ifndef _AGAR_GUI_SCROLLVIEW_H_
#define _AGAR_GUI_SCROLLVIEW_H_

#include <agar/gui/widget.h>
#include <agar/gui/scrollbar.h>

#include "begin_code.h"

typedef struct ag_scrollview {
	struct ag_widget wid;

	Uint flags;
#define AG_SCROLLVIEW_HFILL	0x01
#define AG_SCROLLVIEW_VFILL	0x02
#define AG_SCROLLVIEW_PAN_X	0x04 /* X is pannable */ 
#define AG_SCROLLVIEW_PAN_Y	0x08 /* Y is pannable */
#define AG_SCROLLVIEW_PANNING	0x10 /* Panning in progress */
#define AG_SCROLLVIEW_BY_CURSOR	0x20 /* Panning with cursor allowed */
#define AG_SCROLLVIEW_BY_KBD	0x40 /* Panning with keyboard allowed */
#define AG_SCROLLVIEW_EXPAND	(AG_SCROLLVIEW_HFILL|AG_SCROLLVIEW_VFILL)
#define AG_SCROLLVIEW_PAN_XY	(AG_SCROLLVIEW_PAN_X|AG_SCROLLVIEW_PAN_Y)

	enum ag_widget_packing pack;	/* Packing mode */
	int wPre, hPre;			/* Requested geometry */
	int xOffs, yOffs;		/* Display offset */
	int xMin, xMax, yMin, yMax;	/* Display boundaries */
	AG_Scrollbar *hbar, *vbar;	/* Scrollbars for panning */
} AG_Scrollview;

__BEGIN_DECLS
extern AG_WidgetClass agScrollviewClass;

AG_Scrollview	*AG_ScrollviewNew(void *, Uint);
void		 AG_ScrollviewSizeHint(AG_Scrollview *, Uint, Uint);
__END_DECLS

#include "close_code.h"
#endif /* _AGAR_GUI_SCROLLVIEW_H_ */