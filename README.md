# Bare-Metal RPi 3B+ Driver Development

This repository contains the implementation of a bare-metal firmware stack for the **Raspberry Pi 3B+ (BCM2837)**. The project documents the progression from low-level memory-mapped I/O (MMIO) to a functional I2C bus scanner used for hardware debugging.

---

## 🚀 Implemented Modules

### 1. GPIO & Memory Mapping
Established the foundation for interacting with hardware peripherals.
* **MMIO Base:** Defined at `0x3F000000` for the RPi 3B+.
* **Function Selection:** Developed a flexible `gpio_funct_select` to configure pins for specific hardware roles (e.g., UART or I2C).

### 2. Mini UART Driver
Created a serial communication interface to replace "LED debugging" with real-time text logs.
* **Initialization:** Configured GPIO 14/15 and set the baud rate using the `AUX_MU_BAUD` register.
* **Hex Formatting:** Added `uart_send_hex` to display memory addresses and raw sensor data in a readable format.



### 3. System Timer & Interrupts
Implemented precise timing and asynchronous event handling.
* **ARM Generic Timer:** Configured the `cntp_tval_el0` register for 1ms heartbeat interrupts.
* **IRQ Handling:** Set up the exception vector logic to increment a global `ticks` counter.

### 4. I2C (BSC) & Bus Scanner
Enabled communication with external sensors via the Broadcom Serial Controller (BSC1).
* **I2C Driver:** Developed polling-based `i2c_send` and `i2c_recv` functions that monitor the `DONE` and `ERR` status bits.
* **Bus Scanner:** The kernel now iterates from addresses `0x03` to `0x77`, detecting any connected peripherals (like IMUs) by checking for ACK/NACK responses.



---

## 🛠 Hardware Configuration

| Peripheral | Pins | Function | Base Address |
| :--- | :--- | :--- | :--- |
| **Mini UART** | GPIO 14, 15 | Debug Console | `0x3F215000` |
| **I2C1 (BSC1)** | GPIO 2, 3 | Sensor Interface | `0x3F804000` |
| **System Timer**| Internal | Delays/Ticks | `0x3F003000` |

---

## 📂 File Structure
* `kernel_main.c`: Core logic for the I2C scanner.
* `uart.c/h`: Mini UART driver implementation.
* `i2c.c/h`: BSC1 peripheral control.
* `gpio.c/h`: Register definitions and pin configuration.
* `timer.c`: ARM Generic Timer setup.
* `irq_handler.c`: Interrupt service routine for system ticks.

---

## ⚙️ How to Run
1. Ensure your RPi 3B+ is connected to a USB-to-TTL adapter on GPIO 14/15.
2. Compile the source into a `kernel8.img`.
3. Boot the Pi; the console will output "Scanning I2C Bus..." and list the hex addresses of all detected devices.
