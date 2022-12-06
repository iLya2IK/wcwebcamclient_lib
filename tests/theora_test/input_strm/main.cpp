/*! \file main.cpp
 *  A simple test program to demonstrate input streaming.
 */

/*===============================================================*/
/* This is an example of how to use the wcWebCamClient library.  */
/* In this example, a client is created, authorized on the       */
/* server, and starts listening to streaming from selected input */
/* device. All frames of then incoming stream are encoded in     */
/* Theora format and saved as an .OGV output file.               */
/*                                                               */
/* Part of wcWebCamClient library project                        */
/*                                                               */
/* Copyright 2022 Ilya Medvedkov                                 */
/*===============================================================*

 The example uses code from the Theora project (test png2theora).

 https://github.com/xiph/theora/blob/master/examples/png2theora.c
 *****************************************************************/

#include <iostream>
#include <fstream>
#include <wcwebcamclient.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <math.h>
#include <wcJSON.h>
#include <png.h>
#include <ogg/ogg.h>
#include "theora/theoraenc.h"
#include "../../commline_tools.h"

using namespace std;

/* The total program timeout in seconds */
static const uint32_t TIME_OUT = 60;
static const size_t MAX_INPUT_BUFFER = 0x7D000;

static uint8_t running = 0;

static ofstream outfile;
static char * mem_frame_buffer = NULL;
static size_t frame_size = 0;
static std::string input_device;

static bool theora_initialized = false;
static th_ycbcr_buffer ycbcr;
static uint32_t video_fps_numerator = 24;
static uint32_t video_fps_denominator = 1;
static uint32_t video_aspect_numerator = 0;
static uint32_t video_aspect_denominator = 0;
static uint32_t discrete_delta = 300;
static bool soft_target = false;
/* if video_rate > 0 : soft_target = true and encoder flags configure to use
   the video_rate value */
static uint32_t video_rate = 0;
/* if video_rate == 0 : soft_target = false */
static uint32_t video_quality = 25;
ogg_uint32_t keyframe_frequency=64;
static th_pixel_fmt chroma_format = TH_PF_420;

static ogg_stream_state ogg_os;
static ogg_packet op;
static ogg_page og;

static th_enc_ctx      *td = NULL;
static th_info          ti;

static uint8_t clamp(int d)
{
    if(d < 0)
        return 0;

    if(d > 255)
        return 255;

    return d;
}

static int ilog(unsigned _v)
{
    int ret;
    for(ret=0; _v; ret++)
        _v>>=1;
    return ret;
}

/* Covert the png rgb data to the ycbcr buffer
   src: xiph/theora/blob/master/examples/png2theora.c */
