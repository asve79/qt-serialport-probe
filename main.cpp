#include <QCoreApplication>

#include <stdio.h>
#include <QtDebug>
#include <QtSerialPort/QSerialPort>
#include <QDateTime>

void sleepFor(qint64 milliseconds){
    qint64 timeToExitFunction = QDateTime::currentMSecsSinceEpoch()+milliseconds;
    while(timeToExitFunction>QDateTime::currentMSecsSinceEpoch()){
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    printf("test");
    qDebug() << "test2";

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
    return a.exec();
}
