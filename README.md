# ADC Binary Log Inspector

A C99 program for inspecting a binary ADC sensor log file.

The program reads the ADC dataset, validates the binary header, processes the sensor records, detects abnormal readings, checks sequence continuity, and generates output reports.

## Features

- Reads `adc_sensor_log.bin`
- Validates the file header and magic number
- Reads packed 16-byte ADC records
- Converts raw 12-bit ADC values to voltage
- Converts raw temperature values to Celsius
- Calculates per-channel voltage statistics
- Calculates per-channel temperature statistics
- Detects overvoltage and undervoltage readings
- Detects status flag faults
- Checks sequence numbers for missing records
- Creates `results.txt`
- Creates `fault_report.txt`

## Project Files

- `main.c` - program entry point and overall workflow
- `adc.c` / `adc.h` - ADC conversion, summaries, fault checks and sequence checking
- `io.c` / `io.h` - binary file loading and output report writing
- `stats.c` / `stats.h` - reusable running statistics tracker
- `CMakeLists.txt` - CMake build configuration

## Build with GCC

gcc main.c adc.c io.c stats.c -o adc_processor.exe -lm

# Run on Windows PowerShell
.\adc_processor.exe adc_sensor_log.bin

# Build with CMake
mkdir build
cd build
cmake ..
cmake --build .

# Output Files

results.txt - channel statistics, fault counts and sequence gap summary
fault_report.txt - individual fault events with timestamp, channel, voltage and reason

# Notes

The binary ADC record structure is packed because the dataset stores each record as exactly 16 bytes.

The sequence gap report intentionally stores and prints up to 32 gaps. The program still scans the full dataset for sequence gaps, but the report output is capped to keep the reporting buffer simple and fixed-size.