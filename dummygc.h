#ifndef MONITORS_H
#define MONITORS_H

#ifdef __cplusplus
extern "C" {
#endif

extern void ctx_new(void **p_data, void **p_dc, void **p_rc, int *p_err);
extern void ctx_make_current(void *p_dc, void *p_rc, int *p_err);
extern void ctx_destroy(void *p_data);

#ifdef __cplusplus
}
#endif

#endif /* MONITORS_H */