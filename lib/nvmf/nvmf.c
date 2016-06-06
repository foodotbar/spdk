/*-
 *   BSD LICENSE
 *
 *   Copyright (c) Intel Corporation.
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Intel Corporation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <arpa/inet.h>

#include "conn.h"
#include "controller.h"
#include "port.h"
#include "init_grp.h"
#include "nvmf_internal.h"
#include "nvmf.h"
#include "rdma.h"
#include "subsystem_grp.h"
#include "spdk/conf.h"
#include "spdk/log.h"
#include "spdk/pci.h"
#include "spdk/trace.h"

SPDK_LOG_REGISTER_TRACE_FLAG("nvmf", SPDK_TRACE_NVMF)

/*
 * The NVMf library maintains context for a list of subsystems. Each
 * subsystem will be associated with one or more NVMe controllers
 * that the library discovers.  It is expected that the NVMf library
 * consumer will make requests to create the desired subsystems.
 */
struct nvmf_driver {
	pthread_mutex_t mutex;
};

static struct nvmf_driver g_nvmf_driver;

extern struct rte_mempool *request_mempool;

int
nvmf_initialize(void)
{
	struct nvmf_driver		*system = &g_nvmf_driver;
	int err;

	if (request_mempool == NULL) {
		fprintf(stderr, "NVMf application has not created request mempool!\n");
		return -1;
	}

	err = pthread_mutex_init(&system->mutex, NULL);
	if (err < 0) {
		fprintf(stderr, "NVMf system pthread_mutex_init() failed\n");
		return -1;
	}

	return 0;
}

void
nvmf_shutdown(void)
{
	SPDK_TRACELOG(SPDK_TRACE_NVMF, "nvmf_shutdown\n");

	spdk_nvmf_shutdown_nvme();
}