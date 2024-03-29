#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "cos_auth.h"
#include "cos_util.h"
#include "cos_api.h"
#include "cos_config.h"
#include "cos_sample_util.h"

void delete_object()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    cos_request_options_t *options = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_sample_request_options(options, is_cname);
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);

    s = cos_delete_object(options, &bucket, &object, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("delete object succeed\n");
    } else {
        printf("delete object failed\n");
    }

    aos_pool_destroy(p);
}

void delete_object_sample()
{
    delete_object();
}
