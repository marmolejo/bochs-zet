/////////////////////////////////////////////////////////////////////////
// $Id$
/////////////////////////////////////////////////////////////////////////
//
//   Copyright (c) 2009 Stanislav Shwartsman
//          Written by Stanislav Shwartsman [sshwarts at sourceforge net]
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
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


#include <assert.h>

#include "bochs.h"
#include "cpu/cpu.h"

#include <map>
#include <string>
#include <iostream>
using std::cout;
using std::endl;

typedef std::map<std::string, unsigned> TStrUIntMap;

struct bx_instr_ia_stats {
   bx_bool active;
   Bit32u ia_cnt[BX_IA_LAST];
   Bit32u total_cnt;
   Bit32u interrupts;
   Bit32u exceptions;
} *ia_stats;

static logfunctions *instrument_log = new logfunctions ();
#define LOG_THIS instrument_log->

void bx_instr_initialize(unsigned cpu)
{
  assert(cpu < BX_SMP_PROCESSORS);

  if (ia_stats == NULL)
      ia_stats = new bx_instr_ia_stats[BX_SMP_PROCESSORS];

  ia_stats[cpu].active = 0;

  fprintf(stderr, "Initialize cpu %d instrumentation module\n", cpu);
}

void bx_instr_reset(unsigned cpu, unsigned type)
{
  ia_stats[cpu].active    = 0;
  ia_stats[cpu].total_cnt = 0;

  for(int n=0; n < BX_IA_LAST; n++)
    ia_stats[cpu].ia_cnt[n] = 0;

  ia_stats[cpu].interrupts = ia_stats[cpu].exceptions = 0;
}

void bx_instr_interrupt(unsigned cpu, unsigned vector)
{
  if(ia_stats[cpu].active) ia_stats[cpu].interrupts++;
}

void bx_instr_exception(unsigned cpu, unsigned vector, unsigned error_code)
{
  if(ia_stats[cpu].active) ia_stats[cpu].exceptions++;
}

void bx_instr_hwinterrupt(unsigned cpu, unsigned vector, Bit16u cs, bx_address eip)
{
  if(ia_stats[cpu].active) ia_stats[cpu].interrupts++;
}

void bx_instr_before_execution(unsigned cpu, bxInstruction_c *i, bx_address offset)
{
  Bit16u sel;
  if(ia_stats[cpu].active) {
    sel = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
    if (sel!=0xf000 && sel!=0xc000) {
      ia_stats[cpu].ia_cnt[i->getIaOpcode()]++;
      ia_stats[cpu].total_cnt++;

// This is to trace the unknown instruction
//      if (strcmp(get_bx_feature_name(i->getIaOpcode()),"0"))
//        BX_CPU(cpu)->debug_disasm_instruction(offset);
    }
  }
}

void bx_instr_debug_cmd(const char *comm)
{
  if(!strcmp(comm,"start"))
    {
      for (int i=0; i<BX_SMP_PROCESSORS; i++) ia_stats[i].active = 1;

      printf ("Statistics are active now for all processors\n");
    }
  else
    {
      if (!strcmp(comm,"stop"))
        {
          for (int i=0; i<BX_SMP_PROCESSORS; i++) ia_stats[i].active = 0;

          printf ("Statistics are not active for all processors\n");
        }
      else
        {
          if (!strcmp(comm,"print"))
            {
              for (int cpu=0; cpu<BX_SMP_PROCESSORS; cpu++)
                {
                  if (ia_stats[cpu].active)
                    {
                      TStrUIntMap fst;   // Feature stats

                      printf("Dump IA stats for CPU %d\n", cpu);
                      printf("--------------------------------------------\n");
                      printf("Interrupts: %d, Exceptions: %d\n",
                             ia_stats[cpu].interrupts, ia_stats[cpu].exceptions);

                      for (int n=0;n < BX_IA_LAST; n++)
                        if (ia_stats[cpu].ia_cnt[n] > 0)
                          fst[get_bx_feature_name(n)]++;

                      printf("\n");
                      printf("Features stats for CPU %d\n", cpu);
                      printf("------------------------\n");

                      // use const_iterator to walk through elements of pairs
                      for ( std::map<std::string, unsigned>
                        ::const_iterator iter = fst.begin();
                        iter != fst.end(); ++iter )

                        {
                          cout << "[> Instructions using feature " << iter->first
                               << ": " << iter->second << " <]" << endl;
                          for (int n=0;n < BX_IA_LAST; n++)
                            if (ia_stats[cpu].ia_cnt[n] > 0
                                && !strcmp(get_bx_feature_name(n),iter->first.c_str()))

                            printf("%s: %d (%f%%)\n", get_bx_opcode_name(n),
                                   ia_stats[cpu].ia_cnt[n],
                                   ia_stats[cpu].ia_cnt[n] * 100.0 / ia_stats[cpu].total_cnt);

                          cout << endl;
                        }
                    }
                  else
                    {
                      printf("Statistics are not currently active for CPU %d\n", cpu);
                    }
                }
            }
          else
            {
              printf("Unknown instrumentation command\n");
            }
        }
    }
}


/*
void bx_instr_mem_data(unsigned cpu, unsigned seg, bx_address offset, unsigned len, unsigned rw)
{
  unsigned index;
  bx_phy_address phy;

  if(!stats || !instruction[cpu].valid) return;

  if (instruction[cpu].num_data_accesses >= MAX_DATA_ACCESSES)
  {
    return;
  }

  bx_address lin = BX_CPU(cpu)->get_laddr(seg, offset);
  bx_bool page_valid = BX_CPU(cpu)->dbg_xlate_linear2phy(lin, &phy);
  phy = A20ADDR(phy);

  // If linear translation doesn't exist, a paging exception will occur.
  // Invalidate physical address data for now.
  if (!page_valid)
  {
    phy = 0;
  }

  index = instruction[cpu].num_data_accesses;
  instruction[cpu].data_access[index].laddr = lin;
  instruction[cpu].data_access[index].paddr = phy;
  instruction[cpu].data_access[index].op    = rw;
//  instruction[cpu].data_access[index].size  = size;
  instruction[cpu].num_data_accesses++;
}

void bx_instr_mem_data_access(unsigned cpu, unsigned seg, unsigned offset, unsigned len, unsigned rw)
{
  return;
}

void bx_instr_inp2(Bit16u addr, unsigned len, Bit32u val)
{
  char tmpbuf[50];
  Bit16u sel;
  if(stats)
  {
    sel = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
    if (sel!=0xf000 && sel!=0xc000) {
      sprintf(tmpbuf, "in%d %x: %x", len, addr, val);
      (*stats)[std::string(tmpbuf)]++;

//      if ((addr&0xffe0)==0x3c0)
//        {
//          printf("in%d %x: %x\n", len, addr, val);
//        }

    }
  }
}

void bx_instr_outp(Bit16u addr, unsigned len, Bit32u val)
{
  char tmpbuf[50];
  Bit16u sel;
  if(stats)
  {
    sel = bx_cpu.sregs[BX_SEG_REG_CS].selector.value;
    if (sel!=0xf000 && sel!=0xc000) {
      sprintf(tmpbuf, "out%d %x: %x", len, addr, val);
      (*stats)[std::string(tmpbuf)]++;

//      if ((addr&0xffe0)==0x3c0)
//        {
//          printf("out%d %x: %x\n", len, addr, val);
//        }

    }
  }
}
*/