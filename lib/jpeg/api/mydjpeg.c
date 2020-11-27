
#include <common.h>
#include <malloc.h>
#include "picture_decoder.h"


int myStreamRead(PictureBitStream* stream, void *buf, int len)
{
    unsigned char *buffer = (unsigned char*)buf;
    if (buffer == NULL)
    {
        printf("buf is NULL\n");
        return 0;
    }
    return len;
}

int myStreamSkip(PictureBitStream* stream, int len)
{
    printf("call myStreamSkip()\n");
    return myStreamRead(stream, NULL, len);
}

int jpeg_test(void *buffer, unsigned int size)
{
    JpegStream jstream;
    PictureInfo pic_info;
    PConfig configure;
    PictureDecoder *pdec= NULL;
    int result = 0;

    // init jstream
    memset(&jstream, 0, sizeof(JpegStream));
    jstream.in_buf_len = size;
    jstream.pbstream.streamRead = myStreamRead;
    jstream.pbstream.streamSkip = myStreamSkip;
    if ((jstream.in_buf = (unsigned char*)malloc(jstream.in_buf_len)) == NULL)
    {
        printf("jstream.in_buf malloc failed\n");
        return -1;
    }

	memcpy(jstream.in_buf, buffer, jstream.in_buf_len);

    // init pic_info
    memset(&pic_info, 0, sizeof(PictureInfo));

    // init configure
    memset(&configure, 0, sizeof(PConfig));
    //configure.output_pixel_format = atoi(argv[3]);
    configure.output_pixel_format = 2;
    configure.scaledown_enable = 1;
    //configure.horizon_scaledown_ratio = atoi(argv[4]);
    configure.horizon_scaledown_ratio = 1;

    if ((pdec = CreatePictureDecoder()) == NULL)
    {
        printf("CreatePictureDecoder() failed\n");
        return -1;
    }
printf("%s %d\n", __FILE__, __LINE__);
    if ((result = InitializePictureDecoder(pdec, &jstream.pbstream)) < 0)
    {
        printf("InitializePictureDecoder() failed\n");
        return -1;
    }
printf("%s %d\n", __FILE__, __LINE__);
    // init pic_info
    memset(&pic_info, 0, sizeof(PictureInfo));
printf("%s %d\n", __FILE__, __LINE__);
    if ((result = DecodePictureHeader(pdec, &pic_info)) < 0)
    {
        printf("DecodePictureHeader() failed\n");
        return -1;
    }
printf("%s %d\n", __FILE__, __LINE__);
    if (configure.horizon_scaledown_ratio > 1)
    {
        pic_info.width /= configure.horizon_scaledown_ratio;
        pic_info.height /= configure.horizon_scaledown_ratio;
    }
    pic_info.data_len = pic_info.width * pic_info.height * (configure.output_pixel_format == 6 ? 4 : 3);
    if ((pic_info.data = (unsigned char*)malloc(pic_info.data_len)) == NULL)
    {
        printf("pic_info.data malloc(%d) failed\n", pic_info.data_len);
        return -1;
    }
printf("%s %d\n", __FILE__, __LINE__);
    if ((result = SetPictureDecoderConfig(pdec, &configure)) < 0)
    {
        printf("SetPictureDecoderConfig() failed\n");
        return -1;
    }
printf("%s %d\n", __FILE__, __LINE__);
    if ((result = DecodePictureData(pdec, &pic_info)) < 0)
    {
        printf("DecodePictureData() failed\n");
        return -1;
    }
printf("%s %d\n", __FILE__, __LINE__);
    DestroyPictureDecoder(pdec);
printf("%s %d\n", __FILE__, __LINE__);
    if (jstream.in_buf)
    {
        free(jstream.in_buf);
        jstream.in_buf = NULL;
        jstream.in_buf_len = 0;
    }

	printf("data 0x%x\n", pic_info.data);
//    if (pic_info.data)
//    {
//        free(pic_info.data);
//        pic_info.data = NULL;
//        pic_info.data_len = 0;
//    }

	{
		volatile int a=1;
		while(a==1);
	}

    return 0;
}

