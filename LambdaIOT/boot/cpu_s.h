/*
  cpu_s.h
  Author: Straight Coder<simpleisrobust@gmail.com>
  Date: Sept 01, 2020
*/

#ifndef _CPU_S_H_
#define _CPU_S_H_

typedef  unsigned long  CPU_SR;

#define  CPU_SR_ALLOC()        CPU_SR  cpu_sr = (CPU_SR)0
#define  CPU_INT_DIS()         do { cpu_sr = CPU_SR_Save(); } while (0)
#define  CPU_INT_EN()          do { CPU_SR_Restore(cpu_sr); } while (0)

#define  CPU_CRITICAL_ENTER()  do { CPU_INT_DIS(); } while (0)
#define  CPU_CRITICAL_EXIT()   do { CPU_INT_EN();  } while (0)

void     CPU_IntDis(void);
void     CPU_IntEn(void);

void     CPU_Nop(void);

CPU_SR   CPU_SR_Save(void);
void     CPU_SR_Restore(CPU_SR cpu_sr);

#endif
