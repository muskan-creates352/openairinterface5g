# SPDX-License-Identifier: LicenseRef-CSSL-1.0

add_library(nr_pdcp_snow3g OBJECT
            nr_pdcp_integrity_nia1.c
            nr_pdcp_security_nea1.c)

add_library(nr_pdcp_core OBJECT
            nr_pdcp_entity.c
            nr_pdcp_sdu.c
            nr_pdcp_integrity_nia1.c
            nr_pdcp_integrity_nia2.c
            nr_pdcp_security_nea1.c
            nr_pdcp_security_nea2.c)
target_include_directories(nr_pdcp_core PRIVATE ../../..)
target_link_libraries(nr_pdcp_core PRIVATE log_headers)

if (ENABLE_TESTS)
  add_subdirectory(tests)
endif ()
