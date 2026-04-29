#ifndef __DP_IPA_H
#define __DP_IPA_H

#include <qdf_types.h>
#include <qdf_nbuf.h>
#include <qdf_lock.h>
#include "dp_types.h"

struct dp_soc;
struct dp_pdev;

typedef QDF_STATUS (*ipa_uc_op_cb_type)(void *usr_ctxt, uint32_t resp);

#ifdef IPA_OFFLOAD

int dp_ipa_uc_detach(struct dp_soc *soc, struct dp_pdev *pdev);
int dp_ipa_uc_attach(struct dp_soc *soc, struct dp_pdev *pdev);
int dp_ipa_ring_resource_setup(struct dp_soc *soc, struct dp_pdev *pdev);

QDF_STATUS dp_ipa_handle_rx_buf_smmu_mapping(struct dp_soc *soc,
					      struct dp_pdev *pdev,
					      dma_addr_t dma_addr,
					      uint8_t *vaddr,
					      uint32_t buf_size,
					      uint32_t num_buf);

bool dp_reo_remap_config(struct dp_soc *soc,
			 uint32_t *remap0, uint32_t *remap1,
			 uint32_t *remap2, uint32_t *remap3);

qdf_nbuf_t dp_ipa_handle_rx_reo_reinject(struct dp_soc *soc, qdf_nbuf_t nbuf);

static inline void
dp_ipa_rx_buf_smmu_mapping_lock(struct dp_soc *soc)
{
}

static inline void
dp_ipa_rx_buf_smmu_mapping_unlock(struct dp_soc *soc)
{
}

static inline void
dp_ipa_reo_ctx_buf_mapping_lock(struct dp_soc *soc, uint8_t reo_ring_num)
{
}

static inline void
dp_ipa_reo_ctx_buf_mapping_unlock(struct dp_soc *soc, uint8_t reo_ring_num)
{
}

#else

static inline int
dp_ipa_uc_attach(struct dp_soc *soc, struct dp_pdev *pdev)
{ return 0; }

static inline int
dp_ipa_uc_detach(struct dp_soc *soc, struct dp_pdev *pdev)
{ return 0; }

static inline QDF_STATUS
dp_ipa_handle_rx_buf_smmu_mapping(struct dp_soc *soc,
				  struct dp_pdev *pdev,
				  dma_addr_t dma_addr,
				  uint8_t *vaddr,
				  uint32_t buf_size,
				  uint32_t num_buf)
{ return QDF_STATUS_SUCCESS; }

static inline void
dp_ipa_rx_buf_smmu_mapping_lock(struct dp_soc *soc)
{}

static inline void
dp_ipa_rx_buf_smmu_mapping_unlock(struct dp_soc *soc)
{}

static inline void
dp_ipa_reo_ctx_buf_mapping_lock(struct dp_soc *soc, uint8_t reo_ring_num)
{}

static inline void
dp_ipa_reo_ctx_buf_mapping_unlock(struct dp_soc *soc, uint8_t reo_ring_num)
{}

#endif
#endif
