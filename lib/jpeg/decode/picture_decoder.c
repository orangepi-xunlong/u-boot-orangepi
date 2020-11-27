#include <common.h>
#include <malloc.h>
#include "picture_decoder.h"
#include "cdjpeg.h"
#include "jversion.h"
#include "jpeglib.h"

#ifdef __cplusplus
extern "C" {
#endif

static const char * const cdjpeg_message_table[] = {
#include "cderror.h"
  NULL
};

typedef struct PictureDecoderContext
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JpegStream *p_stream;
    int in_format;
    int out_format;
}PictureDecoderContext;

PictureDecoder* CreatePictureDecoder(void)
{
    PictureDecoderContext *p_dec = NULL;
    struct jpeg_decompress_struct *pcinfo = NULL;
    struct jpeg_error_mgr *pjerr;

    p_dec = (PictureDecoderContext*)malloc(sizeof(PictureDecoderContext));
    if (p_dec == NULL)
    {
        printf("p_dec malloc failed\n");
        return NULL;
    }
    memset(p_dec, 0, sizeof(PictureDecoderContext));

    pcinfo = &p_dec->cinfo;
    pjerr = &p_dec->jerr;

    /* Initialize the JPEG decompression object with default error handling. */
    pcinfo->err = jpeg_std_error(pjerr);
    jpeg_create_decompress(pcinfo);

    /* Add some application-specific error messages (from cderror.h) */
    pjerr->addon_message_table = cdjpeg_message_table;
    pjerr->first_addon_message = JMSG_FIRSTADDONCODE;
    pjerr->last_addon_message = JMSG_LASTADDONCODE;

    return (PictureDecoder*)p_dec;
}

void DestroyPictureDecoder(PictureDecoder* pDecoder)
{
    PictureDecoderContext *p_dec = (PictureDecoderContext*)pDecoder;
    struct jpeg_decompress_struct *pcinfo = &p_dec->cinfo;
    struct jpeg_error_mgr *pjerr = &p_dec->jerr;

    jpeg_destroy_decompress(pcinfo);
    if (p_dec->p_stream)
    {
        p_dec->p_stream = NULL;
    }

    free(p_dec);

    //exit(pjerr->num_warnings ? EXIT_WARNING : EXIT_SUCCESS);
}

static void cdxInitSource (j_decompress_ptr cinfo)
{
    PictureDecoderContext *p_dec = (PictureDecoderContext*)cinfo;
    struct jpeg_source_mgr *psrc = cinfo->src;
    JpegStream *pjpeg = p_dec->p_stream;

    psrc->bytes_in_buffer = 0;
    psrc->next_input_byte = (const JOCTET*)pjpeg->in_buf;
}

static boolean cdxFillInputBuffer (j_decompress_ptr cinfo)
{
    PictureDecoderContext *p_dec = (PictureDecoderContext*)cinfo;
    struct jpeg_source_mgr *psrc = cinfo->src;
    JpegStream *pjpeg = p_dec->p_stream;
    size_t nbytes = pjpeg->pbstream.streamRead(&pjpeg->pbstream, pjpeg->in_buf, pjpeg->in_buf_len);
    // note that JPEG is happy with less than the full read,
    // as long as the result is non-zero
    if (nbytes == 0)
    {
        printf("nbytes is 0\n");
        return FALSE;
    }

    psrc->next_input_byte = (const JOCTET*)pjpeg->in_buf;
    psrc->bytes_in_buffer = nbytes;
    return TRUE;
}

static void cdxSkipInputData (j_decompress_ptr cinfo, long num_bytes)
{
    PictureDecoderContext *p_dec = (PictureDecoderContext*)cinfo;
    struct jpeg_source_mgr *psrc = cinfo->src;
    JpegStream *pjpeg = p_dec->p_stream;

    if (num_bytes > (long)psrc->bytes_in_buffer)
    {
        size_t bytes_skip = num_bytes - psrc->bytes_in_buffer;
        while (bytes_skip > 0)
        {
            size_t bytes = pjpeg->pbstream.streamSkip(&pjpeg->pbstream, bytes_skip);
            if (bytes <= 0 || bytes > bytes_skip)
            {
                cinfo->err->error_exit((j_common_ptr)cinfo);
                return;
            }
            bytes_skip -= bytes;
        }
        psrc->next_input_byte = (const JOCTET*)pjpeg->in_buf;
        psrc->bytes_in_buffer = 0;
    }
    else
    {
        psrc->next_input_byte += num_bytes;
        psrc->bytes_in_buffer -= num_bytes;
    }

}

static void cdxTermSource (j_decompress_ptr cinfo)
{
    /* Reserve */
}

