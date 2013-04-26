#include "memory.h"

// Member functions

/*
 * @func : dump_mem_tgt
 * @args : 1. reference to a memory target object
 *         2. format string
 *
 * @brief : dump memory target
 */
void ppcsimbooke::ppcsimbooke_memory::memory::dump_mem_tgt(t_mem_tgt &mem_tgt_this, std::string fmtstr){
    LOG_DEBUG4(MSG_FUNC_START);
    std::cout << std::hex << std::showbase;
    std::cout << fmtstr << "base address = " << mem_tgt_this.baseaddr << std::endl;
    std::cout << fmtstr << "size         = " << mem_tgt_this.size << std::endl;
    std::cout << fmtstr << "flags        = " << mem_tgt_this.flags << std::endl;
    std::cout << fmtstr << "name         = " << mem_tgt_this.name << std::endl;
    std::cout << fmtstr << "priority     = " << mem_tgt_this.priority << std::endl;
    std::cout << fmtstr << "target       = " << mem_tgt_str[mem_tgt_this.tgt_type] << std::endl;
    LOG_DEBUG4(MSG_FUNC_END);
}

/*
 * @func : dump_mem_tgt2
 * @args : 1. reference to a memory target object
 *         2. format string
 *
 * @brief : dump memory target
 *
 */
void ppcsimbooke::ppcsimbooke_memory::memory::dump_mem_tgt2(t_mem_tgt &mem_tgt_this, std::string fmtstr){
    LOG_DEBUG4(MSG_FUNC_START);
    std::cout << std::hex << std::showbase;
    std::cout << fmtstr << "tgt = " << mem_tgt_this.name << " with base_addr = " <<
        mem_tgt_this.baseaddr << ", size = " << mem_tgt_this.size << ", priority = " <<
        mem_tgt_this.priority << ", tgt_type = " << mem_tgt_str[mem_tgt_this.tgt_type] <<
        std::endl;
    LOG_DEBUG4(MSG_FUNC_END);
}

/*
 * @func : is_overlapping_tgt
 * @args : const ref. to a memory target
 *
 * @brief : checks if this target overlaps with other targets and returns and bool status value
 *
 */
bool ppcsimbooke::ppcsimbooke_memory::memory::is_overlapping_tgt(const t_mem_tgt &mem_tgt_this){
    LOG_DEBUG4(MSG_FUNC_START);
    for(mem_tgt_iter iter_this = mem_tgt.begin(); iter_this != mem_tgt.end(); iter_this++){
        if((mem_tgt_this.baseaddr >= iter_this->baseaddr) &&
                (mem_tgt_this.baseaddr <= iter_this->endaddr) &&
                (mem_tgt_this.priority <= iter_this->priority)){
            LOG_DEBUG4(MSG_FUNC_END);
            return true;
        }
        if((mem_tgt_this.endaddr >= iter_this->baseaddr) &&
                (mem_tgt_this.endaddr < iter_this->endaddr) &&
                (mem_tgt_this.priority <= iter_this->priority)){
            LOG_DEBUG4(MSG_FUNC_END);
            return true;
        }
    }
    LOG_DEBUG4(MSG_FUNC_END);
    return false;
}

/*
 * @func : is_paddr_there
 * @args : iterator to the memory target, physical address
 *
 * @brief : check if the passed physical address is there in the passed target
 * @return : bool
 */
bool ppcsimbooke::ppcsimbooke_memory::memory::is_paddr_there(typename ppcsimbooke::ppcsimbooke_memory::memory::mem_tgt_iter iter_this, uint64_t paddr){
    LOG_DEBUG4(MSG_FUNC_START);
    if((iter_this->baseaddr <= paddr) && (iter_this->endaddr >= paddr)){
        LOG_DEBUG4(MSG_FUNC_END);
        return true;
    }
    LOG_DEBUG4(MSG_FUNC_END);
    return false; 
}

/*
 * @func : select_mem_tgt
 * @args : physical address
 *
 * @brief: select a memory target based on physical address
 * @return : iterator ( pointer ) to the selected memory target
 */
typename ppcsimbooke::ppcsimbooke_memory::memory::mem_tgt_iter ppcsimbooke::ppcsimbooke_memory::memory::select_mem_tgt(uint64_t paddr) throw(sim_except){
    LOG_DEBUG4(MSG_FUNC_START);
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
    LOG_DEBUG4(std::hex, std::showbase, "target selected with baseaddr=", iter_last->baseaddr, " ,size=",
        iter_last->size, " ,priority=", iter_last->priority, " ,name=", iter_last->name, " ,tgt_type=",
        mem_tgt_str[iter_last->tgt_type]);
    LOG_DEBUG4(MSG_FUNC_END);
    return iter_last;
}

/*
 * @func : paddr_to_hostpage
 * @args : physical address
 *
 * @brief: returns a pointer to the host page
 */
