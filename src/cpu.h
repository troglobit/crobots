/* cpu.h - the routines to execute crobot instructions
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
#ifndef CROBOTS_CPU_H_
#define CROBOTS_CPU_H_

long push(long k);
long pop(void);
void cycle(void);
void binaryop(int op);
void robot_go(struct robot *r);
void dumpvar(long *pool, int size);

#endif /* CROBOTS_CPU_H_ */

/**
 * Local Variables:
 *  indent-tabs-mode: nil
 *  c-file-style: "gnu"
 * End:
 */
