/*
 * QEMU/AFL Header
 * 
 * Copyright (C) 2022 JSSEC
 * 
 * Authors:
 * 	Rayhub <leicq@seu.edu.cn>
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
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AFL_SYSTEM_FUZZING_AFL__ 
#define __AFL_SYSTEM_FUZZING_AFL__ 

/* external variables */
extern const char *afl_input_file;
extern int afl_fuzz_mbedtls;
// extern const char *block_black_list_file;
// extern GHashTable *block_black_list_hash_table;

extern int afl_qemuloop_pipe[2];
extern int afl_wants_to_resume_exec;
extern TCGContext **restart_tcg_ctx;
extern QemuThread *single_tcg_cpu_thread;

extern int afl_enable_ticks;
extern int afl_start;

extern int afl_fork_child;
extern int afl_wants_cpu_to_stop;

extern unsigned char *afl_area_ptr;
extern uint32_t afl_map_size;
extern unsigned char afl_state_ptr[0x400];

/* fcuntions */
void afl_setup(void);
void afl_forkserver(void);

#define AFL_QEMU_SYSTEM_TSL
#define AFL_QEMU_BINARY_ONLY_INSTRUMENT
// #define AFL_QEMU_INSTR_INLINE
// #define AFL_USE_BLACKLIST

void afl_trace(target_ulong);
void afl_trace_const_hash(target_ulong, target_ulong);

#endif