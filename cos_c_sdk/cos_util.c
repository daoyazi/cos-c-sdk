#include "aos_string.h"
#include "aos_util.h"
#include "aos_log.h"
#include "aos_status.h"
#include "cos_auth.h"
#include "cos_util.h"

#ifndef WIN32
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#endif

static char *default_content_type = "application/octet-stream;charset=UTF-8";

static cos_content_type_t file_type[] = {
    {"html", "text/html"},
    {"htm", "text/html"},
    {"shtml", "text/html"},
    {"css", "text/css"},
    {"xml", "text/xml"},
    {"gif", "image/gif"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"js", "application/x-javascript"},
    {"atom", "application/atom+xml"},
    {"rss", "application/rss+xml"},
    {"mml", "text/mathml"},
    {"txt", "text/plain"},
    {"jad", "text/vnd.sun.j2me.app-descriptor"},
    {"wml", "text/vnd.wap.wml"},
    {"htc", "text/x-component"},
    {"png", "image/png"},
    {"tif", "image/tiff"},
    {"tiff", "image/tiff"},
    {"wbmp", "image/vnd.wap.wbmp"},
    {"ico", "image/x-icon"},
    {"jng", "image/x-jng"},
    {"bmp", "image/x-ms-bmp"},
    {"svg", "image/svg+xml"},
    {"svgz", "image/svg+xml"},
    {"webp", "image/webp"},
    {"jar", "application/java-archive"},
    {"war", "application/java-archive"},
    {"ear", "application/java-archive"},
    {"hqx", "application/mac-binhex40"},
    {"doc ", "application/msword"},
    {"pdf", "application/pdf"},
    {"ps", "application/postscript"},
    {"eps", "application/postscript"},
    {"ai", "application/postscript"},
    {"rtf", "application/rtf"},
    {"xls", "application/vnd.ms-excel"},
    {"ppt", "application/vnd.ms-powerpoint"},
    {"wmlc", "application/vnd.wap.wmlc"},
    {"kml", "application/vnd.google-earth.kml+xml"},
    {"kmz", "application/vnd.google-earth.kmz"},
    {"7z", "application/x-7z-compressed"},
    {"cco", "application/x-cocoa"},
    {"jardiff", "application/x-java-archive-diff"},
    {"jnlp", "application/x-java-jnlp-file"},
    {"run", "application/x-makeself"},
    {"pl", "application/x-perl"},
    {"pm", "application/x-perl"},
    {"prc", "application/x-pilot"},
    {"pdb", "application/x-pilot"},
    {"rar", "application/x-rar-compressed"},
    {"rpm", "application/x-redhat-package-manager"},
    {"sea", "application/x-sea"},
    {"swf", "application/x-shockwave-flash"},
    {"sit", "application/x-stuffit"},
    {"tcl", "application/x-tcl"},
    {"tk", "application/x-tcl"},
    {"der", "application/x-x509-ca-cert"},
    {"pem", "application/x-x509-ca-cert"},
    {"crt", "application/x-x509-ca-cert"},
    {"xpi", "application/x-xpinstall"},
    {"xhtml", "application/xhtml+xml"},
    {"zip", "application/zip"},
    {"wgz", "application/x-nokia-widget"},
    {"bin", "application/octet-stream"},
    {"exe", "application/octet-stream"},
    {"dll", "application/octet-stream"},
    {"deb", "application/octet-stream"},
    {"dmg", "application/octet-stream"},
    {"eot", "application/octet-stream"},
    {"iso", "application/octet-stream"},
    {"img", "application/octet-stream"},
    {"msi", "application/octet-stream"},
    {"msp", "application/octet-stream"},
    {"msm", "application/octet-stream"},
    {"mid", "audio/midi"},
    {"midi", "audio/midi"},
    {"kar", "audio/midi"},
    {"mp3", "audio/mpeg"},
    {"ogg", "audio/ogg"},
    {"m4a", "audio/x-m4a"},
    {"ra", "audio/x-realaudio"},
    {"3gpp", "video/3gpp"},
    {"3gp", "video/3gpp"},
    {"mp4", "video/mp4"},
    {"mpeg", "video/mpeg"},
    {"mpg", "video/mpeg"},
    {"mov", "video/quicktime"},
    {"webm", "video/webm"},
    {"flv", "video/x-flv"},
    {"m4v", "video/x-m4v"},
    {"mng", "video/x-mng"},
    {"asx", "video/x-ms-asf"},
    {"asf", "video/x-ms-asf"},
    {"wmv", "video/x-ms-wmv"},
    {"avi", "video/x-msvideo"},
    {"ts", "video/MP2T"},
    {"m3u8", "application/x-mpegURL"},
    {"apk", "application/vnd.android.package-archive"},
    {NULL, NULL}
};

