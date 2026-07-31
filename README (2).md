# Bare-Metal Preemptive Task Scheduler — STM32F407 (ARM Cortex-M4)

A preemptive, priority round-robin task scheduler built from scratch in bare-metal
Embedded C — no RTOS — to demonstrate core kernel mechanics on the ARM Cortex-M4.

## Features

- **Context switching** via the `PendSV` exception, using inline assembly to
  save/restore R4–R11 through per-task Process Stack Pointers (PSP).
- **1ms system tick** driven by SysTick, feeding a global tick counter that
  automatically unblocks delayed tasks.
- **Task Control Blocks (TCBs)** tracking per-task state, stack pointer, and
  handler function across 4 user tasks plus an idle task.
- **Non-busy-wait `task_delay()`** using PRIMASK-based critical sections to
  block/unblock tasks efficiently.
- **Fault diagnostics** — MemManage, BusFault, and UsageFault handlers enabled
  for robust runtime fault reporting.
- **Validated concurrency** — four onboard LEDs toggled at independent periods
  (1000 / 500 / 250 / 125 ms), confirming true concurrent task execution.

## Hardware

- Board: STM32F407 (Discovery / Nucleo — *edit to match your board*)
- 4 onboard LEDs used to visually validate independent task periods

## Project Structure

```
.
├── main.c / main.h        # Task definitions, scheduler init, TCB setup
├── led.c / led.h           # LED GPIO driver
├── stm32_startup.c         # Reset handler, vector table, PendSV/SysTick hooks
├── stm32_ls.ld              # Linker script
├── syscalls.c              # Minimal syscall stubs for newlib
├── Makefile                 # Build rules
```

## Build

Requires `arm-none-eabi-gcc` and `make`.

```bash
make
```

This produces the `.elf` and `.map` output for flashing.

## Flash

Using ST-Link (`st-flash` or STM32CubeProgrammer):

```bash
st-flash write final.bin 0x8000000
```

*(Adjust to your actual flashing tool/workflow if different.)*

## How It Works

1. On reset, `stm32_startup.c` sets up the vector table and jumps to `main()`.
2. `main()` initializes 4 user tasks + 1 idle task, each with its own stack and TCB.
3. SysTick fires every 1ms, incrementing a global tick counter and checking for
   any delayed tasks ready to unblock.
4. `PendSV` performs the actual context switch — saving R4–R11 of the outgoing
   task's PSP and restoring them for the incoming task — implementing
   round-robin scheduling among ready tasks.
5. Each task toggles its assigned LED and calls `task_delay()` for a distinct
   period, so all four blink independently and concurrently — validating
   correct preemption.

## License

MIT
