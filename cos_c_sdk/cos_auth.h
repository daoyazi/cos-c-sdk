#ifndef LIB_COS_AUTH_H
#define LIB_COS_AUTH_H

#include "aos_util.h"
#include "aos_string.h"
#include "aos_http_io.h"
#include "cos_define.h"

COS_CPP_START

/**
  * @brief  sign cos headers 
**/
void cos_sign_headers(aos_pool_t *p, 
                      const aos_string_t *signstr, 
                      const aos_string_t *access_key_id,
                      const aos_string_t *access_key_secret, 
                      aos_table_t *headers);

/**
  * @brief  get string to signature
**/
int cos_get_string_to_sign(aos_pool_t *p, 
                           http_method_e method, 
                           const aos_string_t *canon_res,
                           const aos_table_t *headers, 
                           const aos_table_t *params, 
                           aos_string_t *signstr);

/**
  * @brief  get signed cos request headers
**/
int cos_get_signed_headers(aos_pool_t *p, const aos_string_t *access_key_id, 
                           const aos_string_t *access_key_secret,
                           const aos_string_t* canon_res, aos_http_request_t *req);

/**
  * @brief  sign cos request
**/
int cos_sign_request(aos_http_request_t *req, const cos_config_t *config);

/**
  * @brief  generate cos request Signature
**/
int get_cos_request_signature(const cos_request_options_t *options, aos_http_request_t *req,
        const aos_string_t *expires, aos_string_t *signature);

/**
  * @brief  get cos signed url
**/
int cos_get_signed_url(const cos_request_options_t *options, aos_http_request_t *req,
        const aos_string_t *expires, aos_string_t *auth_url);

COS_CPP_END

#endif
