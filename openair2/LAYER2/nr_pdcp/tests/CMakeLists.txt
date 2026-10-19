# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_executable(snow3g_tests snow3g_tests.c)
target_link_libraries(snow3g_tests PRIVATE nr_pdcp_snow3g minimal_lib SECURITY)
add_test(NAME snow3g_tests
         COMMAND snow3g_tests)
add_dependencies(tests snow3g_tests)

add_executable(benchmark_nr_pdcp_entity benchmark_nr_pdcp_entity.cpp)
target_link_libraries(benchmark_nr_pdcp_entity PRIVATE benchmark::benchmark nr_pdcp_core SECURITY UTIL ${T_LIB} minimal_lib)
add_dependencies(tests benchmark_nr_pdcp_entity)
add_test(NAME benchmark_nr_pdcp_entity
         COMMAND ./benchmark_nr_pdcp_entity)
