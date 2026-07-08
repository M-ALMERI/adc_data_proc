#include "adc.h"

double adc_raw_to_voltage(uint16_t raw_value) {
    return (raw_value / ADC_MAX_RAW) * ADC_VREF;
}

double adc_raw_temp_to_celsius(int16_t raw_temperature) {
    return raw_temperature / 10.0;
}

void adc_copy_record_to_sample(const ADCRawRecord *raw_record, ADCSample *sample) {
    sample->timestamp = raw_record->timestamp;
    sample->channel_id = raw_record->channel_id;
    sample->raw_value = raw_record->raw_value;
    sample->voltage = adc_raw_to_voltage(raw_record->raw_value);
    sample->temperature_c = adc_raw_temp_to_celsius(raw_record->temperature_raw);
    sample->status_flags = raw_record->status_flags;
    sample->sequence_number = raw_record->sequence_number;
}