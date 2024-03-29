/*
 * safe-syscall.inc.S : host-specific assembly fragment
 * to handle signals occurring at the same time as system calls.
 * This is intended to be included by common-user/safe-syscall.S
 *
 * Copyright (C) 2015 Timothy Edward Baldwin <T.E.Baldwin99@members.leeds.ac.uk>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

        .global safe_syscall_base
        .global safe_syscall_start
        .global safe_syscall_end
        .type   safe_syscall_base, @function

        /* This is the entry point for making a system call. The calling
         * convention here is that of a C varargs function with the
         * first argument an 'int *' to the signal_pending flag, the
         * second one the system call number (as a 'long'), and all further
         * arguments being syscall arguments (also 'long').
         */
safe_syscall_base:
        .cfi_startproc
        /* This saves a frame pointer and aligns the stack for the syscall.
         * (It's unclear if the syscall ABI has the same stack alignment
         * requirements as the userspace function call ABI, but better safe than
         * sorry. Appendix A2 of http://www.x86-64.org/documentation/abi.pdf
         * does not list any ABI differences regarding stack alignment.)
         */
        push    %rbp
        .cfi_adjust_cfa_offset 8
        .cfi_rel_offset rbp, 0

        /*
         * The syscall calling convention isn't the same as the C one:
         * we enter with rdi == &signal_pending
         *               rsi == syscall number
         *               rdx, rcx, r8, r9, (stack), (stack) == syscall arguments
         *               and return the result in rax
         * and the syscall instruction needs
         *               rax == syscall number
         *               rdi, rsi, rdx, r10, r8, r9 == syscall arguments
         *               and returns the result in rax
         * Shuffle everything around appropriately.
         * Note that syscall will trash rcx and r11.
         */
        mov     %rsi, %rax /* syscall number */
        mov     %rdi, %rbp /* signal_pending pointer */
        /* and the syscall arguments */
        mov     %rdx, %rdi
        mov     %rcx, %rsi
        mov     %r8,  %rdx
        mov     %r9,  %r10
        mov     16(%rsp), %r8
        mov     24(%rsp), %r9

        /* This next sequence of code works in conjunction with the
         * rewind_if_safe_syscall_function(). If a signal is taken
         * and the interrupted PC is anywhere between 'safe_syscall_start'
         * and 'safe_syscall_end' then we rewind it to 'safe_syscall_start'.
         * The code sequence must therefore be able to cope with this, and
         * the syscall instruction must be the final one in the sequence.
         */
safe_syscall_start:
        /* if signal_pending is non-zero, don't do the call */
        cmpl    $0, (%rbp)
        jnz     2f
        syscall
safe_syscall_end:

        /* code path for having successfully executed the syscall */
#if defined(__linux__)
        /* Linux kernel returns (small) negative errno. */
        cmp     $-4095, %rax
        jae     0f
#elif defined(__FreeBSD__)
        /* FreeBSD kernel returns positive errno and C bit set. */
        jc      1f
#else
#error "unsupported os"
#endif
        pop     %rbp
        .cfi_remember_state
        .cfi_def_cfa_offset 8
        .cfi_restore rbp
        ret
        .cfi_restore_state

#if defined(__linux__)
0:      neg     %eax
        jmp     1f
#endif

        /* code path when we didn't execute the syscall */
2:      mov     $QEMU_ERESTARTSYS, %eax

        /* code path setting errno */
1:      pop     %rbp
        .cfi_def_cfa_offset 8
        .cfi_restore rbp
        jmp     safe_syscall_set_errno_tail
        .cfi_endproc

        .size   safe_syscall_base, .-safe_syscall_base
