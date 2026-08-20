# STM32F407 Bare-Metal Preemptive Scheduler

A **preemptive task scheduler implemented from scratch at the ARM Cortex-M4 register level** using C and inline ARM assembly.

This project does **not** use FreeRTOS, HAL-based scheduling, or any RTOS abstraction. The scheduler directly uses Cortex-M4 exception mechanisms, CPU registers, memory-mapped peripherals, task control blocks, software-managed stacks, and inline assembly for context switching.

> **STM32F407 → Bare-Metal Firmware → SysTick → Task Scheduler → PendSV → Context Switching → 4 Periodic Tasks**

---

## 🚀 Project Objective

The objective of this project is to understand and implement the fundamental mechanisms used inside a preemptive RTOS scheduler on an ARM Cortex-M4.

The scheduler demonstrates:

* Preemptive task scheduling
* Priority-based task selection
* Round-robin scheduling
* Periodic tasks
* Task Control Blocks (TCBs)
* Independent task stacks
* SysTick-based scheduling
* PendSV-based context switching
* Process Stack Pointer (PSP)
* Main Stack Pointer (MSP)
* Critical sections
* Cortex-M4 fault handling
* Register-level peripheral programming
* Inline ARM assembly
* Bare-metal debugging using OpenOCD and GDB
* Runtime debugging through UART and PuTTY

---

# ⭐ Key Features

| Feature         | Implementation         |
| --------------- | ---------------------- |
| MCU             | STM32F407              |
| CPU             | ARM Cortex-M4          |
| Programming     | C + Inline Assembly    |
| Firmware        | Bare-metal             |
| RTOS            | None                   |
| Scheduler       | Preemptive             |
| Scheduling      | Priority + Round-Robin |
| Timer           | SysTick                |
| Context Switch  | PendSV                 |
| Task Stack      | PSP                    |
| Scheduler Stack | MSP                    |
| Task State      | TCB                    |
| Task Count      | 4 periodic tasks       |
| Debug Interface | SWD / ST-LINK          |
| Debug Server    | OpenOCD                |
| Debugger        | ARM GDB                |
| Serial Monitor  | PuTTY                  |
| Build Toolchain | ARM GNU Toolchain      |

---

# 🛠 Hardware Used

* STM32F407-based development board
* ARM Cortex-M4 MCU
* ST-LINK debugger/programmer
* USB cable
* Optional USB-UART interface for serial logging
* PC running ARM GNU Toolchain, OpenOCD, GDB and PuTTY

### MCU

```text
STM32F407
    │
    ▼
ARM Cortex-M4
    │
    ├── SysTick
    ├── PendSV
    ├── NVIC
    ├── MSP
    ├── PSP
    └── Cortex-M exception mechanism
```

---

# 🧠 Architecture

```text
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
                                          ▼
                                       PendSV
                                          │
                                          ▼
                                Context Switching
                                          │
                                          ▼
                                     Next Task
```

---

# 🔄 Scheduler Flow

```text
                 System Tick
                      │
                      ▼
                   SysTick
                      │
                      ▼
             Update task timing
                      │
                      ▼
              Check ready tasks
                      │
                      ▼
           Priority-based selection
                      │
                      ▼
             Round-robin selection
                      │
                      ▼
               Trigger PendSV
                      │
                      ▼
             Context switching
                      │
                      ▼
                Next task runs
```

---

# ⚙️ Cortex-M4 Stack Architecture

The Cortex-M4 provides two stack pointers that are important to this scheduler.

```text
                         Cortex-M4
                             │
                ┌────────────┴────────────┐
                │                         │
               MSP                       PSP
                │                         │
                ▼                         ▼
        Scheduler / ISR              Application Tasks
                                          │
                             ┌────────────┼────────────┐
                             │            │            │
                            Task 1       Task 2       Task 3       Task 4
                             │            │            │            │
                           Stack        Stack        Stack        Stack
```

### MSP — Main Stack Pointer

The MSP is used for:

* Startup
* Exception handling
* Interrupt handlers
* Scheduler/ISR execution

### PSP — Process Stack Pointer

The PSP is used for:

* Application tasks
* Individual task stacks
* Task context

This allows the scheduler and application tasks to use separate stack contexts.

---

# 🧩 Task Control Block

Each task is represented by a Task Control Block.

Conceptually:

```text
                    TCB
                     │
        ┌────────────┼────────────┐
        │            │            │
       PSP        Priority       State
        │
        ├── Task stack
        │
        └── Saved context
```

The TCB allows the scheduler to maintain the execution state of every task.

The exact fields correspond to the implementation in `scheduler.c`.

---

# 🔀 Context Switching

Context switching is performed through **PendSV** and inline ARM assembly.

