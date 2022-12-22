#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <x264.h>

int x264_yuv420p2h264(uint8_t* src,uint8_t* dst, uint32_t dstlen,uint32_t width,uint32_t height)
{
    x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;
    x264_t *h;
    x264_nal_t *nal;
    int i_frame = 0;
    int i_frame_size;
    int i_nal;
    int outlen = 0;
    /* Get default params for preset/tuning */
    if( x264_param_default_preset( &param, "medium", NULL ) < 0 )
    {
        printf("x264_param_default_preset err\n");
        return -1;
    }
    
    /* Configure non-default params */
    param.i_bitdepth = 8;
    param.i_csp = X264_CSP_I420;
    param.i_width  = width;
    param.i_height = height;
    param.b_vfr_input = 0;
    param.b_repeat_headers = 1;
    param.b_annexb = 1;
    param.i_log_level = X264_LOG_ERROR;
    
    /* Apply profile restrictions. */
    if( x264_param_apply_profile( &param, "high" ) < 0 )
    {
        printf("x264_param_apply_profile err\n");
        return -1;
    }


    if( x264_picture_alloc( &pic, param.i_csp, param.i_width, param.i_height ) < 0 )
    {
        printf("x264_picture_alloc err\n");
        return -1;
    }

    h = x264_encoder_open( &param );
    if( !h )
    {
        x264_picture_clean( &pic );
        printf("x264_encoder_open err\n");
        return -1;
    }

    int luma_size = width * height;
    int chroma_size = luma_size / 4;
    i_frame++;
    memcpy(pic.img.plane[0],src,luma_size);
    memcpy(pic.img.plane[1],src+luma_size,chroma_size);
    memcpy(pic.img.plane[2],src+luma_size+chroma_size,chroma_size);
    pic.i_pts = i_frame;
    i_frame_size = x264_encoder_encode( h, &nal, &i_nal, &pic, &pic_out );
    if( i_frame_size < 0 )
    {
        printf("x264_encoder_encode err %d\n",i_frame_size);
        x264_encoder_close( h );
        x264_picture_clean( &pic );
        return -1;
    }
    else if( i_frame_size )
    {
        printf("x264_encoder_encode ok %d\n",i_frame_size);
        if((outlen + i_frame_size) <= dstlen)
        {
            memcpy(dst+outlen,nal->p_payload,i_frame_size);
            outlen += i_frame_size;
        }
        else
        {
            printf("outlen=%d,i_frame_size=%d,>%d\n",outlen,i_frame_size,dstlen);
        }
    }
    /* Flush delayed frames */
    while( x264_encoder_delayed_frames( h ) )
    {
        i_frame_size = x264_encoder_encode( h, &nal, &i_nal, NULL, &pic_out );
        if( i_frame_size < 0 )
        {
            printf("x264_encoder_encode err %d\n",i_frame_size);
            x264_encoder_close( h );
            x264_picture_clean( &pic );
            return -1;
        }
        else if( i_frame_size )
        {
            if((outlen + i_frame_size) <= dstlen)
            {
                memcpy(dst+outlen,nal->p_payload,i_frame_size);
                outlen += i_frame_size;
            }
            else
            {
                printf("outlen=%d,i_frame_size=%d,>%d\n",outlen,i_frame_size,dstlen);
            }
        }
    }

    x264_encoder_close( h );
    x264_picture_clean( &pic );

    return outlen;
}


