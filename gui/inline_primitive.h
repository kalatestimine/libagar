/*	Public domain	*/

/*
 * Write a pixel (AG_Color argument)
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_PutPixel(void *_Nonnull obj, int x, int y, const AG_Color *c)
#else
void
ag_put_pixel(void *obj, int x, int y, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->putPixel(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    c);
}

/*
 * Write a pixel (32-bit value argument in agSurfaceFmt format)
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_PutPixel32(void *_Nonnull obj, int x, int y, Uint32 c)
#else
void
ag_put_pixel_32(void *obj, int x, int y, Uint32 c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->putPixel32(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    c);
}

/*
 * Write a pixel (8-bit RGB component arguments)
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_PutPixelRGB_8(void *_Nonnull obj, int x, int y, Uint8 r, Uint8 g, Uint8 b)
#else
void
ag_put_pixel_rgb_8(void *obj, int x, int y, Uint8 r, Uint8 g, Uint8 b)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->putPixelRGB8(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    r,g,b);
}

/*
 * Blend a pixel (AG_Color argument)
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_BlendPixel(void *_Nonnull obj, int x, int y, const AG_Color *c,
    AG_AlphaFn fnSrc)
#else
void
ag_blend_pixel(void *obj, int x, int y, const AG_Color *c, AG_AlphaFn fnSrc)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->blendPixel(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    c, fnSrc, AG_ALPHA_ZERO);
}

/*
 * Blend a pixel (32-bit value argument in agSurfaceFmt format)
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_BlendPixel32(void *_Nonnull obj, int x, int y, Uint32 px, AG_AlphaFn fnSrc)
#else
void
ag_blend_pixel_32(void *obj, int x, int y, Uint32 px, AG_AlphaFn fnSrc)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Color c;

	AG_GetColor32(&c, px, agSurfaceFmt);
	wid->drvOps->blendPixel(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    &c, fnSrc, AG_ALPHA_ZERO);
}

/*
 * Blend a pixel (RGBA arguments)
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_BlendPixelRGBA(void *_Nonnull obj, int x, int y, Uint8 c[_Nonnull 4],
    AG_AlphaFn fnSrc)
#else
void
ag_blend_pixel_rgba(void *obj, int x, int y, Uint8 c[4], AG_AlphaFn fnSrc)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Color cd;

	AG_ColorRGBA_8(&cd, c[0], c[1], c[2], c[3]);
	wid->drvOps->blendPixel(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    &cd, fnSrc, AG_ALPHA_ZERO);
}

#if AG_MODEL == AG_LARGE					/* LG */

/*
 * Write a pixel (16-bit RGB components).
 */
# ifdef AG_INLINE_HEADER
static __inline__ void
AG_PutPixelRGB(void *_Nonnull obj, int x, int y,
    AG_Component r, AG_Component g, AG_Component b)
# else
void
ag_put_pixel_rgb(void *obj, int x, int y,
    AG_Component r, AG_Component g, AG_Component b)
# endif
{
	AG_PutPixelRGB_16(obj, x,y, r,g,b);
}

/*
 * Write a pixel (64-bit value argument in agSurfaceFmt format)
 */
# ifdef AG_INLINE_HEADER
static __inline__ void
AG_PutPixel64(void *_Nonnull obj, int x, int y, Uint64 px)
# else
void
ag_put_pixel_64(void *obj, int x, int y, Uint64 px)
# endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->putPixel64(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    px);
}

/*
 * Write a pixel (16-bit RGB component arguments)
 */
# ifdef AG_INLINE_HEADER
static __inline__ void
AG_PutPixelRGB_16(void *_Nonnull obj, int x, int y, Uint16 r, Uint16 g, Uint16 b)
# else
void
ag_put_pixel_rgb_16(void *obj, int x, int y, Uint16 r, Uint16 g, Uint16 b)
# endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->putPixelRGB16(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    r,g,b);
}

/*
 * Blend a pixel (64-bit value argument in agSurfaceFmt format)
 */
