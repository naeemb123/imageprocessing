#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER
{
    unsigned short bfType;  //specifies the file type
    unsigned int bfSize;  //specifies the size in bytes of the bitmap file
    unsigned short bfReserved1;  //reserved; must be 0
    unsigned short bfReserved2;  //reserved; must be 0
    unsigned int bOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct tagBITMAPINFOHEADER
{
    unsigned int biSize;  //specifies the number of bytes required by the struct
    int biWidth;  //specifies width in pixels
    int biHeight;  //species height in pixels
    unsigned short biPlanes; //specifies the number of color planes, must be 1
    unsigned short biBitCount; //specifies the number of bit per pixel
    unsigned int biCompression;//spcifies the type of compression
    unsigned int biSizeImage;  //size of image in bytes
    int biXPelsPerMeter;  //number of pixels per meter in x axis
    int biYPelsPerMeter;  //number of pixels per meter in y axis
    unsigned int biClrUsed;  //number of colors used by th ebitmap
    unsigned int biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;
#pragma pack(pop)

typedef struct tagPIXELS
{
  float *red;
  float *green;
  float *blue;
}PIXELCOLORS;



/*
>>FUCTION<<
Read BMP file and return a struct which contains all three colour planes of each
pixel in the image
*/
PIXELCOLORS *readBmp(char *filename, BITMAPINFOHEADER *bitmapInfoHeader, BITMAPFILEHEADER *bitmapFileHeader){


  /*==============================================================================
               OPEN BMP FILE AND READ IN THE DATA INTO THE STRUCTS
  ================================================================================*/
  FILE *filePtr; //our file pointer
  unsigned char *bitmapImage;  //store image data
  int imageIdx=0;  //image index counter
  unsigned char tempRGB;  //our swap variable
  PIXELCOLORS *colorPlanes = (PIXELCOLORS *)malloc(sizeof(PIXELCOLORS));

  //open filename in read binary mode
  filePtr = fopen(filename,"rb");
  if (filePtr == NULL)
      return NULL;
  //read the bitmap file header
  fread(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);


  //verify that this is a bmp file by check bitmap id
  if (bitmapFileHeader->bfType !=0x4D42){
      fclose(filePtr);
      return NULL;
  }

  //read the bitmap info header
  fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr);

  //move file point to the begging of bitmap data
  fseek(filePtr, bitmapFileHeader->bOffBits, SEEK_SET);
  /*==============================================================================*/



  /*==============================================================================
                            READ BITMAP IMAGE DATA
  ================================================================================*/
  //allocate memory for the bitmap image data
  bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

  //verify memory allocation
  if (!bitmapImage)
  {
      free(bitmapImage);
      fclose(filePtr);
      return NULL;
  }

  //read in the bitmap image data
  fread(bitmapImage,bitmapInfoHeader->biSizeImage,1,filePtr);

  //make sure bitmap image data was read
  if (bitmapImage == NULL)
  {
      fclose(filePtr);
      return NULL;
  }
  /*==============================================================================*/




  /*==============================================================================
          CONVERT EACH PIXELS COLOUR TO FLOAT AND STORE IN THE COLOUR PLANE
  ================================================================================*/
  /*==============================
          VARIABLES NEEDED
  ================================*/
  int counter=0;
  float two, twofivefive,pixel,one,f;

  //allocate memory to red colour plane
  colorPlanes->red = (float *)malloc((sizeof(float) * (bitmapInfoHeader->biSizeImage)/3)+1);
  if (!colorPlanes->red){
    free(colorPlanes->red);
    fclose(filePtr);
  }
  //allocate memory to blue colour plane
  colorPlanes->blue = (float *)malloc((sizeof(float) * (bitmapInfoHeader->biSizeImage)/3)+1);
  if (!colorPlanes->blue){
    free(colorPlanes->blue);
    fclose(filePtr);
  }
  //allocate memory to green colour plane
  colorPlanes->green = (float *)malloc((sizeof(float) * (bitmapInfoHeader->biSizeImage)/3)+1);
  if (!colorPlanes->green){
    free(colorPlanes->green);
    fclose(filePtr);
  }
  /*===============================*/


  //convert and store each pixel colour
  for (imageIdx=0; imageIdx<bitmapInfoHeader->biSizeImage; imageIdx+=3){

    two = 2;
    twofivefive = 255;
    pixel = bitmapImage[imageIdx];
    one = 1;

    //convert pixel colour to float
    f = (two/twofivefive)*pixel - one;

    //store blue float colour
    colorPlanes->blue[counter] = f;
    pixel = bitmapImage[imageIdx+1];

    //store green float colour
    f = (two/twofivefive)*pixel - one;
    colorPlanes->green[counter] = f;
    pixel = bitmapImage[imageIdx+2];

    //store red float colour
    f = (two/twofivefive)*pixel - one;
    colorPlanes->red[counter] = f;
    counter++;
  }
  /*==============================================================================*/

    fclose(filePtr);
    return colorPlanes;
}



/*
>>FUCTION<<
convert the manipulated pixels of the image back into integers and store the
new pixels into a list containing the image data.
Write this new image data back out to file
*/
void writeBmp(PIXELCOLORS *colorPlanes, char *filename, BITMAPINFOHEADER *bitmapInfoHeader, BITMAPFILEHEADER *bitmapFileHeader){

/*==============================================================================
CONVERT EACH FLOAT FROM EACH COLOUR PLANE BACK TO INTEGER AND STORE AS NEW IMAGE DATA
================================================================================*/


/*==============================
        VARIABLES NEEDED
================================*/
//Allocate memory to list which will contain the image data
unsigned char *imageData = (unsigned char *)malloc(bitmapInfoHeader->biSizeImage);
float two,twofivefive,one,fblue,fgreen,fred;
unsigned char pixelColblue, pixelColgreen, pixelColred;
int imageIdxCounter=0,imageIdx=0;
/*===============================*/


  for (imageIdx=0; imageIdx<bitmapInfoHeader->biSizeImage; imageIdx+=3){
    two = 2;
    twofivefive = 255;
    one = 1;

    //blue float
    fblue = colorPlanes->blue[imageIdxCounter];
    //green float
    fgreen = colorPlanes->green[imageIdxCounter];
    //red float
    fred = colorPlanes->red[imageIdxCounter];

    //blue integer
    pixelColblue = ((fblue+one))/(two/twofivefive);
    //green integer
    pixelColgreen = ((fgreen+one))/(two/twofivefive);
    //ref integer
    pixelColred = ((fred+one))/(two/twofivefive);

    //store blue into list
    imageData[imageIdx] = pixelColblue;
    //store green into list
    imageData[imageIdx+1] = pixelColgreen;
    //store red into list
    imageData[imageIdx+2] = pixelColred;

    imageIdxCounter++;
  }
/*==============================================================================*/



/*==============================================================================
                      WRITE NEW IMAGE DATA BACK OUT TO FILE
================================================================================*/


/*==============================
        VARIABLES NEEDED
================================*/
  FILE *filePtr;
/*===============================*/


  //open filename in write mode
  filePtr = fopen(filename,"w");
  if (filePtr == NULL)
      return;

  //write data to output file

  //Write File Header
  fwrite(bitmapFileHeader,sizeof(char),sizeof(BITMAPFILEHEADER),filePtr);
  //write Header Info
  fwrite(bitmapInfoHeader,sizeof(char),sizeof(BITMAPINFOHEADER),filePtr);

  //move file pointer to bitmap data
  fseek(filePtr,sizeof(char)*bitmapFileHeader->bOffBits,SEEK_SET);

  //write imageData
  fwrite(imageData,sizeof(char),bitmapInfoHeader->biSizeImage,filePtr);

  fclose(filePtr);
  free(imageData);
  free(colorPlanes->blue);
  free(colorPlanes->green);
  free(colorPlanes->red);
  free(colorPlanes);
  /*==============================================================================*/
}



/*
>>FUCTION<<
for each pixel in the image, apply the kernel
*/
PIXELCOLORS *convolue(PIXELCOLORS *colorPlanes, BITMAPINFOHEADER *bitmapInfoHeader, int kernelW, int kernelH, float *kernel, BITMAPFILEHEADER *bitmapFileHeader){
  /*==============================================================================
                        APPLY THE FILTER TO THE IMAGE
  ================================================================================*/
  /*==============================
          VARIABLES NEEDED
  ================================*/
  //keep track of the x and y coordinates of the image
  int x,y,xx,yy;
  //how many pixels we process
  int kernelSize;
  float avgB, avgG, avgR;

  /*===============================*/

  //setting the distance of each pixel from the centre pixel to check from
  if (kernelW == 3 && kernelH == 3) kernelSize = 1;
  else if (kernelW == 5 && kernelH == 5) kernelSize = 2;
  else if (kernelW == 7 && kernelH == 7) kernelSize = 3; //extreme cases
  else if (kernelW == 9 && kernelH == 9) kernelSize = 4; //extreme cases
  else{
    return NULL;
  }

  //Set the starting pixel to apply the kernel onto
  int startingPixel = (kernelSize*bitmapInfoHeader->biWidth+kernelSize)*3;

  //Set the ending pixel to apply the kernel onto
  int endingPixel = ((bitmapInfoHeader->biWidth) * (bitmapInfoHeader->biHeight))*3 - ((kernelSize*(bitmapInfoHeader->biWidth)) + kernelSize)*3;

//traverse through each x-axis in the image
for (xx = kernelSize; xx<(bitmapInfoHeader->biWidth)-kernelSize; xx++){

  //traverse through each y-axis in the image
  for (yy=kernelSize; yy<(bitmapInfoHeader->biHeight)-kernelSize; yy++){
    avgB = avgG = avgR = 0;
    int column=0;

    /*
    at current pixel currentP = (xx,yy) of the image
    get coordinates for currentP's left pixel (according to the kernel size)
    iterate until we reach currentP's right pixel (according to the kernel size)
    */
    for (x=xx - kernelSize; x<bitmapInfoHeader->biWidth && x <= xx+kernelSize; x++){


      /*
      iterate from top left of the kernel window for the current pixel 'currentP'
      */
      for (y=yy+kernelSize; y<bitmapInfoHeader->biHeight && y>= yy-kernelSize; y--){

          //NOTE: Need to find a way of changing the  x below to iterate from left to right
          // of the kernel and not top to bottom
          //ideas: switch the inner for loops
          avgB += kernel[column] * colorPlanes->blue[x + y*bitmapInfoHeader->biWidth];
          avgG += kernel[column] * colorPlanes->green[x + y*bitmapInfoHeader->biWidth];
          avgR += kernel[column] * colorPlanes->red[x + y*bitmapInfoHeader->biWidth];
          column++;
      }

    }
          //double check the ranges after applying kernel
          if (avgB < -1.000) avgB = -1.000;
          else if (avgB > 1.000) avgB = 1.000;

          if (avgG < -1.000) avgG = -1.000;
          else if (avgG > 1.000) avgG = 1.000;

          if (avgR < -1.0000) avgR = -1.0000;
          else if (avgR > 1.0000) avgR = 1.0000;

    //set the new value, after applying the kernel to the pixel for each colour
    colorPlanes->blue[xx + yy*bitmapInfoHeader->biWidth] = avgB;
    colorPlanes->green[xx + yy*bitmapInfoHeader->biWidth] = avgG;
    colorPlanes->red[xx + yy*bitmapInfoHeader->biWidth] = avgR;
  }
}
/*==============================================================================*/

  return colorPlanes;
}

int main(){
  BITMAPFILEHEADER bitmapFileHeader;
  BITMAPINFOHEADER bitmapInfoHeader;
  PIXELCOLORS *colorPlanes;
  colorPlanes = readBmp("xiahouDun.bmp",&bitmapInfoHeader,&bitmapFileHeader);
  if (colorPlanes == NULL) {
    printf("File not read in correctly\n");
    exit(-1);
  }
  float kernel[9] = {
                     0.0, -.25, 0.0,
                     -.25, 2, -.25,
                     0.0, -.25, 0.0
                   };

clock_t start = clock(), diff;
colorPlanes = convolue(colorPlanes,&bitmapInfoHeader,3,3, kernel, &bitmapFileHeader);
diff = clock() - start;
if (colorPlanes == NULL){
  printf("Kernel size is either incorrect or too large\n");
  exit(-1);
}

int msec = diff * 1000 / CLOCKS_PER_SEC;
printf("Convolution Algorithm Performance: %d seconds %d milliseconds\ns", msec/1000, msec%1000);
writeBmp(colorPlanes,"sharpeningConvolution2.bmp", &bitmapInfoHeader, &bitmapFileHeader);
  return 0;
}
