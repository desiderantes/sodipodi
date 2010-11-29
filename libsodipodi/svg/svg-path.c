#define __SP_SVG_PARSE_C__

#include <math.h>
#include <malloc.h>
#include <stdio.h>

#include "svg.h"

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

struct PGVData {
	char *cdata;
	size_t csize;
	size_t cpos;
};

static unsigned int
pgv_moveto (float x0, float y0, unsigned int flags, void *data)
{
	unsigned char c0[32], c1[32];
	unsigned int l0, l1, len;
	struct PGVData *pgv;

	pgv = (struct PGVData *) data;

	/* "M NUMBER NUMBER " */
	l0 = sp_svg_number_write_d (c0, x0, 6, 0, 0);
	l1 = sp_svg_number_write_d (c1, y0, 6, 0, 0);
	len = 2 + l0 + 1 + l1 + 1;
	if ((pgv->cpos + len + 1) <= pgv->csize) {
		sprintf (pgv->cdata + pgv->cpos, "M %s %s ", c0, c1);
		pgv->cdata[pgv->cpos + len] = 0;
	}
	pgv->cpos += len;
	return 1;
}

static unsigned int
pgv_lineto (float x0, float y0, float x1, float y1, unsigned int flags, void *data)
{
	unsigned char c0[32], c1[32];
	unsigned int l0, l1, len;
	struct PGVData *pgv;

	pgv = (struct PGVData *) data;

	/* "L NUMBER NUMBER " */
	l0 = sp_svg_number_write_d (c0, x1, 6, 0, 0);
	l1 = sp_svg_number_write_d (c1, y1, 6, 0, 0);
	len = 2 + l0 + 1 + l1 + 1;
	if ((pgv->cpos + len + 1) <= pgv->csize) {
		sprintf (pgv->cdata + pgv->cpos, "L %s %s ", c0, c1);
		pgv->cdata[pgv->cpos + len] = 0;
	}
	pgv->cpos += len;
	return 1;
}

static unsigned int
pgv_curveto2 (float x0, float y0, float x1, float y1, float x2, float y2, unsigned int flags, void *data)
{
	unsigned char c0[32], c1[32], c2[32], c3[32];
	unsigned int l0, l1, l2, l3, len;
	struct PGVData *pgv;

	pgv = (struct PGVData *) data;

	/* "Q NUMBER NUMBER NUMBER NUMBER " */
	l0 = sp_svg_number_write_d (c0, x1, 6, 0, 0);
	l1 = sp_svg_number_write_d (c1, y1, 6, 0, 0);
	l2 = sp_svg_number_write_d (c2, x2, 6, 0, 0);
	l3 = sp_svg_number_write_d (c3, y2, 6, 0, 0);
	len = 2 + l0 + 1 + l1 + 1 + l2 + 1 + l3 + 1;
	if ((pgv->cpos + len + 1) <= pgv->csize) {
		sprintf (pgv->cdata + pgv->cpos, "Q %s %s %s %s ", c0, c1, c2, c3);
		pgv->cdata[pgv->cpos + len] = 0;
	}
	pgv->cpos += len;
	return 1;
}

static unsigned int
pgv_curveto3 (float x0, float y0, float x1, float y1, float x2, float y2, float x3, float y3, unsigned int flags, void *data)
{
	unsigned char c0[32], c1[32], c2[32], c3[32], c4[32], c5[32];
	unsigned int l0, l1, l2, l3, l4, l5, len;
	struct PGVData *pgv;

	pgv = (struct PGVData *) data;

	/* "C NUMBER NUMBER NUMBER NUMBER NUMBER NUMBER " */
	l0 = sp_svg_number_write_d (c0, x1, 6, 0, 0);
	l1 = sp_svg_number_write_d (c1, y1, 6, 0, 0);
	l2 = sp_svg_number_write_d (c2, x2, 6, 0, 0);
	l3 = sp_svg_number_write_d (c3, y2, 6, 0, 0);
	l4 = sp_svg_number_write_d (c4, x3, 6, 0, 0);
	l5 = sp_svg_number_write_d (c5, y3, 6, 0, 0);
	len = 2 + l0 + 1 + l1 + 1 + l2 + 1 + l3 + 1 + l4 + 1 + l5 + 1;
	if ((pgv->cpos + len + 1) <= pgv->csize) {
		sprintf (pgv->cdata + pgv->cpos, "C %s %s %s %s %s %s ", c0, c1, c2, c3, c4, c5);
		pgv->cdata[pgv->cpos + len] = 0;
	}
	pgv->cpos += len;
	return 1;
}