# ifdef AG_INLINE_HEADER
static __inline__ void
AG_BlendPixel64(void *_Nonnull obj, int x, int y, Uint64 px, AG_AlphaFn fnSrc)
# else
void
ag_blend_pixel_64(void *obj, int x, int y, Uint64 px, AG_AlphaFn fnSrc)
# endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Color c;

	AG_GetColor32(&c, px, agSurfaceFmt);
	wid->drvOps->blendPixel(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    &c, fnSrc, AG_ALPHA_ZERO);
}

#else								/* !LG */

/*
 * Write a pixel (8-bit RGB components).
 */
# ifdef AG_INLINE_HEADER
static __inline__ void
AG_PutPixelRGB(void *_Nonnull obj, int x, int y,
    AG_Component r, AG_Component g, AG_Component b)
# else
void
ag_put_pixel_rgb(void *obj, int x, int y,
    AG_Component r, AG_Component g, AG_Component b)
# endif
{
	AG_PutPixelRGB_8(obj, x,y, r,g,b);
}

#endif /* !AG_LARGE */

/*
 * Draw a line between two given endpoints.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawLine(void *_Nonnull obj, int x1, int y1, int x2, int y2, const AG_Color *c)
#else
void
ag_draw_line(void *obj, int x1, int y1, int x2, int y2, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawLine(wid->drv,
	    wid->rView.x1 + x1,
	    wid->rView.y1 + y1,
	    wid->rView.x1 + x2,
	    wid->rView.y1 + y2,
	    c);
}

/*
 * Draw a horizontal line.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawLineH(void *_Nonnull obj, int x1, int x2, int y, const AG_Color *c)
#else
void
ag_draw_line_h(void *obj, int x1, int x2, int y, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawLineH(wid->drv,
	    wid->rView.x1 + x1,
	    wid->rView.x1 + x2,
	    wid->rView.y1 + y,
	    c);
}

/*
 * Draw a vertical line.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawLineV(void *_Nonnull obj, int x, int y1, int y2, const AG_Color *c)
#else
void
ag_draw_line_v(void *obj, int x, int y1, int y2, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawLineV(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y1,
	    wid->rView.y1 + y2,
	    c);
}

/*
 * Draw a line with blending.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawLineBlended(void *_Nonnull obj, int x1, int y1, int x2, int y2,
    const AG_Color *c, AG_AlphaFn fnSrc, AG_AlphaFn fnDst)
#else
void
ag_draw_line_blended(void *obj, int x1, int y1, int x2, int y2,
    const AG_Color *c, AG_AlphaFn fnSrc, AG_AlphaFn fnDst)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawLineBlended(wid->drv,
	    wid->rView.x1 + x1,
	    wid->rView.y1 + y1,
	    wid->rView.x1 + x2,
	    wid->rView.y1 + y2,
	    c, fnSrc, fnDst);
}

/*
 * Render a triangle between three given points.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawTriangle(void *_Nonnull obj, const AG_Pt *_Nonnull v1,
    const AG_Pt *_Nonnull v2, const AG_Pt *_Nonnull v3, const AG_Color *c)
#else
void
ag_draw_triangle(void *obj, const AG_Pt *v1, const AG_Pt *v2, const AG_Pt *v3,
    const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	const int x1 = wid->rView.x1;
	const int y1 = wid->rView.y1;
	AG_Pt V1,V2,V3;

	V1.x = x1 + v1->x;
	V1.y = y1 + v1->y;
	V2.x = x1 + v2->x;
	V2.y = y1 + v2->y;
	V3.x = x1 + v3->x;
	V3.y = y1 + v3->y;

	wid->drvOps->drawTriangle(wid->drv, &V1,&V2,&V3, c);
}

/*
 * Render an arrow pointing north.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawArrowUp(void *_Nonnull obj, int x0, int y0, int h, const AG_Color *c)
#else
void
ag_draw_arrow_up(void *obj, int x0, int y0, int h, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawArrow(wid->drv, 0,
	    wid->rView.x1+x0,
	    wid->rView.y1+y0, h, c);
}

/*
 * Render an arrow pointing east.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawArrowRight(void *_Nonnull obj, int x0, int y0, int h, const AG_Color *c)
#else
void
ag_draw_arrow_right(void *obj, int x0, int y0, int h, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawArrow(wid->drv, 1,
	    wid->rView.x1+x0,
	    wid->rView.y1+y0, h, c);
}

/*
 * Render an arrow pointing south.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawArrowDown(void *_Nonnull obj, int x0, int y0, int h, const AG_Color *c)
#else
void
ag_draw_arrow_down(void *obj, int x0, int y0, int h, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawArrow(wid->drv, 2,
	    wid->rView.x1+x0,
	    wid->rView.y1+y0, h, c);
}

/*
 * Render an arrow pointing west.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawArrowLeft(void *_Nonnull obj, int x0, int y0, int h, const AG_Color *c)
#else
void
ag_draw_arrow_left(void *obj, int x0, int y0, int h, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawArrow(wid->drv, 3,
	    wid->rView.x1+x0,
	    wid->rView.y1+y0, h, c);
}

/*
 * Render a 3D-style box with all rounded edges
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawBoxRounded(void *_Nonnull obj, const AG_Rect *_Nonnull r, int z, int rad,
    const AG_Color *cBg)
#else
void
ag_draw_box_rounded(void *obj, const AG_Rect *r, int z, int rad,
    const AG_Color *cBg)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Color c[3];
	AG_Rect rd;
	
	AG_OBJECT_ISA(obj, "AG_Widget:*");

	AG_ColorAdd(&c[0], cBg,  (z<0) ? &agSunkColor : &agRaisedColor);
	AG_ColorAdd(&c[1], &c[0], (z<0) ? &agLowColor  : &agHighColor);
	AG_ColorAdd(&c[2], &c[0], (z<0) ? &agHighColor : &agLowColor);

	rd.x = wid->rView.x1 + r->x;
	rd.y = wid->rView.y1 + r->y;
	rd.w = r->w;
	rd.h = r->h;
	wid->drvOps->drawBoxRounded(wid->drv, &rd, z, rad,
	                            &c[0], &c[1], &c[2]);
}

/*
 * Render a 3D-style box with only the two top edges rounded.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawBoxRoundedTop(void *_Nonnull obj, const AG_Rect *_Nonnull r, int z,
    int rad, const AG_Color *_Nonnull cBg)
#else
void
ag_draw_box_rounded_top(void *obj, const AG_Rect *r, int z, int rad,
    const AG_Color *cBg)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Color c[3];
	AG_Rect rd;
	
	AG_OBJECT_ISA(obj, "AG_Widget:*");

	rd.x = wid->rView.x1 + r->x;
	rd.y = wid->rView.y1 + r->y;
	rd.w = r->w;
	rd.h = r->h;

	c[0] = *cBg;
	AG_ColorAdd(&c[1], &c[0], (z<0) ? &agLowColor  : &agHighColor);
	AG_ColorAdd(&c[2], &c[0], (z<0) ? &agHighColor : &agLowColor);

	wid->drvOps->drawBoxRoundedTop(wid->drv, &rd, z, rad, &c[0],&c[1],&c[2]);
}

/*
 * Draw a circle outline.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawCircle(void *_Nonnull obj, int x, int y, int r,
    const AG_Color *_Nonnull c)
#else
void
ag_draw_circle(void *obj, int x, int y, int r, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawCircle(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    r, c);
}

/*
 * Draw a filled circle.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawCircleFilled(void *_Nonnull obj, int x, int y, int r,
    const AG_Color *_Nonnull c)
#else
void
ag_draw_circle_filled(void *obj, int x, int y, int r, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawCircleFilled(wid->drv,
	    wid->rView.x1 + x,
	    wid->rView.y1 + y,
	    r, c);
}

/*
 * Draw a filled polygon.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawPolygon(void *_Nonnull obj, const AG_Pt *_Nonnull pts, Uint nPts,
    const AG_Color *_Nonnull c)
#else
void
ag_draw_polygon(void *obj, const AG_Pt *pts, Uint nPts, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawPolygon(obj, pts, nPts, c);
}

/*
 * Draw a filled polygon with 32x32 stipple pattern.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawPolygonSti32(void *_Nonnull obj, const AG_Pt *_Nonnull pts, Uint nPts,
    const AG_Color *_Nonnull c, const Uint8 *_Nonnull stipple)
#else
void
ag_draw_polygon_sti32(void *obj, const AG_Pt *pts, Uint nPts, const AG_Color *c,
    const Uint8 *stipple)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;

	wid->drvOps->drawPolygonSti32(wid->drv, pts, nPts, c, stipple);
}

/*
 * Draw a filled rectangle (possibly blended)
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawRect(void *_Nonnull obj, const AG_Rect *_Nonnull r,
    const AG_Color *_Nonnull c)
#else
void
ag_draw_rect(void *obj, const AG_Rect *r, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Rect rd;

	rd.x = wid->rView.x1 + r->x;
	rd.y = wid->rView.y1 + r->y;
	rd.w = r->w;
	rd.h = r->h;

	if (c->a < AG_OPAQUE) {
		wid->drvOps->drawRectBlended(wid->drv, &rd, c,
		    AG_ALPHA_SRC, AG_ALPHA_ONE_MINUS_SRC);
	} else {
		wid->drvOps->drawRectFilled(wid->drv, &rd, c);
	}
}

/*
 * Draw a filled rectangle (always opaque).
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawRectFilled(void *_Nonnull obj, const AG_Rect *_Nonnull r,
    const AG_Color *_Nonnull c)
#else
void
ag_draw_rect_filled(void *obj, const AG_Rect *r, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Rect rd;

	rd.x = wid->rView.x1 + r->x;
	rd.y = wid->rView.y1 + r->y;
	rd.w = r->w;
	rd.h = r->h;
	wid->drvOps->drawRectFilled(wid->drv, &rd, c);
}

/*
 * Draw a filled rectangle with blending (and given source blending function)
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawRectBlended(void *_Nonnull obj, const AG_Rect *_Nonnull r,
    const AG_Color *_Nonnull c, AG_AlphaFn fnSrc, AG_AlphaFn fnDst)
#else
void
ag_draw_rect_blended(void *obj, const AG_Rect *r, const AG_Color *c,
    AG_AlphaFn fnSrc, AG_AlphaFn fnDst)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Rect rd;

	rd.x = wid->rView.x1 + r->x;
	rd.y = wid->rView.y1 + r->y;
	rd.w = r->w;
	rd.h = r->h;

	wid->drvOps->drawRectBlended(wid->drv, &rd, c, fnSrc, fnDst);
}

/*
 * Render a filled rectangle with stipple pattern.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawRectDithered(void *_Nonnull obj, const AG_Rect *_Nonnull r,
    const AG_Color *_Nonnull c)
#else
void
ag_draw_rect_dithered(void *obj, const AG_Rect *r, const AG_Color *c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Rect rd;

	rd.x = wid->rView.x1 + r->x;
	rd.y = wid->rView.y1 + r->y;
	rd.w = r->w;
	rd.h = r->h;

	wid->drvOps->drawRectDithered(wid->drv, &rd, c);
}

/*
 * Render a 3D-style frame
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawFrame(void *_Nonnull obj, const AG_Rect *_Nonnull r, int z,
    const AG_Color *_Nonnull cBase)
#else
void
ag_draw_frame(void *obj, const AG_Rect *r, int z, const AG_Color *cBase)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Driver *drv = wid->drv;
	AG_DriverClass *drvOps = wid->drvOps;
	AG_Color c[2];
	AG_Rect rd;
	int y2, x2;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");

	rd.x = wid->rView.x1 + r->x;
	rd.y = wid->rView.y1 + r->y;
	rd.w = r->w;
	rd.h = r->h;

	AG_ColorAdd(&c[0], cBase, (z<0) ? &agLowColor  : &agHighColor);
	AG_ColorAdd(&c[1], cBase, (z<0) ? &agHighColor : &agLowColor);

	x2 = rd.x + r->w - 1;
	y2 = rd.y + r->h - 1;

	if (c[0].a < AG_OPAQUE) {
		drvOps->drawLineBlended(drv, rd.x, rd.y, x2,   rd.y, &c[0], AG_ALPHA_SRC, AG_ALPHA_ZERO);
		drvOps->drawLineBlended(drv, rd.x, rd.y, rd.x, y2,   &c[0], AG_ALPHA_SRC, AG_ALPHA_ZERO);
	} else {
		drvOps->drawLineH(drv, rd.x, x2,   rd.y, &c[0]);
		drvOps->drawLineV(drv, rd.x, rd.y, y2,   &c[0]);
	}
	if (c[1].a < AG_OPAQUE) {
		drvOps->drawLineBlended(drv, rd.x, y2,   x2, y2, &c[1], AG_ALPHA_SRC, AG_ALPHA_ZERO);
		drvOps->drawLineBlended(drv, x2,   rd.y, x2, y2, &c[1], AG_ALPHA_SRC, AG_ALPHA_ZERO);
	} else {
		drvOps->drawLineH(drv, rd.x, x2,   y2, &c[1]);
		drvOps->drawLineV(drv, x2,   rd.y, y2, &c[1]);
	}
}

/*
 * Render a 3D-style box
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawBox(void *_Nonnull obj, const AG_Rect *_Nonnull r, int z,
    const AG_Color *_Nonnull C)
#else
void
ag_draw_box(void *obj, const AG_Rect *r, int z, const AG_Color *C)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Driver *drv = wid->drv;
	AG_Color c;
	AG_Rect rd;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");

	rd.x = wid->rView.x1 + r->x;
	rd.y = wid->rView.y1 + r->y;
	rd.w = r->w;
	rd.h = r->h;

	AG_ColorAdd(&c, C, (z < 0) ? &agSunkColor : &agRaisedColor);

	if (c.a < AG_OPAQUE) {
		wid->drvOps->drawRectBlended(drv, &rd, &c, AG_ALPHA_SRC,
		    AG_ALPHA_ONE_MINUS_SRC);
	} else {
		wid->drvOps->drawRectFilled(drv, &rd, &c);
	}
	AG_DrawFrame(wid, r, z, &c);
}

/*
 * Render a textured 3D-style box in "disabled" style.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawBoxDisabled(void *_Nonnull obj, const AG_Rect *_Nonnull r, int z,
    const AG_Color *_Nonnull c1, const AG_Color *_Nonnull c2)
#else
void
ag_draw_box_disabled(void *obj, const AG_Rect *r, int z,
    const AG_Color *c1, const AG_Color *c2)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Color c;
	AG_Rect rd;
	
	AG_OBJECT_ISA(wid, "AG_Widget:*");

	rd.x = wid->rView.x1 + r->x;
	rd.y = wid->rView.y1 + r->y;
	rd.w = r->w;
	rd.h = r->h;

	wid->drvOps->drawRectFilled(wid->drv, &rd, c1);
	AG_DrawFrame(wid, r, z, c1);
	AG_ColorAdd(&c, c2, (z<0) ? &agSunkColor : &agRaisedColor);
	wid->drvOps->drawRectDithered(wid->drv, &rd, &c);
}

/*
 * Render 3D-style frame using a specific blending mode.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawFrameBlended(void *_Nonnull obj, const AG_Rect *_Nonnull r,
    const AG_Color *_Nonnull c, AG_AlphaFn fnSrc)
#else
void
ag_draw_frame_blended(void *obj, const AG_Rect *r, const AG_Color *_Nonnull c,
    AG_AlphaFn fnSrc)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Driver *drv = wid->drv;
	AG_DriverClass *drvOps = wid->drvOps;
	const int x = wid->rView.x1 + r->x;
	const int y = wid->rView.y1 + r->y;
	const int x2 = x + r->w - 1;
	const int y2 = y + r->h - 1;

	drvOps->drawLineBlended(drv, x,  y,  x2,  y,  c, fnSrc, AG_ALPHA_ZERO);
	drvOps->drawLineBlended(drv, x,  y,  x,   y2, c, fnSrc, AG_ALPHA_ZERO);
	drvOps->drawLineBlended(drv, x,  y2, x2,  y2, c, fnSrc, AG_ALPHA_ZERO);
	drvOps->drawLineBlended(drv, x2, y,  x2,  y2, c, fnSrc, AG_ALPHA_ZERO);
}

/*
 * Draw a rectangle outline only.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawRectOutline(void *_Nonnull obj, const AG_Rect *_Nonnull r,
    const AG_Color *_Nonnull c)
#else
void
ag_draw_rect_outline(void *obj, const AG_Rect *r, const AG_Color *_Nonnull c)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Driver *drv = wid->drv;
	AG_DriverClass *drvOps = wid->drvOps;
	const int x = wid->rView.x1 + r->x;
	const int y = wid->rView.y1 + r->y;
	const int x2 = x + r->w;
	const int y2 = y + r->h;

	if (c->a < AG_OPAQUE) {
		drvOps->drawLineBlended(drv, x,  y, x2, y,  c, AG_ALPHA_SRC, AG_ALPHA_ZERO);
		drvOps->drawLineBlended(drv, x,  y, x2, y2, c, AG_ALPHA_SRC, AG_ALPHA_ZERO);
		drvOps->drawLineBlended(drv, x,  y, x,  y2, c, AG_ALPHA_SRC, AG_ALPHA_ZERO);
		drvOps->drawLineBlended(drv, x2, y, x,  y2, c, AG_ALPHA_SRC, AG_ALPHA_ZERO);
	} else {
		drvOps->drawLineH(drv, x,  x2, y,  c);
		if (drvOps->flags & AG_DRIVER_OPENGL) {		/* XXX */
			drvOps->drawLineH(drv, x-1,x2, y2, c);
			drvOps->drawLineV(drv, x,  y,  y2, c);
		} else {
			drvOps->drawLineH(drv, x,  x2+1, y2,   c);
			drvOps->drawLineV(drv, x,  y,    y2+1, c);
		}
		drvOps->drawLineV(drv, x2, y,  y2, c);
	}
}

