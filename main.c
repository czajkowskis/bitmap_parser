#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef int32_t LONG;

// https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapfileheader
typedef struct __attribute__ ((__packed__)) tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER, *PBITMAPFILEHEADER;

// https://docs.microsoft.com/pl-pl/previous-versions/dd183376(v=vs.85)
typedef struct __attribute__ ((__packed__)) tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;

//Parsing part

BITMAPFILEHEADER parse_file_header(FILE *bitmap_file){
    BITMAPFILEHEADER file_header;
    fread(&file_header, sizeof(unsigned char), 14, bitmap_file);
    return file_header;
}

BITMAPINFOHEADER parse_info_header(FILE *bitmap_file){
  BITMAPINFOHEADER info_header;
  fread(&info_header, sizeof(unsigned char), 40, bitmap_file);
  return info_header;
}

void print_file_header(BITMAPFILEHEADER file_header){
    printf("BITMAPFILEHEADER:\n");
    printf("  bfType:           0x%X\n", file_header.bfType);
    printf("  bfSize:           %d\n", file_header.bfSize);
    printf("  bfReserved1:      0x%X\n", file_header.bfReserved1);
    printf("  bfReserved2:      0x%X\n", file_header.bfReserved2);
    printf("  bfOffBits:        %d\n", file_header.bfOffBits);
}

void print_info_header(BITMAPINFOHEADER info_header){
    printf("BITMAPINFOHEADER:\n");
    printf("  biSize:           %d\n", info_header.biSize);
    printf("  biWidth:          %d\n", info_header.biWidth);
    printf("  biHeight:         %d\n", info_header.biHeight);
    printf("  biPlanes:         %d\n", info_header.biPlanes);
    printf("  biBitCount:       %d\n", info_header.biBitCount);
    printf("  biCompression:    %d\n", info_header.biCompression);
    printf("  biSizeImage:      %d\n", info_header.biSizeImage);
    printf("  biXPelsPerMeter:  %d\n", info_header.biXPelsPerMeter);
    printf("  biYPelsPerMeter:  %d\n", info_header.biYPelsPerMeter);
    printf("  biClrUsed:        %d\n", info_header.biClrUsed);
    printf("  biClrImportant:   %d\n", info_header.biClrImportant);
}

void print_parsed_headers(BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header){
    print_file_header(file_header);
    print_info_header(info_header);
}


//Histogram part
typedef struct color_histogram{
  int total_pixels;
  int count_0_15;
  int count_16_31;
  int count_32_47;
  int count_48_63;
  int count_64_79;
  int count_80_95;
  int count_96_111;
  int count_112_127;
  int count_128_143;
  int count_144_159;
  int count_160_175;
  int count_176_191;
  int count_192_207;
  int count_208_223;
  int count_224_239;
  int count_240_255;
} color_histogram_t;

void increment_interaval_count(uint8_t value, color_histogram_t *histogram){
  histogram->total_pixels++;
  if (value <= 15){
    histogram->count_0_15++;
  } else if(value <= 31){
    histogram->count_16_31++;
  } else if(value <= 47){
    histogram->count_32_47++;
  } else if(value <= 63){
    histogram->count_48_63++;
  } else if(value <= 79){
    histogram->count_64_79++;
  } else if(value <= 95){
    histogram->count_80_95++;
  } else if(value <= 111){
    histogram->count_96_111++;
  } else if(value <= 127){
    histogram->count_112_127++;
  } else if(value <= 143){
    histogram->count_128_143++;
  } else if(value <= 159){
    histogram->count_144_159++;
  } else if(value <= 175){
    histogram->count_160_175++;
  } else if(value <= 191){
    histogram->count_176_191++;
  } else if(value <= 207){
    histogram->count_192_207++;
  } else if(value <= 223){
    histogram->count_208_223++;
  } else if(value <= 239){
    histogram->count_224_239++;
  } else{
    histogram->count_240_255++;
  }
}

