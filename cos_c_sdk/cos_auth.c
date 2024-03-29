#include "cos_auth.h"
#include "aos_log.h"
#include "cos_util.h"

static const char *g_s_cos_sub_resource_list[] = {
    "acl",
    "uploadId",
    "uploads",
    "partNumber",
    "response-content-type",
    "response-content-language",
    "response-expires",
    "response-cache-control",
    "response-content-disposition",
    "response-content-encoding",
    "append",
    "position",
    "delete",
    NULL,
};

static int is_cos_sub_resource(const char *str);
static int is_cos_canonicalized_header(const char *str);
static int cos_get_canonicalized_headers(aos_pool_t *p, 
        const aos_table_t *headers, aos_buf_t *signbuf);
static int cos_get_canonicalized_resource(aos_pool_t *p, 
        const aos_table_t *params, aos_buf_t *signbuf);

static int is_cos_sub_resource(const char *str)
{
    int i = 0;
    for ( ; g_s_cos_sub_resource_list[i]; i++) {
        if (apr_strnatcmp(g_s_cos_sub_resource_list[i], str) == 0) {
            return 1;
        }
    }
    return 0;
}

static int is_cos_canonicalized_header(const char *str)
{
    size_t len = strlen(COS_CANNONICALIZED_HEADER_PREFIX);
    return strncasecmp(str, COS_CANNONICALIZED_HEADER_PREFIX, len) == 0;
}

static int cos_get_canonicalized_headers(aos_pool_t *p, 
                                         const aos_table_t *headers, 
                                         aos_buf_t *signbuf)
{
    int pos;
    int meta_count = 0;
    int i;
    int len;
    const aos_array_header_t *tarr;
    const aos_table_entry_t *telts;
    char **meta_headers;
    const char *value;
    aos_string_t tmp_str;
    char *tmpbuf = (char*)malloc(AOS_MAX_HEADER_LEN + 1);
    if (NULL == tmpbuf) {
        aos_error_log("malloc %d memory failed.", AOS_MAX_HEADER_LEN + 1);
        return AOSE_OVER_MEMORY;
    }

    if (apr_is_empty_table(headers)) {
        free(tmpbuf);
        return AOSE_OK;
    }

    // sort user meta header
    tarr = aos_table_elts(headers);
    telts = (aos_table_entry_t*)tarr->elts;
    meta_headers = aos_pcalloc(p, tarr->nelts * sizeof(char*));
    for (pos = 0; pos < tarr->nelts; ++pos) {
        if (is_cos_canonicalized_header(telts[pos].key)) {
            aos_string_t key = aos_string(telts[pos].key);
            aos_string_tolower(&key);
            meta_headers[meta_count++] = key.data;
        }
    }
    if (meta_count == 0) {
        free(tmpbuf);
        return AOSE_OK;
    }
    aos_gnome_sort((const char **)meta_headers, meta_count);

    // sign string
    for (i = 0; i < meta_count; ++i) {
        value = apr_table_get(headers, meta_headers[i]);
        aos_str_set(&tmp_str, value);
        aos_strip_space(&tmp_str);
        len = apr_snprintf(tmpbuf, AOS_MAX_HEADER_LEN + 1, "%s:%.*s", 
                           meta_headers[i], tmp_str.len, tmp_str.data);
        if (len > AOS_MAX_HEADER_LEN) {
            free(tmpbuf);
            aos_error_log("user meta header too many, %d > %d.", 
                          len, AOS_MAX_HEADER_LEN);
            return AOSE_INVALID_ARGUMENT;
        }
        tmp_str.data = tmpbuf;
        tmp_str.len = len;
        aos_buf_append_string(p, signbuf, tmpbuf, len);
        aos_buf_append_string(p, signbuf, "\n", sizeof("\n")-1);
    }

    free(tmpbuf);
    return AOSE_OK;
}

