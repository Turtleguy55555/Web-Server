typedef struct slot {
    long id;
    int total;
    int statics;
    int dynamic;
} slot_t;
slot_t* shm_ptr;
int slot_size;