static void rgb_to_yuv(png_bytep *png,
                        th_ycbcr_buffer ycbcr,
                        uint32_t w, uint32_t h)
{
    uint32_t x;
    uint32_t y;

    uint32_t x1;
    uint32_t y1;

    uint64_t yuv_w;

    uint8_t *yuv_y;
    uint8_t *yuv_u;
    uint8_t *yuv_v;

    yuv_w = ycbcr[0].width;

    yuv_y = ycbcr[0].data;
    yuv_u = ycbcr[1].data;
    yuv_v = ycbcr[2].data;

    /*This ignores gamma and RGB primary/whitepoint differences.
      It also isn't terribly fast (though a decent compiler will
      strength-reduce the division to a multiplication).*/

    if (chroma_format == TH_PF_420)
    {
        for(y = 0; y < h; y += 2)
        {
            y1=y+(y+1<h);
            for(x = 0; x < w; x += 2)
            {
                x1=x+(x+1<w);
                png_byte r0 = png[y][3 * x + 0];
                png_byte g0 = png[y][3 * x + 1];
                png_byte b0 = png[y][3 * x + 2];
                png_byte r1 = png[y][3 * x1 + 0];
                png_byte g1 = png[y][3 * x1 + 1];
                png_byte b1 = png[y][3 * x1 + 2];
                png_byte r2 = png[y1][3 * x + 0];
                png_byte g2 = png[y1][3 * x + 1];
                png_byte b2 = png[y1][3 * x + 2];
                png_byte r3 = png[y1][3 * x1 + 0];
                png_byte g3 = png[y1][3 * x1 + 1];
                png_byte b3 = png[y1][3 * x1 + 2];

                yuv_y[x  + y * yuv_w]  = clamp((65481*r0+128553*g0+24966*b0+4207500)/255000);
                yuv_y[x1 + y * yuv_w]  = clamp((65481*r1+128553*g1+24966*b1+4207500)/255000);
                yuv_y[x  + y1 * yuv_w] = clamp((65481*r2+128553*g2+24966*b2+4207500)/255000);
                yuv_y[x1 + y1 * yuv_w] = clamp((65481*r3+128553*g3+24966*b3+4207500)/255000);

                yuv_u[(x >> 1) + (y >> 1) * ycbcr[1].stride] =
                    clamp( ((-33488*r0-65744*g0+99232*b0+29032005)/4 +
                            (-33488*r1-65744*g1+99232*b1+29032005)/4 +
                            (-33488*r2-65744*g2+99232*b2+29032005)/4 +
                            (-33488*r3-65744*g3+99232*b3+29032005)/4)/225930);
                yuv_v[(x >> 1) + (y >> 1) * ycbcr[2].stride] =
                    clamp( ((157024*r0-131488*g0-25536*b0+45940035)/4 +
                            (157024*r1-131488*g1-25536*b1+45940035)/4 +
                            (157024*r2-131488*g2-25536*b2+45940035)/4 +
                            (157024*r3-131488*g3-25536*b3+45940035)/4)/357510);
            }
        }
    }
    else if (chroma_format == TH_PF_444)
    {
        for(y = 0; y < h; y++)
        {
            for(x = 0; x < w; x++)
            {
                png_byte r = png[y][3 * x + 0];
                png_byte g = png[y][3 * x + 1];
                png_byte b = png[y][3 * x + 2];

                yuv_y[x + y * yuv_w] = clamp((65481*r+128553*g+24966*b+4207500)/255000);
                yuv_u[x + y * yuv_w] = clamp((-33488*r-65744*g+99232*b+29032005)/225930);
                yuv_v[x + y * yuv_w] = clamp((157024*r-131488*g-25536*b+45940035)/357510);
            }
        }
    }
    else      /* TH_PF_422 */
    {
        for(y = 0; y < h; y += 1)
        {
            for(x = 0; x < w; x += 2)
            {
                x1=x+(x+1<w);
                png_byte r0 = png[y][3 * x + 0];
                png_byte g0 = png[y][3 * x + 1];
                png_byte b0 = png[y][3 * x + 2];
                png_byte r1 = png[y][3 * x1 + 0];
                png_byte g1 = png[y][3 * x1 + 1];
                png_byte b1 = png[y][3 * x1 + 2];

                yuv_y[x  + y * yuv_w] = clamp((65481*r0+128553*g0+24966*b0+4207500)/255000);
                yuv_y[x1 + y * yuv_w] = clamp((65481*r1+128553*g1+24966*b1+4207500)/255000);

                yuv_u[(x >> 1) + y * ycbcr[1].stride] =
                    clamp( ((-33488*r0-65744*g0+99232*b0+29032005)/2 +
                            (-33488*r1-65744*g1+99232*b1+29032005)/2)/225930);
                yuv_v[(x >> 1) + y * ycbcr[2].stride] =
                    clamp( ((157024*r0-131488*g0-25536*b0+45940035)/2 +
                            (157024*r1-131488*g1-25536*b1+45940035)/2)/357510);
            }
        }
    }

}

struct mem_file
{
    char *buf;
    size_t size;
    size_t cur;
};

void mem_read(png_structp png_ptr, png_bytep buf, png_size_t sz)
{
    mem_file * stream = (mem_file*)png_get_io_ptr(png_ptr);
    size_t cp = sz;
    if ((cp + stream->cur) > stream->size)
    {
        cp = stream->size - stream->cur;
    }
    memcpy(buf, &(stream->buf[stream->cur]), cp);
    stream->cur += cp;
    if (cp != sz)
        png_error(png_ptr, "Read Error");
}

/* Load PNG data from frame
   src: xiph/theora/blob/master/examples/png2theora.c */
