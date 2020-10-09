#define NPIPE 4
#define PSIZE 8

typedef enum PIPESTATUS
{
    FREED,
    BUSY
}
PipeStatus;

typedef struct pipe
{
    char buf[PSIZE];
    int head, tail, data, room;
    PipeStatus status;
}
PIPE;

PIPE pipe[NPIPE];

extern UART uart[4];

int pipe_init()
{
    int i;
    printf("pipe_init()\n");

    for (i = 0; i < NPIPE; i++)
    {
        pipe[i].status = FREED;
    }
}

PIPE *create_pipe()
{
    int i; PIPE *newPipe;
    printf("creating pipe ");
    for (i = 0; i < NPIPE; i++)
    {
        newPipe = &pipe[i];

        if (newPipe->status == FREED)
        {
            newPipe->status = BUSY;
            break;
        }
    }

    newPipe->head = 0;
    newPipe->tail = 0;
    newPipe->data = 0;
    newPipe->room = PSIZE;
    printf("OK\n");

    return newPipe;
}

int read_pipe(PIPE *pPipe, char *buf, int bytesToRead)
{
    int i;
    int exit = TRUE;
    int bytesRead = 0;

    if (bytesToRead == 0)
    {
        return 0;
    }

    for (i = 0; i < NPIPE; i++)
    {
        if (pipe[i].status == BUSY)
        {
            exit = FALSE;
            i = NPIPE;
        }
    }

    if (exit == TRUE || pPipe->status == FREED)
    {
        return -1;
    }

    while (bytesToRead > 0)
    {
        while(pPipe->data)
        {
            *buf = pPipe->buf[pPipe->tail++];
            pPipe->tail %= PSIZE;
            pPipe->data--; pPipe->room++; buf++; bytesRead++; bytesToRead--;

            if (bytesToRead == 0)
            {
                break;
            }
        }

        kwakeup((int)&pPipe->room);

        if (bytesRead)
        {
            return bytesRead;
        }

        ksleep((int)&pPipe->data);
    }
}

int write_pipe(PIPE *pPipe, char *buf, int bytesToWrite)
{
    int i = 0;
    int exit = TRUE;
    int bytesWritten = 0;

    for (i = 0; i < NPIPE; i++)
    {
        if (pipe[i].status == BUSY)
        {
            exit = FALSE;
            i = NPIPE;
        }
    }

    if (exit == TRUE || pPipe->status == FREED)
    {
        return -1;
    }

    if (bytesToWrite <= 0)
    {
        return 0;
    }

    while (bytesToWrite)
    {
        while(pPipe->room)
        {
            pPipe->buf[pPipe->head++] = *buf;
            pPipe->head %= PSIZE;
            pPipe->data++;
            pPipe->room--;
            buf++;
            bytesWritten++;
            bytesToWrite--;
            
            if (bytesToWrite == 0)
            {
                break;
            }
        }
        printf("proc%d wrote %d bytes\n", running->pid, bytesWritten);
        kwakeup((int)&pPipe->data);

        if (bytesToWrite == 0)
        {
            printf("proc%d finished writing %d bytes\n", running->pid, bytesWritten);
            return bytesWritten;
        }

        ksleep((int)&pPipe->room);
    }
}