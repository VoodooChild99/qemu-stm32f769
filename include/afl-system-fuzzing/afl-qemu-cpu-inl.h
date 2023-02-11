/*
 * QEMU/AFL Inline Implementation
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

#ifndef __AFL_SYSTEM_FUZZING_AFL_QEMU_INLINE__ 
#define __AFL_SYSTEM_FUZZING_AFL_QEMU_INLINE__ 

#include <sys/shm.h>
#include "afl.h"
#include "../../../afl-qemu/config.h"

/* init extern vars */
/* input file */
const char *afl_input_file = NULL;
int afl_fuzz_mbedtls = 0;

int afl_qemuloop_pipe[2] = {0, 0};
int afl_wants_to_resume_exec = 0;
TCGContext **restart_tcg_ctx = NULL;
QemuThread *single_tcg_cpu_thread = NULL;

int afl_enable_ticks = 0;
int afl_start = 0;

int afl_fork_child = 0;
int afl_wants_cpu_to_stop;

unsigned char *afl_area_ptr = NULL;
uint32_t afl_map_size = MAP_SIZE;
unsigned char afl_state_ptr[0x400] = {0};

/* instrument ratio */
static unsigned int afl_inst_rms = MAP_SIZE;

void afl_setup(void) {
    char *id_str = getenv(SHM_ENV_VAR);
    char *inst_r = getenv("AFL_INST_RATIO");

    int shm_id;

    if (inst_r) {
        unsigned int r;
        r = atoi(inst_r);

        if (r > 100) {
            r = 100;
        }

        if (!r) {
            r = 1;
        }

        afl_inst_rms = MAP_SIZE * r / 100;
    }

    if (id_str) {
        shm_id = atoi(id_str);
        afl_area_ptr = (unsigned char*)shmat(shm_id, NULL, 0);

        if (afl_area_ptr == (void*)-1) {
            exit(1);
        }

        if (inst_r) {
            afl_area_ptr[0] = 1;
        }
    }
}

static ssize_t uninterrupted_read(int fd, void *buf, size_t size) {
    ssize_t n;
    while ((n = read(fd, buf, size)) == -1 && errno == EINTR) {
        continue;
    }

    return n;
}

#ifdef AFL_QEMU_SYSTEM_TSL
#define TSL_FD (FORKSRV_FD - 1)

static void afl_wait_tsl(int);
#endif

unsigned int afl_forksrv_pid;

void afl_forkserver(void) {
    static unsigned char tmp[4];

    if (!afl_area_ptr) {
        return;
    }

    if (write(FORKSRV_FD + 1, tmp, 4) != 4) {
        return;
    }

    afl_forksrv_pid = getpid();

    /* fork server logic */
    while (1) {
        pid_t child_pid;
        uint32_t was_killed;
        int status;
#ifdef AFL_QEMU_SYSTEM_TSL
        int t_fd[2];
#endif
        if (uninterrupted_read(FORKSRV_FD, &was_killed, 4) !=4) {
            exit(2);
        }

        // if (was_killed) {
        //     if (waitpid(child_pid, &status, 0) < 0) {
        //         exit(1);
        //     }
        // }

#ifdef AFL_QEMU_SYSTEM_TSL
        if (pipe(t_fd) || dup2(t_fd[1], TSL_FD) < 0) {
            exit(3);
        }
        close(t_fd[1]);
#endif
        child_pid = fork();

        if (child_pid < 0) {
            /* fork failed */
            exit(4);
        }

        if (!child_pid) {
            /* child */
            afl_fork_child = 1;
            close(FORKSRV_FD);
            close(FORKSRV_FD + 1);
#ifdef AFL_QEMU_SYSTEM_TSL
            close(t_fd[0]);
#endif
            return;
        } else {
            /* parent */
#ifdef AFL_QEMU_SYSTEM_TSL
            close(TSL_FD);
#endif
            if (write(FORKSRV_FD + 1, &child_pid, 4) != 4) {
                exit(5);
            }

#ifdef AFL_QEMU_SYSTEM_TSL
            afl_wait_tsl(t_fd[0]);
#endif
            if (waitpid(child_pid, &status, 0) < 0) {
                exit(7);
            }

            if (write(FORKSRV_FD + 1, &status, 4) != 4) {
                exit(8);
            }

        }
    }
}

static inline target_ulong do_afl_hash(target_ulong cur_loc) {
    target_ulong h = cur_loc;
    h ^= cur_loc >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    h &= (MAP_SIZE - 1);

    return h;
}

__thread target_ulong prev_loc = 0;

void afl_trace(target_ulong pc) {
    pc = do_afl_hash(pc);
    if (pc) {
        afl_area_ptr[pc ^ prev_loc] += 1;
        prev_loc = pc >> 1;
    }
}

void afl_trace_const_hash(target_ulong index, target_ulong new_prev) {
    if (index) {
        afl_area_ptr[index] += 1;
        prev_loc = new_prev;
    }
}