static int png_read(const void * buffer,
                    size_t buflen,
                    uint32_t *w, uint32_t *h,
                    th_ycbcr_buffer ycbcr)
{
    char * fp = (char *) buffer + WC_STREAM_FRAME_HEADER_SIZE;
    buflen -= WC_STREAM_FRAME_HEADER_SIZE;
    uint8_t header[8];
    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_ptr;
    png_bytep row_data;
    png_bytep *row_pointers;
    png_color_16p bkgd;
    png_uint_32 width;
    png_uint_32 height;
    uint64_t yuv_w;
    uint64_t yuv_h;
    int bit_depth;
    int color_type;
    int interlace_type;
    int compression_type;
    int filter_method;
    png_uint_32 y;

    memcpy(header, fp, 8);
    if(png_sig_cmp(header, 0, 8))
    {
        cerr << "error:not a PNG" << endl;
        return 1;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                     NULL, NULL, NULL);
    if(!png_ptr)
    {
        cerr << "error: couldn't create png read structure" << endl;
        return 1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr)
    {
        cerr << "error:  couldn't create png info structure" << endl;
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return 1;
    }

    end_ptr = png_create_info_struct(png_ptr);
    if(!end_ptr)
    {
        cerr << "error: couldn't create png info structure" << endl;
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return 1;
    }

    struct mem_file f = { .buf = fp, .size = buflen, .cur = 8 };
    png_set_read_fn(png_ptr, &f, mem_read);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
                 &interlace_type, &compression_type, &filter_method);
    png_set_expand(png_ptr);
    if(bit_depth<8)
        png_set_packing(png_ptr);
    if(bit_depth==16)
        png_set_strip_16(png_ptr);
    if(!(color_type&PNG_COLOR_MASK_COLOR))
        png_set_gray_to_rgb(png_ptr);
    if(png_get_bKGD(png_ptr, info_ptr, &bkgd))
    {
        png_set_background(png_ptr, bkgd, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
    }
    /*Note that color_type 2 and 3 can also have alpha, despite not setting the
       PNG_COLOR_MASK_ALPHA bit.
      We always strip it to prevent libpng from overrunning our buffer.*/
    png_set_strip_alpha(png_ptr);

    row_data = (png_bytep)png_malloc(png_ptr,
                                     3*height*width*sizeof(*row_data));
    row_pointers = (png_bytep *)png_malloc(png_ptr,
                                           height*sizeof(*row_pointers));
    for(y = 0; y < height; y++)
    {
        row_pointers[y] = row_data + y*(3*width);
    }
    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, end_ptr);

    *w = width;
    *h = height;
    /* Must hold: yuv_w >= w */
    yuv_w = (*w + 15) & ~15;
    /* Must hold: yuv_h >= h */
    yuv_h = (*h + 15) & ~15;

    /* Do we need to allocate a buffer */
    if (!ycbcr[0].data)
    {
        ycbcr[0].width = yuv_w;
        ycbcr[0].height = yuv_h;
        ycbcr[0].stride = yuv_w;
        ycbcr[1].width = (chroma_format == TH_PF_444) ? yuv_w : (yuv_w >> 1);
        ycbcr[1].stride = ycbcr[1].width;
        ycbcr[1].height = (chroma_format == TH_PF_420) ? (yuv_h >> 1) : yuv_h;
        ycbcr[2].width = ycbcr[1].width;
        ycbcr[2].stride = ycbcr[1].stride;
        ycbcr[2].height = ycbcr[1].height;

        ycbcr[0].data = (uint8_t*) malloc(ycbcr[0].stride * ycbcr[0].height);
        ycbcr[1].data = (uint8_t*) malloc(ycbcr[1].stride * ycbcr[1].height);
        ycbcr[2].data = (uint8_t*) malloc(ycbcr[2].stride * ycbcr[2].height);
    }
    else
    {
        if ((ycbcr[0].width != (int64_t)yuv_w) || (ycbcr[0].height != (int64_t)yuv_h))
        {
            cerr << "Input size " << yuv_w << "x" << yuv_h <<
                    " does not match " << ycbcr[0].width << "x" << ycbcr[0].height << endl;
            return 1;
        }
    }

    rgb_to_yuv(row_pointers, ycbcr, *w, *h);

    png_free(png_ptr, row_pointers);
    png_free(png_ptr, row_data);
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_ptr);

    return 0;
}

/* Initialize the Theora encoder
   src: xiph/theora/blob/master/examples/png2theora.c */
