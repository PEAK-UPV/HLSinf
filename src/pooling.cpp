/**
 * Pooling layer 
 * 		
 * 		MaxPooling
 * 		AvgPooling
 *  
 * @file pooling.cpp
 * @author Raul Ferrando 
 */

#include "conv2D.h"

struct frame_pool_t {
	pixel_out_t pixel[KW_POOLING * KH_POOLING];
};


/*
* Convert module for the pooling layer
* Only works for fixed size of (KW=2, KH=2) and (SW=2, SH=2).
* 
* The output data format is (KH, KW, CPI)
* 
* Parameters:
* 	H   	       : Height of the input image
* 	W		       : Width of the input image
* 	enable_pooling : Activates pooling
* 	in		       : Input stream (pixel_out_t)
*	out 	       : Output stream (frame_pool_t)
*
*	In case enable_pooling is not set, the module bypasses the input
*	to the output, sending the pixel on the first position of the output frame.
*/
static void pool_cvt(int H, int W, int enable_pooling, hls::stream<pixel_out_t> &in, hls::stream<frame_pool_t> &out) {

	frame_pool_t kernel;
	pixel_out_t  buffer0[WMAX];
	pixel_out_t  buffer1[WMAX];
	int          row0;
	int          shift_frame;
	int          send_frame;
	int          odd_col = 0;       // whether we are in an odd col (so to be able to send a frame)
	int          pin_row = 0;
	int          pin_col = 0;
	int          row_write;			// either 0 or 1 (we assume 2x2 kernel)
	int          size_channel = H * W;
	int          iterations = size_channel;
	pixel_out_t  pixel;
	pixel_out_t  p0, p1, p2, p3;
	pixel_out_t  pix_b0, pix_b1;
    //DO_PRAGMA(HLS AGGREGATE variable=pixel)

    #ifdef DEBUG_POOL
	printf("DEBUG_POOL: starts (cvt)\n");
    #endif


	cvt_pooling_iterations:
	for (int i=0; i < iterations; i++) {
		DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=(I_REFERENCE/CPI) * W_REFERENCE * H_REFERENCE)
		#pragma HLS PIPELINE

		// Let's read CPI pixels
		pixel = in.read();

        #ifdef DEBUG_POOL
		printf("DEBUG_POOL: pixel read: ");
		for (int x=0; x<CPO; x++) printf(" %f ", float(pixel.pixel[x]));
		printf("\n");
        #endif

		if (enable_pooling) {

		  row_write = pin_row % 2;

  		  // Let's compute shift and send flag variables
		  shift_frame = (pin_row > 0) & (pin_col > 1);
		  send_frame = row_write & odd_col;
		  row0 = (row_write == 0);

		  // Let's write on the buffer and at the same time
		  // we set the two pixels pix_b0 and pix_b1
	      if (row_write==0) {
	    	  buffer0[pin_col] = pixel;
	    	  pix_b0 = pixel;
	    	  pix_b1 = buffer1[pin_col];
	      } else {
	    	  buffer1[pin_col] = pixel;
	    	  pix_b0 = buffer0[pin_col];
	    	  pix_b1 = pixel;
	      }

		  /* p0 p1 */
		  if (shift_frame) {p0 = p1;} else if (pin_col == 0) p0 = pix_b0;
		  p1 = pix_b0;

		  /* p2 p3 */
		  if (shift_frame) {p2 = p3;} else if (!pin_col) p2 = pix_b1;
		  p3 = pix_b1;

  		  /* Control the iteration count */
		  pin_col++;
		  odd_col = (odd_col + 1) % 2;
		  if (pin_col == W) {
			pin_col = 0;
			pin_row++;
			if (pin_row == H) pin_row = 0;
		  }

		  if (send_frame) {
     	    kernel.pixel[0] = p0; kernel.pixel[1] = p1;
			kernel.pixel[2] = p2; kernel.pixel[3] = p3;
			out << kernel;
            #ifdef DEBUG_POOL
			printf("DEBUG_POOL: Send Frame:\n");
			for (int x=0; x<CPO; x++) printf(" cpo %d: %f %f %f %f\n", x, float(p0.pixel[x]), float(p1.pixel[x]), float(p2.pixel[x]), float(p3.pixel[x]));
            #endif
		  }
		} else {
          kernel.pixel[0] = pixel;
          out << kernel;
		}
	}

    #ifdef DEBUG_POOL
    printf("DEBUG_POOL: ends (cvt)\n");
    #endif

}