/*
 * Render a 3D-style line.
 */
#ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawLine2(void *_Nonnull obj, int x1, int y1, int x2, int y2,
    const AG_Color *_Nonnull C)
#else
void
ag_draw_line_2(void *obj, int x1, int y1, int x2, int y2,
    const AG_Color *C)
#endif
{
	AG_Widget *wid = (AG_Widget *)obj;
	AG_Color c;

	x1 += wid->rView.x1;
	y1 += wid->rView.y1;
	x2 += wid->rView.x1;
	y2 += wid->rView.y1;

	AG_ColorAdd(&c, C, &agHighColor);
	wid->drvOps->drawLine(wid->drv, x1,y1, x2,y2, &c);

	AG_ColorAdd(&c, C, &agLowColor);
	wid->drvOps->drawLine(wid->drv, x1+1,y1+1, x2+1,y2+1, &c);
}

#ifdef AG_HAVE_FLOAT
# ifdef AG_INLINE_HEADER
static __inline__ void
AG_DrawArrowLine(void *_Nonnull obj, int x1, int y1, int x2, int y2,
    AG_ArrowLineType t, int length, double theta, const AG_Color *_Nonnull C)
# else
void
ag_draw_arrow_line(void *obj, int x1, int y1, int x2, int y2,
    AG_ArrowLineType t, int length, double theta, const AG_Color *C)
# endif
{
	AG_OBJECT_ISA(obj, "AG_Widget:*");

	AG_DrawLine(obj, x1,y1, x2,y2, C);

	if ((t == AG_ARROWLINE_FORWARD) || (t == AG_ARROWLINE_BOTH)) {
		AG_DrawArrowhead(obj, x1,y1, x2,y2, length, theta, C);
	}
	if ((t == AG_ARROWLINE_REVERSE) || (t == AG_ARROWLINE_BOTH)) {
		AG_DrawArrowhead(obj, x2,y2, x1,y1, length, theta, C);
	}
}
#endif /* AG_HAVE_FLOAT */