The scheduler saves the current task's CPU context, stores its PSP in the TCB, selects the next task, restores the next task's context, and returns to that task.

```text
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
Load Next Task PSP
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
```

### Important Cortex-M registers

```text
R0-R12
SP
LR
PC
xPSR
MSP
PSP
CONTROL
```

The hardware exception mechanism automatically handles part of the exception stack frame, while the scheduler explicitly saves/restores the software-managed registers.

---

# 📌 SysTick

SysTick provides the scheduler's periodic timing source.

```text
SysTick
   │
   ▼
Scheduler Tick
   │
   ├── Update task timing
   │
   ├── Check delayed tasks
   │
   ├── Determine ready tasks
   │
   └── Request context switch
```

SysTick does not perform the complete context switch itself.

Instead, it prepares the scheduler state and allows **PendSV** to perform the context switch.

---

# 🔄 PendSV

PendSV is used as the scheduler's context-switch exception.

```text
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
```

Using PendSV keeps the actual context-switch operation separate from the periodic timer interrupt.

---

# 🎯 Priority Round-Robin Scheduling

The scheduler combines **priority-based selection** with round-robin behavior.

Conceptually:

```text
             Ready Tasks
                  │
                  ▼
          Find highest priority
                  │
                  ▼
       Multiple tasks at same priority?
             │           │
            Yes          No
             │           │
             ▼           ▼
        Round-robin    Select task
             │
             ▼
        Select next task
```

This provides deterministic task selection while allowing tasks with equal scheduling priority to share CPU time.

---

# ⏱️ Periodic Tasks

The project demonstrates four periodic application tasks.

| Task   |  Period | Purpose       |
| ------ | ------: | ------------- |
| Task 1 |  125 ms | Periodic task |
| Task 2 |  250 ms | Periodic task |
| Task 3 |  500 ms | Periodic task |
| Task 4 | 1000 ms | Periodic task |

> The exact priority/state values should be kept synchronized with the definitions in the source code.

The different periods demonstrate that multiple independent tasks can be managed by the scheduler.

---

# 🔒 Critical Sections

Scheduler data structures must not be modified concurrently by application code and interrupt handlers.

Critical sections are used around sensitive scheduler operations.

```text
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
```

The purpose is to prevent race conditions while scheduler state is being modified.

---

# 🧮 Memory and Task Stacks

Each task requires its own stack.

```text
RAM
│
├── Scheduler / Kernel data
│
├── TCBs
│
├── Task 1 Stack
│
├── Task 2 Stack
│
├── Task 3 Stack
│
└── Task 4 Stack
```

Each task's TCB stores the information required to resume that task.

During a context switch:

```text
Task A PSP
    │
    ▼
Save Context
    │
    ▼
TCB A
    │
    ▼
Scheduler
    │
    ▼
TCB B
    │
    ▼
Task B PSP
    │
    ▼
Restore Context
    │
    ▼
Task B
```

---

# 💥 Cortex-M4 Fault Handling

The project includes Cortex-M fault handling for debugging abnormal processor states.

Important exceptions include:

```text
HardFault
MemManage
BusFault
UsageFault
```

A fault can be investigated by examining:

```text
PC
LR
SP
xPSR
MSP
PSP
```

The faulting PC is particularly useful for locating the instruction that caused the exception.

---

# 🐛 Bare-Metal Debugging

This project can be debugged without an RTOS debugger or abstraction layer.

```text
                         STM32F407
                             │
              ┌──────────────┴──────────────┐
              │                             │
             SWD                           UART
              │                             │
           ST-LINK                      USB-UART
              │                             │
              ▼                             ▼
           OpenOCD                        PuTTY
              │
              ▼
             GDB
              │
              ▼
       Register-level debug
```

---

# 🔌 OpenOCD

Connect the STM32F407 through ST-LINK.

Example:

```bash
openocd \
    -f interface/stlink.cfg \
    -f target/stm32f4x.cfg
```

OpenOCD provides the GDB server used to communicate with the target MCU.

---

# 🧰 GDB

Start the ARM debugger:

```bash
arm-none-eabi-gdb build/stm32-preemptive-scheduler.elf
```

Connect to OpenOCD:

```gdb
target extended-remote localhost:3333
```

Reset and halt:

```gdb
monitor reset halt
```

Program the target:

```gdb
load
```

Continue:

```gdb
continue
```

---

# 🔎 Register-Level Debugging

Useful GDB information includes:

```gdb
info registers
```

Important registers:

```text
R0-R12
SP
LR
PC
xPSR
```

The scheduler can also be investigated through:

```text
MSP
PSP
CONTROL
TCB
Task stack memory
```

Breakpoints can be placed directly inside the scheduler:

