#include "io.h"

#include <stdio.h>
#include <stdlib.h>

static int validate_header(const ADCHeader *header) {
    if (header->magic != ADC_MAGIC) {
        printf("Error: invalid magic number. Expected 0x%08X, got 0x%08X.\n",
               ADC_MAGIC, header->magic);
        return 0;
    }

    if (header->version != ADC_VERSION) {
        printf("Error: unsupported file version.\n");
        return 0;
    }

    if (header->channel_count != ADC_CHANNELS) {
        printf("Error: unexpected channel count.\n");
        return 0;
    }

    if (header->sample_rate_hz != ADC_SAMPLE_RATE) {
        printf("Error: unexpected sample rate.\n");
        return 0;
    }

    return 1;
}

ADCSample *io_load_adc_log(const char *filename, ADCHeader *header) {
    FILE *file = fopen(filename, "rb");

    if (file == NULL) {
        printf("Error: could not open input file '%s'.\n", filename);
        return NULL;
    }

    if (fread(header, sizeof(ADCHeader), 1, file) != 1) {
        printf("Error: could not read the 24-byte file header.\n");
        fclose(file);
        return NULL;
    }

    if (!validate_header(header)) {
        fclose(file);
        return NULL;
    }

    ADCSample *samples = malloc(header->record_count * sizeof(ADCSample));

    if (samples == NULL) {
        printf("Error: memory allocation failed for samples.\n");
        fclose(file);
        return NULL;
    }

    for (size_t i = 0; i < header->record_count; i++) {
        ADCRawRecord raw_record;

        if (fread(&raw_record, sizeof(ADCRawRecord), 1, file) != 1) {
            printf("Error: file ended early. Expected %u records, read %zu.\n",
                   header->record_count, i);
            free(samples);
            fclose(file);
            return NULL;
        }

        adc_copy_record_to_sample(&raw_record, samples + i);
    }

    fclose(file);
    return samples;
}

int io_save_results(const char *filename,
                    const ADCHeader *header,
                    const ADCChannelSummary summaries[ADC_CHANNELS],
                    const ADCSequenceGap *gaps,
                    size_t visible_gap_count,
                    size_t total_gap_count) {
    FILE *file = fopen(filename, "w");

    if (file == NULL) {
        printf("Error: could not write '%s'.\n", filename);
        return 0;
    }

    fprintf(file, "ADC LOG DIAGNOSTIC SUMMARY\n");
    fprintf(file, "==========================\n\n");

    fprintf(file, "FILE CHECK\n");
    fprintf(file, "Magic value accepted : 0x%08X\n", header->magic);
    fprintf(file, "Format version       : %u\n", header->version);
    fprintf(file, "Detected channels    : %u\n", header->channel_count);
    fprintf(file, "Records analysed     : %u\n", header->record_count);
    fprintf(file, "Sampling frequency   : %u Hz\n\n", header->sample_rate_hz);

    fprintf(file, "CHANNEL SNAPSHOTS\n");
    fprintf(file, "-----------------\n\n");

    for (size_t i = 0; i < ADC_CHANNELS; i++) {
        const ADCChannelSummary *summary = &summaries[i];

        fprintf(file, "[ Channel %u ]\n", summary->channel_id);
        fprintf(file, "Samples checked ............ %zu\n", summary->sample_count);
        fprintf(file, "Average voltage ............ %.6f V\n", summary->mean_voltage);
        fprintf(file, "RMS voltage ................ %.6f V\n", summary->rms_voltage);
        fprintf(file, "Voltage range .............. %.6f V to %.6f V\n",
                summary->min_voltage,
                summary->max_voltage);
        fprintf(file, "Voltage spread ............. %.6f V\n", summary->standard_deviation);
        fprintf(file, "Temperature average ........ %.2f C\n", summary->mean_temperature);
        fprintf(file, "Temperature range .......... %.2f C to %.2f C\n",
                summary->min_temperature,
                summary->max_temperature);

        fprintf(file, "\nDetected issues\n");
        fprintf(file, "  Above 3.0 V .............. %zu\n", summary->overvoltage_count);
        fprintf(file, "  Below 0.3 V .............. %zu\n", summary->undervoltage_count);
        fprintf(file, "  Sensor fault bit ......... %zu\n", summary->sensor_fault_count);
        fprintf(file, "  Out-of-range bit ......... %zu\n\n", summary->out_of_range_count);
    }

    fprintf(file, "RECORD ORDER CHECK\n");
    fprintf(file, "------------------\n");

    if (total_gap_count == 0) {
        fprintf(file, "Result: sequence numbers are continuous. No missing records found.\n");
    } else {
        fprintf(file, "Result: %zu sequence gap(s) detected.\n", total_gap_count);
        fprintf(file, "Shown in this report: %zu gap(s).\n\n", visible_gap_count);

        for (size_t i = 0; i < visible_gap_count; i++) {
            fprintf(file, "Gap #%zu\n", i + 1);
            fprintf(file, "  Last sequence before gap : %u\n", gaps[i].previous_sequence);
            fprintf(file, "  First sequence after gap : %u\n", gaps[i].next_sequence);
            fprintf(file, "  Missing sequence range   : %u to %u\n\n",
                    gaps[i].first_missing,
                    gaps[i].last_missing);
        }
    }

    fclose(file);
    return 1;
}