static int theora_init(uint32_t w, uint32_t h)
{
    srand(time(NULL));
    if (ogg_stream_init(&ogg_os, rand()))
    {
        cerr << "error: couldn't create ogg stream state" << endl;
        return 1;
    }

    if (discrete_delta > 0) {
        video_fps_numerator = 1000 / discrete_delta;
        keyframe_frequency = video_fps_numerator * 2 + 1;
    }

    th_info_init(&ti);
    ti.frame_width = ((w + 15) >>4)<<4;
    ti.frame_height = ((h + 15)>>4)<<4;
    ti.pic_width = w;
    ti.pic_height = h;
    ti.pic_x = 0;
    ti.pic_y = 0;
    ti.fps_numerator = video_fps_numerator;
    ti.fps_denominator = video_fps_denominator;
    ti.aspect_numerator = video_aspect_numerator;
    ti.aspect_denominator = video_aspect_denominator;
    ti.colorspace = TH_CS_UNSPECIFIED;
    ti.pixel_fmt = chroma_format;
    ti.target_bitrate = video_rate;
    ti.quality = video_quality;
    ti.keyframe_granule_shift=ilog(keyframe_frequency-1);
    td=th_encode_alloc(&ti);
    th_info_clear(&ti);
    /* setting just the granule shift only allows power-of-two keyframe
       spacing.  Set the actual requested spacing. */
    int ret = th_encode_ctl(td,TH_ENCCTL_SET_KEYFRAME_FREQUENCY_FORCE,
                            &keyframe_frequency,
                            sizeof(keyframe_frequency - 1));
    if (ret < 0)
        cerr << "Could not set keyframe interval to " << (int)keyframe_frequency << endl;

    if (soft_target) {
        /* reverse the rate control flags to favor a 'long time' strategy */
        int arg = TH_RATECTL_CAP_UNDERFLOW;
        ret=th_encode_ctl(td,TH_ENCCTL_SET_RATE_FLAGS,&arg,sizeof(arg));
        if (ret < 0)
            cerr << "Could not set encoder flags for --soft-target" << endl;

        /* Default buffer control is overridden on two-pass */
        if((keyframe_frequency*7>>1) > 5*video_fps_numerator/video_fps_denominator)
            arg = keyframe_frequency * 7 >> 1;
        else
            arg = 5 * video_fps_numerator/video_fps_denominator;
        ret = th_encode_ctl(td,TH_ENCCTL_SET_RATE_BUFFER,&arg,sizeof(arg));
        if (ret < 0)
            cerr << "Could not set rate control buffer for --soft-target" << endl;
    }
    th_comment       tc;
    /* write the bitstream header packets with proper page interleave */
    th_comment_init(&tc);
    /* first packet will get its own page automatically */
    if (th_encode_flushheader(td,&tc,&op) <= 0) {
        cerr << "Internal Theora library error" << endl;
        return 1;
    }
    th_comment_clear(&tc);
    ogg_stream_packetin(&ogg_os,&op);
    if(ogg_stream_pageout(&ogg_os,&og)!=1) {
        cerr << "Internal Ogg library error" << endl;
        return 1;
    }
    outfile.write((const char *)og.header,og.header_len);
    outfile.write((const char *)og.body,og.body_len);
    /* create the remaining theora headers */
    for(;;)
    {
        ret=th_encode_flushheader(td,&tc,&op);
        if(ret<0)
        {
            cerr << "Internal Theora library error" << endl;
            return 1;
        }
        else if(!ret)
            break;
        ogg_stream_packetin(&ogg_os,&op);
    }
    for(;;)
    {
        int result = ogg_stream_flush(&ogg_os,&og);
        if (result < 0) {
            /* can't get here */
            cerr << "Internal Ogg library error" << endl;
            return 1;
        }
        if (result == 0)
            break;
        outfile.write((const char *)og.header,og.header_len);
        outfile.write((const char *)og.body,og.body_len);
    }

    theora_initialized = true;
    return 0;
}

/* Encode and write the new Theora frame
   src: xiph/theora/blob/master/examples/png2theora.c */
static int theora_write_frame(th_ycbcr_buffer ycbcr, int last)
{
    ogg_packet op;
    ogg_page og;

    if(th_encode_ycbcr_in(td, ycbcr))
    {
        cerr << "error: could not encode frame" << endl;
        return 1;
    }

    if(!th_encode_packetout(td, last, &op))
    {
        cerr << "error: could not read packets" << endl;
        return 1;
    }

    ogg_stream_packetin(&ogg_os, &op);
    while (ogg_stream_pageout(&ogg_os, &og))
    {
        outfile.write((const char *)og.header, og.header_len);
        outfile.write((const char *)og.body, og.body_len);
    }
    return 0;
}

