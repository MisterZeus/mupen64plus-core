/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Mupen64plus - memory.h                                                *
 *   Mupen64Plus homepage: http://code.google.com/p/mupen64plus/           *
 *   Copyright (C) 2002 Hacktarux                                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef M64P_DEVICE_MEMORY_MEMORY_H
#define M64P_DEVICE_MEMORY_MEMORY_H

#include <stdint.h>

#include "device/r4300/new_dynarec/new_dynarec.h" /* for NEW_DYNAREC_ARM */

struct memory
{
    uint64_t* rdword;

#if NEW_DYNAREC != NEW_DYNAREC_ARM
/* ARM dynarec uses a different memory layout */
    union {
        uint8_t  wbyte;
        uint16_t whword;
        uint32_t wword;
        uint64_t wdword;
    };

    uint32_t address;
#endif

    void (*readmem[0x10000])(void);
    void (*readmemb[0x10000])(void);
    void (*readmemh[0x10000])(void);
    void (*readmemd[0x10000])(void);
    void (*writemem[0x10000])(void);
    void (*writememb[0x10000])(void);
    void (*writememh[0x10000])(void);
    void (*writememd[0x10000])(void);

#ifdef DBG
    int memtype[0x10000];
    void (*saved_readmemb[0x10000])(void);
    void (*saved_readmemh[0x10000])(void);
    void (*saved_readmem [0x10000])(void);
    void (*saved_readmemd[0x10000])(void);
    void (*saved_writememb[0x10000])(void);
    void (*saved_writememh[0x10000])(void);
    void (*saved_writemem [0x10000])(void);
    void (*saved_writememd[0x10000])(void);
#endif
};

uint32_t* memory_address();
uint8_t*  memory_wbyte();
uint16_t* memory_whword();
uint32_t* memory_wword();
uint64_t* memory_wdword();

/* struct memory definition is required prior including this */
#include "main/main.h"

#define read_word_in_memory() g_dev.mem.readmem[*memory_address()>>16]()
#define read_byte_in_memory() g_dev.mem.readmemb[*memory_address()>>16]()
#define read_hword_in_memory() g_dev.mem.readmemh[*memory_address()>>16]()
#define read_dword_in_memory() g_dev.mem.readmemd[*memory_address()>>16]()
#define write_word_in_memory() g_dev.mem.writemem[*memory_address()>>16]()
#define write_byte_in_memory() g_dev.mem.writememb[*memory_address() >>16]()
#define write_hword_in_memory() g_dev.mem.writememh[*memory_address() >>16]()
#define write_dword_in_memory() g_dev.mem.writememd[*memory_address() >>16]()

#ifndef M64P_BIG_ENDIAN
#if defined(__GNUC__) && (__GNUC__ > 4  || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))
#define sl(x) __builtin_bswap32(x)
#else
#define sl(mot) \
( \
((mot & 0x000000FF) << 24) | \
((mot & 0x0000FF00) <<  8) | \
((mot & 0x00FF0000) >>  8) | \
((mot & 0xFF000000) >> 24) \
)
#endif
#define S8 3
#define S16 2
#define Sh16 1

#else

#define sl(mot) mot
#define S8 0
#define S16 0
#define Sh16 0

#endif

static void masked_write(uint32_t* dst, uint32_t value, uint32_t mask)
{
    *dst = (*dst & ~mask) | (value & mask);
}

void poweron_memory(struct memory* mem);

void map_region(struct memory* mem,
                uint16_t region,
                int type,
                void (*read8)(void),
                void (*read16)(void),
                void (*read32)(void),
                void (*read64)(void),
                void (*write8)(void),
                void (*write16)(void),
                void (*write32)(void),
                void (*write64)(void));

/* XXX: cannot make them static because of dynarec + rdp fb */
void read_rdram(void);
void read_rdramb(void);
void read_rdramh(void);
void read_rdramd(void);
void write_rdram(void);
void write_rdramb(void);
void write_rdramh(void);
void write_rdramd(void);
void read_rdramFB(void);
void read_rdramFBb(void);
void read_rdramFBh(void);
void read_rdramFBd(void);
void write_rdramFB(void);
void write_rdramFBb(void);
void write_rdramFBh(void);
void write_rdramFBd(void);

/* Returns a pointer to a block of contiguous memory
 * Can access RDRAM, SP_DMEM, SP_IMEM and ROM, using TLB if necessary
 * Useful for getting fast access to a zone with executable code. */
uint32_t *fast_mem_access(uint32_t address);

#ifdef DBG
void activate_memory_break_read(struct memory* mem, uint32_t address);
void deactivate_memory_break_read(struct memory* mem, uint32_t address);
void activate_memory_break_write(struct memory* mem, uint32_t address);
void deactivate_memory_break_write(struct memory* mem, uint32_t address);
int get_memory_type(struct memory* mem, uint32_t address);
#endif

#endif
