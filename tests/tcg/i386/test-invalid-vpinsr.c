/*
 * Verify that fixed-width VEX instructions reject reserved VEX.L values.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static sigjmp_buf sigill_env;

static void sigill_handler(int signal)
{
    siglongjmp(sigill_env, signal);
}

static void invalid_vpinsrb(void)
{
    asm volatile(".byte 0xc4, 0xe3, 0x7d, 0x20, 0xc0, 0x00");
}

static void invalid_vpinsrd(void)
{
    asm volatile(".byte 0xc4, 0xe3, 0x7d, 0x22, 0xc0, 0x00");
}

static void invalid_vpinsrw(void)
{
    asm volatile(".byte 0xc5, 0xfd, 0xc4, 0xc0, 0x00");
}

static void invalid_vinsertps(void)
{
    asm volatile(".byte 0xc4, 0xe3, 0x7d, 0x21, 0xc0, 0x00");
}

static void invalid_vphminposuw(void)
{
    asm volatile(".byte 0xc4, 0xe2, 0x7d, 0x41, 0xc0");
}

static void invalid_vaesimc(void)
{
    asm volatile(".byte 0xc4, 0xe2, 0x7d, 0xdb, 0xc0");
}

static void invalid_vaeskeygen(void)
{
    asm volatile(".byte 0xc4, 0xe3, 0x7d, 0xdf, 0xc0, 0x00");
}

static void invalid_vpermd(void)
{
    asm volatile(".byte 0xc4, 0xe2, 0x79, 0x36, 0xc0");
}

static void invalid_vperm2x128(void)
{
    asm volatile(".byte 0xc4, 0xe3, 0x79, 0x06, 0xc0, 0x00");
}

static void expect_sigill(void (*instruction)(void), const char *name)
{
    if (sigsetjmp(sigill_env, 1) == 0) {
        instruction();
        fprintf(stderr, "%s accepted VEX.L=1\n", name);
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    struct sigaction action = {
        .sa_handler = sigill_handler,
    };

    sigemptyset(&action.sa_mask);
    sigaction(SIGILL, &action, NULL);

    expect_sigill(invalid_vpinsrb, "VPINSRB");
    expect_sigill(invalid_vpinsrd, "VPINSRD");
    expect_sigill(invalid_vpinsrw, "VPINSRW");
    expect_sigill(invalid_vinsertps, "VINSERTPS");
    expect_sigill(invalid_vphminposuw, "VPHMINPOSUW");
    expect_sigill(invalid_vaesimc, "VAESIMC");
    expect_sigill(invalid_vaeskeygen, "VAESKEYGEN");
    expect_sigill(invalid_vpermd, "VPERMD");
    expect_sigill(invalid_vperm2x128, "VPERM2x128");
    return EXIT_SUCCESS;
}
