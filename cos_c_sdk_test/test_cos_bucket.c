#include "CuTest.h"
#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "cos_auth.h"
#include "cos_util.h"
#include "cos_json.h"
#include "cos_api.h"
#include "cos_config.h"
#include "cos_test_util.h"

void test_bucket_setup(CuTest *tc)
{
    aos_pool_t *p = NULL;
    int is_cname = 0;
    aos_status_t *s = NULL;
    cos_request_options_t *options = NULL;
    cos_acl_e cos_acl = COS_ACL_PRIVATE;
    char *object_name1 = "cos_test_object1";
    char *object_name2 = "cos_test_object2";
    char *object_name3 = "cos_tmp1/";
    char *object_name4 = "cos_tmp2/";
    char *object_name5 = "cos_tmp3/";
    char *object_name6 = "cos_tmp3/1";
    char *str = "test c cos sdk";
    aos_table_t *headers1 = NULL;
    aos_table_t *headers2 = NULL;
    aos_table_t *headers3 = NULL;
    aos_table_t *headers4 = NULL;
    aos_table_t *headers5 = NULL;
    aos_table_t *headers6 = NULL;

    //set log level, default AOS_LOG_WARN
    aos_log_set_level(AOS_LOG_WARN);

    //set log output, default stderr
    aos_log_set_output(NULL);

    //create test bucket
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    s = create_test_bucket(options, TEST_BUCKET_NAME, cos_acl);

    if (s->code != 200 && s->code != 409)
    {
        CuAssertIntEquals(tc, 1, 0);
    }

    if (s->code == 200)
    {
        CuAssertStrEquals(tc, NULL, s->error_code);
    } else if (s->code == 409)
    {
        CuAssertStrEquals(tc, "BucketAlreadyOwnedByYou", s->error_code);
    }

    //create test object
    headers1 = aos_table_make(p, 0);
    headers2 = aos_table_make(p, 0);
    headers3 = aos_table_make(p, 0);
    headers4 = aos_table_make(p, 0);
    headers5 = aos_table_make(p, 0);
    headers6 = aos_table_make(p, 0);
    create_test_object(options, TEST_BUCKET_NAME, object_name1, str, headers1);
    create_test_object(options, TEST_BUCKET_NAME, object_name2, str, headers2);
    create_test_object(options, TEST_BUCKET_NAME, object_name3, str, headers3);
    create_test_object(options, TEST_BUCKET_NAME, object_name4, str, headers4);
    create_test_object(options, TEST_BUCKET_NAME, object_name5, str, headers5);
    create_test_object(options, TEST_BUCKET_NAME, object_name6, str, headers6);

    aos_pool_destroy(p);
}

void test_bucket_cleanup(CuTest *tc)
{
}

void test_create_bucket(CuTest *tc)
{
    aos_pool_t *p = NULL;
    int is_cname = 0;
    aos_status_t *s = NULL;
    cos_request_options_t *options = NULL;
    cos_acl_e cos_acl;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    cos_acl = COS_ACL_PRIVATE;

    //create the same bucket twice with same bucket acl
    s = create_test_bucket(options, TEST_BUCKET_NAME, cos_acl);
    CuAssertIntEquals(tc, 409, s->code);
    CuAssertStrEquals(tc, "BucketAlreadyOwnedByYou", s->error_code);

    //create the same bucket with different bucket acl
    cos_acl = COS_ACL_PUBLIC_READ;
    s = create_test_bucket(options, TEST_BUCKET_NAME, cos_acl);
    CuAssertIntEquals(tc, 409, s->code);
    CuAssertStrEquals(tc, "BucketAlreadyOwnedByYou", s->error_code);
    aos_pool_destroy(p);

    printf("test_create_bucket ok\n");
}

