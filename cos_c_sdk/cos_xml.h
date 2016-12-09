#ifndef LIBCOS_json_H
#define LIBCOS_json_H

#include "aos_string.h"
#include "aos_transport.h"
#include "aos_status.h"
#include "cos_define.h"

COS_CPP_START

/**
  * @brief  functions for xml body parse 
**/
int get_jsondoc(aos_list_t *bc, cJSON **root);
char *get_jsonnode_value(aos_pool_t *p, cJSON * root, const char *json_path);

/**
  * @brief  build xml body for complete_multipart_upload
**/
char *build_complete_multipart_upload_json(aos_pool_t *p, aos_list_t *bc);

/**
  * @brief  build body for complete multipart upload
**/
void build_complete_multipart_upload_body(aos_pool_t *p, aos_list_t *part_list, aos_list_t *body);

/**
  * @brief  build xml body for delete objects
**/
char *build_objects_json(aos_pool_t *p, aos_list_t *object_list, const char *quiet);

/**
  * @brief  build body for delete objects
**/
void build_delete_objects_body(aos_pool_t *p, aos_list_t *object_list, int is_quiet, 
            aos_list_t *body);

/**
  * @bried  pares acl from xml body for get_bucket_acl
**/
int cos_acl_parse_from_body(aos_pool_t *p, aos_list_t *bc, aos_string_t *cos_acl);

/**
  * @brief parse upload_id from xml body for init multipart upload
**/
int cos_upload_id_parse_from_body(aos_pool_t *p, aos_list_t *bc, aos_string_t *upload_id);

/**
  * @brief parse objects from xml body for list objects
**/
void cos_list_objects_owner_parse(aos_pool_t *p, cJSON *json_node, cos_list_object_content_t *content);
void cos_list_objects_content_parse(aos_pool_t *p, cJSON *json_node, cos_list_object_content_t *content);
void cos_list_objects_contents_parse(aos_pool_t *p, cJSON *root, const char *json_path,
            aos_list_t *object_list);
void cos_list_objects_prefix_parse(aos_pool_t *p, cJSON *root,     
            cos_list_object_common_prefix_t *common_prefix);
void cos_list_objects_common_prefix_parse(aos_pool_t *p, cJSON *root, const char *json_path,
            aos_list_t *common_prefix_list);
int cos_list_objects_parse_from_body(aos_pool_t *p, aos_list_t *bc, aos_list_t *object_list,
            aos_list_t *common_prefix_list, aos_string_t *marker, int *truncated);

/**
  * @brief parse parts from xml body for list upload part
**/
void cos_list_parts_contents_parse(aos_pool_t *p, cJSON *root, const char *json_path,
            aos_list_t *part_list);
void cos_list_parts_content_parse(aos_pool_t *p, cJSON *json_node, cos_list_part_content_t *content);
int cos_list_parts_parse_from_body(aos_pool_t *p, aos_list_t *bc, aos_list_t *part_list, 
            aos_string_t *part_number_marker, int *truncated);

/**
  * @brief  parse uploads from xml body for list multipart upload
**/
void cos_list_multipart_uploads_contents_parse(aos_pool_t *p, cJSON *root, const char *json_path,
            aos_list_t *upload_list);
void cos_list_multipart_uploads_content_parse(aos_pool_t *p, cJSON *json_node,
            cos_list_multipart_upload_content_t *content);
int cos_list_multipart_uploads_parse_from_body(aos_pool_t *p, aos_list_t *bc,
            aos_list_t *upload_list, aos_string_t *key_marker,
            aos_string_t *upload_id_marker, int *truncated);

/**
  * @brief parse delete objects contents from xml body
**/
void cos_delete_objects_contents_parse(aos_pool_t *p, cJSON *root, const char *json_path,
    aos_list_t *object_list);
void cos_object_key_parse(aos_pool_t *p, cJSON * json_node, cos_object_key_t *content);
int cos_delete_objects_parse_from_body(aos_pool_t *p, aos_list_t *bc, aos_list_t *object_list);

COS_CPP_END

#endif
