#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "cos_auth.h"
#include "cos_util.h"
#include "cos_api.h"
#include "cos_config.h"
#include "cos_sample_util.h"

void multipart_upload_file_from_buffer()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    aos_table_t *headers = NULL;
    aos_table_t *complete_headers = NULL;
    aos_table_t *resp_headers = NULL;
    cos_request_options_t *options = NULL;
    aos_string_t upload_id;
    aos_status_t *s = NULL;
    aos_list_t buffer;
    cos_list_upload_part_params_t *params = NULL;
    aos_list_t complete_part_list;
    cos_list_part_content_t *part_content = NULL;
    cos_complete_part_content_t *complete_part_content = NULL;
    int part_num1 = 1;
    int part_num2 = 2;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_sample_request_options(options, is_cname);
    headers = aos_table_make(p, 1);
    complete_headers = aos_table_make(p, 1); 
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, "multipart-key.1");
    
    //init mulitipart
    s = cos_init_multipart_upload(options, &bucket, &object, 
                                  &upload_id, headers, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("Init multipart upload succeeded, upload_id:%.*s\n", 
               upload_id.len, upload_id.data);
    } else {
        printf("Init multipart upload failed\n");
        aos_pool_destroy(p);
        return;
    }    

    //upload part
    aos_list_init(&buffer);
    make_random_body(p, 200*30, &buffer);
    s = cos_upload_part_from_buffer(options, &bucket, &object, &upload_id,
                                    part_num1, &buffer, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("Upload multipart part succeeded\n");
    } else {
        printf("Upload multipart part failed\n");
        aos_pool_destroy(p);
        return;
    }

    aos_list_init(&buffer);
    make_random_body(p, 10, &buffer);
    s = cos_upload_part_from_buffer(options, &bucket, &object, &upload_id,
        part_num2, &buffer, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("Upload multipart part succeeded\n");
    } else {
        printf("Upload multipart part failed\n");
        aos_pool_destroy(p);
        return;
    }    

    //list part
    params = cos_create_list_upload_part_params(p);
    params->max_ret = 1000;
    aos_list_init(&complete_part_list);
    s = cos_list_upload_part(options, &bucket, &object, &upload_id, 
                             params, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("List multipart succeeded\n");
    } else {
        printf("List multipart failed\n");
        aos_pool_destroy(p);
        return;
    }

    aos_list_for_each_entry(cos_list_part_content_t, part_content, &params->part_list, node) {
        complete_part_content = cos_create_complete_part_content(p);
        aos_str_set(&complete_part_content->part_number, 
                    part_content->part_number.data);
        aos_str_set(&complete_part_content->etag, part_content->etag.data);
        aos_list_add_tail(&complete_part_content->node, &complete_part_list);
    }

    //complete multipart
    apr_table_add(complete_headers, COS_CONTENT_TYPE, "video/MP2T");
    s = cos_complete_multipart_upload(options, &bucket, &object, &upload_id,
            &complete_part_list, complete_headers, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("Complete multipart upload succeeded, upload_id:%.*s\n", 
               upload_id.len, upload_id.data);
    } else {
        printf("Complete multipart upload failed\n");
    }

    aos_pool_destroy(p);
}

void multipart_upload_file_from_file()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    aos_table_t *headers = NULL;
    aos_table_t *complete_headers = NULL;
    aos_table_t *resp_headers = NULL;
    cos_request_options_t *options = NULL;
    aos_string_t upload_id;
    cos_upload_file_t *upload_file = NULL;
    aos_status_t *s = NULL;
    cos_list_upload_part_params_t *params = NULL;
    aos_list_t complete_part_list;
    cos_list_part_content_t *part_content = NULL;
    cos_complete_part_content_t *complete_part_content = NULL;
    int part_num = 1;
    int64_t pos = 0;
    int64_t file_length = 0;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_sample_request_options(options, is_cname);
    headers = aos_table_make(p, 1);
    complete_headers = aos_table_make(p, 1);
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, "multipart-key.2");
    
    //init mulitipart
    s = cos_init_multipart_upload(options, &bucket, &object, 
                                  &upload_id, headers, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("Init multipart upload succeeded, upload_id:%.*s\n", 
               upload_id.len, upload_id.data);
    } else {
        printf("Init multipart upload failed\n");
        aos_pool_destroy(p);
        return;
    }

    //upload part from file
    file_length = get_file_size(MULTIPART_UPLOAD_FILE_PATH);
    while(pos < file_length) {
        upload_file = cos_create_upload_file(p);
        aos_str_set(&upload_file->filename, MULTIPART_UPLOAD_FILE_PATH);
        upload_file->file_pos = pos;
        pos += 5 * 1024 * 1024;
        upload_file->file_last = pos < file_length ? pos : file_length; //200k
        s = cos_upload_part_from_file(options, &bucket, &object, &upload_id,
                part_num++, upload_file, &resp_headers);

        if (aos_status_is_ok(s)) {
            printf("Multipart upload part from file succeeded\n");
        } else {
            printf("Multipart upload part from file failed\n");
        }
    }

    //list part
    params = cos_create_list_upload_part_params(p);
    params->max_ret = 1000;
    aos_list_init(&complete_part_list);
    s = cos_list_upload_part(options, &bucket, &object, &upload_id, 
                             params, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("List multipart succeeded\n");
    } else {
        printf("List multipart failed\n");
        aos_pool_destroy(p);
        return;
    }

    aos_list_for_each_entry(cos_list_part_content_t, part_content, &params->part_list, node) {
        complete_part_content = cos_create_complete_part_content(p);
        aos_str_set(&complete_part_content->part_number, 
                    part_content->part_number.data);
        aos_str_set(&complete_part_content->etag, part_content->etag.data);
        aos_list_add_tail(&complete_part_content->node, &complete_part_list);
    }

    //complete multipart
    s = cos_complete_multipart_upload(options, &bucket, &object, &upload_id,
            &complete_part_list, complete_headers, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("Complete multipart upload from file succeeded, upload_id:%.*s\n", 
               upload_id.len, upload_id.data);
    } else {
        printf("Complete multipart upload from file failed\n");
    }

    aos_pool_destroy(p);
}

void abort_multipart_upload()
{
    aos_pool_t *p = NULL;
    aos_string_t bucket;
    aos_string_t object;
    int is_cname = 0;
    aos_table_t *headers = NULL;
    aos_table_t *resp_headers = NULL;
    cos_request_options_t *options = NULL;
    aos_string_t upload_id;
    aos_status_t *s = NULL;

    aos_pool_create(&p, NULL);
    options = cos_request_options_create(p);
    init_sample_request_options(options, is_cname);
    headers = aos_table_make(p, 1);
    aos_str_set(&bucket, BUCKET_NAME);
    aos_str_set(&object, OBJECT_NAME);
    
    s = cos_init_multipart_upload(options, &bucket, &object, 
                                  &upload_id, headers, &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("Init multipart upload succeeded, upload_id:%.*s\n", 
               upload_id.len, upload_id.data);
    } else {
        printf("Init multipart upload failed\n"); 
        aos_pool_destroy(p);
        return;
    }
    
    s = cos_abort_multipart_upload(options, &bucket, &object, &upload_id, 
                                   &resp_headers);

    if (aos_status_is_ok(s)) {
        printf("Abort multipart upload succeeded, upload_id::%.*s\n", 
               upload_id.len, upload_id.data);
    } else {
        printf("Abort multipart upload failed\n"); 
    }    
    aos_pool_destroy(p);
}

void multipart_object_sample()
{
//   multipart_upload_file_from_buffer();
    multipart_upload_file_from_file();
//    abort_multipart_upload();
}
