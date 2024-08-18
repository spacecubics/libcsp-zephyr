/*
 * Copyright (c) 2024 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <csp/csp.h>

void csp_rdp_queue_init(void);
int csp_rdp_queue_tx_size(void);
int csp_rdp_queue_rx_size(void);
void csp_rdp_queue_tx_add(csp_conn_t * conn, csp_packet_t * packet);
void csp_rdp_queue_rx_add(csp_conn_t * conn, csp_packet_t * packet);
csp_packet_t * csp_rdp_queue_tx_get(csp_conn_t * conn);
csp_packet_t * csp_rdp_queue_rx_get(csp_conn_t * conn);

ZTEST(rdp_queue, test_rdp_queue_emtpy)
{
	csp_rdp_queue_init();
	zassert_equal(0, csp_rdp_queue_tx_size());
	zassert_equal(0, csp_rdp_queue_rx_size());
	zassert_is_null(csp_rdp_queue_tx_get(NULL));
	zassert_is_null(csp_rdp_queue_rx_get(NULL));
}

ZTEST(rdp_queue, test_rdp_queue_add_and_get)
{
	csp_packet_t *p1;
	csp_packet_t *p2;

	csp_rdp_queue_init();
	zassert_equal(0, csp_rdp_queue_tx_size());
	zassert_equal(0, csp_rdp_queue_rx_size());

	csp_buffer_init();
	p1 = csp_buffer_get(0);
	zassert_not_null(p1);
	csp_rdp_queue_tx_add(NULL, p1);
	zassert_equal(1, csp_rdp_queue_tx_size());

	p2 = csp_rdp_queue_tx_get(NULL);
	zassert_equal(0, csp_rdp_queue_tx_size());
	zassert_not_null(p2);
	printf("%p %p\n", p1, p2);
	zassert_equal_ptr(p1, p2);
}

ZTEST_SUITE(rdp_queue, NULL, NULL, NULL, NULL, NULL);