/* Callback. CURL was initialized successfully. */
void onCURLinit(wcHandle self, wcTask task)
{
    cout << "CURL initialized" << endl;
}

/* Callback. Authorization was successful. */
void onAuth(wcHandle self, wcTask task)
{
    char * res = NULL;
    /* Get a new session id and display it on the screen. */
    if (wcClientGetStrValue(self, wcstSID, &res) == WC_OK)
    {
        if (res)
        {
            cout << res << endl;
            free(res);
        }
    }
    running = 2;
}

/* Callback. The client has been disabled. */
void onDisconnect(wcHandle self, wcTask task)
{
    cout << "Client disconnected" << endl;
    running = 10;
}

/* Callback. Connection state changed. */
void onConnChanged(wcHandle self, int state)
{
    cout << "Client connection changed " << state << endl;
}

/* Callback. Log was changed. */
void onLog(wcHandle self, const char * str)
{
    /* Display new log entry. */
    cout << str << endl;
}

static const string cDEVICE("device");
static const string cSUBPROTO("subproto");
static const string cDELTA("delta");
static const string RAW_PNG_PROTO("RAW_PNG");
static bool device_online = false;
static bool device_streaming = false;

/* Callback. A device list has changed. */
void onDevicesUpdated(wcHandle self, wcTask tsk, const char * json)
{
    wcJSON jarr(json);
    if (jarr.isArray()) {
        for (int i = 0; i < jarr.length(); i++) {
            wcJSON obj = jarr.getArrayItem(i);
            if (obj.isObject()) {
                wcJSON name = obj.getObjItem(cDEVICE.c_str());
                if (name.isString() && (input_device.compare(name.valueString()) == 0)) {
                    device_online = true;
                    break;
                }
            }
        }
    }

    if (device_online && device_streaming)
        running = 4;
    else
        running = 2;
}

/* Callback. A streams list has changed. */
void onStreamsUpdated(wcHandle self, wcTask tsk, const char * json)
{
    wcJSON jarr(json);
    if (jarr.isArray()) {
        for (int i = 0; i < jarr.length(); i++) {
            wcJSON obj = jarr.getArrayItem(i);
            if (obj.isObject()) {
                wcJSON name = obj.getObjItem(cDEVICE.c_str());
                if (name.isString() && (input_device.compare(name.valueString()) == 0)) {
                    wcJSON subproto = obj.getObjItem(cSUBPROTO.c_str());
                    if (subproto.isString() && (RAW_PNG_PROTO.compare(subproto.valueString())== 0)) {
                        device_streaming = true;
                        wcJSON delta = obj.getObjItem(cDELTA.c_str());
                        if (delta.isNumber()) {
                            discrete_delta = delta.valueInt();
                            if (discrete_delta < 200) discrete_delta = 200;
                        }
                        break;
                    }
                }
            }
        }
    }

    if (device_online && device_streaming)
        running = 4;
    else
        running = 2;
}

/* Callback. IO stream closed. */
void onIOTaskFinished(wcHandle client, wcTask tsk)
{
    cout << "Input stream closed" << endl;
    running = 10;
}

/* Callback. IO stream started. */
void onIOTaskStarted(wcHandle client, wcTask tsk)
{
    cout << "Input stream started" << endl;
}

/* Callback. Task updated. */
void onTaskSynchronized(wcHandle client, wcTask tsk)
{
    wcTaskClass tcid = WC_BASE_TASK;
    /* Get the class for `tsk` */
    if (wcTaskGetClass(tsk, &tcid) == WC_OK) {
        if (tcid == WC_IN_STREAM_TASK) {
            /* The input stream task updated (the new frame has incoming)  */
            frame_size = MAX_INPUT_BUFFER;
            wcInTaskPopFrame(tsk, (void**)&mem_frame_buffer, &frame_size);

            uint32_t w, h;
            /* Read frame as png-image  */
            if (png_read(mem_frame_buffer, frame_size, &w, &h, ycbcr)) {
                cerr << "Could not read frame" << endl;
                running = 10;
                return;
            }
            if (!theora_initialized) {
                /* Initialize Theora encoder  */
                if (theora_init(w, h)) {
                    cerr << "Could not init theora encoder" << endl;
                    running = 10;
                    return;
                }
            }

            /* Write next Theora frame  */
            if (theora_write_frame(ycbcr, 0)) {
                cerr << "Encoding error" << endl;
                running = 10;
                return;
            }
        }
    }
}

