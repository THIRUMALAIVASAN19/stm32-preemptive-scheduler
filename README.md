STM32F407 Bare-Metal Preemptive Scheduler

A preemptive task scheduler built from scratch for the STM32F407 Cortex-M4, using bare-metal C, memory-mapped registers, and inline ARM assembly.

This project implements the fundamental mechanisms behind a preemptive RTOS without using FreeRTOS or another RTOS scheduler.

STM32F407
    ↓
Bare-Metal Firmware
    ↓
SysTick
    ↓
Task Scheduler
    ↓
Priority Scheduling
    ↓
PendSV
    ↓
Context Switching
    ↓
PSP / TCB
    ↓
4 Periodic Tasks + Idle Task

---

🎯 Project Objective

The objective of this project is to understand and implement a preemptive scheduler directly on the ARM Cortex-M4 processor.

Instead of relying on an existing RTOS, the scheduler is implemented using:

- Cortex-M4 CPU registers
- Memory-mapped peripheral registers
- SysTick
- PendSV
- MSP and PSP
- Task Control Blocks
- Software-managed task stacks
- Inline ARM assembly
- Critical sections
- Cortex-M fault handling
- Priority-based scheduling
- Round-robin scheduling
- READY / RUNNING / BLOCKED task states
- Idle task

The project demonstrates what happens underneath a traditional RTOS task-switching API.

---

⭐ Features

- Bare-metal STM32F407 firmware
- No FreeRTOS
- No RTOS scheduler
- Register-level programming
- Inline ARM assembly
- Preemptive scheduling
- SysTick scheduler tick
- PendSV context switching
- PSP-based task execution
- MSP for exceptions and scheduler/ISR execution
- Task Control Blocks (TCBs)
- Independent task stacks
- Priority scheduling
- Round-robin scheduling
- Four periodic application tasks
- READY / RUNNING / BLOCKED states
- Idle Task
- Task block/unblock mechanism
- Critical sections
- Cortex-M4 fault handling
- SWD debugging
- ST-LINK
- OpenOCD
- ARM GDB
- UART debugging
- PuTTY runtime monitoring

---

🧩 Hardware

MCU

STM32F407 — ARM Cortex-M4

The scheduler uses the Cortex-M4 exception and stack mechanisms directly.

Required hardware

- STM32F407 development board
- ST-LINK debugger/programmer
- USB cable
- PC

Optional

- USB-UART interface for serial logging
- PuTTY-compatible serial terminal

---

🛠 Software / Tools

Tool| Purpose
ARM GNU Toolchain| Compile and link firmware
"arm-none-eabi-gcc"| C compiler
"arm-none-eabi-gdb"| Debugger
OpenOCD| Debug server / flashing
ST-LINK| SWD interface
PuTTY| UART runtime monitoring
Make| Build automation

---

🏗️ System Architecture

                         STM32F407
                        Cortex-M4
                            │
                            ▼
                  Register-Level Firmware
                            │
              ┌─────────────┴─────────────┐
              │                           │
       Memory-Mapped I/O             CPU Registers
              │                           │
        GPIO / UART               MSP / PSP / CONTROL
                                          │
                                          ▼
                                      SysTick
                                          │
                                          ▼
                                  Task Scheduler
                                          │
                             ┌────────────┴────────────┐
                             │                         │
                       Task State                Task Selection
                             │                         │
                     READY/BLOCKED            Priority/RR
                             │                         │
                             └────────────┬────────────┘
                                          │
                                          ▼
                                       PendSV
                                          │
                                          ▼
                                Context Switching
                                          │
                                          ▼
                                      Next Task

---

🔄 Scheduler Flow

                 SysTick
                    │
                    ▼
             Scheduler Tick
                    │
                    ▼
          Update task timing
                    │
                    ▼
        Check BLOCKED tasks
                    │
                    ▼
       Unblock expired tasks
                    │
                    ▼
          Find READY tasks
                    │
                    ▼
        Select highest priority
                    │
                    ▼
       Round-robin if required
                    │
                    ▼
             Request PendSV
                    │
                    ▼
          Context Switching
                    │
                    ▼
             Next Task

---

🧵 Task Model