void process_row(int bytes_to_process, unsigned char row[], color_histogram_t *blue_histogram, color_histogram_t *green_histogram, color_histogram_t *red_histogram){
  for(int byte_number = 0; byte_number < bytes_to_process; byte_number++){
    switch(byte_number % 3){
      case 0 :
        increment_interaval_count(row[byte_number], blue_histogram);
        break;
      case 1 :
        increment_interaval_count(row[byte_number], green_histogram);
        break;
      case 2 :
        increment_interaval_count(row[byte_number], red_histogram);
        break;
      default :
        break;
    }
  }
}

void print_histogram(char *histogram_color, color_histogram_t histogram){
  printf("%s:\n", histogram_color);
  printf("  0-15:     %0.2f%%\n", ((float)histogram.count_0_15 / histogram.total_pixels * 100));
  printf("  16-31:    %0.2f%%\n", ((float)histogram.count_16_31 / histogram.total_pixels * 100));
  printf("  32-47:    %0.2f%%\n", ((float)histogram.count_32_47 / histogram.total_pixels * 100));
  printf("  48-63:    %0.2f%%\n", ((float)histogram.count_48_63 / histogram.total_pixels * 100));
  printf("  64-79:    %0.2f%%\n", ((float)histogram.count_64_79 / histogram.total_pixels * 100));
  printf("  80-95:    %0.2f%%\n", ((float)histogram.count_80_95 / histogram.total_pixels * 100));
  printf("  96-111:   %0.2f%%\n", ((float)histogram.count_96_111 / histogram.total_pixels * 100));
  printf("  112-127:  %0.2f%%\n", ((float)histogram.count_112_127 / histogram.total_pixels * 100));
  printf("  128-143:  %0.2f%%\n", ((float)histogram.count_128_143 / histogram.total_pixels * 100));
  printf("  144-159:  %0.2f%%\n", ((float)histogram.count_144_159 / histogram.total_pixels * 100));
  printf("  160-175:  %0.2f%%\n", ((float)histogram.count_160_175 / histogram.total_pixels * 100));
  printf("  176-191:  %0.2f%%\n", ((float)histogram.count_176_191 / histogram.total_pixels * 100));
  printf("  192-207:  %0.2f%%\n", ((float)histogram.count_192_207 / histogram.total_pixels * 100));
  printf("  208-223:  %0.2f%%\n", ((float)histogram.count_208_223 / histogram.total_pixels * 100));
  printf("  224-239:  %0.2f%%\n", ((float)histogram.count_224_239 / histogram.total_pixels * 100));
  printf("  240-255:  %0.2f%%\n", ((float)histogram.count_240_255 / histogram.total_pixels * 100)) ;
  return;
}

void parse_pixel_array(FILE *bitmap_file, BITMAPFILEHEADER file_header, BITMAPINFOHEADER info_header){
  fseek(bitmap_file, file_header.bfOffBits, SEEK_SET);
  if(info_header.biBitCount != 24 || info_header.biCompression != 0){
    printf("Histogram calculation is only supported for uncompressed 24-bit bitmaps\n");
    printf("Your file parameters:    Bitcount: %d   Compression: %d", info_header.biBitCount, info_header.biCompression);
  } else{
    color_histogram_t red_histogram = {};
    color_histogram_t green_histogram = {};
    color_histogram_t blue_histogram = {};
    int row_length = ((info_header.biBitCount * info_header.biWidth + 31)/32)*4;
    int bytes_to_process = 3 * info_header.biWidth;

    unsigned char row[row_length];
    while(fread(row, sizeof(unsigned char), row_length, bitmap_file)){
      if(feof(bitmap_file)){
        break;
      }
      process_row(bytes_to_process, row, &blue_histogram, &green_histogram, &red_histogram);
    }
    print_histogram("BLUE", blue_histogram);
    print_histogram("GREEN", green_histogram);
    print_histogram("RED", red_histogram);
  }
  
}

// Grayscale part