static int starts_with(const aos_string_t *str, const char *prefix) {
    uint32_t i;
    if(NULL != str && prefix && str->len > 0 && strlen(prefix)) {
        for(i = 0; str->data[i] != '\0' && prefix[i] != '\0'; i++) {
            if(prefix[i] != str->data[i]) return 0;
        }
        return 1;
    }
    return 0;
}

static void generate_proto(const cos_request_options_t *options, 
                           aos_http_request_t *req) 
{
    const char *proto;
    proto = starts_with(&options->config->endpoint, AOS_HTTP_PREFIX) ? 
            AOS_HTTP_PREFIX : "";
    proto = starts_with(&options->config->endpoint, AOS_HTTPS_PREFIX) ? 
            AOS_HTTPS_PREFIX : proto;
    req->proto = apr_psprintf(options->pool, "%.*s", (int)strlen(proto), proto);
}

int is_valid_ip(const char *str)
{
    if (INADDR_NONE == inet_addr(str) || INADDR_ANY == inet_addr(str)) {
        return 0;
    }
    return 1;
}

cos_config_t *cos_config_create(aos_pool_t *p)
{
    return (cos_config_t *)aos_pcalloc(p, sizeof(cos_config_t));
}

cos_request_options_t *cos_request_options_create(aos_pool_t *p)
{
    int s;
    cos_request_options_t *options;

    if(p == NULL) {
        if ((s = aos_pool_create(&p, NULL)) != APR_SUCCESS) {
            aos_fatal_log("aos_pool_create failure.");
            return NULL;
        }
    }

    options = (cos_request_options_t *)aos_pcalloc(p, sizeof(cos_request_options_t));
    options->pool = p;

    return options;
}

void cos_get_object_uri(const cos_request_options_t *options,
                        const aos_string_t *bucket,
                        const aos_string_t *object,
                        aos_http_request_t *req)
{
    int32_t proto_len;
    const char *raw_endpoint_str;
    aos_string_t raw_endpoint;

    generate_proto(options, req);

    proto_len = strlen(req->proto);

    req->resource = apr_psprintf(options->pool, "%.*s/%.*s", 
                                 bucket->len, bucket->data, 
                                 object->len, object->data);

    raw_endpoint_str = aos_pstrdup(options->pool, 
            &options->config->endpoint) + proto_len;
    raw_endpoint.len = options->config->endpoint.len - proto_len;
    raw_endpoint.data = options->config->endpoint.data + proto_len;

    if (options->config->is_cname) {
        req->host = apr_psprintf(options->pool, "%.*s",
                raw_endpoint.len, raw_endpoint.data);
        req->uri = object->data;
    } else if (is_valid_ip(raw_endpoint_str)) {
        req->host = apr_psprintf(options->pool, "%.*s",
                raw_endpoint.len, raw_endpoint.data);
        req->uri = apr_psprintf(options->pool, "%.*s/%.*s",
                                bucket->len, bucket->data, 
                                object->len, object->data);
    } else {
        req->host = apr_psprintf(options->pool, "%.*s.%.*s",
                bucket->len, bucket->data, 
                raw_endpoint.len, raw_endpoint.data);
        req->uri = object->data;
    }
}

void cos_get_bucket_uri(const cos_request_options_t *options, 
                        const aos_string_t *bucket,
                        aos_http_request_t *req)
{
    int32_t proto_len;
    const char *raw_endpoint_str;
    aos_string_t raw_endpoint;

    generate_proto(options, req);

    proto_len = strlen(req->proto);
    raw_endpoint_str = aos_pstrdup(options->pool, 
            &options->config->endpoint) + proto_len;
    raw_endpoint.len = options->config->endpoint.len - proto_len;
    raw_endpoint.data = options->config->endpoint.data + proto_len;

    if (is_valid_ip(raw_endpoint_str)) {
        req->resource = apr_psprintf(options->pool, "%.*s", 
                bucket->len, bucket->data);
    } else {
        req->resource = apr_psprintf(options->pool, "%.*s/", 
                bucket->len, bucket->data);
    }
    
    if (options->config->is_cname || 
        is_valid_ip(raw_endpoint_str))
    {
        req->host = apr_psprintf(options->pool, "%.*s", 
                raw_endpoint.len, raw_endpoint.data);
        req->uri = apr_psprintf(options->pool, "%.*s", bucket->len, 
                                bucket->data);
    } else {
        req->host = apr_psprintf(options->pool, "%.*s.%.*s", 
                bucket->len, bucket->data, 
                raw_endpoint.len, raw_endpoint.data);
        req->uri = apr_psprintf(options->pool, "%s", "");
    }
}

