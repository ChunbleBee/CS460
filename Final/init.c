#pragma once
#include "ucode.c"
#define CHILDPROCESS 0

typedef enum bool {false, true};
typedef int ProcID;
typedef int FileDesc;

void main(int argc, char* argv[])
{
    ProcID Console, SerialPort0, SerialPort1, Process;
    FileDesc In, Out;
    int Status;

    In = open("/dev/tty0", O_RDONLY);
    Out = open("/dev/tty0", O_WRONLY);

    Console = fork();

    if (Console == CHILDPROCESS)
    {
        exec("login /dev/tty0");
    }

    // If we're at this point, this is the parent console process //
    // because exec() should overwrite the forked child, and make it a new process //
    
    // Fork for Serial ports //
    SerialPort0 = fork();
    if (SerialPort0 == CHILDPROCESS)
    {
        exec("login /dev/ttyS0");
    }

    // As above, so below - we're still in the init() proc
    SerialPort1 = fork();
    if (SerialPort1 == CHILDPROCESS)
    {
        exec("login /dev/ttyS1");
    }

    while (true)
    {
        Process = wait(&Status);

        if (Process == Console)
        {
            Console = fork();

            if (Console == CHILDPROCESS)
            {
                exec("login /dev/tty0");
            }
        }

        if (Process == SerialPort0)
        {
            SerialPort0 = fork();
            if (SerialPort0 == CHILDPROCESS)
            {
                exec("login /dev/ttyS0");
            }
        }

        if (Process == SerialPort1)
        {
            SerialPort1 = fork();
            if (SerialPort1 == CHILDPROCESS)
            {
                exec("login /dev/ttyS1");
            }
        }
    }
}