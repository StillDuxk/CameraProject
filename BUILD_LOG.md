# Build Log

## Major Challenges

### SD Card Failures
**Issue:**
Images saved once but failed on subsequent captures.

**Resolution:**
- Removed OLED-related conflicts.
- Simplified display integration.
- Verified SD initialization sequence.

### Image Overwrite Problem
**Issue:**
Images restarted from `photo0.jpg` after reboot.

**Resolution:**
- Implemented automatic SD card scanning.
- Added persistent filename generation:
  - `IMG0001.JPG`
  - `IMG0002.JPG`
  - `IMG0003.JPG`

### Portable Power
**Issue:**
Required standalone operation.

**Resolution:**
- Integrated 3S lithium battery pack.
- Added XL4015 buck converter.
- Verified stable 5V output.

## Lessons Learned
- SD card debugging on ESP32-CAM can be difficult.
- Test one subsystem at a time.
- Battery power should be validated before enclosure design.
- Persistent file naming is critical for real-world usability.
