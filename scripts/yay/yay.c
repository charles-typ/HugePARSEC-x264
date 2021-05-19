/*

  yay - fast and simple yuv viewer

  (c) 2005-2010 by Matthias Wientapper
  (m.wientapper@gmx.de)

  Support of multiple formats added by Cuero Bugot.

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "regex.h"
#include "SDL.h"

#define RANDOM_OUTPUT


Uint32 width = 0;
Uint32 height = 0;
char *vfilename;
FILE *fpointer;
Uint8 *y_data, *cr_data, *cb_data;
#ifdef RANDOM_OUTPUT
FILE *frandom;
Uint8 *r_y_data, *r_cr_data, *r_cb_data;
#endif
Uint16 zoom = 1;
Uint16 min_zoom = 1;
Uint16 frame = 0;
Uint16 quit = 0;
Uint8 grid = 0;
Uint8 bpp = 0;
int cfidc = 1;
int isY4M = 0;

static const Uint8 SubWidthC[4] =
  {
    0, 2, 2, 1
  };
static const Uint8 SubHeightC[4] =
  {
    0, 2, 1, 1
  };
static const Uint8 SubSizeC[4] =
  {
    0, 4, 2, 1
  };
static const Uint8 MbWidthC[4] =
  {
    0, 8, 8, 16
  };
static const Uint8 MbHeightC[4] =
  {
    0, 8, 16, 16
  };
static const Uint8 FrameSize2C[4] =
  {
    2, 3, 4, 6
  };


int load_frame(){
  Uint32 cnt;
  /* Fill in video data */
//  if(isY4M)
//	  fseek(fpointer, strlen("FRAME "), SEEK_CUR);
#ifdef RANDOM_OUTPUT
  char *FRAME = "FRAME\n";
  fwrite(FRAME, 1, strlen("FRAME "), frandom);
  printf("Writing this frame\n");
#endif
  //width = 7680;
  //height = 4320;
  //cnt = fread(y_data, 1, width*height, fpointer);
  cnt = width * height;

#ifdef RANDOM_OUTPUT
  int offset = 0;
  for(offset = 0; offset < cnt; offset++) {
      r_y_data[offset] = rand() % 256;
  }
  fwrite(r_y_data, 1, cnt, frandom);
#endif
  //fprintf(stderr,"read %d y bytes\n", cnt);
  if(cnt < width*height){
    return 0;
  }
  else if (cfidc>0)
    {
      //cnt = fread(cb_data, 1, height * width / SubSizeC[cfidc], fpointer);
      cnt = height*width / SubSizeC[cfidc];

#ifdef RANDOM_OUTPUT
      for(offset = 0; offset < cnt; offset++) {
          r_cb_data[offset] = rand() % 256;
      }
        fwrite(r_cb_data, 1, cnt, frandom);
#endif
      // fprintf(stderr,"read %d cb bytes\n", cnt);
      if(cnt < width * height / 4){
	return 0;
      } else {
	//cnt = fread(cr_data, 1, height * width / SubSizeC[cfidc], fpointer);
      cnt = height*width / SubSizeC[cfidc];
	// fprintf(stderr,"read %d cr bytes\n", cnt);
    //
#ifdef RANDOM_OUTPUT
      for(offset = 0; offset < cnt; offset++) {
          r_cr_data[offset] = rand() % 256;
      }
  fwrite(r_cr_data, 1, cnt, frandom);
#endif
	if(cnt < width * height / 4){
	  return 0;
	}
      }
    }
  return 1;
}

void print_usage(){
  fprintf(stdout, "Usage: yay [-s <widht>x<heigh>] [-f format] filename.yuv\n\t format can be: 0-Y only, 1-YUV420, 2-YUV422, 3-YUV444\n");
}


