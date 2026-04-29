// SPDX-License-Identifier: BSD-3-Clause
#include <qdf_types.h>
#include <qdf_mem.h>
#include <qdf_lock.h>
#include <qdf_atomic.h>
#include <cdp_txrx_ops.h>
#include "dp_ipa.h"
#include "dp_internal.h"
#include "dp_txrx.h"

#ifdef IPA_OFFLOAD

QDF_STATUS
dp_ipa_handle_rx_buf_smmu_mapping(struct dp_soc *soc,
				  struct dp_pdev *pdev,
				  dma_addr_t dma_addr,
				  uint8_t *vaddr,
				  uint32_t buf_size,
				  uint32_t num_buf)
{
	return QDF_STATUS_SUCCESS;
}

int
dp_ipa_uc_attach(struct dp_soc *soc, struct dp_pdev *pdev)
{
	return 0;
}

int
dp_ipa_uc_detach(struct dp_soc *soc, struct dp_pdev *pdev)
{
	return 0;
}

int
dp_ipa_ring_resource_setup(struct dp_soc *soc, struct dp_pdev *pdev)
{
	return 0;
}

QDF_STATUS
dp_ipa_get_resource(struct cdp_soc_t *soc_hdl, uint8_t pdev_id)
{
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS
dp_ipa_set_doorbell_paddr(struct cdp_soc_t *soc_hdl, uint8_t pdev_id)
{
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS
dp_ipa_op_response(struct cdp_soc_t *soc_hdl,
		   uint8_t pdev_id,
		   uint32_t resp)
{
	return QDF_STATUS_SUCCESS;
}

QDF_STATUS
dp_ipa_register_op_cb(struct cdp_soc_t *soc_hdl,
		      uint8_t pdev_id,
		      ipa_uc_op_cb_type op_cb,
		      void *usr_ctxt)
{
	return QDF_STATUS_SUCCESS;
}

void
dp_ipa_deregister_op_cb(struct cdp_soc_t *soc_hdl, uint8_t pdev_id)
{
}

bool
dp_reo_remap_config(struct dp_soc *soc,
		    uint32_t *remap0, uint32_t *remap1,
		    uint32_t *remap2, uint32_t *remap3)
{
	return false;
}

qdf_nbuf_t
dp_ipa_handle_rx_reo_reinject(struct dp_soc *soc, qdf_nbuf_t nbuf)
{
	return nbuf;
}

#endif