#ifdef AFL_QEMU_SYSTEM_TSL

static void afl_request_code_translate(target_ulong, target_ulong, uint32_t,
                                        uint32_t, int, CPUArchState*);
static void afl_request_block_chaining(TranslationBlock*, TranslationBlock*,
                                        int, int, CPUArchState*);

struct afl_tsl {
    target_ulong pc;
    target_ulong cs_base;
    uint32_t flags;
    uint32_t cflags;
    int cpu_id;
};

struct afl_chain {
    target_ulong last_pc;
    target_ulong last_cs_base;
    uint32_t last_flags;
    uint32_t last_cflags;
    target_ulong pc;
    target_ulong cs_base;
    uint32_t flags;
    uint32_t cflags;
    int tb_exit;
    int cpu_id;
};

static void afl_request_code_translate(target_ulong pc, target_ulong cs_base,
                                        uint32_t flags, uint32_t cflags,
                                        int cpu_id, CPUArchState *env) {
    struct afl_tsl t;
    uint8_t magic = 0;

    if (!afl_fork_child) {
        return;
    }

    t.pc        = pc;
    t.cs_base   = cs_base;
    t.flags     = flags;
    t.cflags    = cflags;
    t.cpu_id    = cpu_id;

    if (write(TSL_FD, &magic, sizeof(magic)) != sizeof(magic)) {
        return;
    }

    if (write(TSL_FD, &t, sizeof(t)) != sizeof(t)) {
        return;
    }

    afl_extract_arch_state(afl_state_ptr, env, true);
    if (write(TSL_FD, afl_state_ptr, 0x400) != 0x400) {
        return;
    }
}

static void afl_request_block_chaining(TranslationBlock* last_tb,
                                        TranslationBlock* tb,
                                        int tb_exit, int cpu_id,
                                        CPUArchState *env) {
    struct afl_chain c;
    uint8_t magic = 1;

    if (!afl_fork_child) {
        return;
    }

    c.last_pc       = last_tb->pc;
    c.last_cs_base  = last_tb->cs_base;
    c.last_flags    = last_tb->flags;
    c.last_cflags   = last_tb->cflags;

    c.pc            = tb->pc;
    c.cs_base       = tb->cs_base;
    c.flags         = tb->flags;
    c.cflags        = tb->cflags;

    c.tb_exit       = tb_exit;
    c.cpu_id        = cpu_id;

    if (write(TSL_FD, &magic, sizeof(magic)) != sizeof(magic)) {
        return;
    }

    if (write(TSL_FD, &c, sizeof(c)) != sizeof(c)) {
        return;
    }

    afl_extract_arch_state(afl_state_ptr, env, true);
    if (write(TSL_FD, afl_state_ptr, 0x400) != 0x400) {
        return;
    }
}

static inline void tb_add_jump(TranslationBlock *tb, int n,
                               TranslationBlock *tb_next);

static void afl_wait_tsl(int fd) {
    struct afl_tsl t;
    struct afl_chain c;
    TranslationBlock *tb;
    TranslationBlock *last_tb;
    CPUState *cpu = NULL;
    uint8_t magic;

    while (1) {
        if (read(fd, &magic, sizeof(magic)) != sizeof(magic)) {
            break;
        }

        if (magic) {
            /* block chaining request */
            if (read(fd, &c, sizeof(c)) != sizeof(c)) {
                break;
            }

            if (read(fd, afl_state_ptr, 0x400) != 0x400) {
                break;
            }

            cpu = qemu_get_cpu(c.cpu_id);
            assert(cpu);
            if (mttcg_enabled) {
                tcg_ctx = restart_tcg_ctx[c.cpu_id];
            }
            afl_load_arch_state(afl_state_ptr, cpu->env_ptr, true);
            cpu->env_modified = true;

            last_tb = tb_lookup(cpu, c.last_pc, c.last_cs_base, c.last_flags,
                                c.last_cflags);
            tb = tb_lookup(cpu, c.pc, c.cs_base, c.flags, c.cflags);

            if (tb && last_tb) {
                tb_add_jump(last_tb, c.tb_exit, tb);
            }
        } else {
            /* code translation request */
            if (read(fd, &t, sizeof(t)) != sizeof(t)) {
                break;
            }

            if (read(fd, afl_state_ptr, 0x400) != 0x400) {
                break;
            }

            cpu = qemu_get_cpu(t.cpu_id);
            assert(cpu);
            if (mttcg_enabled) {
                tcg_ctx = restart_tcg_ctx[t.cpu_id];
            }
            afl_load_arch_state(afl_state_ptr, cpu->env_ptr, true);
            cpu->env_modified = true;
            mmap_lock();
            tb = tb_gen_code(cpu, t.pc, t.cs_base, t.flags, t.cflags);
            mmap_unlock();
        }
    }

    close(fd);
}
#endif
#endif