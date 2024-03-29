#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "cos_auth.h"
#include "cos_util.h"
#include "cos_api.h"
#include "cos_config.h"
#include "cos_sample_util.h"

void head_object()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    cos_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    char *content_length_str = NULL;
    char *object_type = NULL;
    int64_t content_length = 0;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_sample_request_options(options, is_cname);
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);
    headers = aos_table_make(p, 0);

    s = cos_head_object(options, &bucket, &object, headers, &resp_headers);
    
    if (aos_status_is_ok(s)) {
        content_length_str = (char*)apr_table_get(resp_headers, COS_CONTENT_LENGTH);
        if (content_length_str != NULL) {
            content_length = atol(content_length_str);
        }

        object_type = (char*)apr_table_get(resp_headers, COS_OBJECT_TYPE);
        
        printf("head object succeeded, object type:%s, content_length:%ld\n", 
               object_type, content_length);
    } else {
        printf("head object failed\n");
    }

    aos_pool_destroy(p);
}

void head_object_sample()
{
    head_object();
}