void cdxJpegSrc (PictureDecoderContext *p_dec)
{
    struct jpeg_decompress_struct *pcinfo = &p_dec->cinfo;
    struct jpeg_source_mgr *psrc;

    if (pcinfo->src == NULL)
    {	/* first time for this JPEG object? */
        pcinfo->src = (struct jpeg_source_mgr *)
            (*pcinfo->mem->alloc_small) ((j_common_ptr) pcinfo, JPOOL_PERMANENT,
            SIZEOF(struct jpeg_source_mgr));
        if (pcinfo->src == NULL)
        {
            printf("pcinfo->src malloc failed\n");
            return;
        }
        memset(pcinfo->src, 0 , sizeof(struct jpeg_source_mgr));
    }

    psrc = pcinfo->src;
    psrc->init_source = cdxInitSource;
    psrc->fill_input_buffer = cdxFillInputBuffer;
    psrc->skip_input_data = cdxSkipInputData;
    psrc->resync_to_restart = jpeg_resync_to_restart;
    psrc->term_source = cdxTermSource;
    psrc->bytes_in_buffer = 0;
    psrc->next_input_byte = NULL;
}

int InitializePictureDecoder(PictureDecoder* pDecoder, PictureBitStream* pStream)
{
    PictureDecoderContext *p_dec = NULL;
    if (pStream == NULL)
    {
        printf("pStream is NULL\n");
        return -1;
    }

    p_dec = (PictureDecoderContext*)pDecoder;
    p_dec->p_stream = (JpegStream*)pStream;
    if (p_dec->p_stream->in_buf == NULL)
    {
        if ((p_dec->p_stream->in_buf = (unsigned char*)malloc(IN_FILE_MALLOC_LEN)) == NULL)
        {
            printf("p_dec->p_stream->in_buf malloc failed\n");
            return -1;
        }
        p_dec->p_stream->in_buf_len = IN_FILE_MALLOC_LEN;
    }

    cdxJpegSrc(p_dec);

    return 0;
}

void ResetPictureDecoder(PictureDecoder* pDecoder)
{
    PictureDecoderContext *p_dec = (PictureDecoderContext*)pDecoder;
    struct jpeg_decompress_struct *pcinfo = &p_dec->cinfo;
    struct jpeg_error_mgr *pjerr = &p_dec->jerr;

    jpeg_destroy_decompress(pcinfo);
    if (p_dec->p_stream)
    {
        free(p_dec->p_stream);
        p_dec->p_stream = NULL;
    }

    memset(p_dec, 0, sizeof(PictureDecoderContext));

    /* Initialize the JPEG decompression object with default error handling. */
    pcinfo->err = jpeg_std_error(pjerr);
    jpeg_create_decompress(pcinfo);

    /* Add some application-specific error messages (from cderror.h) */
    pjerr->addon_message_table = cdjpeg_message_table;
    pjerr->first_addon_message = JMSG_FIRSTADDONCODE;
    pjerr->last_addon_message = JMSG_LASTADDONCODE;
}

int DecodePictureHeader(PictureDecoder* pDecoder, PictureInfo* pPictureInfo)
{
    PictureDecoderContext *p_dec = NULL;
    struct jpeg_decompress_struct *pcinfo = NULL;
    if (pPictureInfo == NULL)
    {
        printf("pPictureInfo is NULL, now malloc it\n");
        pPictureInfo = (PictureInfo*)malloc(sizeof(PictureInfo));
        if (pPictureInfo == NULL)
        {
            printf("pPictureInfo malloc failed\n");
            return -1;
        }
        memset(pPictureInfo, 0, sizeof(PictureInfo));
    }

    p_dec= (PictureDecoderContext*)pDecoder;


    pcinfo = &p_dec->cinfo;
    if (p_dec->p_stream->in_buf)
    {
        jpeg_read_header(pcinfo, TRUE);
    }
    else
    {
        printf("p_dec->p_stream->in_buf is NULL\n");
    }
    pPictureInfo->pixel_format = p_dec->in_format = pcinfo->jpeg_color_space;
    pPictureInfo->width = pcinfo->output_width = pcinfo->image_width;
    pPictureInfo->height = pcinfo->output_height = pcinfo->image_height;
    printf("PictureInfo: pixel_format=%d, width=%d, height=%d\n",
        pPictureInfo->pixel_format, pPictureInfo->width, pPictureInfo->height);

    return 0;
}

