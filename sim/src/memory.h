// memory.h (memory facilities for cpu module)
// This file contains memory class and corresponding member functions.
// 
// Author : Vikas Chouhan (presentisgood@gmail.com)
// Copyright 2012.
// 
// This file is part of ppc-sim library bundled with test_gen_ppc.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 as
// published by the Free Software Foundation.
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


#ifndef    _MEMORY_HPP_
#define    _MEMORY_HPP_

#include <elfio/elfio.hpp>        // for elf loader
#include "config.h"
#include "utils.h"

/* Memory target types */
static const int TGT_DDR  = 1;      // DDR
static const int TGT_CCSR = 2;      // CCSR
static const int TGT_IFC  = 3;      // Integrated Flash Controller

/*
 * Main memory class
 *
 * NOTE:  1. For time being, no two memory devices should have overlapping PAs
 *        2. Only one object can be crated for this class
 *        3. It's cumpulsory that all base address should be page (4K) aligned.
 */
#define  MEM_T
#define  MEM_PPC           memory
#define  MEM_PPC_T         MEM_PPC

class MEM_PPC {

    #define    MEM_READ                    0
    #define    MEM_WRITE                   1

    #define    MEMORY_ACCESS_FAILED        0
    #define    MEMORY_ACCESS_OK            1
    #define    MEMORY_ACCESS_OK_WRITE      2
    #define    MEMORY_NOT_FULL_PAGE        256

    #define    PAGE_SIZE                   0x1000      // All physical regions are devided into pages of 4K each.

    /* Map to convert memory target type to string */
    struct t_mem_tgt_type_str : std::map<int, std::string> {
        t_mem_tgt_type_str(){
            (*this)[TGT_DDR] = "ddr";
            (*this)[TGT_CCSR] = "ccsr";
            (*this)[TGT_IFC] = "ifc";
        }
        ~t_mem_tgt_type_str(){}
    } mem_tgt_str;

    // Memory mapped device
    struct t_mem_tgt {
        uint64_t       baseaddr;      /* base address of this device */
        uint64_t       endaddr;       // End address
        uint64_t       size;          /* size should be a multiple of 4K only */
        uint32_t       flags;         /* Flags for this device */
        int            priority;      /* priority of this region ( only used in case of overlapping regions ) */
        std::string         name;          /* name */
        int                 tgt_type;      /* target type */

        // Page hash information
        bool                                           can_free_pages;    /* Flag to check if pages for this mem tgt can be freed */
        std::map<uint64_t, uint8_t*>                   page_hash;     /* Hash of 4K pages hashed by page number */
        

    };

    // Memory single element attributes
    uint64_t            pa_max;             /* Maximum physical address */
    uint64_t            pn_max;             /* Page number max ( pa_max / PAGE_SIZE ) */
    int                 m_bits;             /* Number of physical address lines */

    // Memory target attributes
    int                                     n_tgts;             /* Number of targets */
    std::list<t_mem_tgt>                    mem_tgt;            /* List of memory targets */
    int                                     mem_tgt_modified;   // Flag to assert that memory targets were modified

    // This cache will be searched unless mem_tgt_modified=1.
    // If mem_tgt_modified=1, that means memory targets have been modified somehow and
    // we need to re-locate host page for the effective number ( and also update this cache )
    std::map<uint64_t, uint8_t*>            page_hash_cache;    // Cache of page_hash

    public:
    typedef typename std::list<t_mem_tgt>::iterator           mem_tgt_iter;       /* Memory target iterator */
    typedef std::map<uint64_t, uint8_t*>::iterator            page_hash_iter;     // Page hash iterator

    // Memory page usage attributes
    //uint64_t                                n_pages;            /* Number of pages currently in use */
    //std::vector<uint64_t>                   pn_vec;             /* vector of page numbers */
    //typedef std::vector<uint64_t>::iterator pn_vec_iter;        /* page number vector iterator */

    