void rewrite_headers(BITMAPFILEHEADER *file_header, BITMAPINFOHEADER *info_header, FILE *output_file){
  fwrite(file_header, sizeof(unsigned char), 14, output_file);
  fwrite(info_header, sizeof(unsigned char), 40, output_file);
}

void process_row_grayscale(unsigned char row[], int row_length, int bytes_to_process, FILE *output_file){
  for(int i = 0; i < bytes_to_process; i += 3){
    unsigned char value_to_write = (row[i] + row[i+1] + row[i+2])/3;
    for(int j = 0; j < 3; j++){
      if(fputc(value_to_write, output_file) == EOF){
        printf("Something went wrong");
        break;
      };
    }
  }
  for(int i = 0; i < row_length-bytes_to_process; i++){
    fputc(0, output_file);
  }
}

void pixel_array_transform_grayscale(FILE *input_file, FILE *output_file, int row_length, int bytes_to_process){
  unsigned char *row = (unsigned char*)malloc(row_length*sizeof(unsigned char));
  while(fread(row, sizeof(unsigned char), row_length, input_file)){
      if(feof(input_file)){
        break;
      }
      process_row_grayscale(row, row_length, bytes_to_process, output_file);
  }
  free(row);
}


void create_grayscale_file(char *input_file_path, char *output_file_path){
  FILE *input_file = fopen(input_file_path,"rb");
  if(!input_file){
    printf("Error while opening file %s\n", input_file_path);
    perror("Error");
    return;
  }

  FILE *output_file = fopen(output_file_path, "wb");
  if(!output_file){
    printf("Error while opening file %s\n",output_file_path);
    perror("Error");
    return;
  }

  BITMAPFILEHEADER file_header = parse_file_header(input_file);
  BITMAPINFOHEADER info_header = parse_info_header(input_file);
  if(info_header.biBitCount != 24 || info_header.biCompression != 0){
    printf("Grayscale transformation is only supported for uncompressed 24-bit bitmaps\n");
    printf("Your file parameters:    Bitcount: %d   Compression: %d\n", info_header.biBitCount, info_header.biCompression);
    return;
  }
  int row_length = ((info_header.biBitCount * info_header.biWidth + 31)/32)*4;
  int bytes_to_process = 3 * info_header.biWidth;
  rewrite_headers(&file_header, &info_header, output_file);
  fseek(input_file, file_header.bfOffBits, SEEK_SET);
  fseek(output_file,file_header.bfOffBits, SEEK_SET);
  pixel_array_transform_grayscale(input_file, output_file, row_length, bytes_to_process);
  fclose(input_file);
  fclose(output_file);
  return;
}

// Steganography part

void rewrite_pixel_array(FILE *input_file, FILE *output_file, int row_length){
  unsigned char *row = (unsigned char*)malloc(row_length*sizeof(unsigned char));
  while(fread(row, sizeof(unsigned char), row_length, input_file)){
      if(feof(input_file)){
        break;
      }
      for(int i = 0; i < row_length; i++){
        fputc(row[i], output_file);
      }
  }
  free(row);
}

void encode_message(FILE *input_file, FILE *output_file, BITMAPFILEHEADER file_header, char *message){
  fseek(input_file, file_header.bfOffBits, SEEK_SET);
  fseek(output_file,file_header.bfOffBits, SEEK_SET);
  int message_length = strlen(message);
  if (message_length > 255){
    printf("The message is to long. The maximum length is 255 characters");
    return;
  }
  unsigned char pixel_byte;
  for(int i = 0; i < 8; i++){
    pixel_byte = fgetc(input_file);
    if(message_length % 2 == 0){
      if(pixel_byte % 2 == 1){
        pixel_byte -= 1;
      }
    } else{
      if(pixel_byte % 2 == 0){
        pixel_byte += 1;
      }
    }
    fputc(pixel_byte, output_file);
    message_length /= 2;
  }

  for(unsigned long long i = 0; i < strlen(message); i++){
    int current_char = (int)message[i];
    for (int j = 0; j < 8; j++){
      pixel_byte = fgetc(input_file);
      if (current_char % 2 == 0){
        if(pixel_byte % 2 == 1){
          pixel_byte -= 1;
        }
      } else{
        if(pixel_byte % 2 == 0){
          pixel_byte += 1;
        }
      }
        fputc(pixel_byte, output_file);
        current_char /= 2;
    }
  }
  printf("The message was successfully encoded");
}