The scheduler manages multiple application tasks.

                 Scheduler
                     │
       ┌─────────────┼─────────────┐-------------|
       │             │             │             |
      T1             T2            T3            T4
       │             │             │             │
    Stack 1       Stack 2       Stack 3       Stack 4
       │             │             │             │
       └─────────────┴─────────────┴─────────────┘
                         │
                         ▼
                        TCB

---

📋 Task Table

Task| Period| State| Stack| Description
Task 1| 125 ms| READY/RUNNING/BLOCKED| Private| Periodic task
Task 2| 250 ms| READY/RUNNING/BLOCKED| Private| Periodic task
Task 3| 500 ms| READY/RUNNING/BLOCKED| Private| Periodic task
Task 4| 1000 ms| READY/RUNNING/BLOCKED| Private| Periodic task
Idle Task| Always| READY| Private| Runs when no application task is ready

«Keep the exact priority values synchronized with the implementation in the source code.»

---

🔁 Task States

Every task has an explicit scheduling state.

                  ┌──────────────┐
                  │    READY     │
                  └──────┬───────┘
                         │
                  Scheduler selects
                         │
                         ▼
                  ┌──────────────┐
                  │   RUNNING    │
                  └──────┬───────┘
                         │
                ┌────────┴────────┐
                │                 │
            Preemption       Period complete
                │                 │
                ▼                 ▼
             READY           ┌──────────────┐
                             │   BLOCKED    │
                             └──────┬───────┘
                                    │
                              Period expires
                                    │
                                    ▼
                                  READY

READY

A READY task is eligible to run.

It can be selected by the scheduler.

RUNNING

The RUNNING task currently owns the CPU.

Because the STM32F407 is single-core, only one application task executes at a time.

BLOCKED

A BLOCKED task is temporarily removed from normal scheduling.

For periodic tasks, the task can become BLOCKED while waiting for its next activation period.

---

🔓 Block / Unblock

A periodic task can transition:

RUNNING
   │
   ▼
Period complete
   │
   ▼
BLOCKED
   │
   │ SysTick updates timing
   │
   ▼
Period expires
   │
   ▼
READY

The scheduler periodically checks BLOCKED tasks.

SysTick
   │
   ▼
Update timing
   │
   ▼
Check BLOCKED tasks
   │
   ├── Still waiting
   │
   └── Expired
         │
         ▼
       READY

Only READY tasks participate in normal task selection.

---

💤 Idle Task

The scheduler includes an Idle Task.

Its purpose is to guarantee that there is always a valid task context when all application tasks are blocked.

Task 1 ── BLOCKED
Task 2 ── BLOCKED
Task 3 ── BLOCKED
Task 4 ── BLOCKED
              │
              ▼
          Scheduler
              │
              ▼
          Idle Task

The Idle Task has the lowest scheduling priority.

Highest Priority
       │
       ▼
     Task 1
     Task 2
     Task 3
     Task 4
       │
       ▼
   Idle Task
       │
       ▼
Lowest Priority

The Idle Task may perform background processing or use "WFI" for low-power operation if implemented.

---

🎯 Priority + Round-Robin Scheduling

The scheduler uses priority-based selection.

READY Tasks
     │
     ▼
Highest Priority
     │
     ▼
Multiple tasks at same priority?
     │
   ┌─┴─┐
  Yes  No
   │    │
   ▼    ▼
Round  Select
Robin   Task
   │
   ▼
Next Task

The scheduler first considers task priority.

If multiple READY tasks have the same scheduling priority, round-robin selection can be used to share processor time between them.

---

⏱️ SysTick

SysTick provides the periodic scheduler timing source.

                SysTick
                   │
                   ▼
             Scheduler Tick
                   │
          ┌────────┼─────────┐
          │        │         │
       Update    Check     Update
       timing   blocked    state
                   │
                   ▼
              Task Ready?
                   │
                   ▼
              Scheduler
                   │
                   ▼
                 PendSV

SysTick is responsible for time-related scheduler operations.

The actual context switch is delegated to PendSV.

---

⚡ PendSV

PendSV is used for deferred context switching.

SysTick
   │
   ▼
Scheduler decision
   │
   ▼
PendSV requested
   │
   ▼
PendSV_Handler
   │
   ▼
Save current context
   │
   ▼
Select next task
   │
   ▼
Restore next context
   │
   ▼
Return to task

This separates scheduler timing from the actual register save/restore operation.

---

🧠 MSP vs PSP

