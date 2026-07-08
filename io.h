#ifndef IO_H
#define IO_H

#include "adc.h"

ADCSample *io_load_adc_log(const char *filename, ADCHeader *header);

int io_save_results(const char *filename,
                    const ADCHeader *header,
                    const ADCChannelSummary summaries[ADC_CHANNELS],
                    const ADCSequenceGap *gaps,
                    size_t visible_gap_count,
                    size_t total_gap_count);

int io_save_fault_log(const char *filename,
                      const ADCSample *samples,
                      size_t count);

#endif