void create_encoded_file(char *input_file_path, char *output_file_path, char *message){
  FILE *input_file = fopen(input_file_path,"rb");
  if(!input_file){
    printf("Error while opening file %s\n", input_file_path);
    perror("Error");
    return;
  }

  FILE *output_file = fopen(output_file_path, "wb");
  if(!output_file){
    printf("Error while opening file %s\n",output_file_path);
    perror("Error");
    return;
  }

  BITMAPFILEHEADER file_header = parse_file_header(input_file);
  BITMAPINFOHEADER info_header = parse_info_header(input_file);
  if(info_header.biBitCount != 24 || info_header.biCompression != 0){
    printf("Steganography is only supported for uncompressed 24-bit bitmaps\n");
    printf("Your file parameters:    Bitcount: %d   Compression: %d\n", info_header.biBitCount, info_header.biCompression);
    return;
  }
  int row_length = ((info_header.biBitCount * info_header.biWidth + 31)/32)*4;
  rewrite_headers(&file_header, &info_header, output_file);
  fseek(input_file, file_header.bfOffBits, SEEK_SET);
  fseek(output_file,file_header.bfOffBits, SEEK_SET);
  rewrite_pixel_array(input_file, output_file, row_length);
  encode_message(input_file, output_file, file_header, message);
  fclose(input_file);
  fclose(output_file);
}

void decode_message(FILE *input_file, BITMAPFILEHEADER file_header){
  unsigned char pixel_byte;
  int message_length = 0;
  unsigned char current_char;
  int current_order = 1;
  fseek(input_file, file_header.bfOffBits, SEEK_SET);
  for(int i = 0 ; i < 8; i++){
      pixel_byte = fgetc(input_file);
      if(pixel_byte % 2 == 1){
          message_length += current_order;
      }
      current_order *= 2;
  }
  printf("Decoded message: ");
  for(int i=0 ; i < message_length; i++){
    current_char = 0;
    current_order = 1;
    for(int j = 0; j < 8; j++){
        pixel_byte = fgetc(input_file);
        if(pixel_byte % 2 == 1){
            current_char += current_order;
        }
        current_order *= 2;
    }
    printf("%c",current_char);
  }
  printf("\n");
}



int main(int argc, char *argv[]){
    if(argc == 2){
      char *bitmap_path = argv[1];
      FILE *bitmap_file = fopen(bitmap_path,"rb");
      if(!bitmap_file){
        printf("Error while opening file %s\n", bitmap_path);
        perror("Error");
      }
      BITMAPFILEHEADER file_header = parse_file_header(bitmap_file);
      BITMAPINFOHEADER info_header = parse_info_header(bitmap_file);
      print_parsed_headers(file_header, info_header);
      parse_pixel_array(bitmap_file, file_header, info_header);
      char answer;
      printf("Decode stegonography? [y/n] ");
      scanf("%c", &answer);
      if(answer == 'y'){
        decode_message(bitmap_file, file_header);
      }
      fclose(bitmap_file);
    }
    else if(argc ==3){
      char *input_file_path = argv[1];
      char *output_file_path = argv[2];
      create_grayscale_file(input_file_path, output_file_path);
      printf("The grayscale bitmap was successfully created.");
    }else if(argc == 4){
      char *input_file_path = argv[1];
      char *output_file_path = argv[2];
      char *message = argv[3];
      create_encoded_file(input_file_path, output_file_path, message);
    }
    return 0;
}