uint8_t* ppcsimbooke::ppcsimbooke_memory::memory::paddr_to_hostpage(uint64_t paddr){
    LOG_DEBUG4(MSG_FUNC_START);
   
    uint64_t pageno = (paddr >> MIN_PGSZ_SHIFT);
    // Search in cache first
    if likely(mem_tgt_modified == 0){
        if likely((page_hash_cache.find(pageno) != page_hash_cache.end()) && page_hash_cache[pageno] != NULL){
            return page_hash_cache[pageno];
        }
    }

    mem_tgt_iter iter_this = select_mem_tgt(paddr);
    LASSERT_THROW_UNLIKELY(iter_this != mem_tgt.end(), sim_except(SIM_EXCEPT_EFAULT, "No valid target found for this address"), DEBUG4);

    if(!iter_this->page_hash[pageno]){
        iter_this->page_hash[pageno] = new uint8_t[MIN_PGSZ];
        page_hash_cache[pageno] = iter_this->page_hash[pageno];   // Cache this newly created page
    }

    // Clear mem_tgt_modified, just in case it was already set
    mem_tgt_modified = 0;

    LOG_DEBUG4(MSG_FUNC_END);
    return iter_this->page_hash[pageno];
}

/*
 * @func : paddr_to_hostaddr
 * @args : physical address
 *
 * @brief: return a host pointer to the physical address
 * @return: unsigned char pointer to the host address
 */
uint8_t* ppcsimbooke::ppcsimbooke_memory::memory::paddr_to_hostaddr(uint64_t paddr){
    LOG_DEBUG4(MSG_FUNC_START);
    uint64_t pageno = (paddr >> MIN_PGSZ_SHIFT);
    size_t   offset =  (paddr - (pageno << MIN_PGSZ_SHIFT));
    LOG_DEBUG4(MSG_FUNC_END);
    return (paddr_to_hostpage(paddr) + offset);
}

/*
 * @func : register_memory_target
 * @args : base address, size, name, flags, memory target type
 *
 * @brief : registers a memory target type
 */
