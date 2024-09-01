/*
 * Copyright (c) 2024 Space Cubics, LLC.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/ztest.h>
#include <csp/csp.h>

#define QUEUE_SIZE_MAX (CSP_RDP_MAX_WINDOW * 2)

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

/* test when queue size is 1 */
ZTEST(rdp_queue, test_rdp_queue_one)
{
	csp_packet_t *packet;

	/* create queue */
	csp_rdp_queue_init();
	zassert_equal(0, csp_rdp_queue_tx_size());

	/* create packet */
	csp_buffer_init();
	packet = csp_buffer_get(0);
	zassert_not_null(packet);

	/* add packets to queue */
	csp_rdp_queue_tx_add(NULL, packet);
	int size = csp_rdp_queue_tx_size();
	zassert_equal(1, size, "queue size is not 1. queue size is %d.", size);
}

/* enqueue and dequeue test  */
ZTEST(rdp_queue, test_rdp_queue_enqueue_dequeue)
{
	csp_packet_t *enqueue_packet;
	csp_packet_t *dequeue_packet;

	/* create queue */
	csp_rdp_queue_init();
	zassert_equal(0, csp_rdp_queue_tx_size());

	/* create packet */
	csp_buffer_init();
	enqueue_packet = csp_buffer_get(0);
	zassert_not_null(enqueue_packet);

	/* enqueue */
	csp_rdp_queue_tx_add(NULL, enqueue_packet);
	int size = csp_rdp_queue_tx_size();
	zassert_equal(1, size, "queue size is not 1. queue size is %d.", size);

	/* dequeue */
	dequeue_packet = csp_rdp_queue_tx_get(NULL);
	zassert_not_null(dequeue_packet);
	size = csp_rdp_queue_tx_size();
	zassert_equal(0, size, "queue size is not 0. queue size is %d.", size);
	zassert_equal_ptr(enqueue_packet, dequeue_packet);
}

/* test when queue size is max */
ZTEST(rdp_queue, test_rdp_queue_max)
{
	csp_packet_t *packet;

	/* create queue */
	csp_rdp_queue_init();
	zassert_equal(0, csp_rdp_queue_tx_size());

	/* enqueue until queue is filled */
	csp_buffer_init();
	for (int i = 0; i < QUEUE_SIZE_MAX; i++) {
		/* create packet */
		packet = csp_buffer_get(0);
		zassert_not_null(packet);

		/* enqueue */
		csp_rdp_queue_tx_add(NULL, packet);
		zassert_equal((i+1), csp_rdp_queue_tx_size());
	}

	int size = csp_rdp_queue_tx_size();
	zassert_equal(QUEUE_SIZE_MAX, size, "queue size is not max. queue size is %d.", size);

	/* can not add when queue size is max */
	packet = csp_buffer_get(0);
	zassert_not_null(packet);
	csp_rdp_queue_tx_add(NULL, packet);

	size = csp_rdp_queue_tx_size();
	zassert_equal(QUEUE_SIZE_MAX, size, "queue size is not max. queue size is %d.", size);
}

/* queue up to max, then dequeue all */
ZTEST(rdp_queue, test_rdp_enqueue_max_and_dequeue)
{
	csp_packet_t *packet;

	/* create queue */
	csp_rdp_queue_init();
	zassert_equal(0, csp_rdp_queue_tx_size());

	/* enqueue until queue is filled */
	csp_buffer_init();
	for (int i = 0; i < QUEUE_SIZE_MAX; i++) {
		/* create packet */
		packet = csp_buffer_get(0);
		zassert_not_null(packet);

		/* enqueue */
		csp_rdp_queue_tx_add(NULL, packet);
		zassert_equal((i+1), csp_rdp_queue_tx_size());
	}

	int size = csp_rdp_queue_tx_size();
	zassert_equal(QUEUE_SIZE_MAX, size, "queue size is not max. queue size is %d.", size);

	/* dequeue all */
	for (int i = QUEUE_SIZE_MAX; i > 0; i--) {
		/* dequeue packet */
		packet = csp_rdp_queue_tx_get(NULL);
		zassert_not_null(packet);
		zassert_equal((i-1), csp_rdp_queue_tx_size());
	}

	size = csp_rdp_queue_tx_size();
	zassert_equal(0, size, "queue size is not 0. queue size is %d.", size);

	/* can not dequeue when queue size is 0 */
	packet = csp_rdp_queue_tx_get(NULL);
	zassert_is_null(packet);
	size = csp_rdp_queue_tx_size();
	zassert_equal(0, size, "queue size is not 0. queue size is %d.", size);
}

/* queue flush test */
ZTEST(rdp_queue, test_rdp_queue_flush)
{
	csp_conn_t *conn;
	csp_packet_t *packet_tx;
	csp_packet_t *packet_rx;

	/* create connection */
	csp_conn_init();
	conn = csp_conn_allocate(CONN_CLIENT);

	/* create queue */
	csp_rdp_queue_init();
	zassert_equal(0, csp_rdp_queue_tx_size());
	zassert_equal(0, csp_rdp_queue_rx_size());

	/* create packet */
	csp_buffer_init();
	packet_tx = csp_buffer_get(0);
	zassert_not_null(packet_tx);
	packet_rx = csp_buffer_get(0);
	zassert_not_null(packet_rx);

	/* enqueue */
	csp_rdp_queue_tx_add(NULL, packet_tx);
	int size = csp_rdp_queue_tx_size();
	zassert_equal(1, size, "tx_queue size is not 1. tx_queue size is %d.", size);
	csp_rdp_queue_rx_add(NULL, packet_rx);
	size = csp_rdp_queue_rx_size();
	zassert_equal(1, size, "rx_queue size is not 1. rx_queue size is %d.", size);

	/* queue flush */
	csp_rdp_queue_flush(conn);
	size = csp_rdp_queue_tx_size();
	zassert_equal(0, size, "tx_queue size is not 0. tx_queue size is %d.", size);
	size = csp_rdp_queue_rx_size();
	zassert_equal(0, size, "rx_queue size is not 0. rx_queue size is %d.", size);
}

/* uninitialized queue test */
ZTEST(rdp_queue, test_queue_no_init)
{
	csp_packet_t *packet;

	/* create packet */
	csp_buffer_init();
	packet = csp_buffer_get(0);
	zassert_not_null(packet);

	/* enqueue */
	csp_rdp_queue_tx_add(NULL, packet);
	int size = csp_rdp_queue_tx_size();
	zassert_equal(1, size, "queue size is not 1. queue size is %d.", size);
}

ZTEST_SUITE(rdp_queue, NULL, NULL, NULL, NULL, NULL);
