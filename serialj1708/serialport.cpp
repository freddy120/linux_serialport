#include "serialport.h"

#include <stdlib.h>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <iostream>

#define SERIAL_DEVICE "/dev/ttyACM0"

using namespace std;


Serialport::Serialport(){}

void Serialport::begin(){
   fd = open_port(); // open serial port

   if(fd != -1){

       //create thread for reading data from serial port
       pthread_create(&_readport,NULL,&Serialport::read_thread,this);

   }


}


void* Serialport::read_thread(void *i){
    return ((Serialport *)i)->readport();
}

void* Serialport::readport(){
    if(fd < 0) return 0;

    unsigned char byte_read;
    int bytes_read;

    while(true){
        bytes_read = read(fd,&byte_read,1);
        if(bytes_read == 0){
            printf("%d\n",bytes_read);
            continue;
        }

        printf("%c",byte_read);
    }

    return 0;
}

int Serialport::open_port(void){

    int fd; /* File descriptor for the port */
    fd = open(SERIAL_DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd == -1)
    {
        /* Could not open the port.*/
        perror("open_port: Unable to open port - ");
        cout<<"can't open port"<<endl;
    }else{
        cout<<"open port success!\n"<<endl;
        fcntl(fd, F_SETFL, 0); // blocking behavior read port
        //fcntl(fd, F_SETFL, FNDELAY);// return 0 if no characters are available


        struct termios port_settings;      // structure to store the port settings in

        cfsetispeed(&port_settings, B9600);    // set baud rates
        cfsetospeed(&port_settings, B9600);

        port_settings.c_cflag &= ~PARENB;    // set no parity, stop bits, data bits
        port_settings.c_cflag &= ~CSTOPB;
        port_settings.c_cflag &= ~CSIZE;
        port_settings.c_cflag |= CS8;


        port_settings.c_iflag = 0;
        port_settings.c_oflag = 0;
        port_settings.c_lflag = 0;
        //port_settings.c_cc[VMIN] = 1;//ARRAY_SIZE  wait for a single byte
        port_settings.c_cc[VTIME] = 1;/* Timeout after 0.1 seconds */

        tcsetattr(fd, TCSANOW, &port_settings);    // apply the settings to the port

    }


    return (fd);

}


void Serialport::close_port(void){
    close(fd);
}

void Serialport::write_port(const char *msg, int len){
    int n;
    n = write(fd,msg,len);

    if (n < 0)
        cout<< "write to port failed!"<<endl;
    else
        printf("%d write bytes \n",n);

}
//void Serialport::write_port(int fd, const char* msg,int len){

//    int n;
//    n = write(fd,msg,len);

//    if (n < 0)
//        cout<< "write to port failed!"<<endl;
//    else
//        printf("%d bytes \n",n);


//}