static unsigned int
pgv_endpath (float ex, float ey, float sx, float sy, unsigned int flags, void *data)
{
	unsigned int len;
	struct PGVData *pgv;

	pgv = (struct PGVData *) data;

	if (flags & NR_PATH_CLOSED) {
		/* "Z " */
		len = 2;
		if ((pgv->cpos + len + 1) <= pgv->csize) {
			sprintf (pgv->cdata + pgv->cpos, "Z ");
			pgv->cdata[pgv->cpos + len] = 0;
		}
		pgv->cpos += len;
	}

	return 1;
}

NRPathGVector pgv = {
	pgv_moveto,
	pgv_lineto,
	pgv_curveto2,
	pgv_curveto3,
	pgv_endpath
};

unsigned char *
sp_svg_write_path (const NRPath *path)
{
	struct PGVData pgvdata;
	pgvdata.cdata = NULL;
	pgvdata.csize = 0;
	pgvdata.cpos = 0;
	nr_path_forall (path, NULL, &pgv, &pgvdata);
	pgvdata.csize = pgvdata.cpos + 1;
	pgvdata.cdata = (char *) malloc (pgvdata.csize);
	pgvdata.cpos = 0;
	nr_path_forall (path, NULL, &pgv, &pgvdata);
	return (unsigned char *) pgvdata.cdata;
}

/* 
   svg-path.c: Parse SVG path element data into bezier path.
 
   Copyright (C) 2000 Eazel, Inc.
   Copyright (C) 2000 Lauris Kaplinski
   Copyright (C) 2001 Ximian, Inc.
  
   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.
  
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
  
   You should have received a copy of the GNU General Public
   License along with this program; if not, write to the
   Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
  
   Authors:
     Raph Levien <raph@artofcode.com>
     Lauris Kaplinski <lauris@ximian.com>
*/

#if 0
#include <string.h>
#include <libart_lgpl/art_misc.h>
#include "gnome-canvas-bpath-util.h"
#include "svg.h"
#endif

/* This module parses an SVG path element into an RsvgBpathDef.

   At present, there is no support for <marker> or any other contextual
   information from the SVG file. The API will need to change rather
   significantly to support these.

   Reference: SVG working draft 3 March 2000, section 8.
*/

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif  /*  M_PI  */

#if 0
/* We are lazy ;-) (Lauris) */
#define rsvg_bpath_def_new gnome_canvas_bpath_def_new
#define rsvg_bpath_def_moveto gnome_canvas_bpath_def_moveto
#define rsvg_bpath_def_lineto gnome_canvas_bpath_def_lineto
#define rsvg_bpath_def_curveto gnome_canvas_bpath_def_curveto
#define rsvg_bpath_def_closepath gnome_canvas_bpath_def_closepath
#endif

typedef struct _RSVGParsePathCtx RSVGParsePathCtx;

struct _RSVGParsePathCtx {
	NRDynamicPath *dpath;
	double cpx, cpy;  /* current point */
	double rpx, rpy;  /* reflection point (for 's' and 't' commands) */
	char cmd;         /* current command (lowercase) */
	int param;        /* parameter number */
	unsigned int rel;     /* true if relative coords */
	double params[7]; /* parameters that have been parsed */
};

