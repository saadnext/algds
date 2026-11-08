#ifndef ALGDS_ERROR_H
#define ALGDS_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { ALGDS_OK = 0, ALGDS_ERR_INVALID_ARG, ALGDS_ERR_NOMEM, ALGDS_ERR_RANGE } algds_status;

#ifdef __cplusplus
}
#endif

#endif /* ALGDS_ERROR_H */
