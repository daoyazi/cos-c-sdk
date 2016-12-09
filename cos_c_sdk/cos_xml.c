#include "aos_string.h"
#include "aos_list.h"
#include "aos_buf.h"
#include "aos_util.h"
#include "aos_log.h"
#include "aos_status.h"
#include "cos_util.h"
#include "cos_auth.h"
#include "cos_json.h"
#include "cos_xml.h"
#include "cos_define.h"
#include "aos_string.h"

static int get_truncated_from_json(aos_pool_t *p, cJSON *json_node, const char *truncated_json_path)
{
    char *is_truncated;
    int truncated = 0;
    is_truncated = get_jsonnode_value(p, json_node, truncated_json_path);
    if (is_truncated) {
        truncated = strcasecmp(is_truncated, "false") == 0 ? 0 : 1;
    }
    return truncated;
}

static char* new_json_buff(aos_pool_t *p, cJSON *doc)
{
    return cJSON_PrintUnformatted(p, doc);
}

int get_jsondoc(aos_list_t *bc, cJSON **root)
{
    int res;

    if (aos_list_empty(bc)) {
        return AOSE_json_PARSE_ERROR;
    }

    if ((res = aos_parse_json_body(bc, root)) != AOSE_OK) {
        return AOSE_json_PARSE_ERROR;
    }

    return AOSE_OK;
}

char *get_jsonnode_value(aos_pool_t *p, cJSON *json_node, const char *json_path)
{
    char *node_content;

    cJSON *node = cJSON_GetObjectItem(json_node, json_path);
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        return node_content;
    }
    return NULL;
}

int cos_acl_parse_from_body(aos_pool_t *p, aos_list_t *bc, aos_string_t *cos_acl)
{
    int res;
    cJSON *doc = NULL;
    char *acl;

    res = get_jsondoc(bc, &doc);
    cJSON *node = cJSON_GetObjectItem(doc, "AccessControlList");
    if (node) {
        acl = get_jsonnode_value(p, node, "Grant");
        if (acl) {
            aos_str_set(cos_acl, acl);
        }        
    }
    cJSON_Delete(doc);
    return res;
}

void cos_list_objects_owner_parse(aos_pool_t *p, cJSON *json_node, cos_list_object_content_t *content)
{
    cJSON *node;
    char *node_content;
    char *owner_id;
    char *owner_display_name;

    node = cJSON_GetObjectItem(json_node, "ID");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        owner_id = apr_pstrdup(p, node_content);
        aos_str_set(&content->owner_id, owner_id);
    }

    node = cJSON_GetObjectItem(json_node, "DisplayName");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        owner_display_name = apr_pstrdup(p, node_content);
        aos_str_set(&content->owner_display_name, owner_display_name);
    }
}

void cos_list_objects_content_parse(aos_pool_t *p, cJSON *json_node, cos_list_object_content_t *content)
{
    char *key;
    char *last_modified;
    char *etag;
    char *size;
    char *node_content;
    cJSON *node;

    node = cJSON_GetObjectItem(json_node, "Key");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        key = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->key, key);
    }

    node = cJSON_GetObjectItem(json_node, "LastModified");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        last_modified = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->last_modified, last_modified);
    }

    node = cJSON_GetObjectItem(json_node, "ETag");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        etag = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->etag, etag);
    }

    node = cJSON_GetObjectItem(json_node, "Size");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        size = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->size, size);
    }

    node = cJSON_GetObjectItem(json_node, "LastModified");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        last_modified = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->last_modified, last_modified);
    }

    node = cJSON_GetObjectItem(json_node, "Owner");
    if (NULL != node) {
        cos_list_objects_owner_parse(p, node, content);
    }
}

void cos_list_objects_contents_parse(aos_pool_t *p, cJSON *root, const char *json_path, aos_list_t *object_list)
{
    cJSON *node;
    cos_list_object_content_t *content;

    node = cJSON_GetObjectItem(root, json_path);
    int count = cJSON_GetArraySize(node);
    int i = 0;
    for (i=0; i<count; i++) {
        cJSON *objectItem = cJSON_GetArrayItem(node, i);
        content = cos_create_list_object_content(p);
        cos_list_objects_content_parse(p, objectItem, content);
        aos_list_add_tail(&content->node, object_list);
    }
}

