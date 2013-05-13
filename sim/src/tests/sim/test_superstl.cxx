#include <iostream>
#include "superstl.h"

// chunk list class type0
struct chunk_list_class_type0 {
    int v;

    chunk_list_class_type0() {}
    chunk_list_class_type0(int v_): v(v_) {}
};

inline std::ostream& operator<<(std::ostream& ostr, chunk_list_class_type0& var){
    ostr << " [v=" << var.v << "]" << " ";
    return ostr;
}

// self hash class type0
struct self_hash_class_type0 {
    int                     key;
    superstl::selflistlink  hashlink;
    int                     v;

    self_hash_class_type0(int key_, int v_): key(key_), v(v_) {}
};

inline std::ostream& operator<<(std::ostream& ostr, self_hash_class_type0& var){
    ostr << " " << "[key=" << var.key << "," << "value=" << var.v << "]" << " ";
    return ostr;
}

// self hash class type0 link manager
struct self_hash_class_type0_link_manager {
    static inline self_hash_class_type0* objof(superstl::selflistlink* link) {
        return superstl_baseof(self_hash_class_type0, hashlink, link);
    }

    static inline int& keyof(self_hash_class_type0* obj) {
        return obj->key;
    }

    static inline superstl::selflistlink* linkof(self_hash_class_type0* obj) {
        return &obj->hashlink;
    }
};

int main(){
    std::cout << "SUPERSTL test." << std::endl;

    typedef superstl::ChunkList<chunk_list_class_type0, 3>  cl;
    cl v0;

    chunk_list_class_type0 clct0_0 = chunk_list_class_type0(6);
    chunk_list_class_type0 clct0_1 = chunk_list_class_type0(12);
    chunk_list_class_type0 clct0_2 = chunk_list_class_type0(3);
    chunk_list_class_type0 clct0_3 = chunk_list_class_type0(43);

    cl::Locator loc0, loc1, loc2, loc3;

    v0.add(clct0_0, loc0);
    v0.add(clct0_1, loc1);
    v0.add(clct0_2, loc2);
    v0.add(clct0_3, loc3);

    std::cout << "Indexes for the added chunks are " << loc0.index << "," << loc1.index << "," << loc2.index << " & " << loc3.index << "." << std::endl;

    cl::Iterator iter1(&v0);
    chunk_list_class_type0*  clct0_ptr0;

    // printing all chunks
    std::cout << "All chunks in chunk list..." << std::endl;
    while((clct0_ptr0 = iter1.next())){
        std::cout << *clct0_ptr0 << std::endl;
    }

    typedef superstl::SelfHashtable<int, self_hash_class_type0, 32*1024, self_hash_class_type0_link_manager> sht;
    sht sht_v0;

    self_hash_class_type0 *shct0_0 = new self_hash_class_type0(4, 7);
    self_hash_class_type0 *shct0_1 = new self_hash_class_type0(1, 9);
    self_hash_class_type0 *shct0_2 = new self_hash_class_type0(7, 3);

    // Add all data
    sht_v0.add(shct0_0);
    sht_v0.add(shct0_1);
    sht_v0.add(shct0_2);

    // Define temporaries
    self_hash_class_type0* sht_ptr0;
    sht::Iterator iter0(sht_v0);

    // print all nodes
    std::cout << "Printing all nodes..." << std::endl;
    while((sht_ptr0 = iter0.next())){
        std::cout << *sht_ptr0 << std::endl;
    }

    // Remove one node
    std::cout << "Removing node " << shct0_0 << "...." << std::endl;
    sht_v0.remove(shct0_0);

    std::cout << "Printing all nodes again..." << std::endl;
    iter0.reset(sht_v0);         // reset iterator
    while((sht_ptr0 = iter0.next())){
        std::cout << *sht_ptr0 << std::endl;
    }

    // purging all nodes
    sht_v0.clear_and_free();

    return 0;
}
