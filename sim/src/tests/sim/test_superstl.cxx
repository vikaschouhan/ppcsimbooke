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

struct self_hash_class_type0_key {
    int x;
    int y;

    self_hash_class_type0_key(): x(0), y(0) {}
    self_hash_class_type0_key(int x_, int y_): x(x_), y(y_) {}
    bool operator==(const self_hash_class_type0_key& key) const {
        return (x == key.x) && (y == key.y);
    }
};

inline std::ostream& operator<<(std::ostream& ostr, const self_hash_class_type0_key& key){
    ostr << "[x=" << key.x << ",y=" << key.y << "]";
    return ostr;
}

// self hash class type0
struct self_hash_class_type0 {
    self_hash_class_type0_key    key;
    superstl::selflistlink       hashlink;
    int                          v;

    self_hash_class_type0(self_hash_class_type0_key key_, int v_): key(key_), v(v_) {}
};

inline std::ostream& operator<<(std::ostream& ostr, const self_hash_class_type0& var){
    ostr << " " << "[key=" << var.key << "," << "value=" << var.v << "]" << " ";
    return ostr;
}

// self hash class type0 link manager
struct self_hash_class_type0_link_manager {
    static inline self_hash_class_type0* objof(superstl::selflistlink* link) {
        return superstl_baseof(self_hash_class_type0, hashlink, link);
    }

    static inline self_hash_class_type0_key& keyof(self_hash_class_type0* obj) {
        return obj->key;
    }

    static inline superstl::selflistlink* linkof(self_hash_class_type0* obj) {
        return &obj->hashlink;
    }
};

struct self_hash_class_type0_key_manager {
    static inline int hash(const self_hash_class_type0_key& key){
        superstl::CRC32 h;
        h << key;
        return h;
    }
    static inline bool equal(const self_hash_class_type0_key& a, const self_hash_class_type0_key& b){
        return a == b;
    }
    static inline self_hash_class_type0_key dup(const self_hash_class_type0_key& key){
        return key;
    }
    static inline void free(self_hash_class_type0_key& key) {}
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

    typedef superstl::SelfHashtable<self_hash_class_type0_key, self_hash_class_type0, 32*1024,
            self_hash_class_type0_link_manager, self_hash_class_type0_key_manager> sht;
    sht sht_v0;

    self_hash_class_type0 *shct0_0 = new self_hash_class_type0(self_hash_class_type0_key(2, 8), 7);
    self_hash_class_type0 *shct0_1 = new self_hash_class_type0(self_hash_class_type0_key(1, 7), 9);
    self_hash_class_type0 *shct0_2 = new self_hash_class_type0(self_hash_class_type0_key(8, 4), 3);
    self_hash_class_type0 *tmp_shct0 = NULL;

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

    // searching for node
    std::cout << "Searching for node " << self_hash_class_type0_key(8, 4) << std::endl;
    tmp_shct0 = sht_v0.get(self_hash_class_type0_key(8, 4));
    std::cout << "Got " << tmp_shct0 << std::endl;
    if(tmp_shct0 != NULL) std::cout << "with data = " << *tmp_shct0 << std::endl;

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