int SetPictureDecoderConfig(PictureDecoder*  pDecoder, PConfig* pVConfig)
{
    PictureDecoderContext *p_dec = NULL;
    struct jpeg_decompress_struct *pcinfo = NULL;
    if (pVConfig == NULL)
    {
        printf("pVConfig is NULL\n");
        return -1;
    }

    p_dec = (PictureDecoderContext*)pDecoder;
    pcinfo = &p_dec->cinfo;
    p_dec->out_format = pcinfo->out_color_space = pVConfig->output_pixel_format;
    if (pVConfig->output_pixel_format == 6)
    {
        pcinfo->out_color_space = JCS_RGB;
    }
    if (p_dec->out_format != 2 && p_dec->out_format != 3 && p_dec->out_format != 6)
    {
        printf("p_dec->out_format is not 2 or 3 or 6\n");
        return -1;
    }
    if (pVConfig->scaledown_enable)
    {
        pcinfo->scale_denom = pVConfig->horizon_scaledown_ratio;
    }
    if (pVConfig->rotation_enable)
    {
        /* Reserve */
    }

    return 0;
}



int DecodePictureData(PictureDecoder* pDecoder, PictureInfo* pPictureInfo)
{
    PictureDecoderContext *p_dec = (PictureDecoderContext*)pDecoder;
    struct jpeg_decompress_struct *pcinfo = &p_dec->cinfo;
    unsigned char **pbuffer; /* Output row buffer */
    int row_stride;     /* physical row width in output buffer */
    int finished = 1;
    unsigned char *pixels = pPictureInfo->data;

    (void) jpeg_start_decompress(pcinfo);

    /* JSAMPLEs per row in output buffer */
    row_stride = pcinfo->output_width * pcinfo->output_components;

    /* Make a one-row-high sample array that will go away when done with image */
    pbuffer = (*pcinfo->mem->alloc_sarray)((j_common_ptr) pcinfo, JPOOL_IMAGE, row_stride, 1);

    while (pcinfo->output_scanline < pcinfo->output_height)
    {
printf("%s %d\n", __FILE__, __LINE__);
	    int num_rows = jpeg_read_scanlines(pcinfo, pbuffer, 1);

	    if (num_rows == 0)
	    {
	        finished = 0;
	        break;
	    }

		// Greyscale
	    if (pcinfo->output_components == 1)
	    {
	        unsigned int i;
	        unsigned char *in = *pbuffer;
	        for (i = 0; i < pcinfo->output_width * num_rows; ++i)
	        {
	            *pixels++ = *in; // red
	            *pixels++ = *in; // green
	            *pixels++ = *in; // blue
	            if (p_dec->out_format == 6)
	            {
	                *pixels++ = 255;
	            }
	            ++in;
	        }
	    }
	    // RGB
	    else if (pcinfo->output_components == 3)
	    {
	        // RGBA
	        if (p_dec->out_format == 6)
	        {
	            int i, j;
	            unsigned char *in = *pbuffer;
	            for (i = 0; i< num_rows; i++)
	            {
	                for (j = 0; j < (int)pcinfo->output_width; j++)
	                {
	                    memcpy(pixels, in, 3);
	                    pixels += 3;
	                    in += 3;
	                    *pixels++ = 255;
	                }
	            }
	        }
	        else
	        {
	            memcpy(pixels, *pbuffer, pcinfo->output_width * num_rows * 3);
	            pixels += pcinfo->output_width * num_rows * 3;
	        }
	    }
    }
printf("%s %d\n", __FILE__, __LINE__);
    pPictureInfo->pixel_format = p_dec->out_format;
    pPictureInfo->width = pcinfo->output_width;
    pPictureInfo->height = pcinfo->output_height;

    if (p_dec->out_format == 2)
    {
        pPictureInfo->data_len = pPictureInfo->width * pPictureInfo->height * 3;
    }
    else if (p_dec->out_format == 6)
    {
        pPictureInfo->data_len = pPictureInfo->width * pPictureInfo->height * 4;
    }
    else if (p_dec->out_format == 3)
    {
        int i, len;
        unsigned char *py = NULL, *pu = NULL, *pv = NULL;
        unsigned char *pyuv[3] = {NULL, NULL, NULL};
        unsigned char *pp = pPictureInfo->data;
        pPictureInfo->data_len = pPictureInfo->width * pPictureInfo->height * 3;
        len = pPictureInfo->data_len / 3;
        for (i = 0; i< 3; i++)
        {
            if ((pyuv[i] = malloc(len)) == NULL)
            {
                printf("pyuv[%d] malloc failed\n", i);
                return -1;
            }
        }
        py = pyuv[0];
        pu = pyuv[1];
        pv = pyuv[2];
        for (i = 0; i < len; i++)
        {
            *py++ = *pp++;
            *pu++ = *pp++;
            *pv++ = *pp++;
        }
        memcpy(pPictureInfo->data, pyuv[0], len);
        memcpy((pPictureInfo->data + len), pyuv[1], len);
        memcpy((pPictureInfo->data + len * 2), pyuv[2], len);
        for (i = 0; i < 3; i++)
        {
            free(pyuv[i]);
            pyuv[i] = NULL;
        }

    }

    if(finished)
    {
    	(void) jpeg_finish_decompress(pcinfo);
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
