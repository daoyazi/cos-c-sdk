#include "aos_log.h"
#include "aos_util.h"
#include "aos_status.h"
#include "cos_json.h"

const char AOS_json_PARSE_ERROR_CODE[] = "ParseXmlError";
const char AOS_OPEN_FILE_ERROR_CODE[] = "OpenFileFail";
const char AOS_HTTP_IO_ERROR_CODE[] = "HttpIoError";
const char AOS_UNKNOWN_ERROR_CODE[] = "UnknownError";
const char AOS_CLIENT_ERROR_CODE[] = "ClientError";
const char AOS_UTF8_ENCODE_ERROR_CODE[] = "Utf8EncodeFail";

aos_status_t *aos_status_create(aos_pool_t *p)
{
    return (aos_status_t *)aos_pcalloc(p, sizeof(aos_status_t));
}

aos_status_t *aos_status_dup(aos_pool_t *p, aos_status_t *src)
{
    aos_status_t *dst = aos_status_create(p);
    dst->code = src->code;
    dst->error_code = apr_pstrdup(p, src->error_code);
    dst->error_msg = apr_pstrdup(p, src->error_msg);
    return dst;
}

int aos_should_retry(aos_status_t *s) {
    int aos_error_code = 0;

    if (s == NULL || s->code / 100 == 2) {
        return 0;
    }

    if (s->code / 100 == 5) {
        return 1;
    }

    if (s->error_code != NULL) {
        aos_error_code = atoi(s->error_code);
        if (aos_error_code == AOSE_REQUEST_TIMEOUT || aos_error_code == AOSE_CONNECTION_FAILED) {
            return 1;
        }
    }

    return 0;
}

aos_status_t *aos_status_parse_from_body(aos_pool_t *p, aos_list_t *bc, int code, aos_status_t *s)
{
    int res;
    cJSON *root, *node;
    char *node_content;

    if (s == NULL) {
        s = aos_status_create(p);
    }
    s->code = code;

    if (aos_http_is_ok(code)) {
        return s;
    }

    if (aos_list_empty(bc)) {
        s->error_code = (char *)AOS_UNKNOWN_ERROR_CODE;
        return s;
    }

    if ((res = aos_parse_json_body(bc, &root)) != AOSE_OK) {
        s->error_code = (char *)AOS_UNKNOWN_ERROR_CODE;
        return s;
    }

    node = cJSON_GetObjectItem(root, "Code");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        s->error_code = apr_pstrdup(p, (char *)node_content);
    }

    node = cJSON_GetObjectItem(root, "Message");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        s->error_msg = apr_pstrdup(p, (char *)node_content);
    }

    cJSON_Delete(root);

    return s;
}