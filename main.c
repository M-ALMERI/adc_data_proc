#include "adc.h"
#include "io.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    ADCHeader header;
    ADCSample *samples;
    ADCChannelSummary summaries[ADC_CHANNELS];
    ADCSequenceGap gaps[MAX_REPORTED_GAPS];

    size_t total_gap_count;
    size_t visible_gap_count;

    if (argc != 2) {
        printf("Usage: %s adc_sensor_log.bin\n", argv[0]);
        return 1;
    }

    printf("Opening ADC binary log: %s\n", argv[1]);

    samples = io_load_adc_log(argv[1], &header);

    if (samples == NULL) {
        return 1;
    }

    adc_build_channel_summaries(samples, header.record_count, summaries);

    total_gap_count = adc_find_sequence_gaps(samples,
                                             header.record_count,
                                             gaps,
                                             MAX_REPORTED_GAPS);

    visible_gap_count = total_gap_count;

    if (visible_gap_count > MAX_REPORTED_GAPS) {
        visible_gap_count = MAX_REPORTED_GAPS;
    }

    if (!io_save_results("results.txt",
                         &header,
                         summaries,
                         gaps,
                         visible_gap_count,
                         total_gap_count)) {
        free(samples);
        return 1;
    }

    if (!io_save_fault_log("fault_report.txt",
                           samples,
                           header.record_count)) {
        free(samples);
        return 1;
    }

    free(samples);

    printf("Processing complete.\n");
    printf("Output files created: results.txt and fault_report.txt\n");

    return 0;
}