/*
 * SPDX-License-Identifier: LicenseRef-CSSL-1.0
 */
#include "benchmark/benchmark.h"
#include <vector>
#include <cstdint>
#include <cstring>

extern "C" {
#include "common/platform_types.h"
#include "openair2/LAYER2/nr_pdcp/nr_pdcp_entity.h"
#include "common/utils/LOG/log.h"
#include "common/config/config_load_configmodule.h"
extern configmodule_interface_t *uniqCfg;
}

static void deliver_sdu_dummy(void *deliver_sdu_data,
                              struct nr_pdcp_entity_s *entity,
                              char *buf,
                              int size,
                              const nr_pdcp_integrity_data_t *msg_integrity)
{
  (void)deliver_sdu_data;
  (void)entity;
  (void)buf;
  (void)size;
  (void)msg_integrity;
}

static void deliver_pdu_dummy(void *deliver_pdu_data,
                              ue_id_t ue_id,
                              int rb_id,
                              char *buf,
                              int size,
                              int sdu_id)
{
  (void)deliver_pdu_data;
  (void)ue_id;
  (void)rb_id;
  (void)buf;
  (void)size;
  (void)sdu_id;
}

static void run_pdcp_tx_benchmark(benchmark::State &state,
                                  int cipher_algo,
                                  int integrity_algo)
{
  const int sdu_size = state.range(0);

  nr_pdcp_entity_security_keys_and_algos_t sec = {0};
  sec.ciphering_algorithm = cipher_algo;
  sec.integrity_algorithm = integrity_algo;
  for (int i = 0; i < 16; i++) {
    sec.ciphering_key[i] = (uint8_t)(i + 1);
    sec.integrity_key[i] = (uint8_t)(i + 0x11);
  }

  nr_pdcp_entity_t *entity = new_nr_pdcp_entity(
      NR_PDCP_DRB_AM,
      1,     // is_gnb = 1 (Downlink TX)
      1,     // rb_id = 1
      1,     // pdusession_id = 1
      false, // has_sdap_rx
      false, // has_sdap_tx
      deliver_sdu_dummy,
      nullptr,
      deliver_pdu_dummy,
      nullptr,
      LONG_SN_SIZE, // 18-bit SN
      -1,           // t_reordering = -1 (disabled)
      -1,           // discard_timer = -1 (disabled)
      &sec);

  std::vector<char> sdu_buffer(sdu_size, 0xAB);
  char pdu_buffer[2048];
  int sdu_id = 0;

  for (auto _ : state) {
    int pdu_size = entity->process_sdu(entity,
                                       sdu_buffer.data(),
                                       sdu_size,
                                       sdu_id++,
                                       pdu_buffer,
                                       sizeof(pdu_buffer));
    benchmark::DoNotOptimize(pdu_size);
    benchmark::DoNotOptimize(pdu_buffer);
  }

  state.SetBytesProcessed(int64_t(state.iterations()) * sdu_size);
  state.SetItemsProcessed(int64_t(state.iterations()));

  entity->delete_entity(entity);
}

static void BM_nr_pdcp_tx_null(benchmark::State &state)
{
  run_pdcp_tx_benchmark(state, 0, 0); // NEA0 + NIA0
}

static void BM_nr_pdcp_tx_aes(benchmark::State &state)
{
  run_pdcp_tx_benchmark(state, 2, 2); // NEA2 + NIA2
}

static void BM_nr_pdcp_tx_snow3g(benchmark::State &state)
{
  run_pdcp_tx_benchmark(state, 1, 1); // NEA1 + NIA1
}

BENCHMARK(BM_nr_pdcp_tx_null)->Arg(64)->Arg(256)->Arg(1024)->Arg(1500);
BENCHMARK(BM_nr_pdcp_tx_aes)->Arg(64)->Arg(256)->Arg(1024)->Arg(1500);
BENCHMARK(BM_nr_pdcp_tx_snow3g)->Arg(64)->Arg(256)->Arg(1024)->Arg(1500);

int main(int argc, char **argv)
{
  logInit();
  uniqCfg = load_configmodule(argc, argv, CONFIG_ENABLECMDLINEONLY);
  if (g_log != nullptr) {
    g_log->log_component[PDCP].level = -1;
  }
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  return 0;
}