    private:
    void dump_mem_tgt(t_mem_tgt &mem_tgt_this, std::string fmtstr="    ");
    void dump_mem_tgt2(t_mem_tgt &mem_tgt_this, std::string fmtstr="");
    bool is_overlapping_tgt(const t_mem_tgt &mem_tgt_this);
    bool is_paddr_there(mem_tgt_iter iter_this, uint64_t paddr);
    mem_tgt_iter select_mem_tgt(uint64_t paddr) throw(sim_except);
    uint8_t *paddr_to_hostpage(uint64_t paddr);
    uint8_t *paddr_to_hostaddr(uint64_t paddr);

    public:
    //
    // @func : constructor
    // @args : number of physical address lines ( To gauge the total physical address space )
    //
    memory() : m_bits(CPU_PHY_ADDR_SIZE){
        LOG_DEBUG4(MSG_FUNC_START);
        this->pa_max = (1LL << m_bits) - 1 ;
        this->pn_max = (this->pa_max) >> static_cast<int>(log2(PAGE_SIZE));
        this->mem_tgt_modified = 0;

        // Register a default DDR of the whole supported address range
        this->register_memory_target(0x0, (1LL << m_bits), "ddr0", 0, TGT_DDR, 0);
        LOG_DEBUG4(MSG_FUNC_END);
    }

    //
    // @func : destructor
    // @args : none
    //
    ~memory(){
        LOG_DEBUG4(MSG_FUNC_START);
        // clear all phy pages
        for(mem_tgt_iter iter0 = mem_tgt.begin(); iter0 != mem_tgt.end(); iter0++){
            for(page_hash_iter iter1 = iter0->page_hash.begin(); iter1 != iter0->page_hash.end(); iter1++){
                delete[] iter1->second;
            }
        }
        LOG_DEBUG4(MSG_FUNC_END);
    }

    void register_memory_target(uint64_t ba, size_t size, std::string name, uint32_t flags, int tgt_type, int prio = 0);
    void dump_all_memory_targets();
    void dump_all_page_maps();
    void dump_all_pages(std::ostream &ostr = std::cout);
    void dump_page(uint64_t addr, std::ostream &ostr = std::cout);

    // Memory I/O
    void write_from_buffer(uint64_t addr, uint8_t* buff, size_t size);
    uint8_t *read_to_buffer(uint64_t addr, uint8_t *buff, size_t size);
    const uint8_t *host_ptr(uint64_t addr);
    void write_from_file(uint64_t addr, std::string file_name, size_t size);
    void read_to_file(uint64_t addr, std::string file_name, size_t size);
    void read_to_ascii_file(uint64_t addr, std::string file_name, size_t size);
    uint8_t read8(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    void write8(uint64_t addr, uint8_t value, int endianness = EMUL_BIG_ENDIAN);
    uint16_t read16(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    void write16(uint64_t addr, uint16_t value, int endianness = EMUL_BIG_ENDIAN);
    uint32_t read32(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    void write32(uint64_t addr, uint32_t value, int endianness = EMUL_BIG_ENDIAN);
    uint64_t read64(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    void write64(uint64_t addr, uint64_t value, int endianness = EMUL_BIG_ENDIAN);

    // Load /store versions for integers
    uint8_t load_byte(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    void store_byte(uint64_t addr, uint8_t data, int endianness = EMUL_BIG_ENDIAN);
    uint16_t load_halfword(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    void store_halfword(uint64_t addr, uint16_t data, int endianness = EMUL_BIG_ENDIAN);
    uint32_t load_word(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    void store_word(uint64_t addr, uint32_t data, int endianness = EMUL_BIG_ENDIAN);
    uint64_t load_doubleword(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    void store_doubleword(uint64_t addr, uint64_t data, int endianness = EMUL_BIG_ENDIAN);
    // Load /store versions for buffers
    void store_buffer(uint64_t addr, uint8_t* buff, size_t size);
    uint8_t *load_buffer(uint64_t addr, uint8_t *buff, size_t size);

    // Load an elf file
    void load_elf(std::string flename);
    
};

#endif    /*  _MEMORY_HPP_  */
