#ifndef UTILS_H
#define UTILS_H

#include "encode.h"
#include "decode.h"
#include "types.h"

Status open_files_encode(EncodeInfo *encInfo);
Status open_files_decode(DecodeInfo *decInfo);

#endif