class MyInputParser : public InputParser
{
public:
    using InputParser::InputParser;

    const std::string& getInput()
    {
        static const std::string CMD_INPUT("--input");
        static const std::string CMD_INPUT_SYN("-i");
        const std::string & res = getCmdOption(CMD_INPUT);
        if (res.empty())
        {
            return getCmdOption(CMD_INPUT_SYN);
        }
        else
            return res;
    }

    const std::string& getOutput()
    {
        static const std::string CMD_OUTPUT("--output");
        static const std::string CMD_OUTPUT_SYN("-o");
        const std::string & res = getCmdOption(CMD_OUTPUT);
        if (res.empty())
        {
            return getCmdOption(CMD_OUTPUT_SYN);
        }
        else
            return res;
    }

    const int getVideoQuality()
    {
        static const std::string CMD_VIDEOQ("--video-quality");
        static const std::string CMD_VIDEOQ_SYN("-v");
        std::string res = getCmdOption(CMD_VIDEOQ);
        if (res.empty())
            res = getCmdOption(CMD_VIDEOQ_SYN);
        int vq;
        if (!res.empty()) {
            vq = rint(atof(res.c_str())*6.3);
            if (vq<0 || vq>63){
                cerr << "Illegal video quality (choose 0 through 10)" << endl;
                return 0;
            }
        } else vq = 25;
        return vq;
    }

    const int getVideoRate()
    {
        static const std::string CMD_VIDEOR("--video-rate");
        static const std::string CMD_VIDEOR_SYN("-V");
        std::string res = getCmdOption(CMD_VIDEOR);
        if (res.empty())
            res = getCmdOption(CMD_VIDEOR_SYN);
        int vq;
        if (!res.empty()) {
            vq = atoi(res.c_str()) * 1024;
        } else vq = 0;
        return vq;
    }

    const th_pixel_fmt getChroma()
    {
        static const std::string CMD_CHROMA("--chroma");
        const std::string & res = getCmdOption(CMD_CHROMA);
        th_pixel_fmt vq;
        if (!res.empty()) {
            int vqv = atoi(res.c_str());
            switch (vqv) {
                case 444 : vq = TH_PF_444;
                           break;
                case 422 : vq = TH_PF_422;
                           break;
                case 420 : vq = TH_PF_420;
                           break;
                default:
                    cerr << "Illegal chroma subsampling: " << vqv << endl;
                    vq = TH_PF_420;
                    break;
            }
        } else vq = TH_PF_420;
        return vq;
    }

    virtual void printCommonHelp() override
    {
        InputParser::printCommonHelp();
        cout << "  -i --input <devicename>         the device name to get data stream from (required);" << endl;
        cout << "  -o --output <filename.ogv>      file name for encoded output (required);" << endl;
        cout << "  -v --video-quality <n>          Theora quality selector from 0 to 10" << endl;
        cout << "                                  (0 yields smallest files but lowest" << endl;
        cout << "                                  video quality. 10 yields highest" << endl;
        cout << "                                  fidelity but large files)" << endl;
        cout << "  -V --video-rate <n>             bitrate target for Theora video in kbits per second" << endl;
        cout << "                                  Use a large reservoir and treat the rate" << endl;
        cout << "                                  as a soft target; rate control is less" << endl;
        cout << "                                  strict but resulting quality is usually" << endl;
        cout << "                                  higher/smoother overall. Soft target also" << endl;
        cout << "                                  allows an optional -v setting to specify" << endl;
        cout << "                                  a minimum allowed quality." << endl;
        cout << "  --chroma {444|422|420}          Use 4:4:4 chroma subsampling" << endl;
        cout << "                                  Use 4:2:2 chroma subsampling" << endl;
        cout << "                                  (4:2:0 is default)" << endl;
    }
};

