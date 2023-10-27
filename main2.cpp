#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<termios.h>   // using the termios.h library
#include<cstring>

int main(){
   int fd, count;

   // Remove O_NDELAY to *wait* on serial read (blocking read)
   if ((fd= open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY))<0){
      perror("UART: Failed to open the file.\n");
      return -1;
   }

   struct termios options;       // the termios structure is vital
   tcgetattr(fd, &options);    // sets the parameters for the file

   // Set up the communication options:
   // 115200 baud, 8-N-1, enable receiver, no modem control lines
   options.c_cflag = B57600 | CS8 | CREAD | CLOCAL;
   options.c_iflag = IGNPAR | ICRNL; // ignore partity errors, convert '\r' to '\n'
   options.c_lflag = 0;

   tcflush(fd, TCIFLUSH);            // discard file information
   tcsetattr(fd, TCSANOW, &options); // changes occur immmediately


    while (true) {
       char transmit[100] = "Hello Raspberry Pi!\n";  // send string
       char receive[100]; //declare a buffer for receiving data
       memset(receive, '\0', sizeof(receive)); // reset receive buffer each run

        fgets(transmit, sizeof(transmit), stdin);

       if ((count = write(fd, transmit, strlen(transmit)))<0){         // transmit
          perror("Failed to write to the output\n");
          return -1;
       }

       usleep(100000);             // give the remote machine a chance to respond


       if ((count = read(fd, (void*)receive, sizeof(receive)))<0){   //receive data
          perror("Failed to read from the input\n");
          return -1;
       }

       if (count==0) printf("There was no data available to read!\n");
       else printf("The following was read in [%d]: %s\n",count,receive);

        if ((receive[0] == 'q') && (count == 2)) {
            printf("Quitting\n");
            close(fd);
            return 0;
        }

    }
    close(fd);
    return 0;
}