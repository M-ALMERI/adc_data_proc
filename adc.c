#include "adc.h"
#include "stats.h"

static void clear_summary(ADCChannelSummary *summary, uint8_t channel_id) {
    summary->channel_id = channel_id;
    summary->sample_count = 0;
    summary->mean_voltage = 0.0;
    summary->rms_voltage = 0.0;
    summary->min_voltage = 0.0;
    summary->max_voltage = 0.0;
    summary->standard_deviation = 0.0;
    summary->mean_temperature = 0.0;
    summary->min_temperature = 0.0;
    summary->max_temperature = 0.0;
    summary->overvoltage_count = 0;
    summary->undervoltage_count = 0;
    summary->sensor_fault_count = 0;
    summary->out_of_range_count = 0;
}

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

int adc_sample_has_fault(const ADCSample *sample) {
    return sample->voltage > ADC_OVERVOLTAGE_LIMIT ||
           sample->voltage < ADC_UNDERVOLTAGE_LIMIT ||
           (sample->status_flags & ADC_FLAG_SENSOR_FAULT) ||
           (sample->status_flags & ADC_FLAG_OUT_OF_RANGE);
}

void adc_build_channel_summaries(const ADCSample *samples,
                                 size_t sample_count,
                                 ADCChannelSummary summaries[ADC_CHANNELS]) {
    StatsTracker voltage_trackers[ADC_CHANNELS];
    StatsTracker temperature_trackers[ADC_CHANNELS];

    for (size_t channel = 0; channel < ADC_CHANNELS; channel++) {
        clear_summary(&summaries[channel], (uint8_t)channel);
        stats_tracker_reset(&voltage_trackers[channel]);
        stats_tracker_reset(&temperature_trackers[channel]);
    }

    const ADCSample *current = samples;
    const ADCSample *end = samples + sample_count;

    while (current < end) {
        if (current->channel_id < ADC_CHANNELS) {
            ADCChannelSummary *summary = &summaries[current->channel_id];

            stats_tracker_add(&voltage_trackers[current->channel_id], current->voltage);
            stats_tracker_add(&temperature_trackers[current->channel_id], current->temperature_c);

            summary->sample_count++;

            if (current->voltage > ADC_OVERVOLTAGE_LIMIT) {
                summary->overvoltage_count++;
            }

            if (current->voltage < ADC_UNDERVOLTAGE_LIMIT) {
                summary->undervoltage_count++;
            }

            if (current->status_flags & ADC_FLAG_SENSOR_FAULT) {
                summary->sensor_fault_count++;
            }

            if (current->status_flags & ADC_FLAG_OUT_OF_RANGE) {
                summary->out_of_range_count++;
            }
        }

        current++;
    }

    for (size_t channel = 0; channel < ADC_CHANNELS; channel++) {
        summaries[channel].mean_voltage = stats_tracker_mean(&voltage_trackers[channel]);
        summaries[channel].rms_voltage = stats_tracker_rms(&voltage_trackers[channel]);
        summaries[channel].min_voltage = voltage_trackers[channel].min;
        summaries[channel].max_voltage = voltage_trackers[channel].max;
        summaries[channel].standard_deviation = stats_tracker_stddev(&voltage_trackers[channel]);

        summaries[channel].mean_temperature = stats_tracker_mean(&temperature_trackers[channel]);
        summaries[channel].min_temperature = temperature_trackers[channel].min;
        summaries[channel].max_temperature = temperature_trackers[channel].max;
    }
}

size_t adc_find_sequence_gaps(const ADCSample *samples,
                              size_t sample_count,
                              ADCSequenceGap *gaps,
                              size_t max_gaps) {
    size_t found = 0;

    for (size_t i = 0; i + 1 < sample_count; i++) {
        uint32_t previous = samples[i].sequence_number;
        uint32_t next = samples[i + 1].sequence_number;

        if (next > previous + 1) {
            if (found < max_gaps) {
                gaps[found].previous_sequence = previous;
                gaps[found].next_sequence = next;
                gaps[found].first_missing = previous + 1;
                gaps[found].last_missing = next - 1;
            }

            found++;
        }
    }

    return found;
}