int main(int argc, char * argv[])
{
    /* Parse command line */
    MyInputParser input(argc, argv);
    const std::string &host = input.getHost();
    const std::string &proxy = input.getProxy();
    const std::string &user = input.getUserName();
    const std::string &pwrd = input.getUserPassword();
    std::string device = input.getDevice();
    if (device.empty())
        device = "device_test001_input";
    input_device = input.getInput();
    if (input_device.empty())
        input_device = "device_test001_output";
    std::string output_file = input.getOutput();
    if (output_file.empty())
        output_file = "output.ogv";

    if (input.showHelp())
    {
        /* Show help doc */
        input.printCommonHelp();
        return 0;
    }

    outfile = ofstream(output_file, ios::out | ios::trunc | ios::binary);
    if (!outfile.is_open())
    {
        cerr << "Can't open output file. Wrong file name or the file is blocked: " << output_file << endl;
        return 1;
    }

    chroma_format = input.getChroma();
    video_quality = input.getVideoQuality();
    video_rate = input.getVideoRate();
    if (video_rate > 0) { soft_target = true; }

    if (!host.empty())
    {
        /* Main part */
        wcHandle client = wcClientCreate();

        /* Configure certificate */
        if (input.ignoreTLSCert())
            wcClientSetBoolState(client, wcstVerifyTLS, WC_FALSE);
        /* Configure proxy */
        if (!proxy.empty())
            wcClientSetStrValue(client, wcstProxy, proxy.c_str());
        /* Configure host */
        wcClientSetStrValue(client, wcstHostName, host.c_str());

        /* Configure callbacks */
        wcSetTaskCallback(client, wccbkSuccessAuth, onAuth);
        wcSetCStringCallback(client, wccbkAddLog, onLog);
        wcSetNotifyCallback(client, wccbkInitCURL, onCURLinit);
        wcSetNotifyCallback(client, wccbkDisconnect, onDisconnect);
        wcSetConnCallback(client, onConnChanged);
        /* Configure task callbacks */
        wcSetJSONStrCallback(client, wccbkSuccessUpdateDevices, onDevicesUpdated);
        wcSetJSONStrCallback(client, wccbkSuccessUpdateStreams, onStreamsUpdated);
        /* Configure streaming callbacks */
        wcSetTaskCallback(client, wccbkAfterLaunchInStream, onIOTaskStarted);
        wcSetTaskCallback(client, wccbkSynchroUpdateTask, onTaskSynchronized);
        wcSetTaskCallback(client, wccbkSuccessIOStream, onIOTaskFinished);

        /* Start client */
        wcClientStart(client);

        auto start_timestamp = std::chrono::system_clock::now();
        while (running < 8)
        {
            switch (running)
            {
            case 0:
            {
                /* Authorize client */
                wcClientSetStrValue(client, wcstDeviceName, device.c_str());
                wcClientAuth(client, user.c_str(), pwrd.c_str());
                running = 1;
                break;
            }
            case 2:
            {
                /* Start to search online devices and streams */
                wcClientInvalidateState(client, wcstDevices);
                wcClientInvalidateState(client, wcstStreams);
                running = 3;
                break;
            }
            case 4:
            {
                /* Allocate buffer */
                mem_frame_buffer = (char*)malloc(MAX_INPUT_BUFFER);
                ycbcr[0].data = 0;
                /* Start the input stream*/
                wcLaunchInStream(client, input_device.c_str(), NULL);
                running = 5;
                break;
            }
            default:
            {
                break;
            }
            }

            if (running < 5)
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            else
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            /* Proceed client */
            wcClientProceed(client);
            wcClientTasksProceed(client);

            auto current_timestamp = std::chrono::system_clock::now();
            auto int_s = chrono::duration_cast<chrono::seconds>(current_timestamp - start_timestamp);

            if (int_s.count() >= TIME_OUT) {
                cout << "timeout" << endl;
                break;
            }
        }
        /* Disconnect client */
        wcClientDisconnect(client);
        /* Destroy client */
        wcClientDestroy(client);

        if (mem_frame_buffer)
            free(mem_frame_buffer);

        if (theora_initialized)
        {
            th_encode_free(td);
            free(ycbcr[0].data);
            free(ycbcr[1].data);
            free(ycbcr[2].data);

            if(ogg_stream_flush(&ogg_os, &og))
            {
                outfile.write((const char *)og.header, og.header_len);
                outfile.write((const char *)og.body, og.body_len);
            }

            outfile.flush();

            ogg_stream_clear(&ogg_os);
        }

        outfile.close();

        return 0;
    }
    else
    {
        cout << "should contain at least --host (host-name) option";
    }
}

