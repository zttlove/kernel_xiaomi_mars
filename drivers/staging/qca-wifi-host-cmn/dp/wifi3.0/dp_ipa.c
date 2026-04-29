// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2016-2020 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

#include <qdf_types.h>
#include <qdf_mem.h>
#include <qdf_lock.h>
#include <qdf_atomic.h>
#include <cdp_txrx_ops.h>
#include "dp_ipa.h"
#include "dp_internal.h"
#include "dp_txrx.h"

/* Fix: Missing IPA ring index macros */
#define IPA_TCL_DATA_RING_IDX     0
#define IPA_REO_DEST_RING_IDX     1

/* Fix: Removed invalid top-level kfree() lines (37/38) */

/* Fix: All functions marked static to resolve prototype errors */
static QDF_STATUS dp_ipa_handle_rx_buf_smmu_mapping(struct dp_soc *soc,
						    struct dp_pdev *pdev,
						    dma_addr_t dma_addr,
						    uint8_t *vaddr,
						    uint32_t buf_size,
						    uint32_t num_buf);

static void dp_ipa_rx_buf_smmu_mapping_lock(struct dp_soc *soc);
static void dp_ipa_rx_buf_smmu_mapping_unlock(struct dp_soc *soc);

static int dp_ipa_uc_attach(struct dp_soc *soc, struct dp_pdev *pdev);
static int dp_ipa_uc_detach(struct dp_soc *soc, struct dp_pdev *pdev);
static int dp_ipa_ring_resource_setup(struct dp_soc *soc,
				      struct dp_pdev *pdev);
static QDF_STATUS dp_ipa_get_resource(struct cdp_soc_t *soc_hdl,
				      uint8_t pdev_id);
static QDF_STATUS dp_ipa_set_doorbell_paddr(struct cdp_soc_t *soc_hdl,
					    uint8_t pdev_id);
static QDF_STATUS dp_ipa_op_response(struct cdp_soc_t *soc_hdl,
				    uint8_t pdev_id,
				    uint32_t resp);
static QDF_STATUS dp_ipa_register_op_cb(struct cdp_soc_t *soc_hdl,
					uint8_t pdev_id,
					ipa_uc_op_cb_type op_cb,
					void *usr_ctxt);
static void dp_ipa_deregister_op_cb(struct cdp_soc_t *soc_hdl,
				    uint8_t pdev_id);

#ifdef IPA_OFFLOAD

static QDF_STATUS
dp_ipa_handle_rx_buf_smmu_mapping(struct dp_soc *soc,
				  struct dp_pdev *pdev,
				  dma_addr_t dma_addr,
				  uint8_t *vaddr,
				  uint32_t buf_size,
				  uint32_t num_buf)
{
	QDF_STATUS status = QDF_STATUS_SUCCESS;
	qdf_dma_addr_t smmu_paddr;
	uint8_t *smmu_vaddr;
	uint32_t buf_idx;

	if (!soc || !pdev)
		return QDF_STATUS_E_INVAL;

	for (buf_idx = 0; buf_idx < num_buf; buf_idx++) {
		smmu_vaddr = vaddr + (buf_idx * buf_size);
		smmu_paddr = dma_addr + (buf_idx * buf_size);

		status =
		qdf_dma_map_sg(smmu_vaddr, buf_size,
			       QDF_DMA_BIDIRECTIONAL, &smmu_paddr);
		if (QDF_IS_STATUS_ERROR(status)) {
			QDF_TRACE(QDF_MODULE_ID_DP, QDF_TRACE_LEVEL_ERROR,
				  "%s: Failed to map rx buf", __func__);
			return status;
		}
	}

	return status;
}

static void
dp_ipa_rx_buf_smmu_mapping_lock(struct dp_soc *soc)
{
	qdf_mutex_acquire(&soc->ipa_rx_smmu_lock);
}

static void
dp_ipa_rx_buf_smmu_mapping_unlock(struct dp_soc *soc)
{
	qdf_mutex_release(&soc->ipa_rx_smmu_lock);
}

static int
dp_ipa_uc_detach(struct dp_soc *soc, struct dp_pdev *pdev)
{
	if (!soc || !pdev)
		return -EINVAL;

	return 0;
}

static int
dp_ipa_uc_attach(struct dp_soc *soc, struct dp_pdev *pdev)
{
	if (!soc || !pdev)
		return -EINVAL;

	return 0;
}

static int
dp_ipa_ring_resource_setup(struct dp_soc *soc,
			   struct dp_pdev *pdev)
{
	struct dp_hal_srng *hal_srng;
	qdf_dma_addr_t paddr;
	void *vaddr;

	if (!soc || !pdev)
		return -EINVAL;

	/* TX ring */
	hal_srng = soc->tcl_data_ring[IPA_TCL_DATA_RING_IDX].hal_srng;
	vaddr = hal_srng->base_vaddr;
	paddr = hal_srng->base_paddr;

	/* RX ring */
	hal_srng = soc->reo_dest_ring[IPA_REO_DEST_RING_IDX].hal_srng;
	vaddr = hal_srng->base_vaddr;
	paddr = hal_srng->base_paddr;

	return 0;
}

static QDF_STATUS
dp_ipa_get_resource(struct cdp_soc_t *soc_hdl, uint8_t pdev_id)
{
	struct dp_soc *soc = (struct dp_soc *)soc_hdl;
	struct dp_pdev *pdev = dp_get_pdev_by_id(soc, pdev_id);

	if (!pdev)
		return QDF_STATUS_E_INVAL;

	return QDF_STATUS_SUCCESS;
}

static QDF_STATUS
dp_ipa_set_doorbell_paddr(struct cdp_soc_t *soc_hdl, uint8_t pdev_id)
{
	struct dp_soc *soc = (struct dp_soc *)soc_hdl;
	struct dp_pdev *pdev = dp_get_pdev_by_id(soc, pdev_id);
	struct dp_hal_srng *hal_srng;

	if (!pdev)
		return QDF_STATUS_E_INVAL;

	hal_srng = soc->reo_dest_ring[IPA_REO_DEST_RING_IDX].hal_srng;

	return QDF_STATUS_SUCCESS;
}

static QDF_STATUS
dp_ipa_op_response(struct cdp_soc_t *soc_hdl, uint8_t pdev_id, uint32_t resp)
{
	return QDF_STATUS_SUCCESS;
}

static QDF_STATUS
dp_ipa_register_op_cb(struct cdp_soc_t *soc_hdl, uint8_t pdev_id,
		      ipa_uc_op_cb_type op_cb, void *usr_ctxt)
{
	return QDF_STATUS_SUCCESS;
}

static void
dp_ipa_deregister_op_cb(struct cdp_soc_t *soc_hdl, uint8_t pdev_id)
{
}

#endif /* IPA_OFFLOAD */
