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