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

static char *default_content_type = "application/octet-stream;charset=UTF-8";

void test_object_setup(CuTest *tc)
{
    aos_pool_t *p = NULL;
    int is_cname = 0;
    aos_status_t *s = NULL;
    cos_request_options_t *options = NULL;
    cos_acl_e cos_acl = COS_ACL_PRIVATE;

    /* create test bucket */
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    s = create_test_bucket(options, TEST_BUCKET_NAME, cos_acl);

    if (s->code != 409 && s->code != 200)
    {
        CuAssertIntEquals(tc, 1, 0);        
    }
    aos_pool_destroy(p);
}

void test_object_cleanup(CuTest *tc)
{
    aos_pool_t *p = NULL;
    int is_cname = 0;
    aos_string_t bucket;
    cos_request_options_t *options = NULL;
    char *object_name1 = "cos_test_put_object.ts";
    char *object_name2 = "cos_test_put_object_from_file.jpg";
    char *object_name3 = "cos_test_object_by_url";
    char *object_name4 = "cos_test_append_object";
    char *object_name5 = "cos_test_append_object_from_file";
    char *object_name6 = "cos_test_copy_object";
    char *object_name7 = "video_1.ts";
    char *object_name8 = "video_2.ts";
    char *object_name9 = "cos_test_put_object_from_file2.txt";
    char *object_name10 = "put_object_from_buffer_with_default_content_type";

    aos_table_t *resp_headers = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);

    /* delete test object */
    delete_test_object(options, TEST_BUCKET_NAME, object_name1);
    delete_test_object(options, TEST_BUCKET_NAME, object_name2);
    delete_test_object(options, TEST_BUCKET_NAME, object_name3);
    delete_test_object(options, TEST_BUCKET_NAME, object_name4);
    delete_test_object(options, TEST_BUCKET_NAME, object_name5);
    delete_test_object(options, TEST_BUCKET_NAME, object_name6);
    delete_test_object(options, TEST_BUCKET_NAME, object_name7);
    delete_test_object(options, TEST_BUCKET_NAME, object_name8);
    delete_test_object(options, TEST_BUCKET_NAME, object_name9);
    delete_test_object(options, TEST_BUCKET_NAME, object_name10);

    /* delete test bucket */
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    cos_delete_bucket(options, &bucket, &resp_headers);

    aos_pool_destroy(p);
}

void test_put_object_from_buffer(CuTest *tc)
{
    aos_pool_t *p = NULL;
    char *object_name = "cos_test_put_object.ts";
    char *str = "test cos c sdk";
    aos_status_t *s = NULL;
    int is_cname = 0;
    aos_string_t bucket;
    aos_string_t object;
    aos_table_t *headers = NULL;
    aos_table_t *head_headers = NULL;
    aos_table_t *head_resp_headers = NULL;
    cos_request_options_t *options = NULL;

    /* test put object */
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    headers = aos_table_make(p, 1);
    apr_table_set(headers, "x-cos-meta-author", "cos");
    s = create_test_object(options, TEST_BUCKET_NAME, object_name, str, headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, headers);

    aos_pool_destroy(p);

    /* head object */
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    init_test_request_options(options, is_cname);
    s = cos_head_object(options, &bucket, &object, 
                        head_headers, &head_resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, head_resp_headers);
    
    char *author = (char*)(apr_table_get(head_resp_headers, "x-cos-meta-author"));
    CuAssertStrEquals(tc, "cos", author);

    // content_type = (char*)(apr_table_get(head_resp_headers, COS_CONTENT_TYPE));
    // CuAssertStrEquals(tc, "video/MP2T", content_type);

    printf("test_put_object_from_buffer ok\n");
}

void test_put_object_from_buffer_with_default_content_type(CuTest *tc)
{
    aos_pool_t *p = NULL;
    char *object_name = "put_object_from_buffer_with_default_content_type";
    char *str = "test cos c sdk";
    aos_status_t *s = NULL;
    int is_cname = 0;
    aos_string_t bucket;
    aos_string_t object;
    aos_table_t *headers = NULL;
    aos_table_t *head_headers = NULL;
    aos_table_t *head_resp_headers = NULL;
    char *content_type = NULL;
    cos_request_options_t *options = NULL;

    /* test put object */
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    headers = aos_table_make(p, 1);
    apr_table_set(headers, "x-cos-meta-author", "cos");
    s = create_test_object(options, TEST_BUCKET_NAME, object_name, str, headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, headers);

    aos_pool_destroy(p);

    /* head object */
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    init_test_request_options(options, is_cname);
    s = cos_head_object(options, &bucket, &object, 
                        head_headers, &head_resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, head_resp_headers);
    
    content_type = (char*)(apr_table_get(head_resp_headers, COS_CONTENT_TYPE));
    CuAssertStrEquals(tc, default_content_type, content_type);

    printf("test_put_object_from_buffer_with_default_content_type ok\n");
}