static void
rsvg_path_arc_segment (RSVGParsePathCtx *ctx,
					   double xc, double yc,
					   double th0, double th1,
					   double rx, double ry, double x_axis_rotation)
{
	double sin_th, cos_th;
	double a00, a01, a10, a11;
	double x1, y1, x2, y2, x3, y3;
	double t;
	double th_half;

	sin_th = sin (x_axis_rotation * (M_PI / 180.0));
	cos_th = cos (x_axis_rotation * (M_PI / 180.0)); 
	/* inverse transform compared with rsvg_path_arc */
	a00 = cos_th * rx;
	a01 = -sin_th * ry;
	a10 = sin_th * rx;
	a11 = cos_th * ry;

	th_half = 0.5 * (th1 - th0);
	t = (8.0 / 3.0) * sin (th_half * 0.5) * sin (th_half * 0.5) / sin (th_half);
	x1 = xc + cos (th0) - t * sin (th0);
	y1 = yc + sin (th0) + t * cos (th0);
	x3 = xc + cos (th1);
	y3 = yc + sin (th1);
	x2 = x3 + t * sin (th1);
	y2 = y3 - t * cos (th1);
	nr_dynamic_path_curveto3 (ctx->dpath, a00 * x1 + a01 * y1, a10 * x1 + a11 * y1, a00 * x2 + a01 * y2, a10 * x2 + a11 * y2, a00 * x3 + a01 * y3, a10 * x3 + a11 * y3);
}

/**
 * rsvg_path_arc: Add an RSVG arc to the path context.
 * @ctx: Path context.
 * @rx: Radius in x direction (before rotation).
 * @ry: Radius in y direction (before rotation).
 * @x_axis_rotation: Rotation angle for axes.
 * @large_arc_flag: 0 for arc length <= 180, 1 for arc >= 180.
 * @sweep: 0 for "negative angle", 1 for "positive angle".
 * @x: New x coordinate.
 * @y: New y coordinate.
 *
 **/
static void
rsvg_path_arc (RSVGParsePathCtx *ctx,
	      double rx, double ry, double x_axis_rotation,
	      int large_arc_flag, int sweep_flag,
	      double x, double y)
{
  double sin_th, cos_th;
  double a00, a01, a10, a11;
  double x0, y0, x1, y1, xc, yc;
  double d, sfactor, sfactor_sq;
  double th0, th1, th_arc;
  double px, py, pl;
  int i, n_segs;

  sin_th = sin (x_axis_rotation * (M_PI / 180.0));
  cos_th = cos (x_axis_rotation * (M_PI / 180.0));
  
  /*
     Correction of out-of-range radii as described in Appendix F.6.6:
     
     1. Ensure radii are non-zero (Done?).
     2. Ensure that radii are positive.
     3. Ensure that radii are large enough.
  */
  
  if(rx < 0.0) rx = -rx;
  if(ry < 0.0) ry = -ry;
  
  px = cos_th * (ctx->cpx - x) * 0.5 + sin_th * (ctx->cpy - y) * 0.5;
  py = cos_th * (ctx->cpy - y) * 0.5 - sin_th * (ctx->cpx - x) * 0.5;
  pl = (px * px) / (rx * rx) + (py * py) / (ry * ry);
  
  if(pl > 1.0)
  {
      pl  = sqrt(pl);
      rx *= pl;
      ry *= pl;
  }
  
  /* Proceed with computations as described in Appendix F.6.5 */
  
  a00 = cos_th / rx;
  a01 = sin_th / rx;
  a10 = -sin_th / ry;
  a11 = cos_th / ry;
  x0 = a00 * ctx->cpx + a01 * ctx->cpy;
  y0 = a10 * ctx->cpx + a11 * ctx->cpy;
  x1 = a00 * x + a01 * y;
  y1 = a10 * x + a11 * y;
  /* (x0, y0) is current point in transformed coordinate space.
     (x1, y1) is new point in transformed coordinate space.

     The arc fits a unit-radius circle in this space.
  */
  d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
  sfactor_sq = 1.0 / d - 0.25;
  if (sfactor_sq < 0) sfactor_sq = 0;
  sfactor = sqrt (sfactor_sq);
  if (sweep_flag == large_arc_flag) sfactor = -sfactor;
  xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
  yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);
  /* (xc, yc) is center of the circle. */

  th0 = atan2 (y0 - yc, x0 - xc);
  th1 = atan2 (y1 - yc, x1 - xc);

  th_arc = th1 - th0;
  if (th_arc < 0 && sweep_flag)
    th_arc += 2 * M_PI;
  else if (th_arc > 0 && !sweep_flag)
    th_arc -= 2 * M_PI;

  n_segs = (int) ceil (fabs (th_arc / (M_PI * 0.5 + 0.001)));

  for (i = 0; i < n_segs; i++)
    rsvg_path_arc_segment (ctx, xc, yc,
			  th0 + i * th_arc / n_segs,
			  th0 + (i + 1) * th_arc / n_segs,
			  rx, ry, x_axis_rotation);

  ctx->cpx = x;
  ctx->cpy = y;
}


