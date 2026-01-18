# STM32F446 DroneCAN Sensor Node — BMP280 (FreeRTOS + libcanard)

![STM32F446 + CAN + BMP280](img/pic_board.jpg)

A minimal **DroneCAN (UAVCAN v0)** sensor node on **STM32F446RE**, publishing:
- **Heartbeat**
- **Pressure**
- **Temperature**

The firmware uses **CMSIS-RTOS (FreeRTOS)**, a small **BSP layer** to isolate HAL, and **libcanard** for DroneCAN transport.

---

## Repository Layout

|  
├── BSP/                          # Board Support Package (HAL wrapper: UART/I2C/CAN/LED)  
|  
├── Core/  
|   ├── Inc/                      # Application headers (main, dronecan)  
|   └── Src/                      # Application sources (tasks + DroneCAN publisher)  
|  
├── Drivers/  
|   ├── CMSIS/                      # CMSIS core headers  
|   └── STM32F4xx_HAL_Driver/       # to be added  
|  
├── SENSORS/                      # BMP280 driver (compensation + engineering units)  
|  
├── Middlewares/  
|   └── libcanard/                # DroneCAN stack (git submodule)  
|   └── Third party/              # Free RTOS stack  


---  


## Build & Run Instructions

### Prerequisites

- **STM32CubeIDE**
- **STM32F446RE** target (custom board / Nucleo)
- **BMP280** sensor wired over **I2C**
- **USB–CAN adapter** (gs_usb compatible)

### To build and flash

1. **Open STM32CubeIDE**
2. **Import the project**:
  - `File → Import → Existing Projects into Workspace` → select this repo’s root folder.  
3. **Build and flash**:


## How It Works

This project is structured like a real embedded application:  
	•	clean separation between BSP, sensor driver, and application  
	•	periodic publishing using RTOS scheduling  
	•	shared sensor state protected by a mutex  

### Main Concept

1) Sensor acquisition (BMP280)  
	•	Reads raw registers over I2C  
	•	Applies Bosch compensation formulas  
	•	Produces engineering units:  
	•	Temperature: °C  
	•	Pressure: hPa  

2) Shared state  
A SensorData_t structure is updated by the SensorTask and read by the CanTask.   

3) DroneCAN publishing (libcanard)  
The CanTask publishes:  
	•	uavcan.node.Heartbeat (uptime, health, mode)  
	•	Pressure (float, Pa)  
	•	Temperature (float, K)  

## License & Credits

	- Main application code: MIT
	- Third-party: 
		libcanard / DroneCAN stack: see submodule license inside Middlewares/libcanard/

## Author
**Alexandre Panhaleux**  
Embedded Software Engineer  
[GitHub: @alexandrephl](https://github.com/alexandrephl)