void test_put_object_from_file(CuTest *tc)
{
    aos_pool_t *p = NULL;
    char *object_name = "video_1.ts";
    char *filename = __FILE__;
    aos_string_t bucket;
    aos_string_t object;
    aos_status_t *s = NULL;
    cos_request_options_t *options = NULL;
    int is_cname = 0;
    aos_table_t *headers = NULL;
    aos_table_t *head_headers = NULL;
    aos_table_t *head_resp_headers = NULL;
    char *content_type = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    headers = aos_table_make(p, 5);
    s = create_test_object_from_file(options, TEST_BUCKET_NAME, 
            object_name, filename, headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, headers);

    aos_pool_destroy(p);

    /* head object */
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    init_test_request_options(options, is_cname);
    s = cos_head_object(options, &bucket, &object, 
                        head_headers, &head_resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, head_resp_headers);
    
    content_type = (char*)(apr_table_get(head_resp_headers, COS_CONTENT_TYPE));
    CuAssertStrEquals(tc, default_content_type, content_type);

    printf("test_put_object_from_file ok\n");
}

void test_put_object_with_large_length_header(CuTest *tc)
{
    aos_pool_t *p = NULL;
    char *object_name = "video_2.ts";
    char *filename = __FILE__;
    aos_status_t *s = NULL;
    cos_request_options_t *options = NULL;
    int is_cname = 0;
    int i = 0;
    int header_length = 0;
    aos_table_t *headers = NULL;
    char *user_meta = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);

    header_length = 1024 * 5;
    user_meta = (char*)calloc(header_length, 1);
    for (; i < header_length - 1; i++) {
        user_meta[i] = 'a';
    }
    user_meta[header_length - 1] = '\0';
    headers = aos_table_make(p, 2);
    apr_table_set(headers, "x-cos-meta-user-meta", user_meta);
    s = create_test_object_from_file(options, TEST_BUCKET_NAME, 
            object_name, filename, headers);
    CuAssertIntEquals(tc, 400, s->code);
    CuAssertStrEquals(tc, "MetaTooLong", s->error_msg);
    CuAssertStrEquals(tc, "InvalidArgument", s->error_code);

    CuAssertPtrNotNull(tc, headers);

    free(user_meta);
    aos_pool_destroy(p);

    printf("test_put_object_with_large_length_header_back_bound ok\n");
}

void test_put_object_from_file_with_content_type(CuTest *tc)
{
    aos_pool_t *p = NULL;
    char *object_name = "cos_test_put_object_from_file2.txt";
    char *filename = __FILE__;
    aos_string_t bucket;
    aos_string_t object;
    aos_status_t *s = NULL;
    cos_request_options_t *options = NULL;
    int is_cname = 0;
    aos_table_t *headers = NULL;
    aos_table_t *head_headers = NULL;
    aos_table_t *head_resp_headers = NULL;
    char *content_type = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    headers = aos_table_make(options->pool, 1);
    apr_table_set(headers, COS_CONTENT_TYPE, "image/jpeg");

    s = create_test_object_from_file(options, TEST_BUCKET_NAME, 
            object_name, filename, headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, headers);

    aos_pool_destroy(p);

    /* head object */
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    init_test_request_options(options, is_cname);
    s = cos_head_object(options, &bucket, &object, 
                        head_headers, &head_resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, head_resp_headers);
    
    content_type = (char*)(apr_table_get(head_resp_headers, COS_CONTENT_TYPE));
    CuAssertStrEquals(tc, "image/jpeg;charset=UTF-8", content_type);

    printf("test_put_object_from_file ok\n");
}

