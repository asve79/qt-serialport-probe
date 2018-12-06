#include <QCoreApplication>

#include <stdio.h>
#include <stdlib.h>
#include <QtDebug>
#include <QtSerialPort/QSerialPort>
#include <QDateTime>
#include <fcntl.h>   /* File Control Definitions           */
#include <termios.h> /* POSIX Terminal Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions 	   */


void sleepFor(qint64 milliseconds){
    qint64 timeToExitFunction = QDateTime::currentMSecsSinceEpoch()+milliseconds;
    while(timeToExitFunction>QDateTime::currentMSecsSinceEpoch()){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

int openport(){
    int fd;
    fd = open("/dev/tnt0",O_RDWR | O_NOCTTY | O_NDELAY);	/* ttyUSB0 is the FT232 based USB2SERIAL Converter   */
                            /* O_RDWR Read/Write access to serial port           */
                            /* O_NOCTTY - No terminal will control the process   */
                            /* O_NDELAY -Non Blocking Mode,Does not care about-  */
                            /* -the status of DCD line,Open() returns immediatly */

    if(fd == -1)						/* Error Checking */
           printf("\n  Error! in Opening ttyUSB0  ");
    else
           printf("\n  ttyUSB0 Opened Successfully ");

    return fd;
}

int initport(int fd){
    /*---------- Setting the Attributes of the serial port using termios structure --------- */

    struct termios SerialPortSettings;	/* Create the structure                          */

    tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

    cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
    cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

    SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
    SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
    SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
    SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

    SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
    SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */


    SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
    SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

    SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

    if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0){ /* Set the attributes to the termios structure*/
        printf("\n  ERROR ! in Setting attributes");
        return 1;
    }else{
        printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");
        return 0;
    }

}

void closeport(int fd){
    qDebug() << "close port";
    close(fd);/* Close the Serial port */
}

int writetoport(int fd, int cnt){
    QByteArray tmp = "test";
    tmp += QByteArray::number(cnt);
    int  bytes_written  = 0;  	/* Value for storing the number of bytes written to the port */

    bytes_written = write(fd,tmp,sizeof(tmp));/* use write() to send data to port                                            */
                                     /* "fd"                   - file descriptor pointing to the opened serial port */
                                     /*	"write_buffer"         - address of the buffer containing data	            */
                                     /* "sizeof(write_buffer)" - No of bytes to write                               */
    printf("\n  %s written to tnt0 (%d)", tmp.data(),sizeof(tmp));
    printf("\n  %d Bytes written to tnt0", bytes_written);
    printf("\n +----------------------------------+\n\n");
    return bytes_written;
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    printf("test");
    qDebug() << "test2";

    int descr;

    descr = openport();
    if (descr == -1) exit(1);

    if (initport(descr) == 1){
        closeport(descr);
        exit(1);
    }

    for(int i=0;;i++){
        writetoport(descr,i);
        sleepFor(1000);
    }


    closeport(descr);

/*
    QSerialPort serialPort;
    serialPort.setPortName("/dev/tnt0");

     const int serialPortBaudRate = QSerialPort::Baud115200;
     serialPort.setBaudRate(serialPortBaudRate);

     if (!serialPort.open(QIODevice::WriteOnly)) {
         qDebug() << QObject::tr("Failed to open port %1, error: ") + serialPort.errorString();
         return 1;
     } else {
         qDebug() << "Port opened";
     }

     int i=1;
     for (;;){

         QString msg="test " + QString::number(i++);
         QByteArray out;
         out.clear();
         out.append(msg);

         const qint64 bytesWritten = serialPort.write(out);

         qDebug() << "written " << bytesWritten << " bytes (" << msg << ")";
         sleepFor(1000);
     }
    serialPort.close();

    */


    return a.exec();
}
