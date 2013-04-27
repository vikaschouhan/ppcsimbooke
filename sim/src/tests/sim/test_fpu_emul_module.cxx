// Some basic tests for FP default values in case of invalid operand error

#define PPCSIMBOOKE_FPU_EMUL_DEBUG     // enable data structs for fpu debug
#include "fpu_emul.h"

using namespace ppcsimbooke::fp_emul;

union fp_bt_u {
    float    f32;
    uint32_t u32;
};

int main(){

    // floating point cum bitstream union
    fp_bt_u a,b, c;

    a.f32 = -2.0/0.0;   // a = -inf
    b.f32 = 1.5435;     // b = norm

    x86_mxcsr m;
    m.ie = 1;           // emulating invalid operand exception without actually generating any exception

    std::cout << std::showbase << std::hex << "a=" << a.u32 << " b=" << b.u32 << std::endl;
    std::cout << "type_of_a=" << fp_operand_str[get_operand_type<uint32_t>(a.u32)] << " type_of_b=" <<
        fp_operand_str[get_operand_type<uint32_t>(b.u32)] << std::endl;

    c.u32 = get_default_results<uint32_t>(fp_op_add, a.u32, b.u32, m);
    std::cout << "c = " << c.f32 << std::endl;

    if(c.u32 == fp_traits<uint32_t>::minus_max){ std::cout << "Pass-0" << std::endl; }
    else                                       { std::cout << "Fail-0" << std::endl; }

    
    a.f32 = 8.0/0.0;    // a = inf
    b.f32 = -2.0/0.0;   // b = -inf

    std::cout << std::showbase << std::hex << "a=" << a.u32 << " b=" << b.u32 << std::endl;
    std::cout << "type_of_a=" << fp_operand_str[get_operand_type<uint32_t>(a.u32)] << " type_of_b=" <<
        fp_operand_str[get_operand_type<uint32_t>(b.u32)] << std::endl;

    c.u32 = get_default_results<uint32_t>(fp_op_add, a.u32, b.u32, m);
    std::cout << "c = " << c.f32 << std::endl;

    if(c.u32 == fp_traits<uint32_t>::plus_max) { std::cout << "Pass-1" << std::endl; }
    else                                       { std::cout << "Fail-1" << std::endl; }
 
    return 0;
}
