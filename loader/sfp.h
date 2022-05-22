#include <stdint.h>

#ifndef KERO_BLASTER_VITA_SFP_H
#define KERO_BLASTER_VITA_SFP_H

#define sfp_float uint32_t
#define sfp_double uint64_t

#define sfp2float(name) (*(float*)&(name))
#define sfp2double(name) (*(double*)&(name))

#define float2sfp(name) (*(sfp_float*)&(name))
#define double2sfp(name) (*(sfp_double*)&(name))

#endif //KERO_BLASTER_VITA_SFP_H