```gdb
break SysTick_Handler
break PendSV_Handler
break HardFault_Handler
```

---

# 🧵 Inspecting PSP

The Process Stack Pointer is critical to task context switching.

```gdb
p/x $psp
```

Stack memory can be inspected using:

```gdb
x/32wx $psp
```

This allows the saved task context to be examined directly.

---

# 🖥️ PuTTY Serial Debugging

UART logging can be monitored using PuTTY.

Recommended configuration:

```text
Connection : Serial
Baud rate  : 115200
Data bits  : 8
Stop bits  : 1
Parity     : None
Flow       : None
```

Example runtime output:

```text
====================================
 STM32F407 PREEMPTIVE SCHEDULER
====================================

Scheduler started

Task 1 running
Task 2 running
Task 3 running
Task 4 running

Context Switch
Current Task : 1
Next Task    : 2
```

The actual UART messages depend on the firmware implementation.

---

# 🏗️ Build Instructions

## 1. Install ARM GNU Toolchain

Verify:

```bash
arm-none-eabi-gcc --version
```

Verify GDB:

```bash
arm-none-eabi-gdb --version
```

## 2. Install OpenOCD

Verify:

```bash
openocd --version
```

## 3. Build

From the repository root:

```bash
make
```

The build should generate the ELF firmware and, depending on the Makefile configuration, binary/hex output.

Typical output:

```text
build/
├── *.o
├── *.elf
├── *.bin
├── *.hex
└── *.map
```

---

# ⚡ Flash Instructions

Connect the STM32F407 board through ST-LINK.

OpenOCD can be used to program the target.

Example:

```bash
openocd \
    -f interface/stlink.cfg \
    -f target/stm32f4x.cfg \
    -c "program build/stm32-preemptive-scheduler.elf verify reset exit"
```

> Use the actual output filename generated by the Makefile.

---

# 🐞 Debug Instructions

Start OpenOCD:

```bash
openocd \
    -f interface/stlink.cfg \
    -f target/stm32f4x.cfg
```

In another terminal:

```bash
arm-none-eabi-gdb build/stm32-preemptive-scheduler.elf
```

Then:

```gdb
target extended-remote localhost:3333
monitor reset halt
load
break PendSV_Handler
continue
```

Now the context-switch mechanism can be inspected instruction by instruction.

---

# 📂 Project Structure

```text
stm32-preemptive-scheduler/
│
├── README.md
├── LICENSE
├── Makefile
├── .gitignore
│
├── src/
│   ├── main.c
│   ├── scheduler.c
│   ├── scheduler.h
│   ├── led.c
│   ├── led.h
│   └── fault.c
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
│   ├── context-switching.md
│   ├── scheduler-design.md
│   ├── memory-stack.md
│   └── cortex-m-faults.md
│
└── media/
    ├── architecture.png
    ├── context-switch.png
    └── scheduler-demo.gif
```

---

# 🎥 Demo

### Scheduler Running

Add a GIF or video showing the four tasks executing on the STM32F407.

```text
[ STM32F407 Scheduler Demo GIF ]
```

Recommended demonstration:

```text
Task 1 → LED / UART
Task 2 → LED / UART
Task 3 → LED / UART
Task 4 → LED / UART
        ↓
   Preemption
        ↓
 Context Switching
        ↓
   Next Task
```

### Debugging Demonstration

A second demonstration should show:

```text
OpenOCD
   ↓
GDB
   ↓
Breakpoint at PendSV_Handler
   ↓
Inspect PSP
   ↓
Inspect TCB
   ↓
Single-step context switch
```

---

# 📸 Architecture Documentation

The project should include diagrams showing:

### 1. Overall Scheduler

```text
STM32F407
    ↓
SysTick
    ↓
Scheduler
    ↓
PendSV
    ↓
Context Switch
    ↓
Task
```

### 2. Stack Architecture

```text
MSP → Scheduler / ISR

PSP → Task 1 Stack
PSP → Task 2 Stack
PSP → Task 3 Stack
PSP → Task 4 Stack
```

### 3. Context Switching

```text
MRS PSP
   ↓
Save R4-R11
   ↓
Save PSP
   ↓
Select Task
   ↓
Restore R4-R11
   ↓
MSR PSP
   ↓
Return
```

---

# 🧪 What This Project Demonstrates

This project demonstrates practical understanding of:

### ARM Cortex-M4

* CPU registers
* Exception entry/return
* MSP
* PSP
* CONTROL
* SysTick
* PendSV
* NVIC
* Stack frames

### Embedded C

* Bare-metal programming
* Memory-mapped registers
* Volatile access
* Interrupt handlers
* Static memory allocation
* Task data structures

### ARM Assembly

