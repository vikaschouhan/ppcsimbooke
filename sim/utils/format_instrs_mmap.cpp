// format_instrs.cpp
//
// This file contains a special parser utility to generate a C++ compliant
// function definition include file from RTL templates for powerPC instrs.
//
// To compile do "g++ -I/usr/include/boost format_instrs.cpp  -lboost_program_options -o format_instrs"
//
// Dependencies : boost c++ library
//
// Usage :
//     format_instrs --ifile inputfile --ofile outputfile ( --ofile is optional )
//
// Authors :
//     Vikas Chouhan ( presentisgood@gmail.com )  Copyright 2012.
//
// This file is part of ppc-sim library bundled with ppc_sim_booke
//
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// It is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this file; see the file COPYING.  If not, write to the
// Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#include <iostream>
#include <fstream>
#include <program_options.hpp>          // Requires boost
#include <algorithm/string.hpp>
#include <unistd.h>                    // for running shell commands
#include <fcntl.h>
#include <sys/mman.h>                  // for mmap
#include <cstring>                     // for strcmp
#include "third_party/ppcdis/ppcdis.h"

// some global variables
ppc_cpu_t  cpu_dialect = -1;

// Return new dialect according to specified cpu name
ppc_cpu_t ppc_parse_cpu(const char *arg)
{
    ppc_cpu_t ppc_cpu = 0;

    /* Sticky bits.  */
    ppc_cpu_t retain_flags = ppc_cpu & (PPC_OPCODE_ALTIVEC | PPC_OPCODE_VSX
                             | PPC_OPCODE_SPE | PPC_OPCODE_ANY);
    unsigned int i;

    for (i = 0; i < sizeof (ppc_opts) / sizeof (ppc_opts[0]); i++)
    {
        if (strcmp (ppc_opts[i].opt, arg) == 0)
        {
            if (ppc_opts[i].sticky)
            {
                retain_flags |= ppc_opts[i].sticky;
                if ((ppc_cpu & ~(ppc_cpu_t) (PPC_OPCODE_ALTIVEC | PPC_OPCODE_VSX
                       | PPC_OPCODE_SPE | PPC_OPCODE_ANY)) != 0)
                break;
            }
            ppc_cpu = ppc_opts[i].cpu;
            break;
        }
    }
    if (i >= sizeof (ppc_opts) / sizeof (ppc_opts[0]))
        return 0;

    ppc_cpu |= retain_flags;
    return ppc_cpu;
}

// Get a 64 bit unique value from the lookup table using the opcode's name
// this 64 bit value = ( 32 bit opcode value ) << 32 | opcode index number in lookup table
uint64_t get_opcode_hash(std::string opcname){

    for (int indx = 0; indx < powerpc_num_opcodes; indx++){
        if(!strcmp(opcname.c_str(), powerpc_opcodes[indx].name)){
            if (!(cpu_dialect != (ppc_cpu_t)(-1) &&
                       ((powerpc_opcodes[indx].flags & cpu_dialect) == 0 || (powerpc_opcodes[indx].deprecated & cpu_dialect) != 0)))
                return (powerpc_opcodes[indx].opcode << 32 | indx);
        }
    }
    // Throw a warning on undefined opcodes
    // NOTE : If an opcode was not found in the opcode table,
    //        this merely means that the extended opcode was not there.
    //        In that case, the disassembler module will automatically take care
    //        not to generate extended opcodes.
    std::cerr << "Warning !!! Undefined opcode " << opcname << std::endl;
    return 0x0;
}

