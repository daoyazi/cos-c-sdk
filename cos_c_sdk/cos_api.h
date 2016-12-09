#ifndef LIBCOS_API_H
#define LIBCOS_API_H

#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "cos_define.h"
#include "cos_util.h"

COS_CPP_START

/*
 * @brief  create cos bucket
 * @param[in]   options       the cos request options
 * @param[in]   bucket        the cos bucket name
 * @param[in]   cos_acl       the cos bucket acl
 * @param[out]  resp_headers  cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_create_bucket(const cos_request_options_t *options,
                                const aos_string_t *bucket,
                                cos_acl_e cos_acl,
                                aos_table_t **resp_headers);

/*
 * @brief  delete cos bucket
 * @param[in]   options       the cos request options
 * @param[in]   bucket        the cos bucket name
 * @param[out]  resp_headers  cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_delete_bucket(const cos_request_options_t *options, 
                                const aos_string_t *bucket, 
                                aos_table_t **resp_headers);

/*
 * @brief  put cos bucket acl
 * @param[in]   options       the cos request options
 * @param[in]   bucket        the cos bucket name
 * @param[in]   cos_acl       the cos bucket acl
 * @param[out]  resp_headers  cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_put_bucket_acl(const cos_request_options_t *options, 
                                 const aos_string_t *bucket, 
                                 cos_acl_e cos_acl,
                                 aos_table_t **resp_headers);

/*
 * @brief  get cos bucket acl
 * @param[in]   options       the cos request options
 * @param[in]   bucket        the cos bucket name
 * @param[out]  cos_acl       the cos bucket acl
 * @param[out]  resp_headers  cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_get_bucket_acl(const cos_request_options_t *options,
                                 const aos_string_t *bucket,
                                 aos_string_t *cos_acl,
                                 aos_table_t **resp_headers);

/*
 * @brief  list cos objects
 * @param[in]   options       the cos request options
 * @param[in]   bucket        the cos bucket name
 * @param[in]   params        input params for list object request,
                              including prefix, marker, delimiter, max_ret
 * @param[out]  params        output params for list object response,
                              including truncated, next_marker, obje list
 * @param[out]  resp_headers  cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_list_object(const cos_request_options_t *options, 
                              const aos_string_t *bucket, 
                              cos_list_object_params_t *params, 
                              aos_table_t **resp_headers);

/*
 * @brief  put cos object from buffer
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   buffer              the buffer containing object content
 * @param[in]   headers             the headers for request
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_put_object_from_buffer(const cos_request_options_t *options, 
                                         const aos_string_t *bucket, 
                                         const aos_string_t *object, 
                                         aos_list_t *buffer, 
                                         aos_table_t *headers,
                                         aos_table_t **resp_headers);

/*
 * @brief  put cos object from file
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   filename            the filename to put
 * @param[in]   headers             the headers for request
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_put_object_from_file(const cos_request_options_t *options,
                                       const aos_string_t *bucket, 
                                       const aos_string_t *object, 
                                       const aos_string_t *filename,
                                       aos_table_t *headers, 
                                       aos_table_t **resp_headers);

/*
 * @brief  get cos object to buffer
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   headers             the headers for request
 * @param[in]   params              the params for request
 * @param[out]  buffer              the buffer containing object content
 * @param[out]  resp_headers  cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_get_object_to_buffer(const cos_request_options_t *options, 
                                       const aos_string_t *bucket, 
                                       const aos_string_t *object,
                                       aos_table_t *headers, 
                                       aos_table_t *params,
                                       aos_list_t *buffer, 
                                       aos_table_t **resp_headers);

/*
 * @brief  get cos object to file
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   headers             the headers for request
 * @param[in]   params              the params for request
 * @param[out]  filename            the filename storing object content
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_get_object_to_file(const cos_request_options_t *options,
                                     const aos_string_t *bucket, 
                                     const aos_string_t *object,
                                     aos_table_t *headers, 
                                     aos_table_t *params,
                                     aos_string_t *filename, 
                                     aos_table_t **resp_headers);

/*
 * @brief  head cos object
 * @param[in]   options          the cos request options
 * @param[in]   bucket           the cos bucket name
 * @param[in]   object           the cos object name
 * @param[in]   headers          the headers for request
 * @param[out]  resp_headers     cos server response headers containing object meta
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_head_object(const cos_request_options_t *options, 
                              const aos_string_t *bucket, 
                              const aos_string_t *object,
                              aos_table_t *headers, 
                              aos_table_t **resp_headers);

/*
 * @brief  delete cos object
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_delete_object(const cos_request_options_t *options, 
                                const aos_string_t *bucket, 
                                const aos_string_t *object, 
                                aos_table_t **resp_headers);

/*
 * @brief  delete cos objects
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object_list         the cos object list name
 * @param[in]   is_quiet            is quiet or verbose
 * @param[out]  resp_headers        cos server response headers
 * @param[out]  deleted_object_list deleted object list
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_delete_objects(const cos_request_options_t *options,
                                 const aos_string_t *bucket, 
                                 aos_list_t *object_list, 
                                 int is_quiet,
                                 aos_table_t **resp_headers, 
                                 aos_list_t *deleted_object_list);

/*
 * @brief  delete cos objects by prefix
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   prefix              prefix of delete objects
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_delete_objects_by_prefix(cos_request_options_t *options,
                                           const aos_string_t *bucket, 
                                           const aos_string_t *prefix);

/*
 * @brief  gen signed url for cos object api
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   expires             the end expire time for signed url
 * @param[in]   req                 the aos http request
 * @return  signed url, non-NULL success, NULL failure
 */