void cos_list_objects_prefix_parse(aos_pool_t *p, cJSON *json_node, cos_list_object_common_prefix_t *common_prefix)
{
    char *prefix;
    cJSON *node;
    char *node_content;
    
    node = cJSON_GetObjectItem(json_node, "Prefix");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        prefix = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&common_prefix->prefix, prefix);
    }

}

void cos_list_objects_common_prefix_parse(aos_pool_t *p, cJSON *json_node, const char *json_path,
            aos_list_t *common_prefix_list)
{
    cJSON *node;
    cos_list_object_common_prefix_t *common_prefix;

    node = cJSON_GetObjectItem(json_node, json_path);
    int count = cJSON_GetArraySize(node);
    int i = 0;
    for (i=0; i<count; i++) {
        cJSON *objectItem = cJSON_GetArrayItem(node, i);

        common_prefix = cos_create_list_object_common_prefix(p);
        cos_list_objects_prefix_parse(p, objectItem, common_prefix);
        aos_list_add_tail(&common_prefix->node, common_prefix_list);
    }
}

int cos_list_objects_parse_from_body(aos_pool_t *p, aos_list_t *bc,
    aos_list_t *object_list, aos_list_t *common_prefix_list, aos_string_t *marker, int *truncated)
{
    int res;
    cJSON *root;
    const char next_marker_json_path[] = "NextMarker";
    const char truncated_json_path[] = "IsTruncated";
    const char buckets_json_path[] = "Contents";
    const char common_prefix_json_path[] = "CommonPrefixes";
    char* next_marker = NULL;

    res = get_jsondoc(bc, &root);
    if (res == AOSE_OK) {
        next_marker = get_jsonnode_value(p, root, next_marker_json_path);
        if (next_marker) {
            aos_str_set(marker, next_marker);
        }

        *truncated = get_truncated_from_json(p, root, truncated_json_path);
        
        cos_list_objects_contents_parse(p, root, buckets_json_path, object_list);
        cos_list_objects_common_prefix_parse(p, root, common_prefix_json_path, common_prefix_list);

        cJSON_Delete(root);
    }
    
    return res;
}

int cos_upload_id_parse_from_body(aos_pool_t *p, aos_list_t *bc, aos_string_t *upload_id)
{
    int res;
    cJSON *root;
    const char json_path[] = "UploadId";
    char *id;

    res = get_jsondoc(bc, &root);
    if (res == AOSE_OK) {
        id = get_jsonnode_value(p, root, json_path);
        if (id) {
            aos_str_set(upload_id, strdup(id));
        }
        cJSON_Delete(root);
    }

    return res;
}

void cos_list_parts_contents_parse(aos_pool_t *p, cJSON *root, const char *json_path, 
    aos_list_t *part_list)
{
    cos_list_part_content_t *content;

    cJSON *node = cJSON_GetObjectItem(root, json_path);
    int count = cJSON_GetArraySize(node);
    int i = 0;

    for (i=0; i<count; i++) {
        cJSON *objectItem = cJSON_GetArrayItem(node, i);

        content = cos_create_list_part_content(p);
        cos_list_parts_content_parse(p, objectItem, content);
        aos_list_add_tail(&content->node, part_list);
    }
}

void cos_list_parts_content_parse(aos_pool_t *p, cJSON *json_node, cos_list_part_content_t *content)
{
    char *part_number;
    char *last_modified;
    char *etag;
    char *size;
    char *node_content;
    cJSON *node;

    node = cJSON_GetObjectItem(json_node, "PartNumber");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        part_number = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->part_number, part_number);
    }

    node = cJSON_GetObjectItem(json_node, "LastModified");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        last_modified = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->last_modified, last_modified);
    }

    node = cJSON_GetObjectItem(json_node, "ETag");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        etag = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->etag, etag);
    }

    node = cJSON_GetObjectItem(json_node, "Size");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        size = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->size, size);
    }
}

