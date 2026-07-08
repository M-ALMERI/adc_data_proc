#ifndef IO_H
#define IO_H

#include "adc.h"

ADCSample *io_load_adc_log(const char *filename, ADCHeader *header);

#endif