The Cortex-M4 has two stack pointers:

                     Cortex-M4
                         │
              ┌──────────┴──────────┐
              │                     │
             MSP                   PSP
              │                     │
              ▼                     ▼
      Scheduler / ISR          Application Tasks
                                      │               
                         ┌────────────┼────────────┐____________
                         │            │            │            |
                        T1           T2           T3           T4
                         │            │            │            │
                       Stack        Stack        Stack        Stack

MSP

The Main Stack Pointer is used for:

- Startup
- Exceptions
- Interrupt handlers
- Scheduler/ISR execution

PSP

The Process Stack Pointer is used for:

- Application tasks
- Task stacks
- Task context

The scheduler stores and restores task PSP values through the TCB.

---

🧱 Task Control Block

A Task Control Block stores the scheduler state of an individual task.

Conceptually:

TCB
│
├── PSP
├── Priority
├── State
├── Period / Timing
└── Task information

The PSP is especially important because it points to the saved task context.

Task A
  │
  ▼
PSP
  │
  ▼
Task Stack
  │
  ▼
Saved Context

---

🔀 Context Switching

The context switch is implemented using inline ARM assembly.

Current Task
     │
     ▼
MRS R0, PSP
     │
     ▼
Save R4-R11
     │
     ▼
Save PSP → Current TCB
     │
     ▼
Select Next Task
     │
     ▼
Get Next Task PSP
     │
     ▼
Restore R4-R11
     │
     ▼
MSR PSP, R0
     │
     ▼
Exception Return
     │
     ▼
Next Task Running

Key instructions

Instruction| Purpose
"MRS"| Read special register
"MSR"| Write special register
"STMDB"| Store multiple registers
"LDMIA"| Load multiple registers
"R4-R11"| Software-saved context
"PSP"| Task stack pointer

---

🧮 Context Frame

During exception handling, Cortex-M hardware automatically stacks part of the CPU context.

The scheduler additionally manages the software-saved registers.

Hardware-managed frame
----------------------
R0
R1
R2
R3
R12
LR
PC
xPSR

Software-managed frame
----------------------
R4
R5
R6
R7
R8
R9
R10
R11

This division is fundamental to understanding Cortex-M context switching.

---

💾 Memory / Stack Architecture

                  STM32F407 RAM
                       │
          ┌────────────┴────────────┐
          │                         │
       Scheduler                 Tasks
          │                         │
        TCBs              ┌─────────┼─────────┐_________
                          │         │         │         |
                       Task 1    Task 2    Task 3    Task 4
                          │         │         │         │
                        Stack     Stack     Stack     Stack

Each task has an independent stack.

The TCB contains the information required to resume the task.

---

🔒 Critical Sections

Scheduler data must be protected when being modified by code that can be interrupted.

Enter Critical Section
          │
          ▼
Modify Scheduler State
          │
          ▼
Update TCB / Task State
          │
          ▼
Exit Critical Section

Critical sections prevent inconsistent scheduler state and race conditions during sensitive operations.

---

💥 Cortex-M4 Fault Handling

The project includes Cortex-M fault handling for debugging unexpected processor exceptions.

Relevant fault classes include:

HardFault
MemManage
BusFault
UsageFault

When debugging a fault, inspect:

PC
LR
SP
xPSR
MSP
PSP

The PC can be used to locate the instruction where the fault occurred.

---

🐛 Bare-Metal Debugging

The complete debugging chain is:

                         STM32F407
                             │
              ┌──────────────┴──────────────┐
              │                             │
             SWD                           UART
              │                             │
           ST-LINK                      USB-UART/VCP
              │                             │
              ▼                             ▼
           OpenOCD                        PuTTY
              │
              ▼
             GDB
              │
              ▼
       Register-Level Debug

---

🔌 OpenOCD

Connect the STM32F407 through ST-LINK.

Example:

openocd \
    -f interface/stlink.cfg \
    -f target/stm32f4x.cfg

OpenOCD provides the GDB server interface.

---

🧰 GDB

Start:

arm-none-eabi-gdb build/stm32-preemptive-scheduler.elf

Connect:

target extended-remote localhost:3333

Reset:

monitor reset halt

Program:

load

Continue:

continue

---

🔎 Register Debugging

Display registers:

info registers

Important registers:

R0-R12
SP
LR
PC
xPSR

Scheduler-specific information:

MSP
PSP
CONTROL
TCB
Task stack