void test_get_object_to_buffer(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    char *object_name = "cos_test_put_object.ts";
    aos_string_t object;
    int is_cname = 0;
    cos_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *params = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    aos_list_t buffer;
    aos_buf_t *content = NULL;
    char *expect_content = "test cos c sdk";
    char *buf = NULL;
    int64_t len = 0;
    int64_t size = 0;
    int64_t pos = 0;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    aos_list_init(&buffer);

    /* test get object to buffer */
    s = cos_get_object_to_buffer(options, &bucket, &object, headers, 
                                 params, &buffer, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, resp_headers);

    /* get buffer len */
    len = aos_buf_list_len(&buffer);

    buf = aos_pcalloc(p, (apr_size_t)(len + 1));
    buf[len] = '\0';

    /* copy buffer content to memory */
    aos_list_for_each_entry(aos_buf_t, content, &buffer, node) {
        size = aos_buf_size(content);
        memcpy(buf + pos, content->pos, (size_t)size);
        pos += size;
    }

    CuAssertStrEquals(tc, expect_content, buf);
//    content_type = (char*)(apr_table_get(resp_headers, COS_CONTENT_TYPE));
//    CuAssertStrEquals(tc, "video/MP2T", content_type);
    aos_pool_destroy(p);

    printf("test_get_object_to_buffer ok\n");
}

void test_get_object_to_buffer_with_range(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    char *object_name = "cos_test_put_object.ts";
    aos_string_t object;
    int is_cname = 0;
    cos_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *params = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    aos_list_t buffer;
    aos_buf_t *content = NULL;
    char *expect_content = "cos c sdk";
    char *buf = NULL;
    int64_t len = 0;
    int64_t size = 0;
    int64_t pos = 0;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    headers = aos_table_make(p, 1);
    apr_table_set(headers, "Range", " bytes=5-13");
    aos_list_init(&buffer);

    /* test get object to buffer */
    s = cos_get_object_to_buffer(options, &bucket, &object, headers, 
                                 params, &buffer, &resp_headers);
    CuAssertIntEquals(tc, 206, s->code);
    CuAssertPtrNotNull(tc, resp_headers);

    /* get buffer len */
    len = aos_buf_list_len(&buffer);

    buf = aos_pcalloc(p, (apr_size_t)(len + 1));
    buf[len] = '\0';

    /* copy buffer content to memory */
    aos_list_for_each_entry(aos_buf_t, content, &buffer, node) {
        size = aos_buf_size(content);
        memcpy(buf + pos, content->pos, (size_t)size);
        pos += size;
    }

    CuAssertStrEquals(tc, expect_content, buf);
    aos_pool_destroy(p);

    printf("test_get_object_to_buffer_with_range ok\n");
}

void test_get_object_to_file(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    char *object_name = "cos_test_put_object_from_file2.txt";
    aos_string_t object;
    char *filename = "cos_test_get_object_to_file";
    char *source_filename = __FILE__;
    aos_string_t file;
    cos_request_options_t *options = NULL; 
    int is_cname = 0;
    aos_table_t *headers = NULL;
    aos_table_t *params = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    char *content_type = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    aos_str_set(&file, filename);

    /* test get object to file */
    s = cos_get_object_to_file(options, &bucket, &object, headers, 
                               params, &file, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertIntEquals(tc, get_file_size(source_filename), get_file_size(filename));
    content_type = (char*)(apr_table_get(resp_headers, COS_CONTENT_TYPE));
    CuAssertStrEquals(tc, "image/jpeg;charset=UTF-8", content_type);
    CuAssertPtrNotNull(tc, resp_headers);

    remove(filename);
    aos_pool_destroy(p);

    printf("test_get_object_to_file ok\n");
}

void test_head_object(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    char *object_name = "cos_test_put_object.ts";
    int is_cname = 0;
    cos_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;
    char *user_meta = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    headers = aos_table_make(p, 0);

    /* test head object */
    s = cos_head_object(options, &bucket, &object, headers, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, resp_headers);
    
    user_meta = (char*)(apr_table_get(resp_headers, "x-cos-meta-author"));
    CuAssertStrEquals(tc, "cos", user_meta);

    aos_pool_destroy(p);

    printf("test_head_object ok\n");
}

void test_head_object_with_not_exist(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    char *object_name = "not_exist.object";
    int is_cname = 0;
    cos_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    headers = aos_table_make(p, 0);

    /* test head object */
    s = cos_head_object(options, &bucket, &object, headers, &resp_headers);
    CuAssertIntEquals(tc, 404, s->code);
    CuAssertStrEquals(tc, "UnknownError", s->error_code);
    CuAssertTrue(tc, NULL == s->error_msg);
    CuAssertTrue(tc, 0 != strlen(s->req_id));
    CuAssertPtrNotNull(tc, resp_headers);

    aos_pool_destroy(p);

    printf("test_head_object ok\n");
}

void test_delete_object(CuTest *tc)
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    char *object_name = "cos_test_put_object.ts";
    aos_string_t object;
    int is_cname = 0;
    cos_request_options_t *options = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *s = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
 
    /* test delete object */
    s = cos_delete_object(options, &bucket, &object, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, resp_headers);

    aos_pool_destroy(p);

    printf("test_delete_object ok\n");
}

