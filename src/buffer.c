/*
 * Copyright (c) 2024 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <csp/csp.h>

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
		packets[i] = csp_buffer_get(0);
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
	csp_packet_t *p;
	int i;

	memset(packets, 0, sizeof(packets));

	for (i = 0; i < CSP_BUFFER_COUNT; i++) {
		packets[i] = csp_buffer_get(0);
		zassert_true(packets[i] != NULL, NULL);
	}

	zassert_true(csp_buffer_remaining() == 0);
	p = csp_buffer_get(0);
	zassert_true(p == NULL, NULL);

	for (i = 0; i < CSP_BUFFER_COUNT; i++) {
		csp_buffer_free(packets[i]);
	}
}

ZTEST_SUITE(buffer, NULL, setup, NULL, NULL, NULL);
