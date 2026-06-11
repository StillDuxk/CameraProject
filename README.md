# ESP32 Travel Camera V2

A portable battery-powered digital camera built using the ESP32-CAM module.

## Features
- Physical shutter button
- Physical flash toggle button
- LED flash support
- MicroSD image storage
- Persistent image numbering
- Battery-powered operation
- No smartphone required for image capture

## Hardware Used
- ESP32-CAM (AI Thinker)
- MicroSD Card
- LED Flash
- Push Buttons
- 3S Lithium-Ion Battery Pack
- XL4015 Buck Converter
- FTDI Programmer (for uploading code)

## Operation

### Shutter Button
Press the shutter button to capture and save an image.

### Flash Button
Press the flash button to toggle flash mode ON or OFF.

### Image Storage
Images are automatically stored on the SD card.

Example:
- `IMG0001.JPG`
- `IMG0002.JPG`
- `IMG0003.JPG`

The camera automatically detects existing images and continues numbering after power cycles.

## Project Evolution

### V1
Initial proof-of-concept camera.

**Features:**
- Image capture
- SD card storage
- Basic flash support

### V2
Portable camera system.

**Features:**
- Battery operation
- Physical controls
- Persistent image numbering
- Improved reliability

### Future Improvements (V3)
Potential future upgrades:
- Power switch integration
- Custom enclosure
- GPS tagging
- Video recording
- Image enhancement pipeline

## Author
**Dhakshesh Sivakesh**  
Mechatronics and Automation Engineering  
VIT Chennai
