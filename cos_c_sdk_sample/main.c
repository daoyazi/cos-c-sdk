#include "aos_log.h"
#include "aos_util.h"
#include "aos_string.h"
#include "aos_status.h"
#include "cos_auth.h"
#include "cos_util.h"
#include "cos_api.h"
#include "cos_config.h"
#include "cos_sample_util.h"

extern void append_object_sample();
extern void delete_object_sample();
extern void put_object_sample();
extern void get_object_sample();
extern void head_object_sample();
extern void multipart_object_sample();

int main(int argc, char *argv[])
{
    if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
        exit(1);
    }

//    aos_log_set_level(AOS_LOG_ALL);
    put_object_sample();

    get_object_sample();
    head_object_sample();
    multipart_object_sample();
    delete_object_sample();

    aos_http_io_deinitialize();
    system("pause");

    return 0;
}
