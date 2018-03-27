#ifndef MEX_HANDLE_H
#define MEX_HANDLE_H

void mex_handle_create ();
void mex_handle_destroy();

int   mex_handle_new_tag();

int   mex_handle_add   (void* data, int tag);
int   mex_handle_find  (void* data, int tag);
int   mex_handle_tag   (int handle_id);
void* mex_handle_data  (int handle_id);
void* mex_handle_remove(int handle_id);


#endif /* MEX_HANDLE_H */
