#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stddef.h>

#define ADC_MAGIC 0xADC1BEEF
#define ADC_VERSION 1
#define ADC_CHANNELS 4
#define ADC_SAMPLE_RATE 1000

#define ADC_MAX_RAW 4095.0
#define ADC_VREF 3.3

#define ADC_OVERVOLTAGE_LIMIT 3.0
#define ADC_UNDERVOLTAGE_LIMIT 0.3

#define ADC_FLAG_SENSOR_FAULT 0x01
#define ADC_FLAG_OUT_OF_RANGE 0x02

#define MAX_REPORTED_GAPS 32

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t channel_count;
    uint32_t record_count;
    uint32_t sample_rate_hz;
    uint8_t reserved[8];
} ADCHeader;

#pragma pack(push, 1)
typedef struct {
    float timestamp;
    uint8_t channel_id;
    uint16_t raw_value;
    int16_t temperature_raw;
    uint8_t status_flags;
    uint32_t sequence_number;
    uint8_t reserved[2];
} ADCRawRecord;
#pragma pack(pop)

typedef struct {
    float timestamp;
    uint8_t channel_id;
    uint16_t raw_value;
    double voltage;
    double temperature_c;
    uint8_t status_flags;
    uint32_t sequence_number;
} ADCSample;

typedef struct {
    uint8_t channel_id;
    size_t sample_count;
    double mean_voltage;
    double rms_voltage;
    double min_voltage;
    double max_voltage;
    double standard_deviation;
    double mean_temperature;
    double min_temperature;
    double max_temperature;
    size_t overvoltage_count;
    size_t undervoltage_count;
    size_t sensor_fault_count;
    size_t out_of_range_count;
} ADCChannelSummary;

typedef struct {
    uint32_t previous_sequence;
    uint32_t next_sequence;
    uint32_t first_missing;
    uint32_t last_missing;
} ADCSequenceGap;

double adc_raw_to_voltage(uint16_t raw_value);
double adc_raw_temp_to_celsius(int16_t raw_temperature);
void adc_copy_record_to_sample(const ADCRawRecord *raw_record, ADCSample *sample);

int adc_sample_has_fault(const ADCSample *sample);

void adc_build_channel_summaries(const ADCSample *samples,
                                 size_t sample_count,
                                 ADCChannelSummary summaries[ADC_CHANNELS]);

size_t adc_find_sequence_gaps(const ADCSample *samples,
                              size_t sample_count,
                              ADCSequenceGap *gaps,
                              size_t max_gaps);

#endif