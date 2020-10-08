#define BUFFERSIZE 8
typedef struct semaphore
{
    int spinlock;
    int value;
    PROC * queue;
}
Semaphore;

char buffer[BUFFERSIZE];
char * items = "woodchipper";

int index = 0;
int head = 0;
int tail = 0;

Semaphore empty;
Semaphore full;
Semaphore pmutex;
Semaphore cmutex;

void setup()
{
    empty
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
    while(TRUE)
    {
        P(&empty);
        P(&pmutex);
        buffer[head++] = 'c';
        head %= BUFFERSIZE;
        V(&pmutex);
        V(&full);
    }
}

int Consumer()
{
    char item = items[index];
    index = (++index) % 12;

    P(&full);
    P(&cmutex);
    item = buffer[tail++];
    tail %= BUFFERSIZE;
}