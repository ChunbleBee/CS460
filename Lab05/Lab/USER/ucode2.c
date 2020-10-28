int umenu()
{
  uprintf("-------------------------------------------------------------\n");
  uprintf("getpid getppid ps chname switch sleep wakeup kfork exit  wait\n");
  uprintf("-------------------------------------------------------------\n");
}

int getpid()
{
    int pid;
    pid = syscall(0,0,0,0);
    return pid;
}    

int getppid()
{ 
    return syscall(1,0,0,0);
}

int ugetpid()
{
    int pid = getpid();
    uprintf("pid = %d\n", pid);
}

int ugetppid()
{
    int ppid = getppid();
    uprintf("ppid = %d\n", ppid);
}

int ups()
{
    return syscall(2,0,0,0);
}

int uchname()
{
    char s[32];
    uprintf("input a name string : ");
    ugets(s);
    printf("\n");
    return syscall(3,s,0,0);
}

int uswitch()
{
    return syscall(4,0,0,0);
}

int ugetc()
{
    return syscall(90,0,0,0);
}

int uputc(char c)
{
    if (c >= 'a' && c <= 'z')
    {
        c -= 'a';
        c += 'A';
    }

    return syscall(91,c,0,0);
}

int getPA()
{
    return syscall(92,0,0,0);
}

int usleep()
{
    int pid = getpid();

    if (pid==1){
        printf("P1 does not sleep in Umode\n");
        return -1;
    }

    printf("proc %d go to sleep in kernel\n", pid);
    return syscall(5, pid, 0, 0);
}

int uwakeup()
{
    int pid;
    printf("enter a pid to wakeup: ");
    pid = geti();
    printf("pid=%d\n", pid);
    return syscall(6,pid,0,0);
}

int ukfork()
{
    return syscall(7, "u1", 0, 0);
}

int uexit()
{
    int value;
    printf("enter an exit value : ");
    value = geti();
    syscall(8, value, 0, 0);
}

int uwait()
{
    int pid, status;
    pid = syscall(9, &status, 0, 0);
    printf("pid = %d ", pid);
    if (pid > 0)
        printf("status = %x",status);
    printf("\n");
}
