#ifndef COS_SAMPLE_UTIL_H
#define COS_SAMPLE_UTIL_H

#include "aos_http_io.h"
#include "aos_string.h"
#include "aos_transport.h"
#include "cos_define.h"

COS_CPP_START

void make_rand_string(aos_pool_t *p, int len, aos_string_t *data);

aos_buf_t *make_random_buf(aos_pool_t *p, int len);

void make_random_body(aos_pool_t *p, int count, aos_list_t *bc);

void init_sample_config(cos_config_t *config, int is_cname);

void init_sample_request_options(cos_request_options_t *options, int is_cname);

int64_t get_file_size(const char *file_path);

COS_CPP_END

#endif