int cos_list_parts_parse_from_body(aos_pool_t *p, aos_list_t *bc,
    aos_list_t *part_list, aos_string_t *partnumber_marker, int *truncated)
{
    int res;
    cJSON *root;
    const char next_partnumber_marker_json_path[] = "NextPartNumberMarker";
    const char truncated_json_path[] = "IsTruncated";
    const char parts_json_path[] = "Parts";
    char *next_partnumber_marker;

    res = get_jsondoc(bc, &root);
    if (res == AOSE_OK) {
        next_partnumber_marker = get_jsonnode_value(p, root,
                next_partnumber_marker_json_path);
        if (next_partnumber_marker) {
            aos_str_set(partnumber_marker, next_partnumber_marker);
        }

        *truncated = get_truncated_from_json(p, root, truncated_json_path);

        cos_list_parts_contents_parse(p, root, parts_json_path, part_list);

        cJSON_Delete(root);
    }

    return res;
}

void cos_list_multipart_uploads_contents_parse(aos_pool_t *p, cJSON *root, const char *json_path,
    aos_list_t *upload_list)
{
    cos_list_multipart_upload_content_t *content;
    cJSON *node;

    node = cJSON_GetObjectItem(root, json_path);
    int count = cJSON_GetArraySize(node);
    int i = 0;

    for (i=0; i<count; i++) {
        cJSON *objectItem = cJSON_GetArrayItem(node, i);

        content = cos_create_list_multipart_upload_content(p);
        cos_list_multipart_uploads_content_parse(p, objectItem, content);
        aos_list_add_tail(&content->node, upload_list);
    }
}

void cos_list_multipart_uploads_content_parse(aos_pool_t *p, cJSON *json_node, 
    cos_list_multipart_upload_content_t *content)
{
    char *key;
    char *upload_id;
    char *initiated;
    char *node_content;
    cJSON *node;

    node = cJSON_GetObjectItem(json_node, "Key");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        key = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->key, key);
    }

    node = cJSON_GetObjectItem(json_node, "UploadId");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);        
        upload_id = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->upload_id, upload_id);
    }

    node = cJSON_GetObjectItem(json_node, "Initiated");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);
        initiated = apr_pstrdup(p, (char *)node_content);
        aos_str_set(&content->initiated, initiated);
    }
}

int cos_list_multipart_uploads_parse_from_body(aos_pool_t *p, aos_list_t *bc,
    aos_list_t *upload_list, aos_string_t *key_marker,
    aos_string_t *upload_id_marker, int *truncated)
{
    int res;
    cJSON *root;
    const char next_key_marker_json_path[] = "NextKeyMarker";
    const char next_upload_id_marker_json_path[] = "NextUploadIdMarker";
    const char truncated_json_path[] = "IsTruncated";
    const char uploads_json_path[] = "Uploads";
    char *next_key_marker;
    char *next_upload_id_marker;

    res = get_jsondoc(bc, &root);
    if (res == AOSE_OK) {
        next_key_marker = get_jsonnode_value(p, root, next_key_marker_json_path);
        if (next_key_marker) {
            aos_str_set(key_marker, next_key_marker);
        }

        next_upload_id_marker = get_jsonnode_value(p, root, next_upload_id_marker_json_path);
        if (next_upload_id_marker) {
            aos_str_set(upload_id_marker, next_upload_id_marker);
        }

        *truncated = get_truncated_from_json(p, root, truncated_json_path);

        cos_list_multipart_uploads_contents_parse(p, root, uploads_json_path, upload_list);

        cJSON_Delete(root);
    }

    return res;
}

static int strip_quote(char c)
{
    if (c == '"')
        return 1;
    return 0;
}

char *build_complete_multipart_upload_json(aos_pool_t *p, aos_list_t *bc)
{
    char *json_buff;
    char *complete_part_json;
    aos_string_t json_doc;
    cJSON *root_node;
    cJSON *nodes;
    cos_complete_part_content_t *content;

    root_node = cJSON_CreateObject();
    cJSON_AddItemToObject(root_node, "Parts", nodes=cJSON_CreateArray());

    aos_list_for_each_entry(cos_complete_part_content_t, content, bc, node) {
        cJSON *element = cJSON_CreateObject();

        cJSON_AddNumberToObject(element, "PartNumber", atoi(content->part_number.data));
        aos_strip_str_func(&content->etag, strip_quote);
        content->etag.data[content->etag.len] = 0;
        cJSON_AddStringToObject(element, "ETag", content->etag.data);

        cJSON_AddItemToArray(nodes, element);
    }
    
    json_buff = new_json_buff(p, root_node);
    if (json_buff == NULL) {
        return NULL;
    }
    aos_str_set(&json_doc, json_buff);
    complete_part_json = aos_pstrdup(p, &json_doc);

    cJSON_Delete(root_node);

    return complete_part_json;
}

