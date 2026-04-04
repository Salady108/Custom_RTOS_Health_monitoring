import tkinter as tk
import serial
import threading
import time

# --- CONFIGURATION ---
SERIAL_PORT = '/dev/ttyUSB0'  # Linux standard port
BAUD_RATE = 115200

# --- MODERN UI COLORS ---
BG_MAIN = "#0f172a"       # Deep Slate
CARD_BG = "#1e293b"       # Lighter Slate
TEXT_MAIN = "#f8fafc"     # White
TEXT_MUTED = "#94a3b8"    # Gray
ACCENT_BLUE = "#38bdf8"   # Cyan/Blue for Heart Rate
ALERT_RED = "#ef4444"     # Emergency Red
SAFE_GREEN = "#10b981"    # Safe Green
WARNING_YELLOW = "#f59e0b"

class ModernLifeGuard(tk.Tk):
    def __init__(self):
        super().__init__()

        self.title("LifeGuard OS")
        self.geometry("1024x650") 
        self.configure(bg=BG_MAIN)
        
        # Grid Configuration for a clean desktop layout
        self.grid_columnconfigure(0, weight=1)
        self.grid_columnconfigure(1, weight=1)
        self.grid_rowconfigure(1, weight=1)

        # --- TOP HEADER ---
        self.header_frame = tk.Frame(self, bg=BG_MAIN)
        self.header_frame.grid(row=0, column=0, columnspan=2, sticky="ew", pady=(20, 10), padx=30)
        
        self.sys_status = tk.Label(self.header_frame, text="🟢 MONITORING ACTIVE", font=("Segoe UI", 16, "bold"), bg=BG_MAIN, fg=SAFE_GREEN)
        self.sys_status.pack(side=tk.LEFT)

        self.fall_count = 0
        self.counter_label = tk.Label(self.header_frame, text=f"Falls Detected: {self.fall_count}", font=("Segoe UI", 16, "bold"), bg=BG_MAIN, fg=WARNING_YELLOW)
        self.counter_label.pack(side=tk.RIGHT)

        # --- LEFT CARD: HEART RATE ---
        self.hr_card = tk.Frame(self, bg=CARD_BG, highlightbackground="#334155", highlightthickness=1)
        self.hr_card.grid(row=1, column=0, sticky="nsew", padx=(30, 15), pady=10)
        
        tk.Label(self.hr_card, text="LIVE VITALS", font=("Segoe UI", 14, "bold"), bg=CARD_BG, fg=TEXT_MUTED).pack(pady=(20, 0))
        self.bpm_label = tk.Label(self.hr_card, text="--", font=("Segoe UI", 120, "bold"), bg=CARD_BG, fg=TEXT_MAIN)
        self.bpm_label.pack(expand=True)
        tk.Label(self.hr_card, text="BPM", font=("Segoe UI", 20), bg=CARD_BG, fg=ACCENT_BLUE).pack(pady=(0, 30))

        # Small debug countdown for the BPM-enabled window.
        self.bpm_window_label = tk.Label(
            self.hr_card,
            text="BPM ENABLED: --",
            font=("Segoe UI", 10, "bold"),
            bg=CARD_BG,
            fg=TEXT_MUTED,
        )
        self.bpm_window_label.pack(pady=(0, 14))

        # --- RIGHT CARD: FALL ALERT & SOS ---
        self.sos_card = tk.Frame(self, bg=CARD_BG, highlightbackground="#334155", highlightthickness=1)
        self.sos_card.grid(row=1, column=1, sticky="nsew", padx=(15, 30), pady=10)
        
        self.sos_title = tk.Label(self.sos_card, text="FALL DETECTION", font=("Segoe UI", 14, "bold"), bg=CARD_BG, fg=TEXT_MUTED)
        self.sos_title.pack(pady=(20, 0))
        
        # Standby / Countdown Text
        self.sos_status = tk.Label(self.sos_card, text="STANDBY", font=("Segoe UI", 48, "bold"), bg=CARD_BG, fg=TEXT_MAIN)
        self.sos_status.pack(expand=True)

        # "I'm OK" Button (Hidden by default)
        self.cancel_btn = tk.Button(self.sos_card, text="I'M OK (CANCEL)", font=("Segoe UI", 16, "bold"), 
                                    bg=SAFE_GREEN, fg="white", activebackground="#059669", 
                                    command=self.cancel_sos, relief="flat", borderwidth=0, cursor="hand2")

        # The Dual-Font Dispatch Frame (Hidden by default)
        self.dispatch_frame = tk.Frame(self.sos_card, bg=ALERT_RED)
        self.dispatch_bold = tk.Label(self.dispatch_frame, text="DISPATCHED", font=("Segoe UI", 40, "bold"), bg=ALERT_RED, fg="white")
        self.dispatch_bold.pack(side=tk.TOP)
        self.dispatch_norm = tk.Label(self.dispatch_frame, text="emergency services", font=("Segoe UI", 24), bg=ALERT_RED, fg="white")
        self.dispatch_norm.pack(side=tk.TOP)

        # --- BOTTOM CARD: EMERGENCY CONTACTS ---
        self.contact_card = tk.Frame(self, bg=CARD_BG, highlightbackground="#334155", highlightthickness=1)
        self.contact_card.grid(row=2, column=0, columnspan=2, sticky="ew", padx=30, pady=(10, 30))
        
        tk.Label(self.contact_card, text="EMERGENCY CONTACTS (AUTO-DIAL ON FALL)", font=("Segoe UI", 12, "bold"), bg=CARD_BG, fg=TEXT_MUTED).pack(pady=(15, 10))
        
        contact_frame = tk.Frame(self.contact_card, bg=CARD_BG)
        contact_frame.pack(pady=(0, 20))
        
        tk.Label(contact_frame, text="👤 Son: +91 98765 43210", font=("Segoe UI", 18), bg=CARD_BG, fg=TEXT_MAIN).grid(row=0, column=0, padx=40)
        tk.Label(contact_frame, text="👤 Friend: +91 91234 56789", font=("Segoe UI", 18), bg=CARD_BG, fg=TEXT_MAIN).grid(row=0, column=1, padx=40)
        tk.Label(contact_frame, text="🚑 Ambulance: 108", font=("Segoe UI", 18, "bold"), bg=CARD_BG, fg=ALERT_RED).grid(row=0, column=2, padx=40)

        # State Variables
        self.is_falling = False
        self.countdown = 10
        self.bpm_window_countdown = 10
        self.timer_id = None

        # Start Serial Thread
        self.serial_thread = threading.Thread(target=self.read_serial, daemon=True)
        self.serial_thread.start()

    def update_bpm(self, bpm_value):
        if not self.is_falling:
            self.bpm_label.config(text=str(bpm_value), fg=ACCENT_BLUE)
        else:
            self.bpm_label.config(text=str(bpm_value), fg=TEXT_MAIN)

    def trigger_fall(self):
        if self.is_falling: return # Prevent spam triggering if pi sends multiple fall lines
        
        self.is_falling = True
        self.countdown = 10
        self.bpm_window_countdown = 10
        self.bpm_window_label.config(text=f"BPM ENABLED: {self.bpm_window_countdown}s")
        
        # Update Counter
        self.fall_count += 1
        self.counter_label.config(text=f"Falls Detected: {self.fall_count}")
        
        # Set Alert Colors
        self.sys_status.config(text="⚠️ SOS TRIGGERED", fg=ALERT_RED)
        self.sos_card.config(bg=ALERT_RED)
        self.sos_title.config(bg=ALERT_RED, fg="white", text="HARD FALL DETECTED!")
        
        # Show Countdown and Cancel Button
        self.dispatch_frame.pack_forget()
        self.sos_status.config(bg=ALERT_RED, fg="white", font=("Segoe UI", 32, "bold"))
        self.sos_status.pack(pady=(20, 10))
        self.cancel_btn.pack(pady=(0, 30), ipadx=30, ipady=15)
        
        self.update_countdown()

    def update_countdown(self):
        if not self.is_falling: return
        
        if self.countdown > 0:
            self.sos_status.config(text=f"CALLING EMS IN\n{self.countdown}s")
            self.bpm_window_label.config(text=f"BPM ENABLED: {self.bpm_window_countdown}s")
            self.countdown -= 1
            if self.bpm_window_countdown > 0:
                self.bpm_window_countdown -= 1
            self.timer_id = self.after(1000, self.update_countdown)
        else:
            # COUNTDOWN HIT ZERO: Dispatch EMS
            self.bpm_window_label.config(text="BPM ENABLED: --")
            self.cancel_btn.pack_forget()
            self.sos_status.pack_forget()
            self.dispatch_frame.pack(expand=True)
            
            # Auto-reset the entire UI after 5 seconds of showing "DISPATCHED"
            self.timer_id = self.after(5000, self.cancel_sos)

    def cancel_sos(self):
        # This handles both the manual "I'M OK" click and the 5-second auto-reset
        self.is_falling = False
        if self.timer_id:
            self.after_cancel(self.timer_id)
            
        self.sys_status.config(text="🟢 MONITORING ACTIVE", fg=SAFE_GREEN)
        
        # Reset UI back to Standby
        self.dispatch_frame.pack_forget()
        self.cancel_btn.pack_forget()
        self.bpm_window_label.config(text="BPM ENABLED: --")
        
        self.sos_card.config(bg=CARD_BG)
        self.sos_title.config(bg=CARD_BG, fg=TEXT_MUTED, text="FALL DETECTION")
        
        self.sos_status.config(text="STANDBY", bg=CARD_BG, fg=TEXT_MAIN, font=("Segoe UI", 48, "bold"))
        self.sos_status.pack(expand=True)
        
        self.bpm_label.config(fg=ACCENT_BLUE, text="--")

    def read_serial(self):
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            time.sleep(2)
            
            while True:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    
                    if "BPM:" in line and "PULSE!" in line:
                        try:
                            bpm_part = line.split("BPM: ")[1]
                            bpm = int(''.join(filter(str.isdigit, bpm_part)))
                            self.after(0, self.update_bpm, bpm)
                        except:
                            pass
                            
                    elif "FALL DETECTED!" in line:
                        self.after(0, self.trigger_fall)
                        
                    elif "Fall window ended" in line and not self.is_falling:
                        self.after(0, lambda: self.bpm_label.config(text="--"))
                        
        except Exception as e:
            print(f"Serial Error: {e}")

if __name__ == "__main__":
    app = ModernLifeGuard()
    app.mainloop()