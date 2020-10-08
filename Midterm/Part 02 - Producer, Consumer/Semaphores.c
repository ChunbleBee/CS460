#define BUFFERSIZE 8
typedef struct semaphore
{
    int spinlock;
    int value;
    PROC * queue;
}
Semaphore;

char ACharBuffer[BUFFERSIZE];
char * items[64];

int head = 0;
int tail = 0;

Semaphore empty;
Semaphore full;
Semaphore pmutex;
Semaphore cmutex;

void semaphore_setup()
{
    empty.value = BUFFERSIZE;
    full.value = 0;
    pmutex.value = 1;
    cmutex.value = 1;
}

int P (Semaphore * sema)
{
    int CPSRRegValue = int_off();

    sema->value--;

    if (sema->value < 0)
    {
        block(sema);
    }

    int_on(CPSRRegValue);
}

int V (Semaphore * sema)
{
    int CPSRRegValue = int_off();

    sema->value++;

    if (sema->value <= 0)
    {
        signal(sema);
    }

    int_off(CPSRRegValue);
}

int block (Semaphore * sema)
{
    running->status = BLOCK;
    enqueue(&sema->queue, running);
    tswitch();
}

int signal (Semaphore * sema)
{
    PROC * process = dequeue(&sema->queue);
    process->status = READY;
    enqueue(&readyQueue, process);
}

int Producer()
{
    int index = 0;
    char item = '\t';

    kprintf("Type a string to output: ");
    kgets(items);
    kprintf("\n");

    kprintf(items);

    while (index < 64)
    {
        item = items[index++];
        P(&empty);
        P(&pmutex);
        ACharBuffer[head++] = item;
        head %= BUFFERSIZE;
        V(&pmutex);
        V(&full);
    }

    kexit(running);
}

int Consumer()
{
    char item;
    while (TRUE)
    {
        P(&full);
        P(&cmutex);
        item = ACharBuffer[tail++];
        kputc(item);
        tail %= BUFFERSIZE;
        V(&cmutex);
        V(&empty);
    }
}