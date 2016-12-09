#ifndef LIBCOS_UTIL_H
#define LIBCOS_UTIL_H

#include "aos_string.h"
#include "aos_transport.h"
#include "aos_status.h"
#include "cos_define.h"
#include "cos_json.h"

COS_CPP_START

#define init_sts_token_header() do { \
        if (options->config->sts_token.data != NULL) {\
            apr_table_set(headers, COS_STS_SECURITY_TOKEN, options->config->sts_token.data);\
        }\
    } while(0)

/**
  * @brief  check hostname ends with specific cos domain suffix.
**/
int is_cos_domain(const aos_string_t *str);

/**
  * @brief  check hostname is ip.
**/
int is_valid_ip(const char *str);

/**
  * @brief  get cos acl str according cos_acl
  * @param[in]  cos_acl the cos bucket acl
  * @return cos acl str
**/
const char *get_cos_acl_str(cos_acl_e cos_acl);

/**
  * @brief  create cos config including host, port, access_key_id, access_key_secret, is_cos_domain
**/
cos_config_t *cos_config_create(aos_pool_t *p);

/**
  * @brief  create cos request options
  * @return cos request options
**/
cos_request_options_t *cos_request_options_create(aos_pool_t *p);

/**
  * @brief  init cos request
**/
void cos_init_request(const cos_request_options_t *options, http_method_e method,
        aos_http_request_t **req, aos_table_t *params, aos_table_t *headers, aos_http_response_t **resp);

/**
  * @brief  init cos bucket request
**/
void cos_init_bucket_request(const cos_request_options_t *options, const aos_string_t *bucket,
        http_method_e method, aos_http_request_t **req, aos_table_t *params, aos_table_t *headers,
        aos_http_response_t **resp);
 
/**
  * @brief  init cos object request
**/
void cos_init_object_request(const cos_request_options_t *options, const aos_string_t *bucket,
        const aos_string_t *object, http_method_e method, aos_http_request_t **req, 
        aos_table_t *params, aos_table_t *headers, aos_http_response_t **resp);

/**
  * @brief  init cos request with signed_url
**/
void cos_init_signed_url_request(const cos_request_options_t *options, const aos_string_t *signed_url,
        http_method_e method, aos_http_request_t **req,
        aos_table_t *params, aos_table_t *headers, aos_http_response_t **resp);

/**
  * @brief  cos send request
**/
aos_status_t *cos_send_request(aos_http_controller_t *ctl, aos_http_request_t *req,
        aos_http_response_t *resp);

/**
  * @brief process cos request including sign request, send request, get response
**/
aos_status_t *cos_process_request(const cos_request_options_t *options,
        aos_http_request_t *req, aos_http_response_t *resp);

/**
  * @brief process cos request with signed_url including send request, get response
**/
aos_status_t *cos_process_signed_request(const cos_request_options_t *options, 
        aos_http_request_t *req, aos_http_response_t *resp);

/**
  * @brief  get object uri using third-level domain if hostname is cos domain, otherwise second-level domain
**/
void cos_get_object_uri(const cos_request_options_t *options,
                        const aos_string_t *bucket,
                        const aos_string_t *object,
                        aos_http_request_t *req);

/**
  * @brief   bucket uri using third-level domain if hostname is cos domain, otherwise second-level domain
**/
void cos_get_bucket_uri(const cos_request_options_t *options, 
                        const aos_string_t *bucket,
                        aos_http_request_t *req);

/**
  * @brief  write body content into cos request body from buffer
**/
void cos_write_request_body_from_buffer(aos_list_t *buffer, aos_http_request_t *req);

/**
  * @brief   write body content into cos request body from file
**/
int cos_write_request_body_from_file(aos_pool_t *p, const aos_string_t *filename, aos_http_request_t *req);

/**
  * @brief   write body content into cos request body from multipart upload file
**/
int cos_write_request_body_from_upload_file(aos_pool_t *p, cos_upload_file_t *upload_file, aos_http_request_t *req);

/**
  * @brief  read body content from cos response body to buffer
**/
void cos_init_read_response_body_to_buffer(aos_list_t *buffer, aos_http_response_t *resp);

/**
  * @brief  read body content from cos response body to file
**/
int cos_init_read_response_body_to_file(aos_pool_t *p, const aos_string_t *filename, aos_http_response_t *resp);

/**
  * @brief  create cos api result content
  * @return cos api result content
**/
void *cos_create_api_result_content(aos_pool_t *p, size_t size);
cos_list_object_content_t *cos_create_list_object_content(aos_pool_t *p);
cos_list_object_common_prefix_t *cos_create_list_object_common_prefix(aos_pool_t *p);
cos_list_part_content_t *cos_create_list_part_content(aos_pool_t *p);
cos_list_multipart_upload_content_t *cos_create_list_multipart_upload_content(aos_pool_t *p);
cos_complete_part_content_t *cos_create_complete_part_content(aos_pool_t *p);

/**
  * @brief  create cos api list parameters
  * @return cos api list parameters
**/
cos_list_object_params_t *cos_create_list_object_params(aos_pool_t *p);
cos_list_upload_part_params_t *cos_create_list_upload_part_params(aos_pool_t *p);
cos_list_multipart_upload_params_t *cos_create_list_multipart_upload_params(aos_pool_t *p);

/**
  * @brief  create upload part copy params
  * @return upload part copy params struct for upload part copy
**/
cos_upload_part_copy_params_t *cos_create_upload_part_copy_params(aos_pool_t *p);

/**
  * @brief  create upload file struct for range multipart upload
  * @return upload file struct for range multipart upload
**/
cos_upload_file_t *cos_create_upload_file(aos_pool_t *p);

/**
  * @brief  get content-type for HTTP_POST request
  * @return content-type for HTTP_POST request
**/
void cos_set_multipart_content_type(aos_table_t *headers);

/**
  * @brief  create cos object content for delete objects
  * @return cos object content
**/
cos_object_key_t *cos_create_cos_object_key(aos_pool_t *p);

/**
  * @brief  get part size for multipart upload
**/
void cos_get_part_size(int64_t filesize, int64_t *part_size);

/**
  * @brief  compare function for part sort
**/
int part_sort_cmp(const void *a, const void *b);

/**
  * @brief  set content type for object according to objectname
  * @return cos content type
**/
char *get_content_type(const char *name);
char *get_content_type_by_suffix(const char *suffix);

/**
  * @brief  set content type for object according to  filename
**/
void set_content_type(const char* filename, const char* key, aos_table_t *headers);

aos_table_t* aos_table_create_if_null(const cos_request_options_t *options, 
                                      aos_table_t *table, int table_size);

COS_CPP_END

#endif