/* supply defaults for missing parameters, assuming relative coordinates
   are to be interpreted as x,y */
static void
rsvg_parse_path_default_xy (RSVGParsePathCtx *ctx, int n_params)
{
  int i;

  if (ctx->rel)
    {
      for (i = ctx->param; i < n_params; i++)
	{
	  if (i > 2)
	    ctx->params[i] = ctx->params[i - 2];
	  else if (i == 1)
	    ctx->params[i] = ctx->cpy;
	  else if (i == 0)
	    /* we shouldn't get here (usually ctx->param > 0 as
               precondition) */
	    ctx->params[i] = ctx->cpx;
	}
    }
  else
    {
      for (i = ctx->param; i < n_params; i++)
	ctx->params[i] = 0.0;
    }
}

static void
rsvg_parse_path_do_cmd (RSVGParsePathCtx *ctx, unsigned int final)
{
	double x1, y1, x2, y2, x3, y3;

	switch (ctx->cmd)
	{
	case 'm':
		/* moveto */
		if (ctx->param == 2 || final)
		{
			rsvg_parse_path_default_xy (ctx, 2);
			nr_dynamic_path_moveto (ctx->dpath, ctx->params[0], ctx->params[1]);
			ctx->cpx = ctx->rpx = ctx->params[0];
			ctx->cpy = ctx->rpy = ctx->params[1];
			ctx->param = 0;
		}
		break;
	case 'l':
		/* lineto */
		if (ctx->param == 2 || final)
		{
			rsvg_parse_path_default_xy (ctx, 2);
			nr_dynamic_path_lineto (ctx->dpath, ctx->params[0], ctx->params[1]);
			ctx->cpx = ctx->rpx = ctx->params[0];
			ctx->cpy = ctx->rpy = ctx->params[1];
			ctx->param = 0;
		}
		break;
	case 'c':
		/* curveto */
		if (ctx->param == 6 || final)
		{
			rsvg_parse_path_default_xy (ctx, 6);
			x1 = ctx->params[0];
			y1 = ctx->params[1];
			x2 = ctx->params[2];
			y2 = ctx->params[3];
			x3 = ctx->params[4];
			y3 = ctx->params[5];
			nr_dynamic_path_curveto3 (ctx->dpath, x1, y1, x2, y2, x3, y3);
			ctx->rpx = x2;
			ctx->rpy = y2;
			ctx->cpx = x3;
			ctx->cpy = y3;
			ctx->param = 0;
		}
		break;
	case 's':
		/* smooth curveto */
		if (ctx->param == 4 || final)
		{
			rsvg_parse_path_default_xy (ctx, 4);
			x1 = 2 * ctx->cpx - ctx->rpx;
			y1 = 2 * ctx->cpy - ctx->rpy;
			x2 = ctx->params[0];
			y2 = ctx->params[1];
			x3 = ctx->params[2];
			y3 = ctx->params[3];
			nr_dynamic_path_curveto3 (ctx->dpath, x1, y1, x2, y2, x3, y3);
			ctx->rpx = x2;
			ctx->rpy = y2;
			ctx->cpx = x3;
			ctx->cpy = y3;
			ctx->param = 0;
		}
		break;
	case 'h':
		/* horizontal lineto */
		if (ctx->param == 1)
		{
			nr_dynamic_path_lineto (ctx->dpath, ctx->params[0], ctx->cpy);
			ctx->cpx = ctx->rpx = ctx->params[0];
			ctx->param = 0;
		}
		break;
	case 'v':
		/* vertical lineto */
		if (ctx->param == 1)
		{
			nr_dynamic_path_lineto (ctx->dpath, ctx->cpx, ctx->params[0]);
			ctx->cpy = ctx->rpy = ctx->params[0];
			ctx->param = 0;
		}
		break;
	case 'q':
		/* quadratic bezier curveto */

		/* non-normative reference:
		http://www.icce.rug.nl/erikjan/bluefuzz/beziers/beziers/beziers.html
		*/
		if (ctx->param == 4 || final)
		{
			rsvg_parse_path_default_xy (ctx, 4);
			if (0) {
				/* raise quadratic bezier to cubic */
				x1 = (ctx->cpx + 2 * ctx->params[0]) * (1.0 / 3.0);
				y1 = (ctx->cpy + 2 * ctx->params[1]) * (1.0 / 3.0);
				x3 = ctx->params[2];
				y3 = ctx->params[3];
				x2 = (x3 + 2 * ctx->params[0]) * (1.0 / 3.0);
				y2 = (y3 + 2 * ctx->params[1]) * (1.0 / 3.0);
				nr_dynamic_path_curveto3 (ctx->dpath, x1, y1, x2, y2, x3, y3);
				ctx->rpx = x2;
				ctx->rpy = y2;
				ctx->cpx = x3;
				ctx->cpy = y3;
			} else {
				x1 = ctx->params[0];
				y1 = ctx->params[1];
				x2 = ctx->params[2];
				y2 = ctx->params[3];
				nr_dynamic_path_curveto2 (ctx->dpath, x1, y1, x2, y2);
				ctx->rpx = x1;
				ctx->rpy = y1;
				ctx->cpx = x2;
				ctx->cpy = y2;
			}
			ctx->param = 0;
		}
		break;
	case 't':
		/* Truetype quadratic bezier curveto */
		if (ctx->param == 2 || final)
		{
			double xc, yc; /* quadratic control point */

			xc = 2 * ctx->cpx - ctx->rpx;
			yc = 2 * ctx->cpy - ctx->rpy;
			if (0) {
				/* generate a quadratic bezier with control point = xc, yc */
				x1 = (ctx->cpx + 2 * xc) * (1.0 / 3.0);
				y1 = (ctx->cpy + 2 * yc) * (1.0 / 3.0);
				x3 = ctx->params[0];
				y3 = ctx->params[1];
				x2 = (x3 + 2 * xc) * (1.0 / 3.0);
				y2 = (y3 + 2 * yc) * (1.0 / 3.0);
				nr_dynamic_path_curveto3 (ctx->dpath, x1, y1, x2, y2, x3, y3);
				ctx->rpx = xc;
				ctx->rpy = yc;
				ctx->cpx = x3;
				ctx->cpy = y3;
			} else {
				x1 = xc;
				y1 = xc;
				x2 = ctx->params[0];
				y2 = ctx->params[1];
				nr_dynamic_path_curveto2 (ctx->dpath, x1, y1, x2, y2);
				ctx->rpx = x1;
				ctx->rpy = y1;
				ctx->cpx = x2;
				ctx->cpy = y2;
			}
			ctx->param = 0;
		}
		else if (final)
		{
			if (ctx->param > 2)
			{
				rsvg_parse_path_default_xy (ctx, 4);
				if (0) {
					/* raise quadratic bezier to cubic */
					x1 = (ctx->cpx + 2 * ctx->params[0]) * (1.0 / 3.0);
					y1 = (ctx->cpy + 2 * ctx->params[1]) * (1.0 / 3.0);
					x3 = ctx->params[2];
					y3 = ctx->params[3];
					x2 = (x3 + 2 * ctx->params[0]) * (1.0 / 3.0);
					y2 = (y3 + 2 * ctx->params[1]) * (1.0 / 3.0);
					nr_dynamic_path_curveto3 (ctx->dpath, x1, y1, x2, y2, x3, y3);
					ctx->rpx = x2;
					ctx->rpy = y2;
					ctx->cpx = x3;
					ctx->cpy = y3;
				} else {
					x1 = ctx->params[0];
					y1 = ctx->params[1];
					x2 = ctx->params[2];
					y2 = ctx->params[3];
					nr_dynamic_path_curveto2 (ctx->dpath, x1, y1, x2, y2);
					ctx->rpx = x1;
					ctx->rpy = y1;
					ctx->cpx = x2;
					ctx->cpy = y2;
				}
			}
			else
			{
				rsvg_parse_path_default_xy (ctx, 2);
				nr_dynamic_path_lineto (ctx->dpath, ctx->params[0], ctx->params[1]);
				ctx->cpx = ctx->rpx = ctx->params[0];
				ctx->cpy = ctx->rpy = ctx->params[1];
			}
			ctx->param = 0;
		}
		break;
	case 'a':
		if (ctx->param == 7 || final)
		{
			rsvg_path_arc (ctx, ctx->params[0], ctx->params[1], ctx->params[2], (int) ctx->params[3], (int) ctx->params[4], ctx->params[5], ctx->params[6]);
			ctx->param = 0;
		}
		break;
	default:
		ctx->param = 0;
	}
}

