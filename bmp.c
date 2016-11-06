#include <stdio.h>
#include <stdlib.h>

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

unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader, BITMAPFILEHEADER *bitmapFileHeader)
{
    FILE *filePtr; //our file pointer
    // BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"rb");
    if (filePtr == NULL)
        return NULL;
    //read the bitmap file header
    fread(bitmapFileHeader, sizeof(BITMAPFILEHEADER),1,filePtr);



    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader->bfType !=0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER),1,filePtr); // small edit. forgot to add the closing bracket at sizeof

    //move file point to the begging of bitmap data
    fseek(filePtr, bitmapFileHeader->bOffBits, SEEK_SET);


    //Testing to see if the data has been read correctly
    printf("size of image in bytes:\t%d\n", bitmapInfoHeader->biSizeImage);
    printf("width of image:\t%d\n", bitmapInfoHeader->biWidth);
    printf("height of image:\t%d\n", bitmapInfoHeader->biHeight);
    printf("Number of colour planes:\t%d\n", bitmapInfoHeader->biPlanes);
    printf("Number of bits per pixel:\t%d\n", bitmapInfoHeader->biBitCount);

      //allocate enough memory for the bitmap image data
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




    //swap the r and b values to get RGB (bitmap is BGR)
    // for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage;imageIdx+=3)
    // {
    //
    //     tempRGB = bitmapImage[imageIdx];
    //     bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
    //     bitmapImage[imageIdx + 2] = tempRGB;
    //     // printf("%d %d %d\n",bitmapImage[imageIdx],bitmapImage[imageIdx+1],bitmapImage[imageIdx+2]);
    // }



    //width of image in pixels(24bits) is = bitmapInfoHeader->biWidth*3
    // for (imageIdx=0; imageIdx < (bitmapInfoHeader->biWidth*3)*(bitmapInfoHeader->biHeight);imageIdx+=3){
    //     bitmapImage[imageIdx] = 255;
    //     bitmapImage[imageIdx+1] = 255;
    //     bitmapImage[imageIdx+2] = 255;
    // }
    //
    // // for (int i=0; i<(bitmapInfoHeader->biWidth*3)*(bitmapInfoHeader->biHeight*3); i+=3){
    //   bitmapImage[i] = 255;
    //   bitmapImage[i+1] = 255;
    //   bitmapImage[i+2] = 255;
    // }

    // for (int i =0; i< bitmapInfoHeader->biSizeImage; i++){
    //   printf("%d ",bitmapImage[i]);
    // }
    // printf("\n");

    //close file and return bitmap iamge data

    fclose(filePtr);
    return bitmapImage;
}

void WriteBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader, BITMAPFILEHEADER *bitmapFileHeader, unsigned char *bitmapData){

  FILE *filePtr;

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
  fwrite(bitmapData,sizeof(char),bitmapInfoHeader->biSizeImage,filePtr);
  fclose(filePtr);
}

unsigned char *applySharpeningFilter(BITMAPINFOHEADER *bitmapInfoHeader, unsigned char *bitmapImage){
  int x,xx,y,yy,ile,avgR,avgB,avgG,B,G,R,centerR,centerB,centerG;
  int blurSize = 3;

  for(xx = 0; xx < bitmapInfoHeader->biWidth; xx++)
{
  for(yy = 0; yy < bitmapInfoHeader->biHeight; yy++)
  {
      avgB = avgG = avgR = 0;
      ile = 0;

      for(x = xx; x < bitmapInfoHeader->biWidth && x < xx + blurSize; x++)
      {


          for(y = yy; y < bitmapInfoHeader->biHeight && y < yy + blurSize; y++)
          {
              if (x == xx && y == yy){
                centerB = 9*(bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 0]);
                centerG = 9*(bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 1]);
                centerR = 9*(bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 2]);
              }
              else{
                avgB +=  -1*(bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 0]);
                avgG +=  -1*(bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 1]);
                avgR +=  -1*(bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 2]);
              }

              if (x*3 - y*bitmapInfoHeader->biWidth*3 >= 0)
              {
                avgB += -1*(bitmapImage[x*3 - y*bitmapInfoHeader->biWidth*3 + 0]);
                avgG += -1*(bitmapImage[x*3 - y*bitmapInfoHeader->biWidth*3 + 1]);
                avgR += -1*(bitmapImage[x*3 - y*bitmapInfoHeader->biWidth*3 + 2]);
              }
              ile++;
          }
      }

      /*for blur*/
      // avgB = avgB / ile;
      // avgG = avgG / ile;
      // avgR = avgR / ile;

      avgB = avgB + centerB;
      avgG = avgG + centerG;
      avgR = avgR + centerR;

      if (avgB < 0) avgB = 0;
      else if (avgB > 255) avgB = 255;

      if (avgG < 0) avgG = 0;
      else if (avgG > 255) avgG = 255;

      if (avgR < 0) avgR = 0;
      else if (avgR > 255) avgR = 255;



      bitmapImage[xx*3 + yy*bitmapInfoHeader->biWidth*3 + 0] = avgB;
      bitmapImage[xx*3 + yy*bitmapInfoHeader->biWidth*3 + 1] = avgG;
      bitmapImage[xx*3 + yy*bitmapInfoHeader->biWidth*3 + 2] = avgR;
  }
}

