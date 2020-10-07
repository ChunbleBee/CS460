// defines.h file
#define NULL (void *) 0
#define TRUE 1
#define FALSE 0

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define UART0_BASE_ADDR 0x101f1000
#define UART0_DR   (*((volatile u32 *)(UAR/* code */T0_BASE_ADDR + 0x000)))
#define UART0_FR   (*((volatile u32 *)(UART0_BASE_ADDR + 0x018)))
#define UART0_IMSC (*((volatile u32 *)(UART0_BASE_ADDR + 0x038)))

#define UART1_BASE_ADDR 0x101f2000
#define UART1_DR   (*((volatile u32 *)(UART1_BASE_ADDR + 0x000)))
#define UART1_FR   (*((volatile u32 *)(UART1_BASE_ADDR + 0x018)))
#define UART1_IMSC (*((volatile u32 *)(UART1_BASE_ADDR + 0x038)))

#define KBD_BASE_ADDR 0x10006000
#define KBD_CR (*((volatile u32 *)(KBD_BASE_ADDR + 0x000)))
#define KBD_DR (*((volatile u32 *)(KBD_BASE_ADDR + 0x008)))

#define TIMER0_BASE_ADDR 0x101E2000
#define TIMER0_LR (*((volatile u32 *)(UART0_BASE_ADDR + 0x000)))
#define TIMER0_BR (*((volatile u32 *)(UART0_BASE_ADDR + 0x032)))
#define TLOAD   0x0
#define TVALUE  0x1
#define TCNTL   0x2
#define TINTCLR 0x3
#define TRIS    0x4
#define TMIS    0x5
#define TBGLOAD 0x6

#define VIC_BASE_ADDR 0x10140000
#define VIC_STATUS    (*((volatile u32 *)(VIC_BASE_ADDR + 0x000)))
#define VIC_INTENABLE (*((volatile u32 *)(VIC_BASE_ADDR + 0x010)))
#define VIC_VADDR     (*((volatile u32 *)(VIC_BASE_ADDR + 0x030)))

#define SIC_BASE_ADDR 0x10003000
#define SIC_STATUS    (*((volatile u32 *)(SIC_BASE_ADDR + 0x000)))
#define SIC_INTENABLE (*((volatile u32 *)(SIC_BASE_ADDR + 0x008)))
#define SIC_ENSET     (*((volatile u32 *)(SIC_BASE_ADDR + 0x008)))
#define SIC_PICENSET  (*((volatile u32 *)(SIC_BASE_ADDR + 0x020)))

#define BLUE   0
#define GREEN  1
#define RED    2
#define CYAN   3
#define YELLOW 4
#define PURPLE 5
#define WHITE  6

#define  SSIZE 1024
#define NPROC 9

#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4

#define  printf  kprintf
 
typedef struct proc {
  struct proc *next;
  int    *ksp;

  int    pid;
  int    ppid;
  int    status;
  int    priority;

  struct proc *child;
  struct proc *sibling;
  struct proc *parent;
  
  int    event;
  int    exitCode;

  int    kstack[SSIZE];
} PROC;

typedef struct timer
{
    u32 *base;              // Pointer to the timer's base address as a u32 pointer
    int tick, hh, mm, ss;   // Per timer data area
    char clock[16]          // Displays the current clock of the timer
}
Timer;

typedef struct PROCTIMERNODE
{
    struct PROCTIMERNODE * next;
    PROC * process;
    int timeleft;
}
ProcTimerNode;