static void
rsvg_parse_path_data (RSVGParsePathCtx *ctx, const char *data)
{
  int i = 0;
  double val = 0;
  char c = 0;
  unsigned int in_num = FALSE;
  unsigned int in_frac = FALSE;
  unsigned int in_exp = FALSE;
  unsigned int exp_wait_sign = FALSE;
  int sign = 0;
  int exp = 0;
  int exp_sign = 0;
  double frac = 0.0;

  in_num = FALSE;
  for (i = 0; ; i++)
    {
      c = data[i];
      if (c >= '0' && c <= '9')
	{
	  /* digit */
	  if (in_num)
	    {
	      if (in_exp)
		{
		  exp = (exp * 10) + c - '0';
		  exp_wait_sign = FALSE;
		}
	      else if (in_frac)
		val += (frac *= 0.1) * (c - '0');
	      else
		val = (val * 10) + c - '0';
	    }
	  else
	    {
	      in_num = TRUE;
	      in_frac = FALSE;
	      in_exp = FALSE;
	      exp = 0;
	      exp_sign = 1;
	      exp_wait_sign = FALSE;
	      val = c - '0';
	      sign = 1;
	    }
	}
      else if (c == '.')
	{
	  if (!in_num)
	    {
	      in_num = TRUE;
	      val = 0;
	    }
	  in_frac = TRUE;
	  frac = 1;
	}
      else if ((c == 'E' || c == 'e') && in_num)
	{
	  in_exp = TRUE;
	  exp_wait_sign = TRUE;
	  exp = 0;
	  exp_sign = 1;
	}
      else if ((c == '+' || c == '-') && in_exp)
	{
	  exp_sign = c == '+' ? 1 : -1;
	}
      else if (in_num)
	{
	  /* end of number */

	  val *= sign * pow (10, exp_sign * exp);
	  if (ctx->rel)
	    {
	      /* Handle relative coordinates. This switch statement attempts
		 to determine _what_ the coords are relative to. This is
		 underspecified in the 12 Apr working draft. */
	      switch (ctx->cmd)
		{
		case 'l':
		case 'm':
		case 'c':
		case 's':
		case 'q':
		case 't':
#ifndef RSVGV_RELATIVE
		  /* rule: even-numbered params are x-relative, odd-numbered
		     are y-relative */
		  if ((ctx->param & 1) == 0)
		    val += ctx->cpx;
		  else if ((ctx->param & 1) == 1)
		    val += ctx->cpy;
		  break;
#else
		  /* rule: even-numbered params are x-relative, odd-numbered
		     are y-relative */
		  if (ctx->param == 0 || (ctx->param % 2 ==0))
		    val += ctx->cpx;
		  else 
		    val += ctx->cpy;
		  break;
#endif
		case 'a':
		  /* rule: sixth and seventh are x and y, rest are not
		     relative */
		  if (ctx->param == 5)
		    val += ctx->cpx;
		  else if (ctx->param == 6)
		    val += ctx->cpy;
		  break;
		case 'h':
		  /* rule: x-relative */
		  val += ctx->cpx;
		  break;
		case 'v':
		  /* rule: y-relative */
		  val += ctx->cpy;
		  break;
		}
	    }
	  ctx->params[ctx->param++] = val;
	  rsvg_parse_path_do_cmd (ctx, FALSE);
	  in_num = FALSE;
	}

      if (c == '\0')
	break;
      else if ((c == '+' || c == '-') && !exp_wait_sign)
	{
	  sign = c == '+' ? 1 : -1;;
	  val = 0;
	  in_num = TRUE;
	  in_frac = FALSE;
	  in_exp = FALSE;
	  exp = 0;
	  exp_sign = 1;
	  exp_wait_sign = FALSE;
	}
      else if (c == 'z' || c == 'Z')
	{
	  if (ctx->param)
	    rsvg_parse_path_do_cmd (ctx, TRUE);
#ifdef VERBOSE
	  g_print ("'z' closepath\n");
#endif
	  nr_dynamic_path_closepath (ctx->dpath);
	}
      else if (c >= 'A' && c <= 'Z' && c != 'E')
	{
	  if (ctx->param)
	    rsvg_parse_path_do_cmd (ctx, TRUE);
	  ctx->cmd = c + 'a' - 'A';
	  ctx->rel = FALSE;
	}
      else if (c >= 'a' && c <= 'z' && c != 'e')
	{
	  if (ctx->param)
	    rsvg_parse_path_do_cmd (ctx, TRUE);
	  ctx->cmd = c;
	  ctx->rel = TRUE;
	}
      /* else c _should_ be whitespace or , */
    }
}

