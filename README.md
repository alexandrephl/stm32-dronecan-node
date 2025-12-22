# STM32 DroneCAN Periph Node – Work in Progress

### Hardware
- STM32F446 Nucleo board
- UART2 for diagnostics
- CAN transceiver (SN65HVD230)
- I²C sensor (to be added)

### Software Overview
- CubeIDE + HAL + FreeRTOS (later)
- Custom BSP (LED, UART)
- Planned DroneCAN integration using libcanard

### Current Status
- Board bring-up OK
- LED BSP working
- UART BSP working
