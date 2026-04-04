#include "app_tasks.h"
#include "irq.h"
#include "uart.h"
#include "max30102.h"
#include "mpu6050.h"

#include <stdint.h>

extern uint32_t get_now_ms(void);

static volatile struct {
    int32_t ax;
    int32_t ay;
    int32_t az;
    int fall_window_active;
    uint32_t fall_window_start;
} g_shared;

void sensor_task(void *arg) {
    (void)arg;
    enable_irq();

    uint32_t last_sample_time = get_now_ms();
    int free_fall_seen = 0;
    uint32_t free_fall_time = 0;
    int32_t gravity_axis = 0;
    int32_t gravity_sign = 0;
    int gravity_calibrated = 0;
    int32_t prev_proj = 0;

    const int32_t FREE_FALL_MG = 700;
    const int32_t IMPACT_MG = 1500;
    const int32_t DOWNWARD_MG = -200;
    const uint32_t IMPACT_WINDOW_MS = 1200;
    const uint32_t BPM_WINDOW_MS = 10000;
    const uint32_t SAMPLE_PERIOD_MS = 20;

    for (;;) {
        uint32_t now = get_now_ms();
        if ((now - last_sample_time) < SAMPLE_PERIOD_MS) {
            asm volatile("wfi");
            continue;
        }
        last_sample_time = now;

        int32_t ax = 0;
        int32_t ay = 0;
        int32_t az = 0;

        // Prevent scheduler preemption during I2C transaction.
        disable_irq();
        mpu6050_read_accel(&ax, &ay, &az);
        enable_irq();

        g_shared.ax = ax;
        g_shared.ay = ay;
        g_shared.az = az;

        // Previous working logic: free-fall then impact within a short window.
        int32_t acc_mag_sq = (ax * ax) + (ay * ay) + (az * az);

        if (!gravity_calibrated) {
            int32_t abs_ax = (ax < 0) ? -ax : ax;
            int32_t abs_ay = (ay < 0) ? -ay : ay;
            int32_t abs_az = (az < 0) ? -az : az;

            gravity_axis = abs_ax;
            gravity_sign = (ax >= 0) ? 1 : -1;

            if (abs_ay > gravity_axis) {
                gravity_axis = abs_ay;
                gravity_sign = (ay >= 0) ? 1 : -1;
            }
            if (abs_az > gravity_axis) {
                gravity_axis = abs_az;
                gravity_sign = (az >= 0) ? 1 : -1;
            }

            gravity_calibrated = 1;
        }

        int32_t proj = 0;
        if (gravity_axis == ((ax < 0) ? -ax : ax)) {
            proj = ax * gravity_sign;
        } else if (gravity_axis == ((ay < 0) ? -ay : ay)) {
            proj = ay * gravity_sign;
        } else {
            proj = az * gravity_sign;
        }

        int downward_motion = (proj < DOWNWARD_MG) || (prev_proj < DOWNWARD_MG);
        prev_proj = proj;

        if (!g_shared.fall_window_active && downward_motion &&
            (acc_mag_sq < (FREE_FALL_MG * FREE_FALL_MG))) {
            free_fall_seen = 1;
            free_fall_time = now;
        }

        if (!g_shared.fall_window_active && free_fall_seen && downward_motion &&
            (acc_mag_sq > (IMPACT_MG * IMPACT_MG)) &&
            ((now - free_fall_time) < IMPACT_WINDOW_MS)) {
            free_fall_seen = 0;
            g_shared.fall_window_active = 1;
            g_shared.fall_window_start = now;
            uart_send_string("FALL DETECTED! BPM output enabled.\r\n");
        }

        if (free_fall_seen && ((now - free_fall_time) >= IMPACT_WINDOW_MS)) {
            free_fall_seen = 0;
        }

        if (g_shared.fall_window_active && ((now - g_shared.fall_window_start) >= BPM_WINDOW_MS)) {
            g_shared.fall_window_active = 0;
            uart_send_string("Fall window ended. BPM output disabled.\r\n");
        }
    }
}

void bpm_task(void *arg) {
    (void)arg;
    enable_irq();

    int32_t dc_filter_w = 0;
    int32_t current_ac = 0;
    int beat_armed = 0;
    uint32_t last_beat_time = get_now_ms();
    uint32_t last_sample_time = last_beat_time;
    uint32_t bpm_history[4] = {0, 0, 0, 0};
    int bpm_idx = 0;

    const uint32_t SAMPLE_PERIOD_MS = 20;

    for (;;) {
        uint32_t now = get_now_ms();
        if ((now - last_sample_time) < SAMPLE_PERIOD_MS) {
            asm volatile("wfi");
            continue;
        }
        last_sample_time = now;

        if (!g_shared.fall_window_active) {
            dc_filter_w = 0;
            beat_armed = 0;
            continue;
        }

        uint32_t red_dummy = 0;
        uint32_t ir;

        // Keep MAX30102 access independent from the fall detector task.
        disable_irq();
        max30102_read_fifo(&red_dummy, &ir);
        enable_irq();

        if (ir < 8000) {
            dc_filter_w = 0;
            beat_armed = 0;
            continue;
        }

        if (dc_filter_w == 0) {
            dc_filter_w = (int32_t)ir << 4;
        }

        int32_t old_w = dc_filter_w;
        dc_filter_w = (int32_t)ir + ((old_w * 15) >> 4);
        current_ac = dc_filter_w - old_w;

        if (current_ac > 20) {
            beat_armed = 1;
        }

        if (current_ac < -10 && beat_armed == 1) {
            beat_armed = 0;

            uint32_t delta = now - last_beat_time;
            if (delta > 333 && delta < 1500) {
                uint32_t raw_bpm = 60000 / delta;

                bpm_history[bpm_idx] = raw_bpm;
                bpm_idx = (bpm_idx + 1) % 4;

                uint32_t avg_bpm = 0;
                int count = 0;
                for (int i = 0; i < 4; i++) {
                    if (bpm_history[i] > 0) {
                        avg_bpm += bpm_history[i];
                        count++;
                    }
                }
                if (count > 0) {
                    avg_bpm /= count;
                }

                uart_send_string("\r\n*** PULSE! | BPM: ");
                uart_send_decimal(avg_bpm);
                uart_send_string(" ***\r\n\n");

                last_beat_time = now;
            } else if (delta >= 1500) {
                last_beat_time = now;
            }
        }
    }
}

void idle_task(void *arg) {
    (void)arg;
    enable_irq();

    for (;;) {
        asm volatile("wfi");
    }
}
