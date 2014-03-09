//------------------------------------------------------------------------------
// Copyright (C) 2012, Alexander Rössler
// All rights reserved.
//
//
//------------------------------------------------------------------------------

/**
 * @file   mbus_csv.h
 * 
 * @brief  Functions for outputting mbus data to csv.
 *
 */

#ifndef _MBUS_CSV_H_
#define _MBUS_CSV_H_

#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <mbus/mbus-protocol.h>

const char *mbus_data_record_value_and_unit(mbus_data_record* record);

//
// CSV generating functions
//
void  mbus_str_csv_encode(u_char *dst, const u_char *src, size_t max_len);
char *mbus_data_variable_csv(mbus_data_variable *data, uint primary_address);
char *mbus_data_fixed_csv(mbus_data_fixed *data);
char *mbus_frame_data_csv(mbus_frame_data *data, uint primary_address);

char *mbus_data_variable_header_csv(mbus_data_variable_header *header);

#endif /* _MBUS_CSV_H_ */