void cos_write_request_body_from_buffer(aos_list_t *buffer, 
                                        aos_http_request_t *req)
{
    aos_list_movelist(buffer, &req->body);
    req->body_len = aos_buf_list_len(&req->body);
}

int cos_write_request_body_from_file(aos_pool_t *p, 
                                     const aos_string_t *filename, 
                                     aos_http_request_t *req)
{
    int res = AOSE_OK;
    aos_file_buf_t *fb = aos_create_file_buf(p);
    res = aos_open_file_for_all_read(p, filename->data, fb);
    if (res != AOSE_OK) {
        aos_error_log("Open read file fail, filename:%s\n", filename->data);
        return res;
    }

    req->body_len = fb->file_last;
    req->file_path = filename->data;
    req->file_buf = fb;
    req->type = BODY_IN_FILE;
    req->read_body = aos_read_http_body_file;

    return res;
}

int cos_write_request_body_from_upload_file(aos_pool_t *p, 
                                            cos_upload_file_t *upload_file, 
                                            aos_http_request_t *req)
{
    int res = AOSE_OK;
    aos_file_buf_t *fb = aos_create_file_buf(p);
    res = aos_open_file_for_range_read(p, upload_file->filename.data, 
            upload_file->file_pos, upload_file->file_last, fb);
    if (res != AOSE_OK) {
        aos_error_log("Open read file fail, filename:%s\n", 
                      upload_file->filename.data);
        return res;
    }

    req->body_len = fb->file_last - fb->file_pos;
    req->file_path = upload_file->filename.data;
    req->file_buf = fb;
    req->type = BODY_IN_FILE;
    req->read_body = aos_read_http_body_file;

    return res;
}

void cos_init_read_response_body_to_buffer(aos_list_t *buffer, 
                                           aos_http_response_t *resp)
{
    aos_list_movelist(&resp->body, buffer);
}

int cos_init_read_response_body_to_file(aos_pool_t *p, 
                                        const aos_string_t *filename, 
                                        aos_http_response_t *resp)
{
    int res = AOSE_OK;
    aos_file_buf_t *fb = aos_create_file_buf(p);
    res = aos_open_file_for_write(p, filename->data, fb);
    if (res != AOSE_OK) {
        aos_error_log("Open write file fail, filename:%s\n", filename->data);
        return res;
    }
    resp->file_path = filename->data;
    resp->file_buf = fb;
    resp->write_body = aos_write_http_body_file;
    resp->type = BODY_IN_FILE;

    return res;
}

void *cos_create_api_result_content(aos_pool_t *p, size_t size)
{
    void *result_content = aos_palloc(p, size);
    if (NULL == result_content) {
        return NULL;
    }
    
    aos_list_init((aos_list_t *)result_content);

    return result_content;
}

cos_list_object_content_t *cos_create_list_object_content(aos_pool_t *p)
{
    return (cos_list_object_content_t *)cos_create_api_result_content(
            p, sizeof(cos_list_object_content_t));
}

cos_list_object_common_prefix_t *cos_create_list_object_common_prefix(aos_pool_t *p)
{
    return (cos_list_object_common_prefix_t *)cos_create_api_result_content(
            p, sizeof(cos_list_object_common_prefix_t));
}

cos_list_multipart_upload_content_t *cos_create_list_multipart_upload_content(aos_pool_t *p)
{
    return (cos_list_multipart_upload_content_t*)cos_create_api_result_content(
            p, sizeof(cos_list_multipart_upload_content_t));
}

cos_list_part_content_t *cos_create_list_part_content(aos_pool_t *p)
{
    cos_list_part_content_t *list_part_content = NULL;
    list_part_content = (cos_list_part_content_t*)cos_create_api_result_content(p,
        sizeof(cos_list_part_content_t));

    return list_part_content;
}