Breakpoints:

break SysTick_Handler
break PendSV_Handler
break HardFault_Handler

---

🧵 PSP Debugging

Inspect the Process Stack Pointer:

p/x $psp

Inspect task stack memory:

x/32wx $psp

This allows direct inspection of the context saved on the task stack.

---

🖥️ PuTTY UART Debugging

PuTTY can be used to observe scheduler execution at runtime.

Recommended configuration:

Connection type : Serial
Baud rate       : 115200
Data bits       : 8
Stop bits       : 1
Parity          : None
Flow control    : None

Example output:

========================================
STM32F407 PREEMPTIVE SCHEDULER
========================================

Scheduler started

Task 1 running
Task 2 running
Task 3 running
Task 4 running

Context Switch
Current Task : 1
Next Task    : 2

Context Switch
Current Task : 2
Next Task    : 3

The actual output depends on the UART logging implemented in the firmware.

---

🔨 Build Instructions

Requirements

Install:

- ARM GNU Toolchain
- Make
- OpenOCD

Verify the compiler:

arm-none-eabi-gcc --version

Verify GDB:

arm-none-eabi-gdb --version

Verify OpenOCD:

openocd --version

Build

From the repository root:

make

Typical generated files:

build/
├── *.o
├── *.elf
├── *.bin
├── *.hex
└── *.map

---

⚡ Flash Instructions

Connect the STM32F407 through ST-LINK.

Start OpenOCD or use it directly for programming:

openocd \
    -f interface/stlink.cfg \
    -f target/stm32f4x.cfg \
    -c "program build/stm32-preemptive-scheduler.elf verify reset exit"

Use the exact ELF filename generated by the Makefile.

---

🐞 Debug Instructions

Terminal 1 — OpenOCD

openocd \
    -f interface/stlink.cfg \
    -f target/stm32f4x.cfg

Terminal 2 — GDB

arm-none-eabi-gdb build/stm32-preemptive-scheduler.elf

Then:

target extended-remote localhost:3333
monitor reset halt
load
break PendSV_Handler
continue

At the breakpoint, inspect:

PSP
MSP
PC
LR
xPSR
TCB
R4-R11

This makes it possible to observe the context switch directly at the processor-register level.

---

📂 Project Structure

stm32-preemptive-scheduler/
│
├── README.md
├── LICENSE
├── Makefile
├── .gitignore
│
├── src/
│   ├── main.c
│   ├── led.c
│   └── led.h
│
├── startup/
│   └── stm32_startup.c
│
├── linker/
│   └── stm32_ls.ld
│
├── debug/
│   ├── openocd.cfg
│   ├── gdbinit
│   └── DEBUGGING.md
│
├── docs/
│   ├── architecture.md
│   ├── scheduler-design.md
│   ├── context-switching.md
│   ├── memory-stack.md
│   └── cortex-m-faults.md
│
└── media/
    ├── architecture.png
    ├── context-switch.png
    └── scheduler-demo.gif

«Adjust this tree to match the actual source layout; don't create files merely for appearance.»

---

🎥 Demo

Add a GIF, photograph, or video showing the scheduler running on the STM32F407.

Recommended demonstration:

Task 1
  ↓
Task 2
  ↓
Task 3
  ↓
Task 4
  ↓
Preemption
  ↓
Context Switch
  ↓
Next Task

A debugging demonstration should show:

OpenOCD
   ↓
GDB
   ↓
PendSV breakpoint
   ↓
Inspect PSP
   ↓
Inspect TCB
   ↓
Single-step context switch

---

📊 Architecture Summary

                 STM32F407
                Cortex-M4
                    │
                    ▼
          Register-Level Firmware
                    │
                    ▼
                 SysTick
                    │
                    ▼
             Task Scheduler
                    │
          ┌─────────┴─────────┐
          │                   │
        READY              BLOCKED
          │                   │
          ▼                   │
    Priority Selection        │
          │                   │
          ▼                   │
     Round-Robin              │
          │                   │
          └────────┬──────────┘
                   ▼
                PendSV
                   │
                   ▼
          Context Switching
                   │
                   ▼
              PSP / TCB
                   │
                   ▼
          Next Application Task
                   │
                   ▼
             Idle Task

---

🧠 Register-Level Design

This project intentionally works close to the processor hardware.

