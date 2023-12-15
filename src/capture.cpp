#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <typedefs.h>
#include <capture.h>
#include <Jpeg.h>

#define __DEBUG__

#ifdef __DEBUG__
#define DEBUG_LOG(args) \
    printf("DEBUG: [%s:%d] DbgMsg: %s \r\n", \
        __FUNCTION__, __LINE__, args)

#define DEBUG_ERR(errno,fmt,args...) \
    printf("ERROR: [%s:%d] ErrNum:%d ErrMsg: fmt\r\n", \
        __FUNCTION__, __LINE__, errno, ##args) 
#else
#define DEBUG_LOG(fmt,args...)
#define DEBUG_ERR(fmt,args...)
#endif

//
int camera_init( const char*  dev_name, int  pix_width, int  pix_height,
     CAM_BUF*cam_bufs, int reqbuf_cnt) {
    int                    cam_fd;
    struct v4l2_buffer     buf_frame;
    
    if ((dev_name == NULL) 
        || pix_width <= 0 
        || pix_height <= 0
        || cam_bufs == NULL
        || reqbuf_cnt <= 0)
    {
        DEBUG_ERR(ERROR, "Invalid Parameter!");
        return ERROR;
    }

    if((cam_fd = open(NAME_DEV, O_RDWR | O_NONBLOCK)) < 0) {
        DEBUG_ERR(ERROR, "Open Camera Error!");
        close(cam_fd);
        return ERROR;
    }
    DEBUG_LOG("Open Camera Success!");

    struct v4l2_capability cam_cap;
    if(ioctl(cam_fd, VIDIOC_QUERYCAP, &cam_cap) != 0) {
        DEBUG_ERR(ERROR, "Get Capability Error!");
        close(cam_fd);
        return ERROR;
    }
    
    if(!(cam_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        DEBUG_ERR(ERROR, "Device Can't Support V4L2_CAP_VIDEO_CAPTURE");
        close(cam_fd);
        return ERROR;
    }
    
    if(!(cam_cap.capabilities & V4L2_CAP_STREAMING)) {
        DEBUG_ERR(ERROR, "Device Can't Support V4L2_CAP_STREAMING");
        close(cam_fd);
        return ERROR;
    }
    DEBUG_LOG("Get Capability Success!");
    
    struct v4l2_format cam_fmt;
    memset(&cam_fmt, 0, sizeof(struct v4l2_format));
    cam_fmt.type                    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    cam_fmt.fmt.pix.width           = pix_width;
    cam_fmt.fmt.pix.height          = pix_height;
    cam_fmt.fmt.pix.pixelformat     = V4L2_PIX_FMT_YUYV;
    cam_fmt.fmt.pix.field           = V4L2_FIELD_INTERLACED;
    if(ioctl(cam_fd, VIDIOC_S_FMT, &cam_fmt) < 0) {
        DEBUG_ERR(ERROR, "Set VIDIOC_S_FMT Error!");
        close(cam_fd);
        return ERROR;
    }
    
    if(ioctl(cam_fd, VIDIOC_G_FMT, &cam_fmt) < 0) {
        DEBUG_ERR(ERROR, "Get VIDIOC_G_FMT Error!");
        close(cam_fd);
        return ERROR;
    }
    
    DEBUG_LOG("Set v4l2_format To YUYV Success!");
    
    struct v4l2_requestbuffers cam_req;
    memset(&cam_req, 0, sizeof(struct v4l2_requestbuffers));
    cam_req.count       = reqbuf_cnt;
    cam_req.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    cam_req.memory      = V4L2_MEMORY_MMAP;
    if(ioctl(cam_fd, VIDIOC_REQBUFS, &cam_req) < 0) {
        DEBUG_ERR(ERROR, "Set VIDIOC_REQBUFS Error!");
        close(cam_fd);
        return ERROR;
    }
    
    int i = 0;
    for(i=0; i<cam_req.count; i++) {
        memset(&buf_frame, 0, sizeof(struct v4l2_buffer));
        buf_frame.type      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf_frame.memory    = V4L2_MEMORY_MMAP;
        buf_frame.index     = i;
        //ӳ���û��ռ�
        if(ioctl(cam_fd, VIDIOC_QUERYBUF, &buf_frame) < 0)
        {
            DEBUG_ERR(ERROR, "Set VIDIOC_QUERYBUF error!");
            close(cam_fd);
            return ERROR;
        }
        cam_bufs[i].length = buf_frame.length;
        cam_bufs[i].start = mmap(NULL,
                                buf_frame.length,
                                PROT_READ | PROT_WRITE,
                                MAP_SHARED,
                                cam_fd,
                                buf_frame.m.offset);
                                
        if(cam_bufs[i].start == MAP_FAILED) {
            DEBUG_ERR(ERROR, "Memory Map Failed!");
            close(cam_fd);
            return ERROR;
        }
        buf_frame.type      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf_frame.memory    = V4L2_MEMORY_MMAP;
        buf_frame.index     = i;
        if(ioctl(cam_fd, VIDIOC_QBUF, &buf_frame) < 0) {
            DEBUG_ERR(ERROR, "Set VIDIOC_QBUF error!");
            close(cam_fd);
            return ERROR;
        }
    }
    DEBUG_LOG("Memory Map Success!");
    
    enum v4l2_buf_type buf_type;
    buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(cam_fd, VIDIOC_STREAMON, &buf_type) < 0) {
        DEBUG_ERR(ERROR, "Start Cappture error!");
        close(cam_fd);
        return ERROR;
    }
    DEBUG_LOG("Camera init Success!");
    
    return cam_fd;
}

int camera_start( int cam_fd, 	 
					CAM_BUF*    cam_bufs, 		 
					char*       jpg_buf, 		 
					int         jpg_width, 		//
					int         jpg_height, 	//
					int         quality )
{
    int                 ret; 
    int                 jsize;	 
    fd_set              fds;
    struct timeval      timeout;
    struct v4l2_buffer  buf_frame;	 

    struct timeval  tpstart,tpend;
    float           timeuse;

    FD_ZERO(&fds);		 
    FD_SET(cam_fd, &fds);	 
    
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;
    
    ret = select(cam_fd+1, &fds, NULL, NULL, &timeout);
    if(ret == ERROR) {
        DEBUG_ERR(ERROR, "Select error!");
        close(cam_fd);
        return ERROR;
    } else if(ret == 0) {
        DEBUG_ERR(ERROR, "Select timeout!");
        close(cam_fd);
        return ERROR;
    } else {
        buf_frame.type      = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf_frame.memory    = V4L2_MEMORY_MMAP;
 
        if(ioctl(cam_fd, VIDIOC_DQBUF, &buf_frame)  < 0) {
            DEBUG_ERR(ERROR, "VIDIOC_DQBUF error!");
            close(cam_fd);
            return ERROR;
        }

        gettimeofday(&tpstart,NULL);

        DEBUG_LOG(  "begin compress_yuyv_to_jpeg!");
        char szLog[64]={'0'};
        sprintf( szLog, "#### the quality is :%d \n", quality);
        printf( szLog );

        jsize = compress_yuyv_to_jpeg( (const unsigned char*)cam_bufs[buf_frame.index].start,
                    (unsigned char*)jpg_buf,
                    cam_bufs[buf_frame.index].length,
                    jpg_height,
                    jpg_width, 
                    quality  );

        DEBUG_LOG(  "end compress_yuyv_to_jpeg!");

        gettimeofday(&tpend,NULL);
        timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+ 
        tpend.tv_usec-tpstart.tv_usec;
        timeuse/=1000000; 
        printf("Used Time:%f\n",timeuse);
        
        if(ioctl(cam_fd, VIDIOC_QBUF, &buf_frame) < 0)//�ٽ�������
        {
            DEBUG_ERR(ERROR, "VIDIOC_QBUF error!");
            close(cam_fd);
            return ERROR;
        }
    }
    DEBUG_LOG("Camera Start Success!");
    return jsize;
}

int camera_stop(int cam_fd, CAM_BUF* cam_bufs, int reqbuf_cnt )
{
    int i, ret;
    struct v4l2_buffer buf_frame;
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //�رղɼ�����
    if(ioctl(cam_fd, VIDIOC_STREAMOFF, &type) == ERROR)
    {
	DEBUG_ERR(ERROR, "Camera Stop Error!");
	return ERROR;
    }

    buf_frame.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf_frame.memory = V4L2_MEMORY_MMAP;
    for(i=0; i<reqbuf_cnt; i++)
    {
	    if(ioctl(cam_fd, VIDIOC_DQBUF, &buf_frame) == ERROR)
		    break;
    }

    //����ڴ�ӳ��
    for(i=0; i<reqbuf_cnt; i++)
    {
	    if(munmap(cam_bufs[i].start, cam_bufs[i].length) == ERROR)
        {
            DEBUG_ERR(ERROR, "munmap error!");
            return ERROR;
        }
    }

	close(cam_fd);
    DEBUG_LOG("Camera Stop Success!");
    return OK;
}

int main(void) {
    CAM_BUF CAMERA_BUFS[REQBUF_CNT];
    char    JPEG_BUF[PIX_WIDTH*PIX_HEIGHT] = {0};
    FILE*   jpeg_fd;
    int     jpeg_size;
    int     CAMERA_FD;
    
    char            jpeg_name[20] = {0};
    int             name_i = 0;
    struct timeval  tpstart,tpend; 
    float           timeuse; 

    CAMERA_FD = camera_init(NAME_DEV, 
                            PIX_WIDTH, 
                            PIX_HEIGHT, 
                            CAMERA_BUFS, 
                            REQBUF_CNT);

    while(1) {
        sprintf(jpeg_name, "JPEG%010d.jpg", name_i++);
        if((jpeg_fd = fopen(jpeg_name, "w+")) == NULL) {
            DEBUG_ERR(ERROR, "Open jpeg file error!");
            return ERROR;
        }
        
        DEBUG_LOG("Open jpeg file sucess!");
        jpeg_size = camera_start(CAMERA_FD, 
                                CAMERA_BUFS, 
                                JPEG_BUF, 
                                PIX_WIDTH, 
                                PIX_HEIGHT, 
                                JPEG_QTY);
        
        if(jpeg_size <= 0) {
            DEBUG_ERR(ERROR, "Camera Read Pic Error!");
            return ERROR;
        }

        printf("JPEG Size:%d\r\n", jpeg_size);
        fwrite(JPEG_BUF, jpeg_size, 1, jpeg_fd); //����д���ļ���
        fclose(jpeg_fd);

        DEBUG_LOG("Write jpeg file sucess!");
        sleep(1);
    }
    return OK;
}