void test_delete_bucket(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_status_t *s = NULL;
    aos_string_t bucket;
    cos_acl_e cos_acl;
    int is_cname = 0;
    cos_request_options_t *options;
    aos_table_t *resp_headers = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    cos_acl = COS_ACL_PUBLIC_READ;
    s = create_test_bucket(options, TEST_BUCKET_NAME, cos_acl);

    //delete bucket not empty
    s = cos_delete_bucket(options, &bucket, &resp_headers);
    CuAssertIntEquals(tc, 409, s->code);
    CuAssertStrEquals(tc, "BucketNotEmpty", s->error_code);
    CuAssertTrue(tc, s->req_id != NULL);
    CuAssertPtrNotNull(tc, resp_headers);

    aos_pool_destroy(p);

    printf("test_delete_bucket ok\n");
}

void test_put_bucket_acl(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    int is_cname = 0;
    cos_request_options_t *options = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    cos_acl_e cos_acl;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    cos_acl = COS_ACL_PUBLIC_READ;
    s = cos_put_bucket_acl(options, &bucket, cos_acl, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, resp_headers);
    aos_pool_destroy(p);

    printf("test_put_bucket_acl ok\n");
}

void test_get_bucket_acl(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    int is_cname = 0;
    cos_request_options_t *options = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    aos_string_t cos_acl;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    s = cos_get_bucket_acl(options, &bucket, &cos_acl, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertStrEquals(tc, "public-read", cos_acl.data);
    CuAssertPtrNotNull(tc, resp_headers);
    aos_pool_destroy(p);

    printf("test_get_bucket_acl ok\n");
}

void test_list_object(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    cos_request_options_t *options = NULL;
    int is_cname = 0;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    cos_list_object_params_t *params = NULL;
    cos_list_object_content_t *content = NULL;
    int size = 0;
    char *key = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    params = cos_create_list_object_params(p);
    params->max_ret = 1;
    params->truncated = 0;
    aos_str_set(&params->prefix, "cos_test");
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    s = cos_list_object(options, &bucket, params, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertIntEquals(tc, 1, params->truncated);
    CuAssertStrEquals(tc, "cos_test_object1", params->next_marker.data);
    CuAssertPtrNotNull(tc, resp_headers);

    aos_list_for_each_entry(cos_list_object_content_t, content, &params->object_list, node) {
        ++size;
        key = apr_psprintf(p, "%.*s", content->key.len, content->key.data);
    }
    CuAssertIntEquals(tc, 1 ,size);
    CuAssertStrEquals(tc, "cos_test_object1", key);
    
    size = 0;
    resp_headers = NULL;
    aos_list_init(&params->object_list);
    aos_str_set(&params->marker, params->next_marker.data);
    s = cos_list_object(options, &bucket, params, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertIntEquals(tc, 0, params->truncated);
    aos_list_for_each_entry(cos_list_object_content_t, content, &params->object_list, node) {
        ++size;
        key = apr_psprintf(p, "%.*s", content->key.len, content->key.data);
    }
    CuAssertIntEquals(tc, 1 ,size);
    CuAssertStrEquals(tc, "cos_test_object2", key);
    CuAssertPtrNotNull(tc, resp_headers);
    aos_pool_destroy(p);

    printf("test_list_object ok\n");
}

void test_list_object_with_delimiter(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    cos_request_options_t *options = NULL;
    int is_cname = 0;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    cos_list_object_params_t *params = NULL;
    cos_list_object_common_prefix_t *common_prefix = NULL;
    int size = 0;
    char *prefix = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    params = cos_create_list_object_params(p);
    params->max_ret = 5;
    params->truncated = 0;
    aos_str_set(&params->delimiter, "/");
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    s = cos_list_object(options, &bucket, params, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertIntEquals(tc, 0, params->truncated);
    CuAssertPtrNotNull(tc, resp_headers);

    aos_list_for_each_entry(cos_list_object_common_prefix_t, common_prefix, &params->common_prefix_list, node) {
        ++size;
        prefix = apr_psprintf(p, "%.*s", common_prefix->prefix.len, 
                              common_prefix->prefix.data);
        if (size == 1) {
            CuAssertStrEquals(tc, "cos_tmp1/", prefix);
        } else if(size == 2) {
            CuAssertStrEquals(tc, "cos_tmp2/", prefix);
        }
    }
    CuAssertIntEquals(tc, 2, size);
    aos_pool_destroy(p);

    printf("test_list_object_with_delimiter ok\n");
}

void test_delete_objects_quiet(CuTest *tc)
{
    aos_pool_t *p = NULL;
    int is_cname = 0;
    aos_string_t bucket;
    aos_status_t *s = NULL;
    aos_table_t *resp_headers = NULL;
    cos_request_options_t *options = NULL;
    char *object_name1 = "cos_test_object1";
    char *object_name2 = "cos_test_object2";
    cos_object_key_t *content1 = NULL;
    cos_object_key_t *content2 = NULL;
    aos_list_t object_list;
    aos_list_t deleted_object_list;
    int is_quiet = 1;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);

    aos_list_init(&object_list);
    aos_list_init(&deleted_object_list);
    content1 = cos_create_cos_object_key(p);
    aos_str_set(&content1->key, object_name1);
    aos_list_add_tail(&content1->node, &object_list);
    content2 = cos_create_cos_object_key(p);
    aos_str_set(&content2->key, object_name2);
    aos_list_add_tail(&content2->node, &object_list);

    s = cos_delete_objects(options, &bucket, &object_list, is_quiet,
        &resp_headers, &deleted_object_list);

    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, resp_headers);
    aos_pool_destroy(p);

    printf("test_delete_objects_quiet ok\n");
}

void test_delete_objects_not_quiet(CuTest *tc)
{
    aos_pool_t *p = NULL;
    int is_cname = 0;
    aos_string_t bucket;
    aos_status_t *s = NULL;
    aos_table_t *resp_headers = NULL;
    cos_request_options_t *options = NULL;
    char *object_name1 = "cos_tmp1/";
    char *object_name2 = "cos_tmp2/";
    cos_object_key_t *content = NULL;
    cos_object_key_t *content1 = NULL;
    cos_object_key_t *content2 = NULL;
    aos_list_t object_list;
    aos_list_t deleted_object_list;
    int is_quiet = 0;
    
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);

    aos_list_init(&object_list);
    aos_list_init(&deleted_object_list);
    content1 = cos_create_cos_object_key(p);
    aos_str_set(&content1->key, object_name1);
    aos_list_add_tail(&content1->node, &object_list);
    content2 = cos_create_cos_object_key(p);
    aos_str_set(&content2->key, object_name2);
    aos_list_add_tail(&content2->node, &object_list);
    
    s = cos_delete_objects(options, &bucket, &object_list, is_quiet, 
        &resp_headers, &deleted_object_list);

    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, resp_headers);

    aos_list_for_each_entry(cos_object_key_t, content, &deleted_object_list, node) {
        printf("Deleted key:%.*s\n", content->key.len, content->key.data);
    }
    aos_pool_destroy(p);

    printf("test_delete_objects_not_quiet ok\n");
}