return bitmapImage;
}

unsigned char *applyBlurringFilter(BITMAPINFOHEADER *bitmapInfoHeader, unsigned char *bitmapImage){

  /*Blurring image (getting the average pixel, by surrounding pixels
    How many surrounding pixels above depends on the blurSize;
  */
    int x,xx,y,yy,ile,avgR,avgB,avgG,B,G,R,centerR,centerB,centerG;
    int blurSize = 10;

    for(xx = 0; xx < bitmapInfoHeader->biWidth; xx++)
{
    for(yy = 0; yy < bitmapInfoHeader->biHeight; yy++)
    {
        avgB = avgG = avgR = 0;
        ile = 0;

        for(x = xx; x < bitmapInfoHeader->biWidth && x < xx + blurSize; x++)
        {


            for(y = yy; y < bitmapInfoHeader->biHeight && y < yy + blurSize; y++)
            {
                if (x == xx && y == yy){
                  centerB = (bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 0]);
                  centerG = (bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 1]);
                  centerR = (bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 2]);
                }
                else{
                  avgB +=   (bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 0]);
                  avgG +=   (bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 1]);
                  avgR +=   (bitmapImage[x*3 + y*bitmapInfoHeader->biWidth*3 + 2]);
                }

                if (x*3 - y*bitmapInfoHeader->biWidth*3 >= 0)
                {
                  avgB += (bitmapImage[x*3 - y*bitmapInfoHeader->biWidth*3 + 0]);
                  avgG += (bitmapImage[x*3 - y*bitmapInfoHeader->biWidth*3 + 1]);
                  avgR += (bitmapImage[x*3 - y*bitmapInfoHeader->biWidth*3 + 2]);
                }
                ile++;
            }
        }

        /*for blur*/
        // avgB = avgB / ile;
        // avgG = avgG / ile;
        // avgR = avgR / ile;

        avgB = (avgB + centerB)/ile;
        avgG = (avgG + centerG)/ile;
        avgR = (avgR + centerR)/ile;

        bitmapImage[xx*3 + yy*bitmapInfoHeader->biWidth*3 + 0] = avgB;
        bitmapImage[xx*3 + yy*bitmapInfoHeader->biWidth*3 + 1] = avgG;
        bitmapImage[xx*3 + yy*bitmapInfoHeader->biWidth*3 + 2] = avgR;
    }
}



  return bitmapImage;
}






int main(){

  BITMAPFILEHEADER bitmapFileHeader;
  BITMAPINFOHEADER bitmapInfoHeader;
  unsigned char *bitmapData;
  bitmapData = LoadBitmapFile("xiahouDun.bmp",&bitmapInfoHeader, &bitmapFileHeader);
  printf("image file size:\t%d\n", bitmapInfoHeader.biSizeImage);
  printf("bitmap size: %d\tbitmap type: %c\n", bitmapFileHeader.bfSize, bitmapFileHeader.bfType);

  //Apply sharpening filter
  // bitmapData = applySharpeningFilter(&bitmapInfoHeader, bitmapData);

  bitmapData = applySharpeningFilter(&bitmapInfoHeader, bitmapData);

  //Write to image
  WriteBitmapFile("SharpeningImage.bmp", &bitmapInfoHeader, &bitmapFileHeader, bitmapData);




  return 0;
}