void build_complete_multipart_upload_body(aos_pool_t *p, aos_list_t *part_list, aos_list_t *body)
{
    char *complete_multipart_upload_json;
    aos_buf_t *b;
    
    complete_multipart_upload_json = build_complete_multipart_upload_json(p, part_list);
    aos_list_init(body);
    b = aos_buf_pack(p, complete_multipart_upload_json, strlen(complete_multipart_upload_json));
    aos_list_add_tail(&b->node, body);
}

void cos_delete_objects_contents_parse(aos_pool_t *p, cJSON *root, const char *json_path, aos_list_t *object_list)
{
    cJSON *node;

    node = cJSON_GetObjectItem(root, json_path);
    int count = cJSON_GetArraySize(node);
    int i = 0;

    for (i=0; i<count; i++) {
        cJSON *objectItem = cJSON_GetArrayItem(node, i);

        cos_object_key_t *content = cos_create_cos_object_key(p);
        cos_object_key_parse(p, objectItem, content);
        aos_list_add_tail(&content->node, object_list);
    }
}

void cos_object_key_parse(aos_pool_t *p, cJSON * json_node,
    cos_object_key_t *content)
{   
    char *key;
    char *encoded_key;
    char *node_content;
    cJSON *node;
    
    node = cJSON_GetObjectItem(json_node, "Key");
    if(node !=  NULL) {
        node_content = cJSON_PrintUnformatted(p, node);

        encoded_key = (char*)node_content;
        key = (char *) aos_palloc(p, strlen(encoded_key));
        aos_url_decode(encoded_key, key);
        aos_str_set(&content->key, key);
    }
}

int cos_delete_objects_parse_from_body(aos_pool_t *p, aos_list_t *bc, aos_list_t *object_list)
{
    int res;
    cJSON *root = NULL;
    const char deleted_json_path[] = "Deleteds";

    res = get_jsondoc(bc, &root);
    if (res == AOSE_OK) {
        cos_delete_objects_contents_parse(p, root, deleted_json_path, object_list);
        cJSON_Delete(root);
    }

    return res;
}

char *build_objects_json(aos_pool_t *p, aos_list_t *object_list, const char *quiet)
{
    char *object_json;
    char *json_buff;
    aos_string_t json_doc;
    cJSON *nodes;
    cJSON *root_node;
    cos_object_key_t *content;

    root_node = cJSON_CreateObject();
    if (strcasecmp(quiet, "false") == 0)
    {
        cJSON_AddFalseToObject(root_node, "Quiet");
    } else {
        cJSON_AddTrueToObject(root_node, "Quiet");
    }

    cJSON_AddItemToObject(root_node, "Objects", nodes=cJSON_CreateArray());

    aos_list_for_each_entry(cos_object_key_t, content, object_list, node) {
        cJSON *element = cJSON_CreateObject();
        cJSON_AddStringToObject(element, "Key", content->key.data);
        cJSON_AddItemToArray(nodes, element);
    }

    json_buff = new_json_buff(p, root_node);
    if (json_buff == NULL) {
        return NULL;
    }
    aos_str_set(&json_doc, json_buff);
    object_json = aos_pstrdup(p, &json_doc);

    cJSON_Delete(root_node);

    return object_json;
}

void build_delete_objects_body(aos_pool_t *p, aos_list_t *object_list, int is_quiet, aos_list_t *body)
{
    char *objects_json;
    aos_buf_t *b;
    char *quiet;
    quiet = is_quiet > 0 ? "true": "false";
    objects_json = build_objects_json(p, object_list, quiet);
    aos_list_init(body);
    b = aos_buf_pack(p, objects_json, strlen(objects_json));
    aos_list_add_tail(&b->node, body);
}
