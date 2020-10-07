int color;

#include "type.h"
#include "string.c"
// #include "queue.c"
#include "vid.c"
#include "kbd.c"
#include "exceptions.c"
#include "kernel.c"

void copy_vectors(void) {
   extern u32 vectors_start;
   extern u32 vectors_end;
   u32 *vectors_src = &vectors_start;
   u32 *vectors_dst = (u32 *)0;

   while(vectors_src < &vectors_end)
      *vectors_dst++ = *vectors_src++;
}

int kprintf(char *fmt, ...);

void IRQ_handler()
{
   int vicstatus, sicstatus;
   int ustatus, kstatus;
   int i;

   // read VIC status register to find out which interrupt
   vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
   sicstatus = SIC_STATUS;

   if (vicstatus & (1 << 4))
   {
      if (*((&timers[0])->base+TVALUE) == 0)
      {
         timer_handler(0);
      }
      if (*((&timers[1])->base+TVALUE) == 0)
      {
         timer_handler(1);
      }
   }

   if (vicstatus & (1 << 5))
   {
      if (*((&timers[2])->base+TVALUE) == 0)
      {
         timer_handler(2);
      }
      if (*((&timers[3])->base+TVALUE) == 0)
      {
         timer_handler(3);
      }
   }
   //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
   if (vicstatus & 0x80000000){
      if (sicstatus & 0x08){
         kbd_handler();
      }
   }
}

int body();

int main()
{ 
   color = WHITE;
   fbuf_init();
   kbd_init();
   
   VIC_INTENABLE |= (1 << 4);    // enable timers 1, 2
   VIC_INTENABLE |= (1 << 5);    // Enable timers 3, 4
   VIC_INTENABLE |= (1 << 31);   // SIC to VIC's IRQ31
   // enable KBD IRQ 
   SIC_ENSET = 1 << 3;  // KBD int=3 on SIC
   SIC_PICENSET = 1 << 3;  // KBD int=3 on SIC
 
   kprintf("Welcome to WANIX in Arm\n");
   timer_init();
   timer_start(0);
   init();
   kfork((int)body, 1);

   while(1){
      if (readyQueue)
         tswitch();
   }
}
