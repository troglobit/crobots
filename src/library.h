/* library.h - intrinsic routines, all must push a long onto the stack
 *
 * Copyright (C) 1985-2013 Tom Poindexter
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef CROBOTS_LIBRARY_H_
#define CROBOTS_LIBRARY_H_

/* declare the intrinsic functions, all must push a long value on the stack */
/* these functions don't return a long, but declared long for notation */
void c_scan  (void);  /* scan(degree,res);  >0 = robot distance, 0 = nothing */
void c_cannon(void);  /* cannon(degree,dist); fire cannon */
void c_drive (void);  /* drive(degree,speed); speed 0-100 in % */
void c_damage(void);  /* damage(); = current damage in % */
void c_speed (void);  /* speed(); = current speed */
void c_loc_x (void);  /* loc_x(); = current x location */
void c_loc_y (void);  /* loc_y(); = current y location */
void c_rand  (void);  /* rand(limit); = 0 -- limit (2**15)-1 */
void c_sin   (void);  /* sin(degree); = sin * 100000 */
void c_cos   (void);  /* cos(degree); = cos * 100000 */
void c_tan   (void);  /* tan(degree); = tan * 100000 */
void c_atan  (void);  /* atan(ratio); = degree */
void c_sqrt  (void);  /* sqrt(x); = square root */

#endif /* CROBOTS_LIBRARY_H_ */

/**
 * Local Variables:
 *  indent-tabs-mode: nil
 *  c-file-style: "gnu"
 * End:
 */
