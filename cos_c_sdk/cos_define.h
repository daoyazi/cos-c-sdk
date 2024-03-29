#ifndef LIBCOS_DEFINE_H
#define LIBCOS_DEFINE_H

#include "aos_string.h"
#include "aos_list.h"
#include "aos_transport.h"

#ifdef __cplusplus
# define COS_CPP_START extern "C" {
# define COS_CPP_END }
#else
# define COS_CPP_START
# define COS_CPP_END
#endif

#define aos_json_error_status_set(STATUS, RES) do {                   \
        aos_status_set(STATUS, RES, AOS_json_PARSE_ERROR_CODE, NULL); \
    } while(0)

#define aos_file_error_status_set(STATUS, RES) do {                   \
        aos_status_set(STATUS, RES, AOS_OPEN_FILE_ERROR_CODE, NULL); \
    } while(0)

extern const char COS_CANNONICALIZED_HEADER_ACL[];
extern const char COS_CANNONICALIZED_HEADER_SOURCE[];
extern const char COS_CANNONICALIZED_HEADER_PREFIX[];
extern const char COS_CANNONICALIZED_HEADER_DATE[];
extern const char COS_CANNONICALIZED_HEADER_COPY_SOURCE[];
extern const char COS_CONTENT_MD5[];
extern const char COS_CONTENT_TYPE[];
extern const char COS_CONTENT_LENGTH[];
extern const char COS_DATE[];
extern const char COS_AUTHORIZATION[];
extern const char COS_ACCESSKEYID[];
extern const char COS_EXPECT[];
extern const char COS_EXPIRES[];
extern const char COS_SIGNATURE[];
extern const char COS_ACL[];
extern const char COS_PREFIX[];
extern const char COS_DELIMITER[];
extern const char COS_MARKER[];
extern const char COS_MAX_KEYS[];
extern const char COS_UPLOADS[];
extern const char COS_UPLOAD_ID[];
extern const char COS_MAX_PARTS[];
extern const char COS_KEY_MARKER[];
extern const char COS_UPLOAD_ID_MARKER[];
extern const char COS_MAX_UPLOADS[];
extern const char COS_PARTNUMBER[];
extern const char COS_PART_NUMBER_MARKER[];
extern const char COS_APPEND[];
extern const char COS_POSITION[];
extern const char COS_MULTIPART_CONTENT_TYPE[];
extern const char COS_COPY_SOURCE[];
extern const char COS_COPY_SOURCE_RANGE[];
extern const char COS_STS_SECURITY_TOKEN[];
extern const char COS_REPLACE_OBJECT_META[];
extern const char COS_OBJECT_TYPE[];
extern const char COS_NEXT_APPEND_POSITION[];
extern const char COS_DELETE[];
extern const char COS_YES[];
extern const char COS_OBJECT_TYPE_NORMAL[];
extern const char COS_OBJECT_TYPE_APPENDABLE[];
extern const int COS_MAX_PART_NUM;
extern const int COS_PER_RET_NUM;
extern const int MAX_SUFFIX_LEN;

typedef struct cos_lib_curl_initializer_s cos_lib_curl_initializer_t;

/**
 * cos_acl is an ACL that can be specified when an object is created or
 * updated.  Each canned ACL has a predefined value when expanded to a full
 * set of COS ACL Grants.
 * Private canned ACL gives the owner FULL_CONTROL and no other permissions
 *     are issued
 * Public Read canned ACL gives the owner FULL_CONTROL and all users Read
 *     permission 
 * Public Read Write canned ACL gives the owner FULL_CONTROL and all users
 *     Read and Write permission
 **/
typedef enum {
    COS_ACL_PRIVATE                  = 0,   /*< private */
    COS_ACL_PUBLIC_READ              = 1,   /*< public read */
} cos_acl_e;

typedef struct {
    aos_string_t endpoint;
    aos_string_t access_key_id;
    aos_string_t access_key_secret;
    aos_string_t sts_token;
    int is_cname;
} cos_config_t;

typedef struct {
    cos_config_t *config;
    aos_http_controller_t *ctl; /*< aos http controller, more see aos_transport.h */
    aos_pool_t *pool;
} cos_request_options_t;

typedef struct {
    aos_list_t node;
    aos_string_t key;
    aos_string_t last_modified;
    aos_string_t etag;
    aos_string_t size;
    aos_string_t owner_id;
    aos_string_t owner_display_name;
} cos_list_object_content_t;

typedef struct {
    aos_list_t node;
    aos_string_t prefix;
} cos_list_object_common_prefix_t;

typedef struct {
    aos_list_t node;
    aos_string_t key;
    aos_string_t upload_id;
    aos_string_t initiated;
} cos_list_multipart_upload_content_t;

typedef struct {
    aos_list_t node;
    aos_string_t part_number;
    aos_string_t size;
    aos_string_t etag;
    aos_string_t last_modified;
} cos_list_part_content_t;

typedef struct {
    aos_list_t node;
    aos_string_t part_number;
    aos_string_t etag;
} cos_complete_part_content_t;

typedef struct {
    int part_num;
    char *etag;
} cos_upload_part_t;

typedef struct {
    aos_string_t prefix;
    aos_string_t marker;
    aos_string_t delimiter;
    int max_ret;
    int truncated;
    aos_string_t next_marker;
    aos_list_t object_list;
    aos_list_t common_prefix_list;
} cos_list_object_params_t;

typedef struct {
    aos_string_t part_number_marker;
    int max_ret;
    int truncated;
    aos_string_t next_part_number_marker;
    aos_list_t part_list;
} cos_list_upload_part_params_t;

typedef struct {
    aos_string_t prefix;
    aos_string_t key_marker;
    aos_string_t upload_id_marker;
    aos_string_t delimiter;
    int max_ret;
    int truncated;
    aos_string_t next_key_marker;
    aos_string_t next_upload_id_marker;
    aos_list_t upload_list;
} cos_list_multipart_upload_params_t;

typedef struct {
    aos_string_t source_bucket;
    aos_string_t source_object;
    aos_string_t dest_bucket;
    aos_string_t dest_object;
    aos_string_t upload_id;
    int part_num;
    int64_t range_start;
    int64_t range_end;
} cos_upload_part_copy_params_t;

typedef struct {
    aos_string_t filename;  /**< file range read filename */
    int64_t file_pos;   /**< file range read start position */
    int64_t file_last;  /**< file range read last position */
} cos_upload_file_t;

typedef struct {
    aos_list_t node;
    aos_string_t key;
} cos_object_key_t;

typedef struct {
    char *suffix;
    char *type;
} cos_content_type_t;

#endif
