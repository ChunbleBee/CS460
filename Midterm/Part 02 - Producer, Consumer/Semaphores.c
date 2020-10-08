typedef struct semaphore
{
    int spinlock;
    int value;
    PROC * queue;
} Semaphore;

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