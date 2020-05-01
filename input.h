#ifndef input_h
#define input_h
#include <linux/input.h>



typedef struct in_packet{
    struct input_event ev;
    unsigned char push_sw_buff[9];
};

int pp (int semid);

int vv(int semid);

void entry_input();

#endif /* Header_h */