static int cos_get_canonicalized_resource(aos_pool_t *p, 
                                          const aos_table_t *params, 
                                          aos_buf_t *signbuf)
{
    int pos;
    int subres_count = 0;
    int i;
    int len;
    char sep;
    const char *value;
    char tmpbuf[AOS_MAX_QUERY_ARG_LEN+1];
    char **subres_headers;
    const aos_array_header_t *tarr;
    const aos_table_entry_t *telts;

    if (apr_is_empty_table(params)) {
        return AOSE_OK;
    }

    // sort sub resource param
    tarr = aos_table_elts(params);
    telts = (aos_table_entry_t*)tarr->elts;
    subres_headers = aos_pcalloc(p, tarr->nelts * sizeof(char*));
    for (pos = 0; pos < tarr->nelts; ++pos) {
        if (is_cos_sub_resource(telts[pos].key)) {
            subres_headers[subres_count++] = telts[pos].key;
        }
    }
    if (subres_count == 0) {
        return AOSE_OK;
    }
    aos_gnome_sort((const char **)subres_headers, subres_count);

    // sign string
    sep = '?';
    for (i = 0; i < subres_count; ++i) {
        value = apr_table_get(params, subres_headers[i]);
        if (value != NULL && *value != '\0') {
            len = apr_snprintf(tmpbuf, sizeof(tmpbuf), "%c%s=%s", 
                    sep, subres_headers[i], value);
        } else {
            len = apr_snprintf(tmpbuf, sizeof(tmpbuf), "%c%s", 
                    sep, subres_headers[i]);
        }
        if (len >= AOS_MAX_QUERY_ARG_LEN) {
            aos_error_log("http query params too long, %s.", tmpbuf);
            return AOSE_INVALID_ARGUMENT;
        }
        aos_buf_append_string(p, signbuf, tmpbuf, len);
        sep = '&';
    }

    return AOSE_OK;
}    

int cos_get_string_to_sign(aos_pool_t *p, 
                           http_method_e method, 
                           const aos_string_t *canon_res,
                           const aos_table_t *headers, 
                           const aos_table_t *params, 
                           aos_string_t *signstr)
{
    int res;
    aos_buf_t *signbuf;
    const char *value;
    aos_str_null(signstr);

    signbuf = aos_create_buf(p, 1024);

#define signbuf_append_from_headers(KEY) do {                            \
        if ((value = apr_table_get(headers, KEY)) != NULL) {            \
            aos_buf_append_string(p, signbuf, value, strlen(value));    \
        }                                                               \
        aos_buf_append_string(p, signbuf, "\n", sizeof("\n")-1);        \
    } while (0)

#define signbuf_append(VALUE, LEN) do {                                 \
        aos_buf_append_string(p, signbuf, VALUE, LEN);                  \
        aos_buf_append_string(p, signbuf, "\n", sizeof("\n")-1);        \
    } while (0)
    
    value = aos_http_method_to_string(method);
    signbuf_append(value, strlen(value));    

    signbuf_append_from_headers(COS_CONTENT_MD5);
    signbuf_append_from_headers(COS_CONTENT_TYPE);

    // date
    if ((value = apr_table_get(headers, COS_CANNONICALIZED_HEADER_DATE)) == NULL) {
        value = apr_table_get(headers, COS_DATE);
    }
    if (NULL == value || *value == '\0') {
        aos_error_log("http header date is empty.");
        return AOSE_INVALID_ARGUMENT;
    }
    signbuf_append(value, strlen(value));

    // user meta headers
    if ((res = cos_get_canonicalized_headers(p, headers, signbuf)) != AOSE_OK) {
        return res;
    }

    // canonicalized resource
    aos_buf_append_string(p, signbuf, canon_res->data, canon_res->len);
    
    if (params != NULL && (res = cos_get_canonicalized_resource(p, params, signbuf)) != AOSE_OK) {
        return res;
    }

    // result
    signstr->data = (char *)signbuf->pos;
    signstr->len = aos_buf_size(signbuf);

    return AOSE_OK;
}

void cos_sign_headers(aos_pool_t *p, 
                      const aos_string_t *signstr, 
                      const aos_string_t *access_key_id,
                      const aos_string_t *access_key_secret, 
                      aos_table_t *headers)
{
    int b64Len;
    char *value;
    unsigned char hmac[1024];
    char b64[((32 + 1) * 4) / 3];

    HMAC_SHA256(hmac, (unsigned char *)access_key_secret->data, access_key_secret->len,
              (unsigned char *)signstr->data, signstr->len);

    // Now base-64 encode the results
    b64Len = aos_base64_encode(hmac, 32, b64);
    value = apr_psprintf(p, "COS %.*s:%.*s", access_key_id->len, access_key_id->data, b64Len, b64);
    apr_table_addn(headers, COS_AUTHORIZATION, value);

    return;
}

int cos_get_signed_headers(aos_pool_t *p, 
                           const aos_string_t *access_key_id, 
                           const aos_string_t *access_key_secret,
                           const aos_string_t* canon_res, 
                           aos_http_request_t *req)
{
    int res;
    aos_string_t signstr;

    res = cos_get_string_to_sign(p, req->method, canon_res, 
                                 req->headers, req->query_params, &signstr);
    
    if (res != AOSE_OK) {
        return res;
    }
    
    aos_debug_log("signstr:%.*s.", signstr.len, signstr.data);

    cos_sign_headers(p, &signstr, access_key_id, access_key_secret, req->headers);

    return AOSE_OK;
}