Application
     │
     ▼
Custom Scheduler
     │
     ├── SysTick
     │
     ├── PendSV
     │
     ├── TCB
     │
     ├── PSP
     │
     └── Inline Assembly
             │
             ▼
        Cortex-M4 CPU

No scheduler abstraction hides the context-switch mechanism.

---

🚫 No RTOS

This project does not depend on:

❌ FreeRTOS
❌ CMSIS-RTOS scheduler
❌ RTOS middleware
❌ HAL-based task scheduler
❌ High-level context-switch API

The scheduling mechanism is implemented directly.

---

🧪 Learning Outcomes

This project provides practical experience with:

Cortex-M4

- Processor registers
- Exception entry and return
- SysTick
- PendSV
- NVIC
- MSP
- PSP
- CONTROL
- Stack frames

Embedded C

- Bare-metal firmware
- Memory-mapped registers
- Volatile hardware access
- Interrupt handlers
- Static memory
- Scheduler data structures

ARM Assembly

- "MRS"
- "MSR"
- "STMDB"
- "LDMIA"
- Register preservation
- Stack manipulation
- Context save/restore

Scheduler Design

- Preemption
- Priority scheduling
- Round-robin scheduling
- Periodic execution
- Task blocking
- Task unblocking
- Idle task
- TCB management
- Independent task stacks
- Critical sections

Debugging

- SWD
- ST-LINK
- OpenOCD
- GDB
- UART
- PuTTY
- Register inspection
- Stack inspection
- Fault analysis

---

🚧 Current Limitations

This is an educational bare-metal scheduler rather than a production RTOS.

Possible limitations include:

- Fixed number of tasks
- Static task stacks
- Static scheduler configuration
- No dynamic memory allocator
- Limited synchronization primitives
- No mutex implementation
- No semaphore implementation
- No message queues
- No priority inheritance
- Board-specific peripheral configuration

---

🔮 Future Improvements

Possible future extensions:

- Dynamic task creation
- Task deletion
- General task delay API
- Mutexes
- Semaphores
- Message queues
- Priority inheritance
- Stack overflow detection
- Runtime stack usage measurement
- CPU utilization measurement
- Task statistics
- Tickless scheduling
- Low-power Idle Task
- MPU-based task isolation
- Improved fault diagnostics
- Automated scheduler testing

---

📜 Project Philosophy

«Understand the hardware first, then build the abstraction.»

The project follows this progression:

Cortex-M4
    ↓
CPU Registers
    ↓
Memory-Mapped Hardware
    ↓
Exceptions
    ↓
MSP / PSP
    ↓
Task Stack
    ↓
TCB
    ↓
Context Switching
    ↓
Task States
    ↓
Scheduler
    ↓
Preemption

The purpose is not simply to make four tasks run.

The purpose is to understand how a preemptive scheduler actually takes control of a Cortex-M4 CPU.

---

👨‍💻 Author

THIRUMALAIVASAN K

STM32 / ARM Cortex-M4 / Bare-Metal Firmware Project

---

📄 License

See ""LICENSE"" (LICENSE) for license information.

---

⭐ Project at a Glance

┌──────────────────────────────────────────────────┐
│              STM32F407 Cortex-M4                 │
├──────────────────────────────────────────────────┤
│                                                  │
│          Bare-Metal C + Inline Assembly          │
│                                                  │
├──────────────────────────────────────────────────┤
│                     SysTick                      │
│                        ↓                         │
│                 Task Scheduler                  │
│                        ↓                         │
│             READY / RUNNING / BLOCKED            │
│                        ↓                         │
│             Priority + Round-Robin               │
│                        ↓                         │
│                    PendSV                        │
│                        ↓                         │
│                Context Switching                │
│                        ↓                         │
│                   PSP / TCB                     │
│                        ↓                         │
│           4 Periodic Tasks + Idle Task           │
│                                                  │
├──────────────────────────────────────────────────┤
│              ST-LINK + OpenOCD + GDB             │
│                        +                         │
│                    UART + PuTTY                  │
└──────────────────────────────────────────────────┘

One-line description

A register-level preemptive scheduler for STM32F407, implemented from scratch using bare-metal C and inline ARM assembly to demonstrate SysTick-driven scheduling, PendSV context switching, PSP/TCB management, task states, priority round-robin scheduling, and Cortex-M4 debugging. 
