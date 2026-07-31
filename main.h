/*
 * main.h
 *
 *  Created on: Jan 31, 2026
 *      Author: Admin
 */

#ifndef MAIN_H_
#define MAIN_H_

void task1_handler(void);// this is task1
void task2_handler(void);// this is task2
void task3_handler(void);// this is task3
void task4_handler(void);// this is task4 of application
void init_task_stack(void);
void enable_processor_faults(void);
void init_systick_timer(uint32_t Tick_hz);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void save_psp_value(uint32_t current_psp_value);
void update_next_task(void);
uint32_t get_psp_value(void);
__attribute__((naked)) void switch_sp_to_psp(void);
void SysTick_Handler(void);
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
void update_global_tick_count(void);
void idle_tasks(void);
void schedule(void);
__attribute__((naked)) void PendSV_Handler(void);
void unblock_tasks(void);

void task_delay(uint32_t tick_count);

#define SIZE_TASK_STACK   	1024U
#define SIZE_SCHED_STACK	1024U
#define SRAM_START			0x20000000U
#define SIZE_SRAM			((128) *(1024))
#define SRAM_END			(SRAM_START+SIZE_SRAM)
#define T1_STACK_START		SRAM_END
#define T2_STACK_START		(SRAM_END - (1*(SIZE_TASK_STACK)))
#define T3_STACK_START		(SRAM_END - (2*(SIZE_TASK_STACK)))
#define T4_STACK_START		(SRAM_END - (3*(SIZE_TASK_STACK)))
#define IDLE_STACK_START	(SRAM_END - (4*(SIZE_TASK_STACK)))
#define SCHED_STACK_START	(SRAM_END - (5*(SIZE_TASK_STACK)))


#define TASK_READY_STATE	0x00
#define TASK_BLOCKED_STATE	0xFF

#define INTERRUPT_DISABLE() do { __asm volatile("MOV R0,#0x01"); __asm volatile("MSR PRIMASK,R0");}while(0)
#define INTERRUPT_ENABLE() do { __asm volatile("MOV R0,#0x00"); __asm volatile("MSR PRIMASK,R0");}while(0)

#define TICK_HZ				1000U
#define HSI_CLOCK			16000000U
#define SYSTICK_TIM_CLK		HSI_CLOCK

#define MAX_TASKS 	5
#define DUMMY_XPSP  0x01000000U

#endif /* MAIN_H_ */
