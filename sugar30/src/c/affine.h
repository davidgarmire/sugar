#ifndef AFFINE_H
#define AFFINE_H

void affine_identity(double* T);

void affine_translate    (double* T, double* c);
void affine_translate_xyz(double* T, double x, double y, double z);
void affine_rotate_ox    (double* T, double ox);
void affine_rotate_oy    (double* T, double oy);
void affine_rotate_oz    (double* T, double oz);
void affine_scale        (double* T, double s);
void affine_scale_x      (double* T, double s);
void affine_scale_y      (double* T, double s);
void affine_scale_z      (double* T, double s);
void affine_compose(double* T, double* S);

void affine_apply          (double* T, double* x);
void affine_apply_stride   (double* T, double* x, int stride);
void affine_apply_A        (double* T, double* x);
void affine_apply_A_stride (double* T, double* x, int stride);
void affine_apply_AT       (double* T, double* x);
void affine_apply_AT_stride(double* T, double* x, int stride);


#endif /* AFFINE_H */