cos_complete_part_content_t *cos_create_complete_part_content(aos_pool_t *p)
{
    cos_complete_part_content_t *complete_part_content = NULL;
    complete_part_content = (cos_complete_part_content_t*)cos_create_api_result_content(
            p, sizeof(cos_complete_part_content_t));

    return complete_part_content;
}

cos_list_object_params_t *cos_create_list_object_params(aos_pool_t *p)
{
    cos_list_object_params_t * params;
    params = (cos_list_object_params_t *)aos_pcalloc(
            p, sizeof(cos_list_object_params_t));
    aos_list_init(&params->object_list);
    aos_list_init(&params->common_prefix_list);
    aos_str_set(&params->prefix, "");
    aos_str_set(&params->marker, "");
    aos_str_set(&params->delimiter, "");
    params->truncated = 1;
    params->max_ret = COS_PER_RET_NUM;
    return params;
}

cos_list_upload_part_params_t *cos_create_list_upload_part_params(aos_pool_t *p)
{
    cos_list_upload_part_params_t *params;
    params = (cos_list_upload_part_params_t *)aos_pcalloc(
            p, sizeof(cos_list_upload_part_params_t));
    aos_list_init(&params->part_list);
    aos_str_set(&params->part_number_marker, "");
    params->max_ret = COS_PER_RET_NUM;
    params->truncated = 1;
    return params;
}

cos_list_multipart_upload_params_t *cos_create_list_multipart_upload_params(aos_pool_t *p)
{
    cos_list_multipart_upload_params_t *params;
    params = (cos_list_multipart_upload_params_t *)aos_pcalloc(
            p, sizeof(cos_list_multipart_upload_params_t));
    aos_list_init(&params->upload_list);
    aos_str_set(&params->prefix, "");
    aos_str_set(&params->key_marker, "");
    aos_str_set(&params->upload_id_marker, "");
    aos_str_set(&params->delimiter, "");
    params->truncated = 1;
    params->max_ret = COS_PER_RET_NUM;
    return params;
}

cos_upload_part_copy_params_t *cos_create_upload_part_copy_params(aos_pool_t *p)
{
    return (cos_upload_part_copy_params_t *)aos_pcalloc(
            p, sizeof(cos_upload_part_copy_params_t));
}

cos_upload_file_t *cos_create_upload_file(aos_pool_t *p)
{
    return (cos_upload_file_t *)aos_pcalloc(p, sizeof(cos_upload_file_t));
}

cos_object_key_t *cos_create_cos_object_key(aos_pool_t *p)
{
    return (cos_object_key_t *)aos_pcalloc(p, sizeof(cos_object_key_t));
}

void cos_set_multipart_content_type(aos_table_t *headers)
{
    const char *content_type;
    content_type = (char*)(apr_table_get(headers, COS_CONTENT_TYPE));
    content_type = content_type == NULL ? COS_MULTIPART_CONTENT_TYPE : content_type;
    apr_table_set(headers, COS_CONTENT_TYPE, content_type);
}

const char *get_cos_acl_str(cos_acl_e cos_acl)
{
    switch (cos_acl) {
        case COS_ACL_PRIVATE:
            return  "private";
        case COS_ACL_PUBLIC_READ:
            return "public-read";
        default:
            return NULL;
    }
}

void cos_init_request(const cos_request_options_t *options, 
                      http_method_e method,
                      aos_http_request_t **req, 
                      aos_table_t *params, 
                      aos_table_t *headers, 
                      aos_http_response_t **resp)
{
    *req = aos_http_request_create(options->pool);
    *resp = aos_http_response_create(options->pool);
    (*req)->method = method;
    init_sts_token_header();
    (*req)->headers = headers;
    (*req)->query_params = params;
}

void cos_init_bucket_request(const cos_request_options_t *options, 
                             const aos_string_t *bucket,
                             http_method_e method, 
                             aos_http_request_t **req, 
                             aos_table_t *params, 
                             aos_table_t *headers,
                             aos_http_response_t **resp)
{
    cos_init_request(options, method, req, params, headers, resp);
    cos_get_bucket_uri(options, bucket, *req);
}

void cos_init_object_request(const cos_request_options_t *options, 
                             const aos_string_t *bucket,
                             const aos_string_t *object, 
                             http_method_e method, 
                             aos_http_request_t **req, 
                             aos_table_t *params, 
                             aos_table_t *headers, 
                             aos_http_response_t **resp)
{
    cos_init_request(options, method, req, params, headers, resp);
    cos_get_object_uri(options, bucket, object, *req);
}

