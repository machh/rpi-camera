#ifndef __USB_CAMERA_H_
#define __USB_CAMERA_H_


#include "typedefs.h"

#define DEBUG_ON            1
#define ERROR_ON            1

#define NAME_DEV          "/dev/video0"    
#define PIX_WIDTH           640           
#define PIX_HEIGHT          480             
#define REQBUF_CNT          4               
#define JPEG_QTY            50              


typedef struct _cam_buf
{
	void*       start;
	size_t      length;
} CAM_BUF;

extern int camera_init (
    const char*     dev_name, 
    int             pix_width, 
    int             pix_height,  
    CAM_BUF*        cam_bufs,    
    int             reqbuf_cnt  
);

extern int camera_start (
    int         cam_fd, 
    CAM_BUF*    cam_bufs,
    char*       jpg_buf, 
    int         jpg_width, 
    int         jpg_height, 
    int         quality 
);

int camera_stop (
    int         cam_fd, 
    CAM_BUF*    cam_bufs, 
    int         reqbuf_cnt 
);

#endif