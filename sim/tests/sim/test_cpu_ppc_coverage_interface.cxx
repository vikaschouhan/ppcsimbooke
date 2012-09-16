/*
 * Test to check if cpu_ppc's interface is working fine.
 */

#include "cpu_ppc_coverage.hpp"

int main(){
    CPU_PPC_COVERAGE cov_log0;
    cov_log0.enable();
    cov_log0.log_to_file("test_cpu_ppc_coverage_interface.log");
    cov_log0.probe("addi");
    cov_log0.generate_log();
    return 0;
}