int main(int argc, char** argv){

    std::string ifilename;
    std::string ofilename_tmp;
    std::string ofilename;
    std::string gcc_cmdline;

    namespace po = boost::program_options;
    po::options_description desc("Allowed options.");
    desc.add_options()
        ("help", "help")
        ("ifile", po::value<std::string>(), "Input file_name.")
        ("ofile", po::value<std::string>(), "Output file_name.")
        ;

    po::variables_map va_map;
    po::store(po::parse_command_line(argc, argv, desc), va_map);
    po::notify(va_map);

    if(va_map.count("help")){
        std::cout << desc << std::endl;
        return 1;
    }

    if(!va_map.count("ifile")){
        std::cout << "ifile is required." << std::endl;
        return 1;
    }else{
        ifilename = va_map["ifile"].as<std::string>();
    }

    if(!va_map.count("ofile")){
        ofilename = ifilename;
        // Check if input filename end with some extension
        if(ofilename.find_last_of('.') != std::string::npos){
            ofilename.replace(ofilename.find_last_of('.'), (ofilename.size() - ofilename.find_last_of('.')), ".h"); 
        }else{
            ofilename += ".h";
        }
    }else{
        ofilename = va_map["ofile"].as<std::string>();
    }

    // Get temporary file name
    ofilename_tmp = ifilename;
    // Remove directory names
    if(ofilename_tmp.find_last_of('/') != std::string::npos){
        ofilename_tmp = ofilename_tmp.substr(ofilename_tmp.find_last_of('/') + 1);
    }

    // Check if input filename end with some extension
    if(ofilename_tmp.find_last_of('.') != std::string::npos){
        ofilename_tmp.replace(ofilename_tmp.find_last_of('.'), (ofilename_tmp.size() - ofilename_tmp.find_last_of('.')), ".tcc"); 
    }else{
        ofilename_tmp += ".tcc";
    }

    // Generate a local tcc file after resolving all macros
    gcc_cmdline = "g++ -E -DUMODE=uint32_t -DSMODE=int32_t -DCPU=pcpu -DIC=ic " + ifilename + " &> " + ofilename_tmp;
    system(gcc_cmdline.c_str());

    // parse cpu dialect
    cpu_dialect = ppc_parse_cpu("e500x2");
    
    std::ifstream  istr;
    std::ofstream  ostr;
    size_t         len;
    char           *buff, *curr_ptr, *prev_ptr;
    size_t         curr_len = 0;
    size_t         paren_stack_len = 0;
    char           left_paren = '(';
    char           right_paren = ')';
    char           left_curly = '{';
    char           right_curly = '}';
    char           opcode_start = 'X';
    std::string    fmt_code = "        ";
    int            fd;

    std::string    opcode;
    std::string    opcode_org;
    std::string    opcode_n;
    std::string    opcode_fun;
    std::string    pseudocode;

    // Open files
    istr.open(ofilename_tmp.c_str());
    ostr.open(ofilename.c_str());

    // get file size
    istr.seekg(0, std::ios::end);
    len = istr.tellg();
    istr.seekg(0, std::ios::beg);
    istr.close();

    // MMAP the file contents directly into private address space
    // Since C++ doesn't have a native mmap, we are using POSIX API's mmap
    fd = open(ofilename_tmp.c_str(), O_RDONLY);
    if (fd == -1) {
        std::cerr << "Error opening file for writing" << std::endl;
        return -1;
    }

    buff = reinterpret_cast<char*>(mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0));
    if(buff == MAP_FAILED){
        std::cerr << "Not able to map " << ofilename_tmp.c_str() << std::endl;
        return -1;
    }

    // Start
    curr_ptr = buff;
    while(curr_ptr < (buff + len)){
        if(*curr_ptr == opcode_start){
            while(*curr_ptr != left_paren)
                curr_ptr++;
            // Skip '('
            curr_ptr++;

            prev_ptr = curr_ptr;
            curr_len = 0;
            while(*curr_ptr != right_paren){
                curr_ptr++;
                curr_len++;
            }

            // Initialize opcode [ X(opcode) ]
            opcode.assign(prev_ptr, curr_len);

            // Skip to pseudocode start
            while(*curr_ptr != left_curly)
                curr_ptr++;

            // We hit a left curly brace. Initialize stack len to 1
            paren_stack_len = 1;
            prev_ptr        = curr_ptr;
            curr_ptr++;
            curr_len        = 0;

            // Pick up the code ( between { } )
            while(paren_stack_len){
                if(*curr_ptr == left_curly)
                    paren_stack_len++;
                else if(*curr_ptr == right_curly)
                    paren_stack_len--;
                curr_ptr++;
                curr_len++;
            }

            // Initialize pseudocode.
            pseudocode.assign(prev_ptr, curr_len+1);

            // Add this number of spaces at begenning of each line
            boost::algorithm::replace_all(pseudocode, "\n", "\n" + fmt_code + "    ");
            boost::algorithm::replace_all(pseudocode, "{", "\n" + fmt_code + "{");
            boost::algorithm::replace_all(pseudocode, "}",  "\n" + fmt_code + "}");

            // Replace dot by something more fancy
            opcode_n = opcode;
            opcode_org = opcode;
            boost::algorithm::replace_all(opcode_n, ".", "_dot");
            opcode = opcode_n;
            opcode_fun = opcode_n + "___";
            opcode_n = "___" + opcode_n + "___";

            // Do whatever you want
            // Write the function definition to the output file
            ostr << std::endl;
            ostr << "    // " << opcode << std::endl;
            ostr << "    struct " << opcode_n << " {" << std::endl;
            ostr << "        static void " << opcode_fun << "(CPU_PPC_T *pcpu, instr_call *ic)" << std::endl;
            ostr << pseudocode << std::endl;
            ostr << "    };" << std::endl;
            ostr << "    pcpu->m_ppc_func_hash[" << std::hex << std::showbase
                 << get_opcode_hash(opcode_org) << "] = " << opcode_n << "::" << opcode_fun << ";" << std::endl;
            ostr << std::endl;
        }else{
            // Add comments and blank lines
            ostr << *curr_ptr;
            curr_ptr++;
        }
    }

    // Close output file handle and unmap source file
    ostr.close();
    munmap(buff, len);

    return 0;
}