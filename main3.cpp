#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>   // using the termios.h library
#include<cstring>
#include <thread>
#include <iostream>

int fd;
struct termios options;       // the termios structure is vital

void writeTask() {   
   int counterw = 1;

   while (true) {
      usleep(1000000);
      write(fd, &counterw, sizeof(counterw));
      counterw++;
   }
}

void blockReadTask() {

   int counterr = 0;
   while (true) {
      int rec = read(fd, &counterr, sizeof(int));
      std::cout << "UART: Received <" << counterr << ">\n";
   }
}

int main(){
   // Remove O_NDELAY to *wait* on serial read (blocking read)
   if ((fd= open("/dev/ttyS0", O_RDWR | O_NOCTTY))<0){
      perror("UART: Failed to open the file.\n");
      return -1;
   }

   tcgetattr(fd, &options);    // sets the parameters for the file
   cfmakeraw(&options);

   // Set up the communication options:
   // 115200 baud, 8-N-1, enable receiver, no modem control lines
   options.c_cflag = B57600 | CS8 | CREAD | CLOCAL;
   options.c_cc[VMIN] = sizeof(int);
   options.c_cc[VTIME] = 0;
   // options.c_lflag = 0;

   tcflush(fd, TCIFLUSH);            // discard file information
   tcsetattr(fd, TCSANOW, &options);

   std::thread th2 (blockReadTask);
   std::thread th1 (writeTask);

   th2.join();
   th1.join();

   close(fd);
   return 0;
}