void test_object_by_url(CuTest *tc)
{
    aos_pool_t *p = NULL;
    cos_request_options_t *options = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *params = NULL;
    aos_table_t *resp_headers = NULL;
    aos_http_request_t *req = NULL;
    aos_list_t buffer;
    aos_status_t *s = NULL;
    aos_string_t url;
    apr_time_t now;
    int two_minute = 120;
    int is_cname = 0;
    char *object_name = "cos_test_object_by_url";
    aos_string_t bucket;
    aos_string_t object;
    char *str = "test cos c sdk for object url api";
    char *filename = __FILE__;
    char *filename_download = "cos_test_object_by_url";
    aos_string_t file;
    int64_t effective_time;
    char *url_str = NULL;
    aos_buf_t *content = NULL;
   
    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_test_request_options(options, is_cname);
    req = aos_http_request_create(p);
    headers = aos_table_make(p, 0);
    aos_str_set(&bucket, TEST_BUCKET_NAME);
    aos_str_set(&object, object_name);
    aos_str_set(&file, filename);
    aos_list_init(&buffer);

    now = apr_time_now();
    effective_time = now / 1000000 + two_minute;

    /* test effective url for put_object_from_buffer */
    req->method = HTTP_PUT;
    url_str = gen_test_signed_url(options, TEST_BUCKET_NAME, 
                                  object_name, effective_time, req);
    aos_str_set(&url, url_str);
    aos_list_init(&buffer);
    content = aos_buf_pack(p, str, strlen(str));
    aos_list_add_tail(&content->node, &buffer);
    s = cos_put_object_from_buffer_by_url(options, &url, 
            &buffer, headers, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, resp_headers);

    /* test effective url for put_object_from_file */
    resp_headers = NULL;
    s = cos_put_object_from_file_by_url(options, &url, &file, 
            headers, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, resp_headers);

    /* test effective url for get_object_to_buffer */
    req->method = HTTP_GET;
    url_str = gen_test_signed_url(options, TEST_BUCKET_NAME, 
                                  object_name, effective_time, req);
    aos_str_set(&url, url_str);
    s = cos_get_object_to_buffer_by_url(options, &url, headers, params,
            &buffer, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);

    /* test effective url for get_object_to_file */
    resp_headers = NULL;
    aos_str_set(&file, filename_download);
    s = cos_get_object_to_file_by_url(options, &url, headers, 
            headers, &file, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertIntEquals(tc, get_file_size(filename), get_file_size(filename_download));
    CuAssertPtrNotNull(tc, resp_headers);

    /* test effective url for head_object */
    resp_headers = NULL;
    req->method = HTTP_HEAD;
    url_str = gen_test_signed_url(options, TEST_BUCKET_NAME, 
                                  object_name, effective_time, req);
    aos_str_set(&url, url_str);
    s = cos_head_object_by_url(options, &url, headers, &resp_headers);
    CuAssertIntEquals(tc, 200, s->code);
    CuAssertPtrNotNull(tc, resp_headers);

    remove(filename_download);
    aos_pool_destroy(p);

    printf("test_object_by_url ok\n");
}

CuSuite *test_cos_object()
{
    CuSuite* suite = CuSuiteNew();   

    SUITE_ADD_TEST(suite, test_object_setup);
    SUITE_ADD_TEST(suite, test_put_object_from_buffer);
    SUITE_ADD_TEST(suite, test_put_object_from_file);
    SUITE_ADD_TEST(suite, test_get_object_to_buffer);
    SUITE_ADD_TEST(suite, test_get_object_to_buffer_with_range);
    SUITE_ADD_TEST(suite, test_put_object_from_file_with_content_type);
    SUITE_ADD_TEST(suite, test_put_object_from_buffer_with_default_content_type);
    SUITE_ADD_TEST(suite, test_put_object_with_large_length_header);
    SUITE_ADD_TEST(suite, test_get_object_to_file);
    SUITE_ADD_TEST(suite, test_head_object);
    SUITE_ADD_TEST(suite, test_head_object_with_not_exist);
    SUITE_ADD_TEST(suite, test_object_by_url);
    SUITE_ADD_TEST(suite, test_delete_object);
    SUITE_ADD_TEST(suite, test_object_cleanup); 
    
    return suite;
}
