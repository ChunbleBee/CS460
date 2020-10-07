extern Timer;
volatile Timer timers[4]; //ARM contains only 4 timers

void timer_init()
{
    int i;
    Timer *pTimer;
    for (i = 0; i < 4; i++)
    {
        pTimer = &timer[i];
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

        if (pTimer->ss == 60)
        {
            pTimer->ss = 0;
            pTimer->mm++;

            if (pTimer->mm == 60)
            {
                pTimer->mm = 0;
                pTimer->hh++;
    
                if (pTimer->hh == 24)
                {
                    pTimer->hh = 0;
                }
            }
        }
    }
    pTimer->clock[7] = '0' + (pTimer->ss%10);   // Setting Seconds ones place
    pTimer->clock[6] = '0' + (pTimer->ss/10);   // Setting seconds tens place
    pTimer->clock[4] = '0' + (pTimer->mm%10);   // Setting minutes ones place
    pTimer->clock[3] = '0' + (pTimer->mm/10);   // Setting minutes tens place
    pTimer->clock[1] = '0' + (pTimer->hh%10);   // Setting hour ones place
    pTimer->clock[0] = '0' + (pTimer->hh/10);   // Setting hour tens place

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
    
    Timer * pTimer = &timer[n];
    *(pTimer->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n)
{
    if (n < 0 || n > 4)
        return;

    Timer pTimer = &timers[n];
    *(pTimer->base+TCNTL) &= 0x7F;
}