int main(int argc, char *argv[])
{
  /*
   * x264
   */
  time_t t;
  srand((unsigned) time(&t));

  int num_frame = 512;
  int     opt;
  char    caption[32];
  regex_t reg;
  regmatch_t pm;
  int     result;
  char    picsize[32]="";
  int     used_s_opt = 0;
  int     play_yuv = 0;
  unsigned int start_ticks = 0;
  Uint32 vflags;
  int depth = 8;

  if (argc == 1) {
    print_usage();
    return 1;
  } else {
    while((opt = getopt(argc, argv, "f:s:n:")) != -1)
      switch(opt){
      case 's':
        if (sscanf(optarg, "%dx%d", &width, &height) != 2) {
          fprintf(stdout, "No geometry information provided by -s parameter.\n");
          return 1;
	}
	used_s_opt = 1;
	break;
      case 'f':
	if (sscanf(optarg, "%d", &cfidc) != 1 || (cfidc<0 && cfidc>3)) {
	  fprintf(stdout, "Invalid format provided by -f parameter.\n");
	  return 1;
	}
	break;
    case 'n':
    if(sscanf(optarg, "%d", &num_frame) != 1) {
	  fprintf(stdout, "Invalid format provided by -n parameter.\n");
      return 1;
    }
    break;
      default:
	print_usage();
	return 1;
	break;
      }
  }
  argv += optind;
  argc -= optind;

  printf("Here\n");
  vfilename = argv[0];
  fpointer = fopen(vfilename, "rb");
  if (fpointer == NULL){
    fprintf(stderr, "Error opening %s\n", vfilename);
    return 1;
  }
#ifdef RANDOM_OUTPUT
  char *random_file_name = "random_image";
  frandom = fopen(random_file_name, "wb");
  if (frandom == NULL){
    fprintf(stderr, "Error opening %s\n", vfilename);
    return 1;
  }
  char copy_c = fgetc(fpointer);
  while(copy_c != '\n') {
    fputc(copy_c, frandom);
    copy_c = fgetc(fpointer);
  }
  fputc('\n', frandom);
  fseek(fpointer, 0, SEEK_SET);
#endif


  if(!used_s_opt) {
    // try to find picture size from filename or path
    if (regcomp(&reg, "_[0-9]+x[0-9]+", REG_EXTENDED) != 0) return -1;
    result = regexec(&reg, vfilename, 1, &pm, REG_NOTBOL);
    if(result == 0){
      strncpy(picsize, (vfilename + pm.rm_so + 1), (pm.rm_eo - pm.rm_so -1 ));
      strcat(picsize, "\0");
    }
    if (sscanf(picsize, "%dx%d", &width, &height) != 2) {
		/* Maybe it's Y4M ? */
		const char * y4m_magic = "YUV4MPEG2";
		char input[9];

		if(fread(input, 1, 9, fpointer) != 9 || memcmp(y4m_magic, input, 9)){
			fprintf(stdout, "No geometry information found in path/filename.\nPlease use -s <width>x<height> paramter.\n");
			return 1;
		} else {
			/* We got a Y4M ! Hurray ! */
			fseek(fpointer, 0, SEEK_SET);
			while (!feof(fpointer)){
				// Skip Y4MPEG string
				int c = fgetc(fpointer);
				int d, csp = 0;
				while (!feof(fpointer) && (c != ' ') && (c != '\n')){
					c = fgetc(fpointer);
				}

				while (c == ' ' && !feof(fpointer)){
					// read parameter identifier
					switch (fgetc(fpointer)){
					case 'W':
						width = 0;
						while (!feof(fpointer)){
							c = fgetc(fpointer);

							if (c == ' ' || c == '\n'){
								break;
							} else {
								width = width * 10 + (c - '0');
							}
						}
						break;
					case 'H':
						height = 0;
						while (!feof(fpointer)){
							c = fgetc(fpointer);
							if (c == ' ' || c == '\n'){
								break;
							} else {
								height = height * 10 + (c - '0');
							}
						}
						break;

					case 'F':
						/* rateNum = 0; */
						/* rateDenom = 0; */
						while (!feof(fpointer)){
							c = fgetc(fpointer);
							if (c == '.'){
								/* rateDenom = 1; */
								while (!feof(fpointer)){
									c = fgetc(fpointer);
									if (c == ' ' || c == '\n'){
										break;
									} else {
										/* rateNum = rateNum * 10 + (c - '0'); */
										/* rateDenom = rateDenom * 10; */
									}
								}

								break;
							} else if (c == ':') {
								while (!feof(fpointer)) {
									c = fgetc(fpointer);
									if (c == ' ' || c == '\n') {
										break;
									} else {
										/* rateDenom = rateDenom * 10 + (c - '0'); */
									}
								}
								break;
							} else {
								/* rateNum = rateNum * 10 + (c - '0'); */
							}
						}
						break;

					case 'A':
						/* sarWidth = 0; */
						/* sarHeight = 0; */
						while (!feof(fpointer)) {
							c = fgetc(fpointer);
							if (c == ':'){
								while (!feof(fpointer)){
									c = fgetc(fpointer);
									if (c == ' ' || c == '\n'){
										break;
									} else {
										/* sarHeight = sarHeight * 10 + (c - '0'); */
									}
								}
								break;
							} else {
								/* sarWidth = sarWidth * 10 + (c - '0'); */
							}
						}
						break;

					case 'C':
						csp = 0;
						d = 0;
						while (!feof(fpointer)){
							c = fgetc(fpointer);

							if (c <= '9' && c >= '0'){
								csp = csp * 10 + (c - '0');
							} else if (c == 'p') {
								// example: C420p16
								while (!feof(fpointer)) {
									c = fgetc(fpointer);

									if (c <= '9' && c >= '0')
										d = d * 10 + (c - '0');
									else
										break;
								}
								break;
							} else
								break;
						}
						if (d >= 8 && d <= 16)
							depth = d;
						cfidc = (csp == 444) ? 3 : (csp == 422) ? 2 : 1;
						break;

					default:
						while (!feof(fpointer)) {
							// consume this unsupported configuration word
							c = fgetc(fpointer);
							if (c == ' ' || c == '\n')
								break;
						}

						break;
					}
				}

				if (c == '\n'){
					break;
				}
			}
			isY4M = ftell(fpointer);
		}
    }
  }
  // some WM can't handle small windows...
  if (width < 100){
    zoom = 2;
    min_zoom = 2;
  }
  //printf("using x=%d y=%d\n", width, height);


  /* should allocate memory for y_data, cr_data, cb_data here */
  //width = 7680;
  //height = 4320;
  y_data  = malloc(width * height * sizeof(Uint8));
#ifdef RANDOM_OUTPUT
  r_y_data  = malloc(width * height * sizeof(Uint8));
#endif
  if (cfidc > 0)
    {
      cb_data = malloc(width * height * sizeof(Uint8) / SubSizeC[cfidc]);
      cr_data = malloc(width * height * sizeof(Uint8) / SubSizeC[cfidc]);
#ifdef RANDOM_OUTPUT
      r_cb_data = malloc(width * height * sizeof(Uint8) / SubSizeC[cfidc]);
      r_cr_data = malloc(width * height * sizeof(Uint8) / SubSizeC[cfidc]);
#endif
    }

  /**
	  case SDLK_RIGHT:
	    {
	      // check for next frame existing
	      if(load_frame()){
		frame++;
	      }
          **/
  int frame_count = 0;
  for (frame_count = 0; frame_count < num_frame; frame_count++) {
      load_frame();
  }
  // clean up
  free(y_data);
  free(cb_data);
  free(cr_data);
  fclose(fpointer);
#ifdef RANDOM_OUTPUT
  free(r_y_data);
  free(r_cb_data);
  free(r_cr_data);
  fclose(frandom);
#endif
  if (!used_s_opt)
    regfree(&reg);

  return 0;
}