void test_delete_objects_by_prefix(CuTest *tc)
{
    aos_pool_t *p = NULL;
    cos_request_options_t *options = NULL;
    int is_cname = 0;
    aos_string_t bucket;
    aos_status_t *s = NULL;
    aos_string_t prefix;
    char *prefix_str = "cos_tmp3";
    
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&prefix, prefix_str);

    s = cos_delete_objects_by_prefix(options, &bucket, &prefix);
    CuAssertIntEquals(tc, 200, s->code);
    aos_pool_destroy(p);

    printf("test_delete_object_by_prefix ok\n");
}

CuSuite *test_cos_bucket()
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_bucket_setup);
    SUITE_ADD_TEST(suite, test_create_bucket);
    SUITE_ADD_TEST(suite, test_put_bucket_acl);
    SUITE_ADD_TEST(suite, test_get_bucket_acl);
    SUITE_ADD_TEST(suite, test_delete_objects_by_prefix);
    SUITE_ADD_TEST(suite, test_list_object);
    SUITE_ADD_TEST(suite, test_list_object_with_delimiter);
    SUITE_ADD_TEST(suite, test_delete_bucket);
    SUITE_ADD_TEST(suite, test_delete_objects_quiet);
    SUITE_ADD_TEST(suite, test_delete_objects_not_quiet);
    SUITE_ADD_TEST(suite, test_bucket_cleanup);

    return suite;
}