#if 0
gchar *
sp_svg_write_path (const ArtBpath * bpath)
{
	GString *result;
	int i;
	int closed = 0;
	char *res;
	
	g_return_val_if_fail (bpath != NULL, NULL);

	result = g_string_sized_new (40);

	for (i = 0; bpath[i].code != ART_END; i++){
		unsigned char c0[32], c1[32], c2[32], c3[32], c4[32], c5[32];
		switch (bpath [i].code){
		case ART_LINETO:
			sp_svg_number_write_d (c0, bpath[i].x3, 6, 0, 0);
			sp_svg_number_write_d (c1, bpath[i].y3, 6, 0, 0);
			g_string_sprintfa (result, "L %s %s ", c0, c1);
			break;

		case ART_CURVETO:
			sp_svg_number_write_d (c0, bpath[i].x1, 6, 0, 0);
			sp_svg_number_write_d (c1, bpath[i].y1, 6, 0, 0);
			sp_svg_number_write_d (c2, bpath[i].x2, 6, 0, 0);
			sp_svg_number_write_d (c3, bpath[i].y2, 6, 0, 0);
			sp_svg_number_write_d (c4, bpath[i].x3, 6, 0, 0);
			sp_svg_number_write_d (c5, bpath[i].y3, 6, 0, 0);
			g_string_sprintfa (
				result, "C %s %s %s %s %s %s ", c0, c1, c2, c3, c4, c5);
			break;

		case ART_MOVETO_OPEN:
		case ART_MOVETO:
			if (closed) g_string_append  (result, "z ");
			closed = (bpath [i].code == ART_MOVETO);
			sp_svg_number_write_d (c0, bpath[i].x3, 6, 0, 0);
			sp_svg_number_write_d (c1, bpath[i].y3, 6, 0, 0);
			g_string_sprintfa (result, "M %s %s ", c0, c1);
			break;
		default:
			g_assert_not_reached ();
		}
	}
	if (closed)
		g_string_append (result, "z ");
	res = result->str;
	g_string_free (result, 0);

	return res;
}
#endif

NRPath *
sp_svg_read_path (const unsigned char *str)
{
	RSVGParsePathCtx ctx;
	NRDynamicPath dpath;

	nr_dynamic_path_setup (&dpath, 16);

	ctx.dpath = &dpath;
	ctx.cpx = 0.0;
	ctx.cpy = 0.0;
	ctx.cmd = 0;
	ctx.param = 0;

	rsvg_parse_path_data (&ctx, (const char *) str);

	return dpath.path;
}