void ppcsimbooke::ppcsimbooke_memory::memory::register_memory_target(uint64_t ba, size_t size, std::string name, uint32_t flags, int tgt_type, int prio){
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
void ppcsimbooke::ppcsimbooke_memory::memory::dump_all_memory_targets(){
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
void ppcsimbooke::ppcsimbooke_memory::memory::dump_all_page_maps(){
    LOG_DEBUG4(MSG_FUNC_START);
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
    LOG_DEBUG4(MSG_FUNC_END);
}

/*
 * @func  : dump_all_pages
 * @args  : none
 *
 * @brief : dump all pages, along with their contents into a text file / binary files
 * @type  : debug
 */
void ppcsimbooke::ppcsimbooke_memory::memory::dump_all_pages(std::ostream &ostr){
    LOG_DEBUG4(MSG_FUNC_START);
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
    LOG_DEBUG4(MSG_FUNC_END);
}

/*
 * @func  : dump_page
 * @args  : none
 *
 * @brief : dump page by physical address ( i.e containing a physical address )
 * @type  : debug
 */
void ppcsimbooke::ppcsimbooke_memory::memory::dump_page(uint64_t addr, std::ostream &ostr){
    LOG_DEBUG4(MSG_FUNC_START);
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

    LOG_DEBUG4(MSG_FUNC_END);
}

/*
 * @func : write_from_buffer
 * @args : uint64_t address, char *, size_t
 *
 * @brief : writes passed buffer to the memory
 */
void ppcsimbooke::ppcsimbooke_memory::memory::write_from_buffer(uint64_t addr, uint8_t* buff, size_t size){
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
uint8_t* ppcsimbooke::ppcsimbooke_memory::memory::read_to_buffer(uint64_t addr, uint8_t *buff, size_t size){
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
 * @func : host_ptr
 * @args : uint64_t address
 *
 * @brief  : get host pointer directly (useful for fast access)
 * @return : pointer
 */
const uint8_t* ppcsimbooke::ppcsimbooke_memory::memory::host_ptr(uint64_t addr){
    LOG_DEBUG4("Inside memory::host_ptr", std::endl);
    return const_cast<uint8_t*>(paddr_to_hostaddr(addr));
}

/*
 * @func : write_from_file
 * @args : physical address, size, file name
 *
 * @brief : copies size bytes to physical address "addr" from file named "file_name"
 */
void ppcsimbooke::ppcsimbooke_memory::memory::write_from_file(uint64_t addr, std::string file_name, size_t size){
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
void ppcsimbooke::ppcsimbooke_memory::memory::read_to_file(uint64_t addr, std::string file_name, size_t size){
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
void ppcsimbooke::ppcsimbooke_memory::memory::read_to_ascii_file(uint64_t addr, std::string file_name, size_t size){
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
uint8_t ppcsimbooke::ppcsimbooke_memory::memory::read8(uint64_t addr, int endianness){
    LOG_DEBUG4(MSG_FUNC_START);
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    LOG_DEBUG4(MSG_FUNC_END);
    return *hostptr;
}

/* @func : write8
 * @args : physical address, 8bits value, ENDIANNESS MODE
 *
 * @brief : write 8 bits of data at location physical address
 */
void ppcsimbooke::ppcsimbooke_memory::memory::write8(uint64_t addr, uint8_t value, int endianness){
    LOG_DEBUG4(MSG_FUNC_START);
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    *hostptr = value;
    LOG_DEBUG4(MSG_FUNC_END);
}

/*
 * @func : read16
 * @args : physical address, ENDIANNESS MODE
 *
 * @brief : return 16 bits data at physical address
 */ 
uint16_t ppcsimbooke::ppcsimbooke_memory::memory::read16(uint64_t addr, int endianness){
    LOG_DEBUG4(MSG_FUNC_START);
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    LOG_DEBUG4(MSG_FUNC_END);
    return read_buff<uint16_t>(hostptr, endianness);
}

/* @func : write16
 * @args : physical address, 16bit value, ENDIANNESS MODE
 *
 * @brief : write 16 bits of data at location physical address
 */
void ppcsimbooke::ppcsimbooke_memory::memory::write16(uint64_t addr, uint16_t value, int endianness){
    LOG_DEBUG4(MSG_FUNC_START);
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    write_buff<uint16_t>(hostptr, value, endianness);
    LOG_DEBUG4(MSG_FUNC_END);
}

/*
 * @func : read32
 * @args : physical address, ENDIANNESS MODE
 *
 * @brief : return 32 bits data at physical address
 */ 
uint32_t ppcsimbooke::ppcsimbooke_memory::memory::read32(uint64_t addr, int endianness){
    LOG_DEBUG4(MSG_FUNC_START);
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    LOG_DEBUG4(MSG_FUNC_END);
    return read_buff<uint32_t>(hostptr, endianness);
}

/* @func : write32
 * @args : physical address, 32bit value, ENDIANNESS MODE
 *
 * @brief : write 32 bits of data at location physical address
 */
void ppcsimbooke::ppcsimbooke_memory::memory::write32(uint64_t addr, uint32_t value, int endianness){
    LOG_DEBUG4(MSG_FUNC_START);
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    write_buff<uint32_t>(hostptr, value, endianness);
    LOG_DEBUG4(MSG_FUNC_END);
}

/*
 * @func : read64 
 * @args : physical address, endianness
 */
uint64_t ppcsimbooke::ppcsimbooke_memory::memory::read64(uint64_t addr, int endianness){
    LOG_DEBUG4(MSG_FUNC_START);
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    LOG_DEBUG4(MSG_FUNC_END);
    return read_buff<uint64_t>(hostptr, endianness);
}

/*
 * @func : write64
 * @args : physical address, data, endianness
 */
void ppcsimbooke::ppcsimbooke_memory::memory::write64(uint64_t addr, uint64_t value, int endianness){
    LOG_DEBUG4(MSG_FUNC_START);
    uint8_t *hostptr = paddr_to_hostaddr(addr);
    write_buff<uint64_t>(hostptr, value, endianness);
    LOG_DEBUG4(MSG_FUNC_END);
}

// Load /store versions for integers
uint8_t ppcsimbooke::ppcsimbooke_memory::memory::load_byte(uint64_t addr, int endianness){
    return read8(addr, endianness);
}
void ppcsimbooke::ppcsimbooke_memory::memory::store_byte(uint64_t addr, uint8_t data, int endianness){
    write8(addr, data, endianness);
}
uint16_t ppcsimbooke::ppcsimbooke_memory::memory::load_halfword(uint64_t addr, int endianness){
    return read16(addr, endianness);
}
void ppcsimbooke::ppcsimbooke_memory::memory::store_halfword(uint64_t addr, uint16_t data, int endianness){
    write16(addr, data, endianness);
}
uint32_t ppcsimbooke::ppcsimbooke_memory::memory::load_word(uint64_t addr, int endianness){
    return read32(addr, endianness);
}
void ppcsimbooke::ppcsimbooke_memory::memory::store_word(uint64_t addr, uint32_t data, int endianness){
    write32(addr, data, endianness); 
}
uint64_t ppcsimbooke::ppcsimbooke_memory::memory::load_doubleword(uint64_t addr, int endianness){
    return read64(addr, endianness);
}
void ppcsimbooke::ppcsimbooke_memory::memory::store_doubleword(uint64_t addr, uint64_t data, int endianness){
    write64(addr, data, endianness);
}

// Load /store versions for buffers
void ppcsimbooke::ppcsimbooke_memory::memory::store_buffer(uint64_t addr, uint8_t* buff, size_t size){
    write_from_buffer(addr, buff, size);
}
uint8_t* ppcsimbooke::ppcsimbooke_memory::memory::load_buffer(uint64_t addr, uint8_t *buff, size_t size){
    return read_to_buffer(addr, buff, size);
}

// Load an elf file
void ppcsimbooke::ppcsimbooke_memory::memory::load_elf(std::string filename){
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
