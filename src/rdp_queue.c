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
	csp_packet_t *pEnqueuePacket;
	csp_packet_t *pDequeuePacket;

	/* create queue */
	csp_rdp_queue_init();
	zassert_equal(0, csp_rdp_queue_tx_size());

	/* create packet */
	csp_buffer_init();
	pEnqueuePacket = csp_buffer_get(0);
	zassert_not_null(pEnqueuePacket);

	/* enqueue */
	csp_rdp_queue_tx_add(NULL, pEnqueuePacket);
	int size = csp_rdp_queue_tx_size();
	zassert_equal(1, size, "queue size is not 1. queue size is %d.", size);

	/* dequeue */
	pDequeuePacket = csp_rdp_queue_tx_get(NULL);
	zassert_not_null(pDequeuePacket);
	size = csp_rdp_queue_tx_size();
	zassert_equal(0, size, "queue size is not 0. queue size is %d.", size);
	zassert_equal_ptr(pEnqueuePacket, pDequeuePacket);
}

/* test when queue size is MAX */
ZTEST(rdp_queue, test_rdp_queue_MAX)
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
	zassert_equal(QUEUE_SIZE_MAX, size, "queue size is not MAX. queue size is %d.", size);

	/* can not add when queue size is MAX */
	packet = csp_buffer_get(0);
	zassert_not_null(packet);
	csp_rdp_queue_tx_add(NULL, packet);

	size = csp_rdp_queue_tx_size();
	zassert_equal(QUEUE_SIZE_MAX, size, "queue size is not MAX. queue size is %d.", size);
}

/* queue up to Max, then dequeue all */
ZTEST(rdp_queue, test_rdp_enqueue_MAX_and_dequeue)
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
	zassert_equal(QUEUE_SIZE_MAX, size, "queue size is not MAX. queue size is %d.", size);

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

ZTEST_SUITE(rdp_queue, NULL, NULL, NULL, NULL, NULL);
