/* osd.c - On Screen Display routines used to draw all the indicators on top
 * of the game field
 * Copyright (C) 2010 Michal Trybus.
 *
 * This file is part of FreeCG.
 *
 * FreeCG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * FreeCG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FreeCG. If not, see <http://www.gnu.org/licenses/>.
 */

#include "osd.h"
#include "graphics.h"
#include <math.h>

static struct cg_osd osd;

void osd_fuel_init(struct osd_fuel *f, struct osd_element *container,
		double x, double y)
{
	_o(container, x, y,  16, 64,  0.8,  0, 90,  1, 1,  1, gl.ttm);
	osdlib_make_children(container, 16, 0);
	f->bars = container->ch;
	_o(&f->bars[15], 0, 0,  16, 3,  0.8,  388, 387,  16, 3, 0, gl.ttm);
	for (int i = 14; i >= 10; --i)
		_ro(&f->bars[i], &f->bars[i+1], 0, -1,  16, 3,  0.8,  388, 387,  16, 3, 0, gl.ttm);
	for (int i = 9; i >= 4; --i)
		_ro(&f->bars[i], &f->bars[i+1], 0, -1,  16, 3,  0.8,  388, 390,  16, 3, 0, gl.ttm);
	for (int i = 3; i >= 0; --i)
		_ro(&f->bars[i], &f->bars[i+1], 0, -1,  16, 3,  0.8,  388, 393,  16, 3, 0, gl.ttm);
}
void osd_velocity_init(struct osd_velocity *v, struct osd_element *container,
		double x, double y)
{
	_o(container, x,  y,  64, 64,  0.8,    0, 400,  64, 64,  0, gl.ttm);
	osdlib_make_children(container, 5, 1,
			&v->xbar, &v->ybar, &v->mxbar1, &v->mxbar2, &v->mybar);
	_o(v->xbar, 31, 16,   2, 32,  0.8,  384, 366,   2, 32,  0, gl.ttm);
	_o(v->ybar, 16, 31,  32,  2,  0.8,  384, 398,  32,  2,  0, gl.ttm);
	_o(v->mxbar1, 31, 25,   2, 14,  0.8,  386, 382,   2, 14,  0, gl.ttm);
	_o(v->mxbar2, 31, 25,   2, 14,  0.8,  386, 382,   2, 14,  0, gl.ttm);
	_o(v->mybar, 25, 31,  14,  2,  0.8,  386, 396,  14,  2,  0, gl.ttm);
}
void osd_keys_init(struct osd_keys *k, struct osd_element *container,
		double x, double y)
{
	_o(container, x, y,  16, 64,  0.8,    0,  90,   1,  1,  1, gl.ttm);
	osdlib_make_children(container, 4, 0);
	k->keys = container->ch;
	for (int i = 0; i < 4; ++i)
		_o(&k->keys[i], 0, 17*i,  16, 16,  0.2,  256, 360+16*i,  16, 16,  0, gl.ttm);
}
void osd_freight_init(struct osd_freight *f, struct osd_element *container,
		double x, double y, int tex_x, int tex_y)
{
	f->container = container;
	f->max_freight = gl.l->num_all_freight;
	int shelf_pos = 44;
	struct osd_element *img, *shelf;
	/* containter's width is updated in real-time, thus w = 0 */
	_o(container, x, y, 0, 18,  1,  0, 0, 0, 0, 1, gl.ttm);
	osdlib_make_children(container, 2, 1, &img, &shelf);
	_o(img, 0, 0,  36, 18,  0.6,  tex_x, tex_y,  48, 24,  0, gl.ttm);
	_o(shelf, shelf_pos, 6, -shelf_pos, 12,  0.2,  4, 302, 1, 1, 0, gl.ttm);
	osdlib_make_children(shelf, f->max_freight, 0);
	f->freight = shelf->ch;
	_o(&f->freight[0], 4, -2,  16, 16,  0.8,  80, 392,  16, 16, 1, gl.ttm);
	for (size_t i = 1; i < f->max_freight; ++i)
		_ro(&f->freight[i], &f->freight[i-1], -3, 0,  16, 16,  0.8,  80, 392,  16, 16, 1, gl.ttm);
}
void osd_life_init(struct osd_life *l, struct osd_element *container,
		double x, double y)
{
	l->max_life = gl.l->num_1ups + DEFAULT_LIFE;
	int cwidth  = (l->max_life-1)*14 + SHIP_W + 2*2;
	int cheight = SHIP_H;
	_o(container, x, y, cwidth, cheight,  0,  4, 302, 1, 1,  0, gl.ttm);
	osdlib_make_children(container, l->max_life, 0);
	l->ships = container->ch;
	for (size_t i = 0; i < l->max_life; ++i) {
		_o(&l->ships[i], -((signed)i*14 + .1), 0, SHIP_W, SHIP_H,  0.5,
				SHIP_ON_IMG_X + 12*SHIP_W*(i%2), SHIP_ON_IMG_Y,
				SHIP_W, SHIP_H, 0, gl.ttm);
		l->ships[i].z = i*0.01;
	}
}
void osd_init()
{
	const struct osdlib_font osd_font = {
		.t = gl.ftm,
		.w = 16,
		.h = 16,
		.tex_x = 0,
		.tex_y = 0,
		.offset = 32
	};
	_o(&osd.root, 0, 0, gl.win_w, gl.win_h, 1, 0, 0, 0, 0, 1, gl.ttm);
	osdlib_make_children(&osd.root, 4, 1, &osd.rect, &osd.panel, &osd.gameover, &osd.victory);
	/* left rect */
	_o(osd.rect, 0,  -.1,  144, 80,  0.8,  0, 90,  1, 1,  0, gl.ttm);
	struct osd_element *fuel_cont, *cross, *key_cont;
	osdlib_make_children(osd.rect, 3, 1, &fuel_cont, &cross, &key_cont);
	osd_fuel_init(&osd.fuel, fuel_cont, 12, 8);
	osd_velocity_init(&osd.velocity, cross, 40, 8);
	osd_keys_init(&osd.keys, key_cont, -12, 8);
	/* panel */
	_o(osd.panel, 142, -.1, -142, 32,  0.8,  0, 90,  1, 1,  0, gl.ttm);
	struct osd_element *lfreight, *sfreight, *hbfreight, *life;
	osdlib_make_children(osd.panel, 4, 1, &lfreight, &sfreight, &hbfreight, &life);
	osd_freight_init(&osd.freight_level, lfreight, 8, 8, 384, 400);
	osd_freight_init(&osd.freight_ship, sfreight, 12, 0, 432, 400);
	sfreight->x.rel = End;
	sfreight->x.orig = Begin;
	sfreight->rel = lfreight;
	osd_freight_init(&osd.freight_hb, hbfreight, 12, 0, 480, 400);
	hbfreight->x.rel = End;
	hbfreight->x.orig = Begin;
	hbfreight->rel = sfreight;
	osd_life_init(&osd.life, life, -8, 6);
	_o(osd.gameover, 0, 0, 160, 32, 0.8, 0, 90, 1, 1, TS, gl.ttm);
	center_on_screen(osd.gameover);
	struct osd_element *gameover_img;
	osdlib_make_children(osd.gameover, 1, 1, &gameover_img);
	_o(gameover_img, 8, 8, 0, 0, 0.8, 0, 0, 0, 0, T, gl.ttm);
	osdlib_make_text(gameover_img, &osd_font, "GAME OVER");
	_o(osd.victory, 0, 0, 144, 32, 0.8, 0, 90, 1, 1, TS, gl.ttm);
	center_on_screen(osd.victory);
	struct osd_element *victory_img;
	osdlib_make_children(osd.victory, 1, 1, &victory_img);
	_o(victory_img, 8, 8, 0, 0, 0.8, 0, 0, 0, 0, T, gl.ttm);
	osdlib_make_text(victory_img, &osd_font, "VICTORY!");
}

