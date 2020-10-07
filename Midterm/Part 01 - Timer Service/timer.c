volatile Timer timers[4]; //ARM contains only 4 timers

ProcTimerNode * head;

void timer_enqueue(PROC * process, int time)
{
    ProcTimerNode * cur = head, * prev = NULL;
    if (process == NULL || time <= 0)
        return;
    
    while(cur != NULL)
    {
        prev = cur;
        cur = cur->next;
    }


    ProcTimerNode * new = (ProcTimerNode *)malloc(sizeof(ProcTimerNode));
    new->next = NULL;
    new->process = process;
    new->timeleft = time;

    new->process->status = SLEEP;

    if (prev == NULL)
    {
        head = new;
        timer_start(0);
    }
    else
    {
        prev->next = new;
    }

    ksleep(process);
}

void timer_init()
{
    int i;
    Timer *pTimer;
    for (i = 0; i < 4; i++)
    {
        pTimer = &timers[i];
        pTimer->base = (u32 *)(TIMER0_BASE_ADDR + ((i/2 > 0) ? 0x1000 : 0) - ((i%2 == 0) ? 0x20 : 0));
        *(pTimer->base+TLOAD)   = 0x0;
        *(pTimer->base+TVALUE)  = 0xFFFFFFFF;
        *(pTimer->base+TMIS)    = 0x0;
        *(pTimer->base+TLOAD)   = 0x100;
        *(pTimer->base+TCNTL)   = 0x66;
        *(pTimer->base+TBGLOAD) = 0x1C00;
        pTimer->tick = 0;
        pTimer->hh = 0;
        pTimer->mm = 0;
        pTimer->ss = 0;
        strcpy((char *)pTimer->clock, "00:00:00");
    }
}

void print_timer_queue()
{
    ProcTimerNode * cur = head;

    while(cur != NULL)
    {
        kprintf("[proc: #%d, sec: %d] -> ", cur->process->pid, cur->timeleft);
        cur = cur->next;
    }

    kprintf(" NULL\n");
}

void timer_handler(int n)
{
    int i;
    Timer * pTimer = NULL;

    if (n < 0 || n > 4)
        return;

    pTimer = &timers[n];
    pTimer->tick++;

    if (pTimer->tick == 120)
    {
        pTimer->tick = 0;
        pTimer->ss++;

        pTimer->clock[7] = '0' + (pTimer->ss%10);   // Setting Seconds ones place
        pTimer->clock[6] = '0' + (pTimer->ss/10);   // Setting seconds tens place


        if (head != NULL)
        {
            print_timer_queue();
            head->timeleft--;

            if (head->timeleft <= 0)
            {
                // Dequeue the process from the timer interrupt list.
                ProcTimerNode * interruptor = head;
                head = head->next;
                if (head == NULL) timer_stop(0);

                // Set it's priority to highest
                interruptor->process->priority = -20;

                // Wake it up
                kwakeup(interruptor->process);

                // Delete the node.
                free(interruptor);

                // tswitch()
                tswitch();
            }
        }

        if (pTimer->ss == 60)
        {
            pTimer->ss = 0;
            pTimer->mm++;

            pTimer->clock[4] = '0' + (pTimer->mm%10);   // Setting minutes ones place
            pTimer->clock[3] = '0' + (pTimer->mm/10);   // Setting minutes tens place

            if (pTimer->mm == 60)
            {
                pTimer->mm = 0;
                pTimer->hh++;

                pTimer->clock[1] = '0' + (pTimer->hh%10);   // Setting hour ones place
                pTimer->clock[0] = '0' + (pTimer->hh/10);   // Setting hour tens place
    
                if (pTimer->hh == 24)
                {
                    pTimer->hh = 0;
                }

            }
        }
    }

    timer_clearInterrupt(n);
}

void timer_start(int n)
{
    if (n < 0 || n > 4)
        return;

    Timer * pTimer = &timers[n];
    kprintf("Timer #%d starting, base = %x\n", n, pTimer->base);
    *(pTimer->base+TCNTL) |= 0x80;
}

int timer_clearInterrupt(int n)
{
    if (n < 0 || n > 4)
        return;
    
    Timer * pTimer = &timers[n];
    *(pTimer->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n)
{
    if (n < 0 || n > 4)
        return;

    Timer * pTimer = &(timers[n]);
    *(pTimer->base+TCNTL) &= 0x7F;
}

