#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "cos_auth.h"
#include "cos_util.h"
#include "cos_json.h"
#include "cos_api.h"

char *cos_gen_signed_url(const cos_request_options_t *options,
                         const aos_string_t *bucket, 
                         const aos_string_t *object,
                         int64_t expires, 
                         aos_http_request_t *req)
{
    aos_string_t signed_url;
    char *expires_str = NULL;
    aos_string_t expires_time;
    int res = AOSE_OK;

    expires_str = apr_psprintf(options->pool, "%" APR_INT64_T_FMT, expires);
    aos_str_set(&expires_time, expires_str);
    cos_get_object_uri(options, bucket, object, req);
    res = cos_get_signed_url(options, req, &expires_time, &signed_url);
    if (res != AOSE_OK) {
        return NULL;
    }
    return signed_url.data;
}

aos_status_t *cos_put_object_from_buffer(const cos_request_options_t *options,
                                         const aos_string_t *bucket, 
                                         const aos_string_t *object, 
                                         aos_list_t *buffer,
                                         aos_table_t *headers, 
                                         aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;

    headers = aos_table_create_if_null(options, headers, 2);
    set_content_type(NULL, object->data, headers);
    apr_table_add(headers, COS_EXPECT, "");

    query_params = aos_table_create_if_null(options, query_params, 0);

    cos_init_object_request(options, bucket, object, HTTP_PUT, 
                            &req, query_params, headers, &resp);
    cos_write_request_body_from_buffer(buffer, req);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_put_object_from_file(const cos_request_options_t *options,
                                       const aos_string_t *bucket, 
                                       const aos_string_t *object, 
                                       const aos_string_t *filename,
                                       aos_table_t *headers, 
                                       aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;
    int res = AOSE_OK;

    s = aos_status_create(options->pool);

    headers = aos_table_create_if_null(options, headers, 2);
    set_content_type(filename->data, object->data, headers);
    apr_table_add(headers, COS_EXPECT, "");

    query_params = aos_table_create_if_null(options, query_params, 0);

    cos_init_object_request(options, bucket, object, HTTP_PUT, &req, 
                            query_params, headers, &resp);

    res = cos_write_request_body_from_file(options->pool, filename, req);
    if (res != AOSE_OK) {
        aos_file_error_status_set(s, res);
        return s;
    }

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_get_object_to_buffer(const cos_request_options_t *options, 
                                       const aos_string_t *bucket, 
                                       const aos_string_t *object,
                                       aos_table_t *headers, 
                                       aos_table_t *params,
                                       aos_list_t *buffer, 
                                       aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;

    headers = aos_table_create_if_null(options, headers, 0);
    params = aos_table_create_if_null(options, params, 0);

    cos_init_object_request(options, bucket, object, HTTP_GET, 
                            &req, params, headers, &resp);

    s = cos_process_request(options, req, resp);
    cos_init_read_response_body_to_buffer(buffer, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_get_object_to_file(const cos_request_options_t *options,
                                     const aos_string_t *bucket, 
                                     const aos_string_t *object,
                                     aos_table_t *headers, 
                                     aos_table_t *params,
                                     aos_string_t *filename, 
                                     aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    int res = AOSE_OK;

    headers = aos_table_create_if_null(options, headers, 0);
    params = aos_table_create_if_null(options, params, 0);

    cos_init_object_request(options, bucket, object, HTTP_GET, 
                            &req, params, headers, &resp);

    s = aos_status_create(options->pool);
    res = cos_init_read_response_body_to_file(options->pool, filename, resp);
    if (res != AOSE_OK) {
        aos_file_error_status_set(s, res);
        return s;
    }

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_head_object(const cos_request_options_t *options, 
                              const aos_string_t *bucket, 
                              const aos_string_t *object,
                              aos_table_t *headers, 
                              aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;

    headers = aos_table_create_if_null(options, headers, 0);    

    query_params = aos_table_create_if_null(options, query_params, 0);

    cos_init_object_request(options, bucket, object, HTTP_HEAD, 
                            &req, query_params, headers, &resp);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_delete_object(const cos_request_options_t *options,
                                const aos_string_t *bucket, 
                                const aos_string_t *object, 
                                aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *query_params = NULL;

    headers = aos_table_create_if_null(options, headers, 0);
    query_params = aos_table_create_if_null(options, query_params, 0);

    cos_init_object_request(options, bucket, object, HTTP_DELETE, 
                            &req, query_params, headers, &resp);
    cos_get_object_uri(options, bucket, object, req);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_copy_object(const cos_request_options_t *options, 
                              const aos_string_t *source_bucket, 
                              const aos_string_t *source_object, 
                              const aos_string_t *dest_bucket, 
                              const aos_string_t *dest_object,
                              aos_table_t *headers, 
                              aos_table_t **resp_headers)
{
    char *copy_source = NULL;
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;

    headers = aos_table_create_if_null(options, headers, 2);
    query_params = aos_table_create_if_null(options, query_params, 0);

    /* init headers */
    copy_source = apr_psprintf(options->pool, "/%.*s/%.*s", 
                               source_bucket->len, source_bucket->data, 
                               source_object->len, source_object->data);
    apr_table_set(headers, COS_CANNONICALIZED_HEADER_COPY_SOURCE, copy_source);
    set_content_type(NULL, dest_object->data, headers);

    cos_init_object_request(options, dest_bucket, dest_object, HTTP_PUT, 
                            &req, query_params, headers, &resp);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_append_object_from_buffer(const cos_request_options_t *options,
                                            const aos_string_t *bucket, 
                                            const aos_string_t *object, 
                                            int64_t position,
                                            aos_list_t *buffer, 
                                            aos_table_t *headers, 
                                            aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;
    
    /* init query_params */
    query_params = aos_table_create_if_null(options, query_params, 2);
    apr_table_add(query_params, COS_APPEND, "");
    aos_table_add_int64(query_params, COS_POSITION, position);

    /* init headers */
    headers = aos_table_create_if_null(options, headers, 2);
    set_content_type(NULL, object->data, headers);
    apr_table_add(headers, COS_EXPECT, "");

    cos_init_object_request(options, bucket, object, HTTP_POST, 
                            &req, query_params, headers, &resp);
    cos_write_request_body_from_buffer(buffer, req);

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_append_object_from_file(const cos_request_options_t *options,
                                          const aos_string_t *bucket, 
                                          const aos_string_t *object, 
                                          int64_t position,
                                          const aos_string_t *append_file, 
                                          aos_table_t *headers, 
                                          aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;
    int res = AOSE_OK;

    /* init query_params */
    query_params = aos_table_create_if_null(options, query_params, 2);
    apr_table_add(query_params, COS_APPEND, "");
    aos_table_add_int64(query_params, COS_POSITION, position);
    
    /* init headers */
    headers = aos_table_create_if_null(options, headers, 2);
    set_content_type(append_file->data, object->data, headers);
    apr_table_add(headers, COS_EXPECT, "");

    cos_init_object_request(options, bucket, object, HTTP_POST, 
                            &req, query_params, headers, &resp);
    res = cos_write_request_body_from_file(options->pool, append_file, req);

    s = aos_status_create(options->pool);
    if (res != AOSE_OK) {
        aos_file_error_status_set(s, res);
        return s;
    }

    s = cos_process_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_put_object_from_buffer_by_url(const cos_request_options_t *options,
                                                const aos_string_t *signed_url, 
                                                aos_list_t *buffer, 
                                                aos_table_t *headers,
                                                aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;

    /* init query_params */
    headers = aos_table_create_if_null(options, headers, 0);
    query_params = aos_table_create_if_null(options, query_params, 0);

    cos_init_signed_url_request(options, signed_url, HTTP_PUT, 
                                &req, query_params, headers, &resp);

    cos_write_request_body_from_buffer(buffer, req);

    s = cos_process_signed_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_put_object_from_file_by_url(const cos_request_options_t *options,
                                              const aos_string_t *signed_url, 
                                              aos_string_t *filename, 
                                              aos_table_t *headers,
                                              aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;
    int res = AOSE_OK;

    s = aos_status_create(options->pool);

    headers = aos_table_create_if_null(options, headers, 0);
    query_params = aos_table_create_if_null(options, query_params, 0);

    cos_init_signed_url_request(options, signed_url, HTTP_PUT, 
                                &req, query_params, headers, &resp);
    res = cos_write_request_body_from_file(options->pool, filename, req);
    if (res != AOSE_OK) {
        aos_file_error_status_set(s, res);
        return s;
    }

    s = cos_process_signed_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_get_object_to_buffer_by_url(const cos_request_options_t *options,
                                              const aos_string_t *signed_url, 
                                              aos_table_t *headers,
                                              aos_table_t *params,
                                              aos_list_t *buffer,
                                              aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;

    headers = aos_table_create_if_null(options, headers, 0);
    params = aos_table_create_if_null(options, params, 0);
    
    cos_init_signed_url_request(options, signed_url, HTTP_GET, 
                                &req, params, headers, &resp);

    s = cos_process_signed_request(options, req, resp);
    cos_init_read_response_body_to_buffer(buffer, resp);
    *resp_headers = resp->headers;

    return s;
}

aos_status_t *cos_get_object_to_file_by_url(const cos_request_options_t *options,
                                            const aos_string_t *signed_url, 
                                            aos_table_t *headers, 
                                            aos_table_t *params,
                                            aos_string_t *filename,
                                            aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    int res = AOSE_OK;

    s = aos_status_create(options->pool);

    headers = aos_table_create_if_null(options, headers, 0);
    params = aos_table_create_if_null(options, params, 0);
 
    cos_init_signed_url_request(options, signed_url, HTTP_GET, 
                                &req, params, headers, &resp);

    res = cos_init_read_response_body_to_file(options->pool, filename, resp);
    if (res != AOSE_OK) {
        aos_file_error_status_set(s, res);
        return s;
    }

    s = cos_process_signed_request(options, req, resp);
    *resp_headers = resp->headers;
 
    return s;
}


aos_status_t *cos_head_object_by_url(const cos_request_options_t *options,
                                     const aos_string_t *signed_url, 
                                     aos_table_t *headers, 
                                     aos_table_t **resp_headers)
{
    aos_status_t *s = NULL;
    aos_http_request_t *req = NULL;
    aos_http_response_t *resp = NULL;
    aos_table_t *query_params = NULL;

    headers = aos_table_create_if_null(options, headers, 0);
    query_params = aos_table_create_if_null(options, query_params, 0);
    
    cos_init_signed_url_request(options, signed_url, HTTP_HEAD, 
                                &req, query_params, headers, &resp);

    s = cos_process_signed_request(options, req, resp);
    *resp_headers = resp->headers;

    return s;
}