char *cos_gen_signed_url(const cos_request_options_t *options, 
                         const aos_string_t *bucket,
                         const aos_string_t *object, 
                         int64_t expires, 
                         aos_http_request_t *req);

/*
 * @brief  cos put object from buffer using signed url
 * @param[in]   options             the cos request options
 * @param[in]   signed_url          the signed url for put object
 * @param[in]   buffer              the buffer containing object content
 * @param[in]   headers             the headers for request
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_put_object_from_buffer_by_url(const cos_request_options_t *options,
                                                const aos_string_t *signed_url, 
                                                aos_list_t *buffer, 
                                                aos_table_t *headers,
                                                aos_table_t **resp_headers);

/*
 * @brief  cos put object from file using signed url
 * @param[in]   options             the cos request options
 * @param[in]   signed_url          the signed url for put object
 * @param[in]   filename            the filename containing object content
 * @param[in]   headers             the headers for request
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_put_object_from_file_by_url(const cos_request_options_t *options,
                                              const aos_string_t *signed_url, 
                                              aos_string_t *filename, 
                                              aos_table_t *headers,
                                              aos_table_t **resp_headers);

/*
 * @brief  cos get object to buffer using signed url
 * @param[in]   options             the cos request options
 * @param[in]   signed_url          the signed url for put object
 * @param[in]   buffer              the buffer containing object content
 * @param[in]   headers             the headers for request
 * @param[in]   params              the params for request
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_get_object_to_buffer_by_url(const cos_request_options_t *options,
                                              const aos_string_t *signed_url, 
                                              aos_table_t *headers,
                                              aos_table_t *params,
                                              aos_list_t *buffer,
                                              aos_table_t **resp_headers);

/*
 * @brief  cos get object to file using signed url
 * @param[in]   options             the cos request options
 * @param[in]   signed_url          the signed url for put object
 * @param[in]   headers             the headers for request
 * @param[in]   params              the params for request
 * @param[in]   filename            the filename containing object content
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_get_object_to_file_by_url(const cos_request_options_t *options,
                                            const aos_string_t *signed_url,
                                            aos_table_t *headers, 
                                            aos_table_t *params,
                                            aos_string_t *filename,
                                            aos_table_t **resp_headers);

/*
 * @brief  cos head object using signed url
 * @param[in]   options             the cos request options
 * @param[in]   signed_url          the signed url for put object
 * @param[in]   headers             the headers for request
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_head_object_by_url(const cos_request_options_t *options,
                                     const aos_string_t *signed_url, 
                                     aos_table_t *headers, 
                                     aos_table_t **resp_headers);

/*
 * @brief  cos init multipart upload
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   upload_id           the upload id to upload if has
 * @param[in]   headers             the headers for request
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_init_multipart_upload(const cos_request_options_t *options, 
                                        const aos_string_t *bucket, 
                                        const aos_string_t *object, 
                                        aos_string_t *upload_id,
                                        aos_table_t *headers,
                                        aos_table_t **resp_headers);

/*
 * @brief  cos upload part from buffer
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   upload_id           the upload id to upload if has
 * @param[in]   part_num            the upload part number
 * @param[in]   buffer              the buffer containing upload part content
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_upload_part_from_buffer(const cos_request_options_t *options, 
                                          const aos_string_t *bucket, 
                                          const aos_string_t *object, 
                                          const aos_string_t *upload_id, 
                                          int part_num, 
                                          aos_list_t *buffer, 
                                          aos_table_t **resp_headers);

/*
 * @brief  cos upload part from file
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   upload_id           the upload id to upload if has
 * @param[in]   part_num            the upload part number
 * @param[in]   upload_file         the file containing upload part content
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_upload_part_from_file(const cos_request_options_t *options,
                                        const aos_string_t *bucket, 
                                        const aos_string_t *object,
                                        const aos_string_t *upload_id, 
                                        int part_num, 
                                        cos_upload_file_t *upload_file,
                                        aos_table_t **resp_headers);

/*
 * @brief  cos abort multipart upload
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   upload_id           the upload id to upload if has
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_abort_multipart_upload(const cos_request_options_t *options, 
                                         const aos_string_t *bucket, 
                                         const aos_string_t *object, 
                                         aos_string_t *upload_id, 
                                         aos_table_t **resp_headers);


/*
 * @brief  cos complete multipart upload
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   upload_id           the upload id to upload if has
 * @param[in]   part_list           the uploaded part list to complete
 * @param[in]   headers             the headers for request          
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_complete_multipart_upload(const cos_request_options_t *options, 
                                            const aos_string_t *bucket, 
                                            const aos_string_t *object, 
                                            const aos_string_t *upload_id, 
                                            aos_list_t *part_list, 
                                            aos_table_t *headers,
                                            aos_table_t **resp_headers);

/*
 * @brief  cos list upload part with specific upload_id for object
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   upload_id           the upload id to upload if has
 * @param[in]   params              the input list upload part parameters,
                                    incluing part_number_marker, max_ret
 * @param[out]  params              the output params,
                                    including next_part_number_marker, part_list, truncated
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_list_upload_part(const cos_request_options_t *options, 
                                   const aos_string_t *bucket, 
                                   const aos_string_t *object, 
                                   const aos_string_t *upload_id, 
                                   cos_list_upload_part_params_t *params, 
                                   aos_table_t **resp_headers);

/*
 * @brief  cos list multipart upload for bucket
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   params              the input list multipart upload parameters
 * @param[out]  params              the output params including next_key_marker, next_upload_id_markert, upload_list etc
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_list_multipart_upload(const cos_request_options_t *options, 
                                        const aos_string_t *bucket, 
                                        cos_list_multipart_upload_params_t *params, 
                                        aos_table_t **resp_headers);

/*
 * @brief  cos copy large object using upload part copy
 * @param[in]   options             the cos request options
 * @param[in]   paramsthe           upload part copy parameters
 * @param[in]   headers             the headers for request
 * @param[out]  resp_headers        cos server response headers
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_upload_part_copy(const cos_request_options_t *options,
                                   cos_upload_part_copy_params_t *params, 
                                   aos_table_t *headers, 
                                   aos_table_t **resp_headers);

/*
 * @brief  cos upload file using multipart upload
 * @param[in]   options             the cos request options
 * @param[in]   bucket              the cos bucket name
 * @param[in]   object              the cos object name
 * @param[in]   upload_id           the upload id to upload if has
 * @param[in]   filename            the filename containing object content
 * @param[in]   part_size           the part size for multipart upload
 * @param[in]   headers             the headers for request
 * @return  aos_status_t, code is 2xx success, other failure
 */
aos_status_t *cos_upload_file(cos_request_options_t *options,
                              const aos_string_t *bucket, 
                              const aos_string_t *object, 
                              aos_string_t *upload_id,
                              aos_string_t *filename, 
                              int64_t part_size,
                              aos_table_t *headers);

COS_CPP_END

#endif