* `MRS`
* `MSR`
* `STMDB`
* `LDMIA`
* Register preservation
* Stack manipulation
* Exception context handling

### Scheduler Design

* Preemption
* Priority scheduling
* Round-robin scheduling
* Periodic tasks
* TCB management
* Task stacks
* Critical sections

### Debugging

* SWD
* ST-LINK
* OpenOCD
* ARM GDB
* UART
* PuTTY
* Register inspection
* Stack inspection
* Fault debugging

---

# 🚫 No RTOS Abstraction

This project intentionally avoids:

```text
❌ FreeRTOS
❌ CMSIS-RTOS
❌ HAL scheduler
❌ RTOS middleware
❌ Dynamic task creation
❌ High-level context-switch APIs
```

Instead:

```text
                 Application
                      │
                      ▼
              Custom Scheduler
                      │
             ┌────────┴────────┐
             │                 │
          SysTick            PendSV
             │                 │
             └────────┬────────┘
                      ▼
               Inline Assembly
                      │
                      ▼
              Cortex-M4 Registers
                      │
                      ▼
                 Hardware
```

---

# 📈 Scheduler Execution Model

```text
        Timer Tick
            │
            ▼
         SysTick
            │
            ▼
    Scheduler evaluates
     ready task states
            │
            ▼
   Highest-priority task
       is selected
            │
            ▼
    PendSV is requested
            │
            ▼
    Save current context
            │
            ▼
      Update current
           TCB
            │
            ▼
     Select next task
            │
            ▼
    Restore next context
            │
            ▼
       Update PSP
            │
            ▼
      Return to task
```

---

# 🔬 Why PendSV?

PendSV is designed for deferred context switching.

Instead of performing a potentially lengthy context switch directly inside the SysTick handler:

```text
SysTick
   │
   └── Scheduler decision
           │
           ▼
        PendSV
           │
           └── Context switch
```

This separates **timer/scheduling decisions** from the actual **CPU context switch**.

---

# 🧱 Design Philosophy

The project follows a simple philosophy:

> **Understand the hardware first, then build the abstraction.**

Instead of starting with an RTOS API, this project starts with:

```text
Cortex-M4
    ↓
Registers
    ↓
Exceptions
    ↓
Stacks
    ↓
Context
    ↓
Tasks
    ↓
Scheduler
```

This makes the relationship between hardware and RTOS functionality explicit.

---

# 🚧 Current Limitations

This is an educational bare-metal scheduler rather than a production RTOS.

Possible limitations include:

* Fixed number of tasks
* Static task stacks
* No dynamic memory allocator
* Limited synchronization primitives
* No mutex implementation
* No semaphore implementation
* No message queues
* No sophisticated priority inheritance
* Board-specific peripheral configuration

---

# 🔮 Future Improvements

Possible extensions:

* Dynamic task creation
* Task deletion
* Sleep/delay API
* Mutexes
* Semaphores
* Message queues
* Priority inheritance
* Idle task
* Stack overflow detection
* Runtime CPU utilization
* Task statistics
* System tickless operation
* MPU-based task isolation
* More advanced fault diagnostics
* Automated scheduler tests

---

# 📚 Learning Outcomes

After completing this project, the following concepts can be demonstrated:

```text
ARM Cortex-M4
      ↓
Exception Mechanism
      ↓
SysTick
      ↓
PendSV
      ↓
MSP / PSP
      ↓
Task Stack
      ↓
TCB
      ↓
Context Save/Restore
      ↓
Preemption
      ↓
Task Scheduler
```

---

# 👨‍💻 Author

**THIRUMALAIVASAN K**

Embedded Systems / Firmware Project

---

# 📜 License

See [`LICENSE`](LICENSE) for details.

---

## ⭐ Project Summary

```text
┌─────────────────────────────────────────────┐
│            STM32F407 Cortex-M4              │
├─────────────────────────────────────────────┤
│                                             │
│              Bare-Metal C                   │
│                    +                        │
│            Inline ARM Assembly              │
│                                             │
├─────────────────────────────────────────────┤
│                   SysTick                   │
│                      ↓                      │
│                Task Scheduler               │
│                      ↓                      │
│                   PendSV                   │
│                      ↓                      │
│              Context Switching              │
│                      ↓                      │
│                PSP / TCB                   │
│                      ↓                      │
│              4 Periodic Tasks              │
│                                             │
├─────────────────────────────────────────────┤
│        OpenOCD + GDB + ST-LINK              │
│                 +                           │
│              UART + PuTTY                   │
└─────────────────────────────────────────────┘
```

> **A register-level preemptive scheduler for STM32F407, built from the ground up to understand how an RTOS performs task scheduling and context switching on ARM Cortex-M4.**
