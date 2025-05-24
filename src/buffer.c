/*
 * Copyright (c) 2024 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <setjmp.h>
#include <zephyr/ztest.h>
#include <csp/csp.h>

static jmp_buf panic_jmpbuf;

void csp_panic(const char *msg) {
	printk("Panic called: %s\n", msg);
	longjmp(panic_jmpbuf, 1);
}

static void *setup(void)
{
	csp_init();
	return NULL;
}

ZTEST(buffer, test_buffer_count)
{
	csp_packet_t *packets[CSP_BUFFER_COUNT];
	int i;

	memset(packets, 0, sizeof(packets));

	zassert_true(csp_buffer_remaining() == CSP_BUFFER_COUNT);

	for (i = 0; i < CSP_BUFFER_COUNT; i++) {
		packets[i] = csp_buffer_get_always();
		zassert_true(packets[i] != NULL, NULL);
	}

	zassert_true(csp_buffer_remaining() == 0);

	for (i = 0; i < CSP_BUFFER_COUNT; i++) {
		csp_buffer_free(packets[i]);
	}

	zassert_true(csp_buffer_remaining() == CSP_BUFFER_COUNT);
}

ZTEST(buffer, test_buffer_over_allocate)
{
	csp_packet_t *packets[CSP_BUFFER_COUNT];
	int i;

	memset(packets, 0, sizeof(packets));

	for (i = 0; i < CSP_BUFFER_COUNT; i++) {
		packets[i] = csp_buffer_get_always();
		zassert_true(packets[i] != NULL, NULL);
	}

	zassert_true(csp_buffer_remaining() == 0);
	/*  Define the jump here, first time the jump is 0 but if csp_panic the jump is egal to 1*/
	if (setjmp(panic_jmpbuf) == 0) {
		/* Here don't wait for a value it should not return a value but csp_panic() */
		csp_buffer_get_always();
		zassert_true(false, "csp_buffer_get_always() should have panicked");
	} else {
		/* Here we return via longjmp from csp_panic() */
		zassert_true(true, "csp_panic was called as expected");
	}

	for (i = 0; i < CSP_BUFFER_COUNT; i++) {
		csp_buffer_free(packets[i]);
	}
}

ZTEST_SUITE(buffer, NULL, setup, NULL, NULL, NULL);