/*
* Pooling operation of the layer
*
* Parameters:
* 	HO		          : Height of the output image
*	WO		          : Width of the output image
*	enable_maxpooling : Activates maxpooling operation
*	enable_avgpooling : Activates avgpooling operation
*   in		          : Input stream (frame_pool_t) from the cvt module
*   out		          : Output stream (pixel_out_t)
*
*   If no enable is active then the module bypasses the first pixel of the incomming frame to the output stream
*/
static void pool_pooling(int HO, int WO, int enable_maxpooling, int enable_avgpooling, hls::stream<frame_pool_t> &in, hls::stream<pixel_out_t> &out) {
	
	frame_pool_t kernel;
	pixel_out_t out_pix;
	
	int size_out = HO * WO;
	int size_kernel = KH_POOLING * KW_POOLING;
	int iterations = size_out;
	int enable_pooling = enable_maxpooling | enable_avgpooling;

    #ifdef DEBUG_POOL
	printf("DEBUG_POOL: Starts (pooling)\n");
    #endif

	pooling_loop_iter:
	for (int i=0; i < iterations; i++) {
		DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=((I_REFERENCE / CPI) * W_REFERENCE * H_REFERENCE) / 4)
		#pragma HLS PIPELINE II=1

		// Let's read the input frame
		kernel = in.read();

        #ifdef DEBUG_POOL
		printf("DEBUG_POOL: read ");
		for (int x=0; x<CPO; x++) {
			printf("cpo %d: ", x);
			for (int xx=0; xx<size_kernel; xx++) printf(" %f", float(kernel.pixel[xx].pixel[x]));
			printf("\n");
		}
        #endif

        pooling_loop_cpo:
		for (int cpo=0; cpo < CPO; cpo++) {
          #pragma HLS UNROLL

		  data_type maxpool_value = MIN_DATA_TYPE_VALUE;
		  data_type avgpool_value = 0;

		  pooling_loop_kernel:
		  for (int k=0; k < size_kernel; k++) {
			DO_PRAGMA(HLS LOOP_TRIPCOUNT min=1 max=W_REFERENCE * H_REFERENCE)

			data_type value = kernel.pixel[k].pixel[cpo];

			if (value > maxpool_value) maxpool_value = value;
			avgpool_value += value;
		  }
		  avgpool_value /= size_kernel;

		  out_pix.pixel[cpo] = enable_maxpooling ? maxpool_value : enable_avgpooling ? avgpool_value : kernel.pixel[0].pixel[cpo];
		}

		out << out_pix;
        #ifdef DEBUG_POOL
		printf("DEBUG_POOL: send pixel: ");
		for (int x=0; x<CPO; x++) printf(" %f", float(out_pix.pixel[x]));
        #endif
	}

    #ifdef DEBUG_POOL
    printf("DEBUG_POOL: Ends (pooling)\n");
    #endif
}

void pooling(int H, int W, int enable_maxpooling, int enable_avgpooling, hls::stream<pixel_out_t> &input, hls::stream<pixel_out_t> &output) {

	static hls::stream<frame_pool_t> stream_pool;
    DO_PRAGMA(HLS STREAM variable=stream_pool depth=32)

	int enable_pooling = enable_maxpooling | enable_avgpooling;

	int WO = enable_pooling ? ((W - KW_POOLING)/SW_POOLING + 1) : W;
	int HO = enable_pooling ? ((H - KH_POOLING)/SH_POOLING + 1) : H;

	#pragma HLS DATAFLOW
	pool_cvt(H, W, enable_pooling, input, stream_pool);
	pool_pooling(HO, WO, enable_maxpooling, enable_avgpooling, stream_pool, output);
}
