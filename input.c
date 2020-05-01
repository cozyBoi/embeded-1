#include "output.h"
#include "main.h"
#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/shm.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

int pp (int semid) {
    // p 연산
    struct sembuf p_buf;
    
    p_buf.sem_num = 0;
    p_buf.sem_op = -1;
    p_buf.sem_flg = SEM_UNDO;
    
    if (semop (semid, &p_buf, 1) == -1) exit (1);
    return (0);
}
int vv(int semid) {
    
    // v 연산
    struct sembuf v_buf;
    
    v_buf.sem_num = 0;
    v_buf.sem_op = 1;
    v_buf.sem_flg = SEM_UNDO;
    
    if (semop (semid, &v_buf, 1) == -1) exit (1);
    return (0);
}


typedef struct in_packet{
    struct input_event ev[BUFF_SIZE];
    unsigned char push_sw_buff[9];
};
void entry_input(){
    printf("init input\n");
    struct input_event ev[BUFF_SIZE];
    int fd, rd, value, size = sizeof(struct input_event);
    char* device = "/dev/input/event0";
    int dev, buff_size;
    unsigned char push_sw_buff[MAX_BUTTON];
    struct in_packet in_pac;
    
    if ((fd = open(device, O_RDONLY | O_NONBLOCK)) == -1) {
        printf("%s is not a vaild device.n", device);
    }
    memset(&in_pac, 0, sizeof(struct in_packet));
    dev = open("/dev/fpga_push_switch", O_RDWR | O_NONBLOCK);
    printf("dev : %d\n", dev);
    if (dev<0) {
        printf("Device Open Error\n");
        close(dev);
        return ;
    }
    buff_size = sizeof(push_sw_buff);
    int i;
    key_t key = ftok("./", 1);
    int shmid = shmget(key, sizeof(struct in_packet), IPC_CREAT|0644);
    struct in_packet* shmaddr = (struct in_packet*)shmat(shmid, NULL, 0);
    memset(shmaddr, 0, sizeof(struct packet));
    //shm open
    
    while (1) {
        printf("input start\n");
        int rrdd = 0;
        
        rd = read(fd, ev, size * BUFF_SIZE);
        rrdd = read(dev, &push_sw_buff, buff_size);
        //printf("input read\n");
        shmaddr->ev[0].type = ev[0].type;
        shmaddr->ev[0].value = ev[0].value;
        shmaddr->ev[0].code = ev[0].code;
        
        strcpy(shmaddr->push_sw_buff, push_sw_buff);
        
        printf("[in] shm value's %d %d %d\n",shmaddr->ev[0].type,shmaddr->ev[0].value,shmaddr->ev[0].code);
        int i3 = 0;
        for(i3 = 0; i3 < 9; i3++){
            printf("%d ", shmaddr->push_sw_buff[i3]);
        }
        printf("\n");
        
        printf("[in] value's %d %d %d\n",ev[0].type,ev[0].type,ev[0].type);
        
        for(i3 = 0; i3 < 9; i3++){
            printf("%d ", push_sw_buff[i3]);
        }
        usleep(1000000);
        
        usleep(250000);
    }
    close(dev);
}
