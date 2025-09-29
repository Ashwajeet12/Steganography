#ifndef TYPES_H
#define TYPES_H

/* User defined types */
typedef unsigned int uint;

/* Status will be used in fn. return type */
typedef enum
{
    e_success, // 0
    e_failure // 1
} Status;

typedef enum
{
    e_encode, //0
    e_decode, //1
    e_unsupported //2
} OperationType;

typedef enum
{
    d_success,
    d_failure
} Status_d;


/* Common definitions */
#define MAX_SECRET_BUF_SIZE 256
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

#endif