/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2001  MandrakeSoft S.A.
//
//    MandrakeSoft S.A.
//    43, rue d'Aboukir
//    75002 Paris - France
//    http://www.linux-mandrake.com/
//    http://www.mandrakesoft.com/
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA


#include <assert.h>
#include <map>
#include <string>
#include <iostream>
using std::cerr;
using std::endl;


#include "bochs.h"
#include "cpu/cpu.h"

// maximum size of an instruction
#define MAX_OPCODE_SIZE 16

// maximum physical addresses an instruction can generate
#define MAX_DATA_ACCESSES 1024

// Use this variable to turn on/off collection of instrumentation data
// If you are not using the debugger to turn this on/off, then possibly
// start this at 1 instead of 0.
unsigned instr = 0;
unsigned a0_b0 = 0;
unsigned dx = 0;
unsigned ax = 0;
unsigned invl_ax = 1;

void bx_instr_start()
{
  a0_b0 = 0;
  invl_ax = 1;
  dx = 0;
  ax = 0;
  instr = 1;
  printf("vgaio: movw $0xa000, %%bx\nvgaio: movw %%bx, %%ds\n");
}

void bx_instr_stop()
{
  instr = 0;
}

void bx_instr_phy_write(unsigned cpu, bx_address addr, unsigned len, void *data)
{
/*
  unsigned char *chdata = (unsigned char *) data;
  unsigned new_a0b0;
  unsigned seg;
  unsigned off;

  if (instr && ((addr & 0xfffe0000) == 0xa0000))
    {
      seg = addr >> 16;
      new_a0b0 = (seg == 0xb);
      if (new_a0b0 != a0_b0)
        {
          printf("vgaio: movw $0x%x000, %%bx\nvgaio: movw %%bx, %%ds\n", seg);
          a0_b0 = new_a0b0;
        }

      off = addr & 0x0000ffff;
      if (len == 1)
        printf("vgaio: movb $0x%x, (0x%x)\n", chdata[0], off);
      else if (len == 2)
        printf("vgaio: movw $0x%x%x, (0x%x)\n", chdata[1], chdata[0], off);

      else printf("vgaio: ERROR phy_write()\n");
    }
*/
}

void bx_instr_phy_read(unsigned cpu, bx_address addr, unsigned len)
{/*
  unsigned new_a0b0;
  unsigned seg;
  unsigned off;

  if (instr && ((addr & 0xfffe0000) == 0xa0000))
    {
      seg = addr >> 16;
      new_a0b0 = (seg == 0xb);
      if (new_a0b0 != a0_b0)
        {
          printf("vgaio: movw $0x%x000, %%bx\nvgaio: movw %%bx, %%ds\n", seg);
          a0_b0 = new_a0b0;
        }

      off = addr & 0x0000ffff;
      if (len == 1)
        printf("vgaio: movb (0x%x), %%cl\n", off);
      else if (len == 2)
        printf("vgaio: movw (0x%x), %%cx\n", off);

      else printf("vgaio: ERROR phy_read()\n");
    }
 */
}

void bx_instr_inp2(Bit16u addr, unsigned len, Bit32u val)
{
  if (instr && ((addr & 0xffe0) == 0x03c0))
    {
      if (addr != dx)
        {
          //printf("vgaio: movw $0x%x, %%dx\n", addr);
          dx = addr;
        }

      invl_ax = 1;
      if (len == 1)
//        printf("vgaio: inb %%dx, %%al (dx = %02x)\n", val);
        printf("vgaio: inb $0x%x => %02x\n", addr, val);
      else if (len == 2)
//        printf("vgaio: inw %%dx, %%al (dx = %04x)\n", val);
        printf("vgaio: inw $0x%x => %04x\n", addr, val);
      else printf("vgaio: ERROR inp2()\n");
    }
}

void bx_instr_outp2(Bit16u addr, unsigned len, Bit32u val)
{
 /*
  if (instr && ((addr & 0xffe0) == 0x03c0))
    {
      if (addr != dx)
        {
          printf("vgaio: movw $0x%x, %%dx\n", addr);
          dx = addr;
        }

      if (invl_ax || ax != val)
        {
          printf("vgaio: movw $0x%x, %%ax\n", val);
          invl_ax = 0;
          ax = val;
        }

      if (len == 1)
        printf("vgaio: outb %%al, %%dx\n");
      else if (len == 2)
        printf("vgaio: outw %%ax, %%dx\n");
      else printf("vgaio: ERROR outp2()\n");
    }
 */
}
