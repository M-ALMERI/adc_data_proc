# ADC Binary Log Inspector

A C99 command-line program for inspecting a binary ADC sensor log file.

The program will read the ADC dataset, validate the binary header, process the sensor records, detect abnormal readings, and generate output reports.

## Build with GCC

```bash
gcc main.c adc.c io.c stats.c -o adc_processor.exe -lm
```