void cos_init_signed_url_request(const cos_request_options_t *options, 
                                 const aos_string_t *signed_url,
                                 http_method_e method, 
                                 aos_http_request_t **req, 
                                 aos_table_t *params, 
                                 aos_table_t *headers, 
                                 aos_http_response_t **resp)
{
    cos_init_request(options, method, req, params, headers, resp);
    (*req)->signed_url = signed_url->data;
}

aos_status_t *cos_send_request(aos_http_controller_t *ctl, 
                               aos_http_request_t *req,
                               aos_http_response_t *resp)
{
    aos_status_t *s;
    const char *reason;
    int res = AOSE_OK;

    s = aos_status_create(ctl->pool);
    res = aos_http_send_request(ctl, req, resp);

    if (res != AOSE_OK) {
        reason = aos_http_controller_get_reason(ctl);
        aos_status_set(s, res, AOS_HTTP_IO_ERROR_CODE, reason);
    } else if (!aos_http_is_ok(resp->status)) {
        s = aos_status_parse_from_body(ctl->pool, &resp->body, resp->status, s);
    } else {
        s->code = resp->status;
    }

    s->req_id = (char*)(apr_table_get(resp->headers, "x-cos-request-id"));
    if (s->req_id == NULL) {
        s->req_id = (char*)(apr_table_get(resp->headers, "x-img-request-id"));
        if (s->req_id == NULL) {
            s->req_id = "";
        }
    }

    return s;
}

aos_status_t *cos_process_request(const cos_request_options_t *options,
                                  aos_http_request_t *req, 
                                  aos_http_response_t *resp)
{
    int res = AOSE_OK;
    aos_status_t *s;

    s = aos_status_create(options->pool);
    res = cos_sign_request(req, options->config);
    if (res != AOSE_OK) {
        aos_status_set(s, res, AOS_CLIENT_ERROR_CODE, NULL);
        return s;
    }

    return cos_send_request(options->ctl, req, resp);
}

aos_status_t *cos_process_signed_request(const cos_request_options_t *options,
                                         aos_http_request_t *req, 
                                         aos_http_response_t *resp)
{
    return cos_send_request(options->ctl, req, resp);
}

void cos_get_part_size(int64_t filesize, int64_t *part_size)
{
    if (filesize > (*part_size) * COS_MAX_PART_NUM) {
        *part_size = (filesize + COS_MAX_PART_NUM - 
                      filesize % COS_MAX_PART_NUM) / COS_MAX_PART_NUM;

        aos_warn_log("Part number larger than max limit, "
                     "part size Changed to:%" APR_INT64_T_FMT "\n",
                     *part_size);
    } 
}

int part_sort_cmp(const void *a, const void *b)
{
    return (((cos_upload_part_t*)a)->part_num -
            ((cos_upload_part_t*)b)->part_num > 0 ? 1 : -1);
}

char *get_content_type_by_suffix(const char *suffix)
{
    cos_content_type_t *content_type;

    for (content_type = file_type; content_type->suffix; ++content_type) {
        if (strcasecmp(content_type->suffix, suffix) == 0)
        {
            return content_type->type;
        }
    }
    return default_content_type;
}

char *get_content_type(const char *name)
{
    char *begin;
    char *content_type = NULL;
    begin = strrchr(name, '.');
    if (begin) {
        content_type = get_content_type_by_suffix(begin + 1);
    }
    return content_type;
}

void set_content_type(const char* file_name,
                      const char* key,
                      aos_table_t *headers)
{
    char *user_content_type = NULL;
    char *content_type = NULL;
    const char *mime_key = NULL;

    mime_key = file_name == NULL ? key : file_name;

    user_content_type = (char*)apr_table_get(headers, COS_CONTENT_TYPE);
    if (NULL == user_content_type && mime_key != NULL) {
        content_type = get_content_type(mime_key);
        if (content_type) {
            apr_table_set(headers, COS_CONTENT_TYPE, content_type);
        } else {
            apr_table_set(headers, COS_CONTENT_TYPE, default_content_type);
        }
    }
}

aos_table_t* aos_table_create_if_null(const cos_request_options_t *options, 
                                      aos_table_t *table, 
                                      int table_size) 
{
    if (table == NULL) {
        table = aos_table_make(options->pool, table_size);
    }
    return table;
}
