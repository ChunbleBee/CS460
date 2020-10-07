                         CS460 LAB4

		  DUE and DEMO: 10-1-2020

1. BASE code: LA4pre.tgz
   wait.c files contains ksleep(), kwakeup(), kexit(), kwait() functions,
          which must be completed.

2. Implement ksleep(), kwakeup() as in 5.6.1 of textbook
   ADD two commands: sleep, wakeup to test/demo YOUR ksleep/kwakeup

3. Implement kexit() and kwait() as in the Process Management notes 
   ADD wait command for process to wait for a ZOMBIE child

4. Modify init(), kfork(), kexit(), kwait() to implement the process family tree
   as a BINARY tree:

(1). When a process runs, display its pid, ppid and childList

(2). P1 never die. When a process with children dies, all orphans become
     children of P1

1. download LAB4pre.tgz
2. run   zcat LAB4pre.tgz | tar xvf -   to extract files.
3. Run   mk    to generate t.bin and run it on ARM VM