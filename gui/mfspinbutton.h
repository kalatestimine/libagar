/*	Public domain	*/

#ifndef _AGAR_WIDGET_MFSPINBUTTON_H_
#define _AGAR_WIDGET_MFSPINBUTTON_H_

#include <agar/config/have_float.h>
#if defined(HAVE_FLOAT)

#include <agar/gui/widget.h>
#include <agar/gui/textbox.h>
#include <agar/gui/button.h>
#include <agar/gui/ucombo.h>
#include <agar/gui/units.h>

#include <agar/gui/begin.h>

typedef struct ag_mfspinbutton {
	struct ag_widget wid;		/* AG_Widget -> AG_MFSpinbutton */
	Uint flags;
#define AG_MFSPINBUTTON_NOHFILL	0x01
#define AG_MFSPINBUTTON_VFILL	0x02
#define AG_MFSPINBUTTON_EXCL	0x04	/* Exclusive binding access */
	int writeable;			/* 0 = read-only */
	double xvalue, yvalue;		/* Default value bindings */
	double min, max;		/* Default range bindings */
	float minFlt, maxFlt;
	double inc;			/* Increment for buttons */
	char format[32];		/* Printing format */
	const char *_Nonnull sep;	/* x/y field separator */
	const AG_Unit *_Nonnull unit;	/* Conversion unit */
	char inTxt[128];		/* Input text buffer */
	AG_Textbox *_Nonnull input;	/* Input field */
	AG_UCombo  *_Nullable units;	/* Unit selector */
	AG_Button  *_Nonnull xincbu;	/* X-increment button */
	AG_Button  *_Nonnull xdecbu;	/* X-decrement button */
	AG_Button  *_Nonnull yincbu;	/* Y-increment button */
	AG_Button  *_Nonnull ydecbu;	/* Y-decrement button */
	AG_Timer updateTo;
} AG_MFSpinbutton;

#define AGMFSPINBUTTON(obj)            ((AG_MFSpinbutton *)(obj))
#define AGCMFSPINBUTTON(obj)           ((const AG_MFSpinbutton *)(obj))
#define AG_MFSPINBUTTON_SELF()          AGMFSPINBUTTON( AG_OBJECT(0,"AG_Widget:AG_MFSpinbutton:*") )
#define AG_MFSPINBUTTON_PTR(n)          AGMFSPINBUTTON( AG_OBJECT((n),"AG_Widget:AG_MFSpinbutton:*") )
#define AG_MFSPINBUTTON_NAMED(n)        AGMFSPINBUTTON( AG_OBJECT_NAMED((n),"AG_Widget:AG_MFSpinbutton:*") )
#define AG_CONST_MFSPINBUTTON_SELF()   AGCMFSPINBUTTON( AG_CONST_OBJECT(0,"AG_Widget:AG_MFSpinbutton:*") )
#define AG_CONST_MFSPINBUTTON_PTR(n)   AGCMFSPINBUTTON( AG_CONST_OBJECT((n),"AG_Widget:AG_MFSpinbutton:*") )
#define AG_CONST_MFSPINBUTTON_NAMED(n) AGCMFSPINBUTTON( AG_CONST_OBJECT_NAMED((n),"AG_Widget:AG_MFSpinbutton:*") )

__BEGIN_DECLS
extern AG_WidgetClass agMFSpinbuttonClass;

AG_MFSpinbutton	*_Nonnull AG_MFSpinbuttonNew(void *_Nullable, Uint,
                                             const char *_Nullable,
                                             const char *_Nonnull,
                                             const char *_Nullable);

void AG_MFSpinbuttonUpdate(AG_MFSpinbutton *_Nonnull);
void AG_MFSpinbuttonSetValue(AG_MFSpinbutton *_Nonnull, const char *_Nonnull, double);
void AG_MFSpinbuttonAddValue(AG_MFSpinbutton *_Nonnull, const char *_Nonnull, double);
void AG_MFSpinbuttonSetMin(AG_MFSpinbutton *_Nonnull, double);
void AG_MFSpinbuttonSetMax(AG_MFSpinbutton *_Nonnull, double);
void AG_MFSpinbuttonSetRange(AG_MFSpinbutton *_Nonnull, double,double);
void AG_MFSpinbuttonSetIncrement(AG_MFSpinbutton *_Nonnull, double);
void AG_MFSpinbuttonSelectUnit(AG_MFSpinbutton *_Nonnull, const char *_Nonnull);
void AG_MFSpinbuttonSetPrecision(AG_MFSpinbutton *_Nonnull, const char *_Nonnull,
                                 int);
void AG_MFSpinbuttonSetWriteable(AG_MFSpinbutton *_Nonnull, int);
__END_DECLS

#include <agar/gui/close.h>
#endif /* HAVE_FLOAT */
#endif /* _AGAR_WIDGET_MFSPINBUTTON_H_ */
