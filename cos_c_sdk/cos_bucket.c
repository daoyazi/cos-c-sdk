#include "aos_log.h"
#include "aos_define.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "cos_auth.h"
#include "cos_util.h"
#include "cos_json.h"
#include "cos_xml.h"
#include "cos_api.h"

aos_status_t *cos_create_bucket(const cos_request_options_t *options,
                                const aos_string_t *bucket,
                                cos_acl_e cos_acl,
                                aos_table_t **resp_headers)
{
    const char *cos_acl_str = NULL;
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *query_params = NULL;

    query_params = aos_table_create_if_null(options, query_params, 0);

    //init headers
    headers = aos_table_create_if_null(options, headers, 1);
    cos_acl_str = get_cos_acl_str(cos_acl);
    if (cos_acl_str) {
        apr_table_set(headers, COS_CANNONICALIZED_HEADER_ACL, cos_acl_str);
    }

    cos_init_bucket_request(options, bucket, HTTP_PUT, &req,
                            query_params, headers, &resp);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_delete_bucket(const cos_request_options_t *options,
                                const aos_string_t *bucket,
                                aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;
    aos_table_t *headers = NULL;

    headers = aos_table_create_if_null(options, headers, 0);
    query_params = aos_table_create_if_null(options, query_params, 0);

    cos_init_bucket_request(options, bucket, HTTP_DELETE, &req,
                            query_params, headers, &resp);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_put_bucket_acl(const cos_request_options_t *options,
                                 const aos_string_t *bucket,
                                 cos_acl_e cos_acl,
                                 aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;
    aos_table_t *headers = NULL;
    const char *cos_acl_str = NULL;

    query_params = aos_table_create_if_null(options, query_params, 1);
    apr_table_add(query_params, COS_ACL, "");

    headers = aos_table_create_if_null(options, headers, 1);
    cos_acl_str = get_cos_acl_str(cos_acl);
    if (cos_acl_str) {
        apr_table_set(headers, COS_CANNONICALIZED_HEADER_ACL, cos_acl_str);
    }

    cos_init_bucket_request(options, bucket, HTTP_PUT, &req,
                            query_params, headers, &resp);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_get_bucket_acl(const cos_request_options_t *options,
                                 const aos_string_t *bucket,
                                 aos_string_t *cos_acl,
                                 aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    int res;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;
    aos_table_t *headers = NULL;

    query_params = aos_table_create_if_null(options, query_params, 1);
    apr_table_add(query_params, COS_ACL, "");

    headers = aos_table_create_if_null(options, headers, 0);

    cos_init_bucket_request(options, bucket, HTTP_GET, &req,
                            query_params, headers, &resp);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;
    if (!aos_status_is_ok(s)) {
        return s;
    }

    res = cos_acl_parse_from_body(options->pool, &resp->body, cos_acl);
    if (res != AOSE_OK) {
        aos_json_error_status_set(s, res);
    }

    return s;
}

aos_status_t *cos_list_object(const cos_request_options_t *options,
                              const aos_string_t *bucket,
                              cos_list_object_params_t *params,
                              aos_table_t **resp_headers)
{
    int res;
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;
    aos_table_t *headers = NULL;

    //init query_params
    query_params = aos_table_create_if_null(options, query_params, 4);
    apr_table_add(query_params, COS_PREFIX, params->prefix.data);
    apr_table_add(query_params, COS_DELIMITER, params->delimiter.data);
    apr_table_add(query_params, COS_MARKER, params->marker.data);
    aos_table_add_int(query_params, COS_MAX_KEYS, params->max_ret);

    //init headers
    headers = aos_table_create_if_null(options, headers, 0);

    cos_init_bucket_request(options, bucket, HTTP_GET, &req,
                            query_params, headers, &resp);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;
    if (!aos_status_is_ok(s)) {
        return s;
    }

    res = cos_list_objects_parse_from_body(options->pool, &resp->body,
            &params->object_list, &params->common_prefix_list,
            &params->next_marker, &params->truncated);
    if (res != AOSE_OK) {
        aos_json_error_status_set(s, res);
    }

    return s;
}

aos_status_t *cos_delete_objects(const cos_request_options_t *options,
                                 const aos_string_t *bucket,
                                 aos_list_t *object_list,
                                 int is_quiet,
                                 aos_table_t **resp_headers,
                                 aos_list_t *deleted_object_list)
{
    int res;
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *query_params = NULL;
    aos_list_t body;
//    unsigned char *md5 = NULL;
    char *buf = NULL;
    int64_t body_len;
    //init query_params
    query_params = aos_table_create_if_null(options, query_params, 1);
    apr_table_add(query_params, COS_DELETE, "");

    //init headers
    headers = aos_table_create_if_null(options, headers, 1);
    apr_table_add(headers, COS_CONTENT_TYPE, COS_MULTIPART_CONTENT_TYPE);

    cos_init_bucket_request(options, bucket, HTTP_POST, &req,
                            query_params, headers, &resp);

    build_delete_objects_body(options->pool, object_list, is_quiet, &body);

    //add Content-MD5
    body_len = aos_buf_list_len(&body);
    buf = aos_buf_list_content(options->pool, &body);
    unsigned char *md5 = aos_md5(options->pool, buf, (apr_size_t)body_len);
    char buff[33];
    buff[32] = 0;
    int i = 0;
    for(i=0;i<16;i++)
    {
        sprintf(buff+i*2, "%02x", md5[i]);
    }
    apr_table_addn(headers, COS_CONTENT_MD5, buff);


    cos_write_request_body_from_buffer(&body, req);
    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    if (is_quiet) {
        return s;
    }

    if (!aos_status_is_ok(s)) {
        return s;
    }

    res = cos_delete_objects_parse_from_body(options->pool, &resp->body,
                                             deleted_object_list);
    if (res != AOSE_OK) {
        aos_json_error_status_set(s, res);
    }

    return s;
}

aos_status_t *cos_delete_objects_by_prefix(cos_request_options_t *options,
                                           const aos_string_t *bucket,
                                           const aos_string_t *prefix)
{
    aos_pool_t *subpool = NULL;
    aos_pool_t *parent_pool = NULL;
    int is_quiet = 1;
    aos_status_t *s = NULL;
    aos_status_t *ret = NULL;
    cos_list_object_params_t *params = NULL;
    int list_object_count = 0;

    parent_pool = options->pool;
    params = cos_create_list_object_params(parent_pool);
    if (prefix->data == NULL) {
        aos_str_set(&params->prefix, "");
    } else {
        aos_str_set(&params->prefix, prefix->data);
    }
    while (params->truncated) {
        aos_table_t *list_object_resp_headers = NULL;
        aos_list_t object_list;
        aos_list_t deleted_object_list;
        cos_list_object_content_t *list_content = NULL;
        aos_table_t *delete_objects_resp_headers = NULL;
        char *key = NULL;

        aos_pool_create(&subpool, parent_pool);
        options->pool = subpool;
        list_object_count = 0;
        aos_list_init(&object_list);
        s = cos_list_object(options, bucket, params, &list_object_resp_headers);
        if (!aos_status_is_ok(s)) {
            ret = aos_status_dup(parent_pool, s);
            aos_pool_destroy(subpool);
            options->pool = parent_pool;
            return ret;
        }

        aos_list_for_each_entry(cos_list_object_content_t, list_content, &params->object_list, node) {
            cos_object_key_t *object_key = cos_create_cos_object_key(parent_pool);
            key = apr_psprintf(parent_pool, "%.*s", list_content->key.len,
                               list_content->key.data);
            aos_str_set(&object_key->key, key);
            aos_list_add_tail(&object_key->node, &object_list);
            list_object_count += 1;
        }
        if (list_object_count == 0)
        {
            ret = aos_status_dup(parent_pool, s);
            aos_pool_destroy(subpool);
            options->pool = parent_pool;
            return ret;
        }
        aos_pool_destroy(subpool);

        aos_list_init(&deleted_object_list);
        aos_pool_create(&subpool, parent_pool);
        options->pool = subpool;
        s = cos_delete_objects(options, bucket, &object_list, is_quiet,
                               &delete_objects_resp_headers, &deleted_object_list);
        if (!aos_status_is_ok(s)) {
            ret = aos_status_dup(parent_pool, s);
            aos_pool_destroy(subpool);
            options->pool = parent_pool;
            return ret;
        }
        if (!params->truncated) {
            ret = aos_status_dup(parent_pool, s);
        }

        aos_pool_destroy(subpool);

        aos_list_init(&params->object_list);
        if (params->next_marker.data) {
            aos_str_set(&params->marker, params->next_marker.data);
        }
    }
    options->pool = parent_pool;
    return ret;
}
