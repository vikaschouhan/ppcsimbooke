/*
  memory.hpp ( memory facilities for cpu module )
  This file contains memory class and corresponding member functions.

  Author : Vikas Chouhan ( presentisgood@gmail.com )
  Copyright 2012.

  This file is part of ppc-sim library bundled with test_gen_ppc.

  This library is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  It is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU General Public License
  along with this file; see the file COPYING.  If not, write to the
  Free Software Foundation, 51 Franklin Street - Fifth Floor, Boston,
  MA 02110-1301, USA.
*/

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
#define  MEM_T             template<int n>
#define  MEM_PPC           memory
#define  MEM_PPC_T         MEM_PPC<n>

template<int nbits> class MEM_PPC {

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
    inline void dump_mem_tgt(t_mem_tgt &mem_tgt_this, std::string fmtstr="    ");
    inline void dump_mem_tgt2(t_mem_tgt &mem_tgt_this, std::string fmtstr="");
    inline bool is_overlapping_tgt(const t_mem_tgt &mem_tgt_this);
    inline bool is_paddr_there(mem_tgt_iter iter_this, uint64_t paddr);
    inline mem_tgt_iter select_mem_tgt(uint64_t paddr) throw(sim_except);
    uint8_t *paddr_to_hostpage(uint64_t paddr);
    uint8_t *paddr_to_hostaddr(uint64_t paddr);

    public:
    //
    // @func : constructor
    // @args : number of physical address lines ( To gauge the total physical address space )
    //
    memory() : m_bits(nbits){
        LOG("DEBUG4") << MSG_FUNC_START;
        this->pa_max = (1LL << m_bits) - 1 ;
        this->pn_max = (this->pa_max) >> static_cast<int>(log2(PAGE_SIZE));
        this->mem_tgt_modified = 0;

        // Register a default DDR of the whole supported address range
        this->register_memory_target(0x0, (1LL << m_bits), "ddr0", 0, TGT_DDR, 0);
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    //
    // @func : destructor
    // @args : none
    //
    ~memory(){
        LOG("DEBUG4") << MSG_FUNC_START; 
        LOG("DEBUG4") << MSG_FUNC_END;
    }

    void register_memory_target(uint64_t ba, size_t size, std::string name, uint32_t flags, int tgt_type, int prio = 0);
    void dump_all_memory_targets();
    void dump_all_page_maps();
    void dump_all_pages(std::ostream &ostr = std::cout);
    void dump_page(uint64_t addr, std::ostream &ostr = std::cout);

    // Memory I/O
    inline void write_from_buffer(uint64_t addr, uint8_t* buff, size_t size);
    inline uint8_t *read_to_buffer(uint64_t addr, uint8_t *buff, size_t size);
    inline void write_from_file(uint64_t addr, std::string file_name, size_t size);
    inline void read_to_file(uint64_t addr, std::string file_name, size_t size);
    inline void read_to_ascii_file(uint64_t addr, std::string file_name, size_t size);
    inline uint8_t read8(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    inline void write8(uint64_t addr, uint8_t value, int endianness = EMUL_BIG_ENDIAN);
    inline uint16_t read16(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    inline void write16(uint64_t addr, uint16_t value, int endianness = EMUL_BIG_ENDIAN);
    inline uint32_t read32(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    inline void write32(uint64_t addr, uint32_t value, int endianness = EMUL_BIG_ENDIAN);
    inline uint64_t read64(uint64_t addr, int endianness = EMUL_BIG_ENDIAN);
    inline void write64(uint64_t addr, uint64_t value, int endianness = EMUL_BIG_ENDIAN);

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

// Member functions

/*
 * @func : dump_mem_tgt
 * @args : 1. reference to a memory target object
 *         2. format string
 *
 * @brief : dump memory target
 */
MEM_T void MEM_PPC_T::dump_mem_tgt(t_mem_tgt &mem_tgt_this, std::string fmtstr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::cout << std::hex << std::showbase;
    std::cout << fmtstr << "base address = " << mem_tgt_this.baseaddr << std::endl;
    std::cout << fmtstr << "size         = " << mem_tgt_this.size << std::endl;
    std::cout << fmtstr << "flags        = " << mem_tgt_this.flags << std::endl;
    std::cout << fmtstr << "name         = " << mem_tgt_this.name << std::endl;
    std::cout << fmtstr << "priority     = " << mem_tgt_this.priority << std::endl;
    std::cout << fmtstr << "target       = " << mem_tgt_str[mem_tgt_this.tgt_type] << std::endl;
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : dump_mem_tgt2
 * @args : 1. reference to a memory target object
 *         2. format string
 *
 * @brief : dump memory target
 *
 */
MEM_T void MEM_PPC_T::dump_mem_tgt2(t_mem_tgt &mem_tgt_this, std::string fmtstr){
    LOG("DEBUG4") << MSG_FUNC_START;
    std::cout << std::hex << std::showbase;
    std::cout << fmtstr << "tgt = " << mem_tgt_this.name << " with base_addr = " <<
        mem_tgt_this.baseaddr << ", size = " << mem_tgt_this.size << ", priority = " <<
        mem_tgt_this.priority << ", tgt_type = " << mem_tgt_str[mem_tgt_this.tgt_type] <<
        std::endl;
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : is_overlapping_tgt
 * @args : const ref. to a memory target
 *
 * @brief : checks if this target overlaps with other targets and returns and bool status value
 *
 */
MEM_T bool MEM_PPC_T::is_overlapping_tgt(const t_mem_tgt &mem_tgt_this){
    LOG("DEBUG4") << MSG_FUNC_START;
    for(mem_tgt_iter iter_this = mem_tgt.begin(); iter_this != mem_tgt.end(); iter_this++){
        if((mem_tgt_this.baseaddr >= iter_this->baseaddr) &&
                (mem_tgt_this.baseaddr <= iter_this->endaddr) &&
                (mem_tgt_this.priority <= iter_this->priority)){
            LOG("DEBUG4") << MSG_FUNC_END;
            return true;
        }
        if((mem_tgt_this.endaddr >= iter_this->baseaddr) &&
                (mem_tgt_this.endaddr < iter_this->endaddr) &&
                (mem_tgt_this.priority <= iter_this->priority)){
            LOG("DEBUG4") << MSG_FUNC_END;
            return true;
        }
    }
    LOG("DEBUG4") << MSG_FUNC_END;
    return false;
}

/*
 * @func : is_paddr_there
 * @args : iterator to the memory target, physical address
 *
 * @brief : check if the passed physical address is there in the passed target
 * @return : bool
 */
MEM_T bool MEM_PPC_T::is_paddr_there(typename MEM_PPC_T::mem_tgt_iter iter_this, uint64_t paddr){
    LOG("DEBUG4") << MSG_FUNC_START;
    if((iter_this->baseaddr <= paddr) && (iter_this->endaddr >= paddr)){
        LOG("DEBUG4") << MSG_FUNC_END;
        return true;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
    return false; 
}

/*
 * @func : select_mem_tgt
 * @args : physical address
 *
 * @brief: select a memory target based on physical address
 * @return : iterator ( pointer ) to the selected memory target
 */
MEM_T typename MEM_PPC_T::mem_tgt_iter MEM_PPC_T::select_mem_tgt(uint64_t paddr) throw(sim_except){
    LOG("DEBUG4") << MSG_FUNC_START;
    mem_tgt_iter iter_last = mem_tgt.end();
    for(mem_tgt_iter iter_this = mem_tgt.begin(); iter_this != mem_tgt.end(); iter_this++){
        if(is_paddr_there(iter_this, paddr)){
            if(iter_last == mem_tgt.end())
                iter_last = iter_this;
            else if(iter_last->priority < iter_this->priority){
                iter_last = iter_this;
            }else if(iter_last->priority == iter_this->priority){
                throw(sim_except_fatal("two overlapping memory targets having same priority"));
            }
        }
    }
    LOG("DEBUG4") << std::hex << std::showbase << "target selected with baseaddr=" << iter_last->baseaddr << " ,size=" <<
        iter_last->size << " ,priority=" << iter_last->priority << " ,name=" << iter_last->name << " ,tgt_type=" <<
        mem_tgt_str[iter_last->tgt_type];
    LOG("DEBUG4") << MSG_FUNC_END;
    return iter_last;
}

/*
 * @func : paddr_to_hostpage
 * @args : physical address
 *
 * @brief: returns a pointer to the host page
 */
MEM_T uint8_t* MEM_PPC_T::paddr_to_hostpage(uint64_t paddr){
    LOG("DEBUG4") << MSG_FUNC_START;
   
    uint64_t pageno = (paddr >> MIN_PGSZ_SHIFT);
    // Search in cache first
    if(mem_tgt_modified == 0){
        if((page_hash_cache.find(pageno) != page_hash_cache.end()) && page_hash_cache[pageno] != NULL){
            return page_hash_cache[pageno];
        }
    }

    mem_tgt_iter iter_this = select_mem_tgt(paddr);
    LASSERT_THROW(iter_this != mem_tgt.end(), sim_except(SIM_EXCEPT_EFAULT, "No valid target found for this address"), DEBUG4);

    if(!iter_this->page_hash[pageno]){
        iter_this->page_hash[pageno] = new uint8_t[MIN_PGSZ];
        page_hash_cache[pageno] = iter_this->page_hash[pageno];   // Cache this newly created page
    }

    // Clear mem_tgt_modified, just in case it was already set
    mem_tgt_modified = 0;

    LOG("DEBUG4") << MSG_FUNC_END;
    return iter_this->page_hash[pageno];
}

/*
 * @func : paddr_to_hostaddr
 * @args : physical address
 *
 * @brief: return a host pointer to the physical address
 * @return: unsigned char pointer to the host address
 */
MEM_T uint8_t* MEM_PPC_T::paddr_to_hostaddr(uint64_t paddr){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint64_t pageno = (paddr >> MIN_PGSZ_SHIFT);
    size_t   offset =  (paddr - (pageno << MIN_PGSZ_SHIFT));
    LOG("DEBUG4") << MSG_FUNC_END;
    return (paddr_to_hostpage(paddr) + offset);
}

/*
 * @func : register_memory_target
 * @args : base address, size, name, flags, memory target type
 *
 * @brief : registers a memory target type
 */
MEM_T void MEM_PPC_T::register_memory_target(uint64_t ba, size_t size, std::string name, uint32_t flags, int tgt_type, int prio){
    // Check for legal values.
    if(size > static_cast<size_t>(1 << this->m_bits)){
        // throw exception
    }
    if(mem_tgt_str.find(tgt_type) == mem_tgt_str.end()){
        // throw exception
    }

    // Round page size and base address to next valid value
    size = rnd_pgsz_n(size);
    ba   = rnd_pgsz_n(ba);
    
    // Need to verify page size and also check if ba is aligned with it.
    struct t_mem_tgt l_tgt;
    l_tgt.baseaddr = ba;
    l_tgt.size     = size;
    l_tgt.endaddr  = (ba + size - 1);
    l_tgt.name     = name;
    l_tgt.tgt_type = tgt_type;
    l_tgt.flags    = flags;
    l_tgt.priority = prio;

    // clear the hash
    l_tgt.page_hash.clear();

    if(is_overlapping_tgt(l_tgt)){
        std::cerr << std::hex << std::showbase << "Target with baseaddr = " << l_tgt.baseaddr <<
           " ,size = " << l_tgt.size << " ,name = " << l_tgt.name << " ,tgt_type = " << l_tgt.tgt_type <<
           " overlaps with some other target. Please " << "specifiy with priority higher than " <<
           l_tgt.priority << std::endl;
        return;
    }

    n_tgts++;
    mem_tgt.push_back(l_tgt);
}

/*
 * @func : dump_all_memory_targets
 * @args : none
 *
 * @brief : dump all memory targets
 */
MEM_T void MEM_PPC_T::dump_all_memory_targets(){
    for(mem_tgt_iter iter_this = mem_tgt.begin(); iter_this != mem_tgt.end(); iter_this++){
        std::cout << BAR0 << std::endl;
        dump_mem_tgt(*iter_this);
        std::cout << BAR0 << std::endl;
    }
}

/*
 * @func : dump_all_page_maps
 * @args : none
 *
 * @brief : dump all pages hashes for all memory targets.
 * @type : debug
 */
MEM_T void MEM_PPC_T::dump_all_page_maps(){
    LOG("DEBUG4") << MSG_FUNC_START;
    for(mem_tgt_iter iter_this = mem_tgt.begin(); iter_this != mem_tgt.end(); iter_this++){
        dump_mem_tgt2(*iter_this);
        std::cout << BAR0 << std::endl;
        for(page_hash_iter iter_this_1 = iter_this->page_hash.begin();
                iter_this_1 != iter_this->page_hash.end(); iter_this_1++){
            std::cout << std::hex << std::showbase << "pghsh[" << iter_this_1->first <<
                "] = " << (uint64_t)iter_this_1->second << std::endl;
        }
        std::cout << BAR0 << std::endl;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func  : dump_all_pages
 * @args  : none
 *
 * @brief : dump all pages, along with their contents into a text file / binary files
 * @type  : debug
 */
MEM_T void MEM_PPC_T::dump_all_pages(std::ostream &ostr){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *ptr = NULL;
    int grp_size = 4;
    int columns = 8;
    uint64_t addr = 0;
    int i;

    ostr << std::noshowbase;

    for(mem_tgt_iter iter_this = mem_tgt.begin(); iter_this != mem_tgt.end(); iter_this++){
        for(page_hash_iter iter_this_1 = iter_this->page_hash.begin();
                iter_this_1 != iter_this->page_hash.end(); iter_this_1++){
            // Print base address of this page
            ostr << BAR0 << std::endl;
            ostr << "ra:" << "0x" << std::hex << std::setfill('0') << std::setw(16) << (iter_this_1->first << MIN_PGSZ_SHIFT) << std::endl;
            ostr << BAR0 << std::endl;

            ptr = iter_this_1->second;
            addr = 0;

            for(i=0; i<MIN_PGSZ; i++){
                if(!(i % (columns*grp_size))){
                    ostr << std::endl;
                    ostr << std::hex << "0x" << std::setfill('0') << std::setw(16) << addr ;
                    addr += grp_size*columns;
                }
                if(!(i % grp_size)){
                    ostr << "    ";
                }
                ostr << std::hex << std::setw(2) << (unsigned int)(*ptr);
                ptr++;
            }
            ostr << std::endl;

        }
    } 
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func  : dump_page
 * @args  : none
 *
 * @brief : dump page by physical address ( i.e containing a physical address )
 * @type  : debug
 */
MEM_T void MEM_PPC_T::dump_page(uint64_t addr, std::ostream &ostr){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *ptr = NULL;
    int grp_size = 4;
    int columns = 8;
    int i;

    ostr << std::noshowbase;

    // Print base address of this page
    ostr << BAR0 << std::endl;
    ostr << "ra:" << "0x" << std::hex << std::setfill('0') << std::setw(16) << (addr & ~(MIN_PGSZ - 1)) << std::endl;
    ostr << BAR0 << std::endl;

    ptr = paddr_to_hostpage(addr);
    addr = 0;

    for(i=0; i<MIN_PGSZ; i++){
        if(!(i % (columns*grp_size))){
            ostr << std::endl;
            ostr << std::hex << "0x" << std::setfill('0') << std::setw(16) << addr ;
            addr += grp_size*columns;
        }
        if(!(i % grp_size)){
            ostr << "    ";
        }
        ostr << std::hex << std::setw(2) << (unsigned int)(*ptr);
        ptr++;
    }
    ostr << std::endl;

    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : write_from_buffer
 * @args : uint64_t address, char *, size_t
 *
 * @brief : writes passed buffer to the memory
 */
MEM_T void MEM_PPC_T::write_from_buffer(uint64_t addr, uint8_t* buff, size_t size){
    size_t curr_size;
    size_t offset;
    uint8_t *curr_page = NULL;

    while(size > 0){
        curr_size = min(size, (rnd_pgsz_n_plus_1(addr)-addr));
        offset = addr - rnd_pgsz_p(addr);

        // Get pointer to the host page for this address
        curr_page = paddr_to_hostpage(addr);
        // Copy this chunk into the page
        memcpy((curr_page + offset), buff, curr_size);

        addr += curr_size;
        buff += curr_size;
        size -= curr_size;
    }
}

/*
 * @func : read_to_buffer
 * @args : uint64_t address, uint8_t *buff, size_t size
 *
 * @brief: reads data into buffer from requested physical address
 * @return : buffer pointer
 */
MEM_T uint8_t* MEM_PPC_T::read_to_buffer(uint64_t addr, uint8_t *buff, size_t size){
    size_t curr_size;
    size_t offset;
    uint8_t *curr_page = NULL;

    while(size > 0){
        curr_size = min(size, (rnd_pgsz_n_plus_1(addr)-addr));
        offset = addr - rnd_pgsz_p(addr);

        // Get pointer to the host page for this address
        curr_page = paddr_to_hostpage(addr);
        // Copy this chunk into the page
        memcpy(buff, (curr_page + offset), curr_size);

        addr += curr_size;
        buff += curr_size;
        size -= curr_size;
    }
    return buff;
}

/*
 * @func : write_from_file
 * @args : physical address, size, file name
 *
 * @brief : copies size bytes to physical address "addr" from file named "file_name"
 */
MEM_T void MEM_PPC_T::write_from_file(uint64_t addr, std::string file_name, size_t size){
    std::ifstream ih;
    ih.open(file_name.c_str(), std::istream::in | std::ifstream::binary);
    LASSERT_THROW(ih.fail() == 0, sim_except(SIM_EXCEPT_ENOFILE, "opening file failed."), DEBUG4);
   
    uint8_t *buff = new uint8_t[size];
    ih.read(reinterpret_cast<char*>(buff), size);
    write_from_buffer(addr, buff, size);
    delete[] buff;

    ih.close();
}

/*
 * @func : read_to_file
 * @args : physical address, file name, size
 *
 * @brief : copies size bytes from physical addr "addr" to file name "file_name"
 */
MEM_T void MEM_PPC_T::read_to_file(uint64_t addr, std::string file_name, size_t size){
    std::ofstream oh;
    oh.open(file_name.c_str(), std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    LASSERT_THROW(oh.fail() == 0, sim_except(SIM_EXCEPT_ENOFILE, "opening file failed."), DEBUG4);

    uint8_t *buff = new uint8_t[size];
    read_to_buffer(addr, buff, size);
    oh.write(reinterpret_cast<char*>(buff), size);
    delete[] buff;

    oh.close();
}

/*
 * @func : read_to_ascii_file
 * @args : physical address, file name, size
 *
 * @brief : writes size bytes from physical addr "addr" to file name "file_name" coverting them to ascii in the process
 */
MEM_T void MEM_PPC_T::read_to_ascii_file(uint64_t addr, std::string file_name, size_t size){
    std::ofstream oh;
    oh.open(file_name.c_str(), std::ofstream::out | std::ofstream::trunc);
    LASSERT_THROW(oh.fail() == 0, sim_except(SIM_EXCEPT_ENOFILE, "opening file failed."), DEBUG4);

    uint8_t *buff = new uint8_t[size];
    read_to_buffer(addr, buff, size);

    for(size_t i=0; i<size; i++){
        if(buff[i] >= 0x20 && buff[i] <= 0x7e)                          // All printable chars from SPACE to ~
            oh << buff[i];
        else if(buff[i] == 0xa || buff[i] == 0xc || buff[i] == 0xd)     // Line feed, Form feed, Carriage return
            oh << std::endl;
        else if(buff[i] == 0x9)                                         // Horizontal tab
            oh << "\t";
        else
            oh << std::endl;
    }

    delete[] buff;

    oh.close();
}

/*
 * @func : read8
 * @args : physical address, ENDIANNESS MODE
 *
 * @brief : return 8 bits data at physical address
 */ 
MEM_T uint8_t MEM_PPC_T::read8(uint64_t addr, int endianness){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    LOG("DEBUG4") << MSG_FUNC_END;
    return *hostptr;
}

/* @func : write8
 * @args : physical address, 8bits value, ENDIANNESS MODE
 *
 * @brief : write 8 bits of data at location physical address
 */
MEM_T void MEM_PPC_T::write8(uint64_t addr, uint8_t value, int endianness){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    *hostptr = value;
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : read16
 * @args : physical address, ENDIANNESS MODE
 *
 * @brief : return 16 bits data at physical address
 */ 
MEM_T uint16_t MEM_PPC_T::read16(uint64_t addr, int endianness){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    uint16_t value = 0;
    if(endianness == EMUL_BIG_ENDIAN){
        value = (static_cast<uint16_t>(hostptr[0]) << 8) |
                (static_cast<uint16_t>(hostptr[1]) << 0) ;
    }else{
        value = (static_cast<uint16_t>(hostptr[1]) << 8) |
                (static_cast<uint16_t>(hostptr[0]) << 0) ;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
    return value;
}

/* @func : write16
 * @args : physical address, 16bit value, ENDIANNESS MODE
 *
 * @brief : write 16 bits of data at location physical address
 */
MEM_T void MEM_PPC_T::write16(uint64_t addr, uint16_t value, int endianness){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    if(endianness == EMUL_BIG_ENDIAN){
        hostptr[0] = (value >> 8);
        hostptr[1] = (value >> 0);
    }else{
        hostptr[1] = (value >> 8);
        hostptr[0] = (value >> 0);
    }
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : read32
 * @args : physical address, ENDIANNESS MODE
 *
 * @brief : return 32 bits data at physical address
 */ 
MEM_T uint32_t MEM_PPC_T::read32(uint64_t addr, int endianness){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    uint32_t value = 0;
    if(endianness == EMUL_BIG_ENDIAN){
        value = (static_cast<uint32_t>(hostptr[0]) << 24) |
                (static_cast<uint32_t>(hostptr[1]) << 16) |
                (static_cast<uint32_t>(hostptr[2]) <<  8) |
                (static_cast<uint32_t>(hostptr[3]) <<  0) ;
    }else{
        value = (static_cast<uint32_t>(hostptr[3]) << 24) |
                (static_cast<uint32_t>(hostptr[2]) << 16) |
                (static_cast<uint32_t>(hostptr[1]) <<  8) |
                (static_cast<uint32_t>(hostptr[0]) <<  0) ;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
    return value;
}

/* @func : write32
 * @args : physical address, 32bit value, ENDIANNESS MODE
 *
 * @brief : write 32 bits of data at location physical address
 */
MEM_T void MEM_PPC_T::write32(uint64_t addr, uint32_t value, int endianness){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    if(endianness == EMUL_BIG_ENDIAN){
        hostptr[0] = (value >> 24);
        hostptr[1] = (value >> 16);
        hostptr[2] = (value >>  8);
        hostptr[3] = (value >>  0);
    }else{
        hostptr[3] = (value >> 24);
        hostptr[2] = (value >> 16);
        hostptr[1] = (value >>  8);
        hostptr[0] = (value >>  0);
    }
    LOG("DEBUG4") << MSG_FUNC_END;
}

/*
 * @func : read64 
 * @args : physical address, endianness
 */
MEM_T uint64_t MEM_PPC_T::read64(uint64_t addr, int endianness){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    uint64_t value = 0;
    if(endianness == EMUL_BIG_ENDIAN){
        value = (static_cast<uint64_t>(hostptr[0]) << 56) |
                (static_cast<uint64_t>(hostptr[1]) << 48) |
                (static_cast<uint64_t>(hostptr[2]) << 40) |
                (static_cast<uint64_t>(hostptr[3]) << 32) |
                (static_cast<uint64_t>(hostptr[4]) << 24) |
                (static_cast<uint64_t>(hostptr[5]) << 16) |
                (static_cast<uint64_t>(hostptr[6]) <<  8) |
                (static_cast<uint64_t>(hostptr[7]) <<  0) ;
    }else{
        value = (static_cast<uint64_t>(hostptr[7]) << 56) |
                (static_cast<uint64_t>(hostptr[6]) << 48) |
                (static_cast<uint64_t>(hostptr[5]) << 40) |
                (static_cast<uint64_t>(hostptr[4]) << 32) |
                (static_cast<uint64_t>(hostptr[3]) << 24) |
                (static_cast<uint64_t>(hostptr[2]) << 16) |
                (static_cast<uint64_t>(hostptr[1]) <<  8) |
                (static_cast<uint64_t>(hostptr[0]) <<  0) ;
    }
    LOG("DEBUG4") << MSG_FUNC_END;
    return value;
}

/*
 * @func : write64
 * @args : physical address, data, endianness
 */
MEM_T void MEM_PPC_T::write64(uint64_t addr, uint64_t value, int endianness){
    LOG("DEBUG4") << MSG_FUNC_START;
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    if(endianness == EMUL_BIG_ENDIAN){
        hostptr[0] = (value >> 56);
        hostptr[1] = (value >> 48);
        hostptr[2] = (value >> 40);
        hostptr[3] = (value >> 32);
        hostptr[4] = (value >> 24);
        hostptr[5] = (value >> 16);
        hostptr[6] = (value >>  8);
        hostptr[7] = (value >>  0);
    }else{
        hostptr[7] = (value >> 56);
        hostptr[6] = (value >> 48);
        hostptr[5] = (value >> 40);
        hostptr[4] = (value >> 32);
        hostptr[3] = (value >> 24);
        hostptr[2] = (value >> 16);
        hostptr[1] = (value >>  8);
        hostptr[0] = (value >>  0);
    }
    LOG("DEBUG4") << MSG_FUNC_END;
}

// Load /store versions for integers
MEM_T uint8_t MEM_PPC_T::load_byte(uint64_t addr, int endianness){
    return read8(addr, endianness);
}
MEM_T void MEM_PPC_T::store_byte(uint64_t addr, uint8_t data, int endianness){
    write8(addr, data, endianness);
}
MEM_T uint16_t MEM_PPC_T::load_halfword(uint64_t addr, int endianness){
    return read16(addr, endianness);
}
MEM_T void MEM_PPC_T::store_halfword(uint64_t addr, uint16_t data, int endianness){
    write16(addr, data, endianness);
}
MEM_T uint32_t MEM_PPC_T::load_word(uint64_t addr, int endianness){
    return read32(addr, endianness);
}
MEM_T void MEM_PPC_T::store_word(uint64_t addr, uint32_t data, int endianness){
    write32(addr, data, endianness); 
}
MEM_T uint64_t MEM_PPC_T::load_doubleword(uint64_t addr, int endianness){
    return read64(addr, endianness);
}
MEM_T void MEM_PPC_T::store_doubleword(uint64_t addr, uint64_t data, int endianness){
    write64(addr, data, endianness);
}

// Load /store versions for buffers
MEM_T void MEM_PPC_T::store_buffer(uint64_t addr, uint8_t* buff, size_t size){
    write_from_buffer(addr, buff, size);
}
MEM_T uint8_t* MEM_PPC_T::load_buffer(uint64_t addr, uint8_t *buff, size_t size){
    return read_to_buffer(addr, buff, size);
}

// Load an elf file
MEM_T void MEM_PPC_T::load_elf(std::string filename){
    ELFIO::elfio elfreader;
    LASSERT_THROW(elfreader.load(filename), sim_except(SIM_EXCEPT_ENOFILE, "File couldn't be opened"), DEBUG4);
   
    const ELFIO::section    *psec   = NULL; 
    ELFIO::Elf_Half          sec_num = elfreader.sections.size();
    ELFIO::Elf64_Addr        secaddr;
    ELFIO::Elf_Xword         secflags;
    std::string              secname;
    ELFIO::Elf_Half          secidx;
    ELFIO::Elf_Word          sectype;
    ELFIO::Elf_Xword         secsize;
    char                     *data = NULL;

    // Loading sections
    for(int i=0; i<sec_num; i++){
        psec     = elfreader.sections[i];
        secaddr  = psec->get_address();
        secflags = psec->get_flags();
        secname  = psec->get_name();
        secidx   = psec->get_index();
        sectype  = psec->get_type();
        secsize  = psec->get_size();

        if((sectype == SHT_PROGBITS) && (secflags & SHF_ALLOC)){
            data = const_cast<char*>(psec->get_data());
            std::cout << "Loading section[" << secidx << "] " << secname << std::hex
                      <<" size=" << secsize << " at 0x" << secaddr << std::endl;
            write_from_buffer(secaddr, reinterpret_cast<uint8_t *>(data), secsize);
        }
    }
}

#endif    /*  _MEMORY_HPP_  */
