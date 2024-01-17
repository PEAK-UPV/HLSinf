#ifndef __OPENCV_H
#define __OPENCV_H

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C
#endif

EXTERN_C void fn_load_image_as_rgb(char *sz_file_name, int width, int height, int depth, void *ptr, int resize, int crop, float mean0, float mean1, float mean2, float std0, float std1, float std2);
//EXTERN_C void fn_open_camera(int *width, int *height);
//EXTERN_C void fn_show_camera();
//EXTERN_C void fn_get_frame_from_camera(int width, int height, float *ptr);
//EXTERN_C void fn_show_frame(int show_prediction, char *prediction_text1, char *prediction_text2, char *fps_text, int show_box, int x1, int y1, int x2, int y2);
//EXTERN_C void fn_prepare_frame(float *buffer, int width, int height, int channels, int width_frame, int height_frame, int denormalize);
//EXTERN_C void fn_capture_frame_from_camera();
#endif

