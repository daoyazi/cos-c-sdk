#include <sys/stat.h>
#include <stdlib.h>
#include "cos_config.h"
#include "cos_api.h"
#include "cos_sample_util.h"

void make_rand_string(aos_pool_t *p, int len, aos_string_t *data)
{
    char *str = NULL;
    int i = 0;
    str = (char *)aos_palloc(p, len + 1);
    for ( ; i < len; i++) {
        str[i] = rand() % 128;
    }
    str[len] = '\0';
    aos_str_set(data, str);
}

aos_buf_t *make_random_buf(aos_pool_t *p, int len)
{
    int bytes;
    aos_buf_t *b;
    aos_string_t str;

    make_rand_string(p, 16, &str);
    b = aos_create_buf(p, len);

    while (b->last < b->end) {
        bytes = b->end - b->last;
        bytes = aos_min(bytes, 16);
        memcpy(b->last, str.data, bytes);
        b->last += bytes;
    }

    return b;
}

void make_random_body(aos_pool_t *p, int count, aos_list_t *bc)
{
    int i = 0;
    int len;
    aos_buf_t *b;

    srand((int)time(0));
    for (; i < count; ++i) {
        len = 1 + (int)(4096.0*rand() / (RAND_MAX+1.0));
        b = make_random_buf(p, len);
        aos_list_add_tail(&b->node, bc);
    }
}

void init_sample_config(cos_config_t *config, int is_cname)
{
    aos_str_set(&config->endpoint, COS_ENDPOINT);
    aos_str_set(&config->access_key_id, ACCESS_KEY_ID);
    aos_str_set(&config->access_key_secret, ACCESS_KEY_SECRET);
    config->is_cname = is_cname;
}

void init_sample_request_options(cos_request_options_t *options, int is_cname)
{
    options->config = cos_config_create(options->pool);
    init_sample_config(options->config, is_cname);
    options->ctl = aos_http_controller_create(options->pool, 0);
}

int64_t get_file_size(const char *file_path)
{
    int64_t filesize = -1;
    struct stat statbuff;

    if(stat(file_path, &statbuff) < 0){
        return filesize;
    } else {
        filesize = statbuff.st_size;
    }

    return filesize;
}
