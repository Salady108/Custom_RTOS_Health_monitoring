# Raspberry Pi 3B+ Heart Monitor RTOS Project

This repository contains a bare-metal AArch64 firmware project for the Raspberry Pi 3B+ that evolved from low-level MMIO drivers into a small RTOS-style sensor system.

The current firmware does four main jobs:

1. Reads acceleration data from the MPU6050 to detect a fall.
2. Uses the MAX30102 to estimate heart rate while the fall window is active.
3. Runs a periodic timer tick and a small task scheduler.
4. Streams runtime events to a Python dashboard over USB serial.

The result is not a full general-purpose RTOS, but it does include the core pieces: timer interrupt, task switching, separate task stacks, and interrupt-driven execution.

---

## What The Project Does

### Fall Detection
The MPU6050 is used to detect a fall using a simple sequence:

1. Detect free-fall from low acceleration magnitude.
2. Confirm impact within a short time window.
3. Open a 10-second BPM-enable window.
4. Return to standby after the window ends.

Fall detection is independent of the MAX30102, so a glitchy pulse sensor does not block fall alerts.

### Heart Rate Monitoring
The MAX30102 is read only while the fall window is active. When the code sees a pulse-like ripple, it estimates BPM and prints it over UART.

### RTOS-Style Scheduling
The kernel uses:

- a periodic timer interrupt,
- a context-switching IRQ wrapper,
- separate task stacks,
- a round-robin scheduler,
- a sensor task, a BPM task, and an idle task.

This is a small custom RTOS-style design for an embedded systems demonstration.

### Desktop Monitoring UI
`ui_dashboard.py` provides a Python/Tkinter desktop dashboard that reads serial output and shows:

- live BPM values,
- fall detection status,
- a 10-second countdown while BPM output is enabled,
- emergency-style visual alerts.

---

## Software Stack

### Firmware
- Language: C and AArch64 assembly
- Target: Raspberry Pi 3B+ / BCM2837
- Execution mode: bare metal
- Interrupts: ARM generic timer + IRQ vector handling

### Drivers and Subsystems
- `gpio.c/h` for MMIO GPIO control
- `uart.c/h` for mini UART serial output
- `i2c.c/h` for sensor communication
- `timer.c` for 1 ms timer ticks
- `irq.c/h` for interrupt enable and routing
- `boot.s` for reset entry, vector table, and IRQ context switching
- `scheduler.c/h` for simple task scheduling
- `app_tasks.c/h` for sensor, BPM, and idle tasks

### Dashboard
- Python 3
- Tkinter
- pyserial

---

## Hardware Used

| Peripheral | Purpose | Interface |
| --- | --- | --- |
| Raspberry Pi 3B+ | Main board | Bare metal AArch64 |
| MPU6050 | Fall detection IMU | I2C |
| MAX30102 | Heart-rate sensor | I2C |
| USB-to-serial adapter | Debug output / dashboard input | UART |

### Wiring Notes
- `GPIO 2` and `GPIO 3` are used for I2C1.
- `GPIO 14` and `GPIO 15` are used for Mini UART TX/RX.
- The Pi 3B+ uses the `0x3F...` peripheral base.

---

## Repository Layout

- `boot.s` - reset entry, vector table, and task context restore
- `kernel_main.c` - kernel entry point and task creation
- `scheduler.c/h` - task creation and round-robin scheduler
- `irq.c/h` - IRQ enable and routing
- `irq_handler.c` - timer tick handler and tick counter
- `timer.c/h` - generic timer setup
- `gpio.c/h` - GPIO MMIO definitions and helpers
- `uart.c/h` - Mini UART driver
- `i2c.c/h` - I2C driver for sensors
- `mpu6050.c/h` - IMU initialization and accel/gyro reads
- `max30102.c/h` - pulse sensor initialization and FIFO reads
- `app_tasks.c/h` - sensor task, BPM task, idle task
- `ui_dashboard.py` - Tkinter serial dashboard
- `push.py` - helper script for deployment, if you use it in your workflow

---

## Build Requirements

### Firmware Toolchain
- `aarch64-linux-gnu-gcc`
- `aarch64-linux-gnu-objcopy`
- `make`

### Python Dashboard
- Python 3
- `pyserial`

If `pyserial` is missing:

```bash
pip install pyserial
```

---

## How To Build

From the repository root:

```bash
make
```

This produces:

- `kernel.elf`
- `kernel.bin`
- `kernel8.img`

To clean the build:

```bash
make clean
```

---

## How To Run On The Raspberry Pi

1. Copy `kernel8.img` to the Pi boot partition.
2. Make sure the Pi is configured to boot the custom kernel.
3. Connect the UART adapter to `GPIO 14` and `GPIO 15`.
4. Power on the Pi.

On boot, the firmware prints a heading and then begins running the RTOS-style tasks.

---

## How To Run The Desktop Dashboard

The dashboard listens to serial output from the Pi and shows the system state visually.

### Start the dashboard

```bash
python3 ui_dashboard.py
```

### Default serial port
The dashboard currently uses:

```python
SERIAL_PORT = '/dev/ttyUSB0'
BAUD_RATE = 115200
```

If your adapter shows up on a different device, edit `ui_dashboard.py` and update `SERIAL_PORT`.

---

## Expected Serial Output

Typical runtime messages include:

- `FALL DETECTED! BPM output enabled.`
- `*** PULSE! | BPM: ... ***`
- `Fall window ended. BPM output disabled.`

While debugging, you may also see MPU telemetry or task startup lines if you re-enable them.

---

## Firmware Architecture

### Boot Flow
1. The Pi starts in `boot.s`.
2. The vector table is installed.
3. The kernel stack is set up.
4. `.bss` is cleared.
5. `kernel_main()` is called.

### Task Flow
The kernel creates three tasks:

- `sensor_task` - reads MPU6050 and detects fall events
- `bpm_task` - reads MAX30102 and prints BPM during the active window
- `idle_task` - sleeps with `wfi` when nothing else needs CPU time

### Interrupt Flow
The ARM generic timer raises a periodic interrupt.
The IRQ wrapper saves CPU context, calls the C IRQ handler, then restores the next task context.

---

## Notes

- This is a minimal RTOS-style kernel, not a full-featured production RTOS.
- Fall detection is independent from the MAX30102 path.
- The dashboard is meant for demonstration and debugging, not medical use.
- Sensor thresholds may need tuning depending on the mounting orientation and hardware setup.

---

## Troubleshooting

### No serial output
- Check the UART wiring.
- Confirm the adapter is on the correct `/dev/ttyUSB*` path.
- Confirm baud rate is `115200`.

### No fall detection
- Verify MPU6050 wiring and power.
- Check that the Pi prints the normal fall messages.
- Try adjusting `FREE_FALL_MG`, `IMPACT_MG`, or `IMPACT_WINDOW_MS` in `app_tasks.c`.

### BPM not appearing
- The BPM task only runs when the fall window is active.
- Make sure the MAX30102 is wired correctly.
- If the sensor is noisy, the BPM value may stay blank or unstable.

---

## Project Summary

This project demonstrates how to build a small bare-metal embedded system with:

- device drivers,
- timer interrupts,
- task scheduling,
- sensor fusion logic,
- a serial debug UI,
- and a practical fall-alert / heart-rate monitoring workflow.

