ProcTimerNode nodes[NPROC];
ProcTimerNode * freed;
ProcTimerNode * head;

void timer_enqueue(PROC * process, int time)
{
    ProcTimerNode * cur = head, * prev = NULL;
    if (process == NULL || time <= 0)
        return;
    
    while(cur != NULL)
    {
        if (time - cur->timeleft > 0)
        {
            time -= cur->timeleft;
            prev = cur;
            cur = cur->next;
        }
    }


    ProcTimerNode * new = freed;
    freed = freed->next;

    new->next = cur;
    new->process = process;
    new->timeleft = time;

    if (prev == NULL)
    {
        head = new;
    }
    else
    {
        prev->next = new;
    }
    if (cur != NULL)
    {
        while (cur != NULL)
        {
            cur->timeleft -= time;
            cur = cur->next;
        }
    }

    ksleep(process);
}

void timer_dequeue(int n)
{
    // Dequeue the process from the timer interrupt list.
    ProcTimerNode * interruptor = head;
    int prevPriority            = interruptor->process->priority;

    head = head->next;

    // Set it's priority to highest
    interruptor->process->priority = __INT8_MAX__;

    // Wake it up
    kwakeup(interruptor->process);
    interruptor->process->priority = prevPriority;
    printList("Ready Queue", readyQueue);

    // free the node
    initialize_timer_node(interruptor);

    // switch processes
    tswitch();
}

void initialize_timer_node(ProcTimerNode * node)
{
    node->next = freed;
    node->process = NULL;
    node->timeleft = -1;
    freed = node;
}

void timer_init()
{
    int i;
    Timer *pTimer;

    for (i = 0; i < NPROC; i++)
    {
        initialize_timer_node(&nodes[i]);
    }

    (&timers[0])->base = (u32 *)(TIMER0_BASE_ADDR + 0x0000);
    (&timers[1])->base = (u32 *)(TIMER0_BASE_ADDR + 0x0020);
    (&timers[2])->base = (u32 *)(TIMER0_BASE_ADDR + 0x1000);
    (&timers[3])->base = (u32 *)(TIMER0_BASE_ADDR + 0x1020);

    for (i = 0; i < 4; i++)
    {
        pTimer = &timers[i];
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
    }
}

void print_timer_queue()
{
    ProcTimerNode * cur = head;
    kprintf("\n");
    while(cur != NULL)
    {
        kprintf("[proc: #%d, sec: %d] -> ", cur->process->pid, cur->timeleft);
        cur = cur->next;
    }

    kprintf(" NULL\n");
}

void timer_handler(int n)
{
    timers[n].tick++;

    if (timers[n].tick == 60)
    {
        timers[n].tick = 0;

        if (head != NULL)
        {
            head->timeleft--;
            print_timer_queue();

            if (head->timeleft <= 0)
            {
                int CPSRRegValue = int_off();
                timer_dequeue(n);
                int_on(CPSRRegValue);
            }
        }
    }

    timer_clearInterrupt(n);
}

void timer_start(int n)
{
    if (n < 0 || n >= 4)
        return;

    Timer * pTimer = &timers[n];
    kprintf("Timer #%d starting, base = %x\n", n, pTimer->base);
    *(pTimer->base+TCNTL) |= 0x80;
}

int timer_clearInterrupt(int n)
{
    if (n < 0 || n >= 4)
        return;
    
    Timer * pTimer = &timers[n];
    *(pTimer->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n)
{
    if (n < 0 || n >= 4)
        return;

    Timer * pTimer = &(timers[n]);
    *(pTimer->base+TCNTL) &= 0x7F;
}