void osd_fuel_step(struct osd_fuel *f, double fuel)
{
	for (size_t i = 0; i < (size_t)ceil(fuel); ++i)
		f->bars[i].a = 0.8;
	for (size_t i = (size_t)ceil(fuel); i < 16; ++i)
		f->bars[i].a = 0.1;
}
void osd_velocity_step(struct osd_velocity *v, double vx, double vy,
		double max_vx, double max_vy)
{
	v->xbar->x.v   = fmin(64, fmax(0,  vx/3     + 31));
	v->ybar->y.v   = fmin(64, fmax(0,  vy/3     + 31));
	v->mxbar1->x.v = fmin(64, fmax(0,  max_vx/3 + 31));
	v->mxbar2->x.v = fmin(64, fmax(0, -max_vx/3 + 31));
	v->mybar->y.v  = fmin(64, fmax(0,  max_vy/3 + 31));
}
void osd_keys_step(struct osd_keys *k, const int *keys)
{
	for (size_t i = 0; i < 4; ++i) {
		if (!keys[i]) {
			k->keys[i].a = 0.2;
			k->keys[i].tex_x = 256;
		} else {
			k->keys[i].tex_x = 256 + 16 * ((int)(gl.l->time*KEY_ANIM_SPEED + i) % 8);
			k->keys[i].a = 0.8;
		}
	}
}
void osd_freight_step(struct osd_freight *f, const struct freight *flist, size_t nfreight)
{
	for (size_t i = 0; i < nfreight; ++i) {
		f->freight[i].tr = Opaque;
		f->freight[i].tex_x = 80 + 16*flist[i].f;
	}
	for (size_t i = nfreight; i < f->max_freight; ++i)
		f->freight[i].tr = TransparentElement;
	/* shelf length = number of freights + number of gaps + 2x margin(4) */
	int shelf_len = f->max_freight*16 + (f->max_freight-1)*3 + 8;
	f->container->w = shelf_len + 44;
}
void osd_life_step(struct osd_life *l, size_t life)
{
	for (size_t i = 0; i < life; ++i)
		l->ships[i].tr = Opaque;
	for (size_t i = life; i < l->max_life; ++i)
		l->ships[i].tr = TransparentElement;
}
void osd_step()
{
	struct ship *ship = gl.l->ship;
	osd_fuel_step(&osd.fuel, ship->fuel);
	osd_velocity_step(&osd.velocity, ship->vx, ship->vy,
			ship->max_vx, ship->max_vy);
	osd_keys_step(&osd.keys, ship->keys);
	size_t nfreight = cg_freight_remaining(gl.l);
	struct freight freight[nfreight];
	cg_get_freight_airports(gl.l, freight);
	osd_freight_step(&osd.freight_level, freight, nfreight);
	osd.freight_ship.max_freight = ship->max_freight;
	osd_freight_step(&osd.freight_ship, ship->freight, ship->num_freight);
	struct airport *hb = gl.l->hb;
	osd_freight_step(&osd.freight_hb, hb->c.freight, hb->num_cargo);
	osd_life_step(&osd.life, max(0, gl.l->ship->life));
	if (gl.l->status == Victory)
		osd.victory->tr = Opaque;
	if (gl.l->status == Lost)
		osd.gameover->tr = Opaque;
}
void osd_draw()
{
	osdlib_draw(&osd.root);
}
