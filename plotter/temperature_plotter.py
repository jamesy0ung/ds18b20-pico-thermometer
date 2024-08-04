import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# Device settings
VENDOR_ID = 0xf055
PRODUCT_ID = 0x0012
BAUD_RATE = 115200

# Plot settings
MAX_DATA_POINTS = 100

class TemperaturePlotter:
    def __init__(self):
        self.ser = None
        self.x_data = deque(maxlen=MAX_DATA_POINTS)
        self.y_data = deque(maxlen=MAX_DATA_POINTS)
        self.data_counter = 0

    def find_serial_port(self):
        print("Searching for serial ports...")
        for port, desc, hwid in serial.tools.list_ports.comports():
            print(f"  - Found port: {port}, desc: {desc}, hwid: {hwid}")
            if f"VID:PID={VENDOR_ID:04X}:{PRODUCT_ID:04X}" in hwid.upper():
                return port
        return None

    def setup_serial(self):
        port = self.find_serial_port()
        if not port:
            raise ValueError(f"Device with VID:0x{VENDOR_ID:04x} PID:0x{PRODUCT_ID:04x} not found.")
        
        try:
            self.ser = serial.Serial(port, BAUD_RATE, timeout=0.1)
            print(f"Serial port {port} opened successfully.")
        except serial.SerialException as e:
            raise IOError(f"Error opening serial port: {e}")

    def setup_plot(self):
        self.fig, self.ax = plt.subplots()
        self.ax.set_xlabel('Data Point')
        self.ax.set_ylabel('Temperature (°C)')
        self.ax.set_title('Live Temperature Reading')
        self.ax.grid(True)

        self.line, = self.ax.plot([], [], 'r-', linewidth=2)
        self.avg_temp_text = self.ax.text(0.05, 0.95, '', transform=self.ax.transAxes)
        self.last_temp_text = self.ax.text(0.05, 0.90, '', transform=self.ax.transAxes)

    def animate(self, i):
        if self.ser.in_waiting > 0:
            data_line = self.ser.readline().decode('utf-8').strip()
            print(f"Received data: {data_line}")
            try:
                temperature = float(data_line)
                self.update_data(temperature)
                self.update_plot()
            except ValueError as e:
                print(f"Error parsing data: {data_line} - {e}")
        return self.line,

    def update_data(self, temperature):
        self.data_counter += 1
        self.x_data.append(self.data_counter)
        self.y_data.append(temperature)

    def update_plot(self):
        self.line.set_data(self.x_data, self.y_data)
        self.ax.relim()
        self.ax.autoscale_view()

        avg_temp = sum(self.y_data) / len(self.y_data) if self.y_data else 0
        self.avg_temp_text.set_text(f"Avg: {avg_temp:.2f} °C")
        self.last_temp_text.set_text(f"Last: {self.y_data[-1]:.2f} °C")
        self.fig.canvas.draw()

    def run(self):
        self.setup_serial()
        self.setup_plot()
        ani = animation.FuncAnimation(self.fig, self.animate, interval=100, blit=True, save_count=MAX_DATA_POINTS)
        plt.show()

    def close(self):
        if self.ser:
            self.ser.close()

if __name__ == "__main__":
    plotter = TemperaturePlotter()
    try:
        plotter.run()
    except (ValueError, IOError) as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("Program interrupted by user.")
    finally:
        plotter.close()
        print("Program terminated.")
