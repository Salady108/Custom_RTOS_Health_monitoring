#include "uart.h"
#include "i2c.h"
#include "max30102.h"

// --- THE PI 3B+ CORRECT ADDRESS ---
// The System Timer on BCM2837 (Pi 3B+) is at 0x3F003004
#define SYSTEM_TIMER_LO (*(volatile uint32_t*)(0x3F003004))

uint32_t get_now_ms(void) {
    return SYSTEM_TIMER_LO / 1000;
}

void delay_ms(uint32_t ms) {
    uint32_t start = get_now_ms();
    while ((get_now_ms() - start) < ms);
}

void kernel_main(void) {
    uart_init();
    i2c_init();
    max30102_init();

    uart_send_string("\n\n========================================\n");
    uart_send_string("   PI 3B+ PRECISION HEART MONITOR       \n");
    uart_send_string("========================================\n");

    uint32_t red, ir;
    int32_t dc_filter_w = 0;
    int32_t current_ac = 0;
    
    uint32_t last_beat_time = get_now_ms();
    uint32_t bpm_history[4] = {0, 0, 0, 0};
    int bpm_idx = 0;
    int beat_armed = 0;

    while(1) {
        uint32_t frame_start = get_now_ms();

        max30102_read_fifo(&red, &ir);

        // 1. Finger Detection
        if (ir < 20000) {
            uart_send_string("Waiting for finger... \r");
            dc_filter_w = 0;
            while((get_now_ms() - frame_start) < 20);
            continue;
        }

        // 2. DC Removal Filter
        int32_t old_w = dc_filter_w;
        dc_filter_w = (int32_t)ir + ((old_w * 15) >> 4); 
        current_ac = dc_filter_w - old_w;

        // 3. State Machine (Thresholds tuned for Pi 3B+ stability)
        if (current_ac > 150) { // Arm when signal rises
            beat_armed = 1;
        }

        if (current_ac < -50 && beat_armed == 1) { // Trigger on the falling edge
            beat_armed = 0; 

            uint32_t now = get_now_ms();
            uint32_t delta = now - last_beat_time;

            // Human range: 40 BPM (1500ms) to 180 BPM (333ms)
            if (delta > 333 && delta < 1500) {
                uint32_t raw_bpm = 60000 / delta;
                
                bpm_history[bpm_idx] = raw_bpm;
                bpm_idx = (bpm_idx + 1) % 4;
                
                uint32_t avg_bpm = 0;
                int count = 0;
                for(int i=0; i<4; i++) {
                    if(bpm_history[i] > 0) {
                        avg_bpm += bpm_history[i];
                        count++;
                    }
                }
                if(count > 0) avg_bpm /= count;

                uart_send_string("PULSE! | BPM: ");
                uart_send_decimal(avg_bpm);
                uart_send_string("        \r\n");
                
                last_beat_time = now;
            } 
            else if (delta >= 1500) {
                // Reset timer if it's been too long between pulses
                last_beat_time = now;
            }
        }

        // Maintain a steady 50Hz sample rate
        while((get_now_ms() - frame_start) < 20);
    }
}