int cos_sign_request(aos_http_request_t *req, 
                     const cos_config_t *config)
{
    aos_string_t canon_res;
    char canon_buf[AOS_MAX_URI_LEN];
    char real_canon_buf[AOS_MAX_URI_LEN];
    char datestr[AOS_MAX_GMT_TIME_LEN];
    const char *value;
    int res = AOSE_OK;
    int len = 0;
    
    len = strlen(req->resource);
    if (len >= AOS_MAX_URI_LEN - 1) {
        aos_error_log("http resource too long, %s.", req->resource);
        return AOSE_INVALID_ARGUMENT;
    }

    canon_res.data = canon_buf;
    canon_res.len = apr_snprintf(canon_buf, sizeof(canon_buf), "/%s", req->resource);

    int i = 0;
    int j = 0;
    int count = 0;
    while (i < canon_res.len) {
      if (canon_buf[i]=='/') {
        count++;
        if (count > 2) {
          real_canon_buf[j++] = '%';
          real_canon_buf[j++] = '2';
          real_canon_buf[j++] = 'F';
          i++;
          continue;
        }
      }
      real_canon_buf[j] = canon_buf[i];
      j++;
      i++;
    }
    real_canon_buf[j] = 0;
    canon_res.data = real_canon_buf;
    canon_res.len = j;

    if ((value = apr_table_get(req->headers, COS_CANNONICALIZED_HEADER_DATE)) == NULL) {
        aos_get_gmt_str_time(datestr);
        apr_table_set(req->headers, COS_DATE, datestr);
    }

    res = cos_get_signed_headers(req->pool, &config->access_key_id, 
                                 &config->access_key_secret, &canon_res, req);
    return res;
}

int get_cos_request_signature(const cos_request_options_t *options, 
                              aos_http_request_t *req,
                              const aos_string_t *expires, 
                              aos_string_t *signature)
{
    aos_string_t canon_res;
    char canon_buf[AOS_MAX_URI_LEN];
    const char *value;
    aos_string_t signstr;
    int res = AOSE_OK;
    int b64Len;
    unsigned char hmac[1024];
    char b64[((32 + 1) * 4) / 3];

    canon_res.data = canon_buf;
    canon_res.len = apr_snprintf(canon_buf, sizeof(canon_buf), "/%s", req->resource);

    apr_table_set(req->headers, COS_DATE, expires->data);

    if ((res = cos_get_string_to_sign(options->pool, req->method, &canon_res, 
        req->headers, req->query_params, &signstr))!= AOSE_OK) {
        return res;
    }

    HMAC_SHA256(hmac, (unsigned char *)options->config->access_key_secret.data, 
              options->config->access_key_secret.len,
              (unsigned char *)signstr.data, signstr.len);

    b64Len = aos_base64_encode(hmac, 32, b64);
    value = apr_psprintf(options->pool, "%.*s", b64Len, b64);
    aos_str_set(signature, value);

    return res;
}

int cos_get_signed_url(const cos_request_options_t *options, 
                       aos_http_request_t *req,
                       const aos_string_t *expires, 
                       aos_string_t *signed_url)
{
    char *signed_url_str;
    aos_string_t querystr;
    char uristr[3*AOS_MAX_URI_LEN+1];
    int res = AOSE_OK;
    aos_string_t signature;
    const char *proto;

    res = get_cos_request_signature(options, req, expires, &signature);
    if (res != AOSE_OK) {
        return res;
    }

    apr_table_set(req->query_params, COS_ACCESSKEYID, 
                  options->config->access_key_id.data);
    apr_table_set(req->query_params, COS_EXPIRES, expires->data);
    apr_table_set(req->query_params, COS_SIGNATURE, signature.data);

    uristr[0] = '\0';
    aos_str_null(&querystr);
    res = aos_url_encode(uristr, req->uri, AOS_MAX_URI_LEN);
    if (res != AOSE_OK) {
        return res;
    }

    res = aos_query_params_to_string(options->pool, req->query_params, &querystr);
    if (res != AOSE_OK) {
        return res;
    }

    proto = strlen(req->proto) != 0 ? req->proto : AOS_HTTP_PREFIX;
    signed_url_str = apr_psprintf(options->pool, "%s%s/%s%.*s",
                                  proto, req->host, uristr,
                                  querystr.len, querystr.data);
    aos_str_set(signed_url, signed_url_str);

    return res;
}
