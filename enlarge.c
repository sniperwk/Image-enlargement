#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int r; // red
    int g; // green
    int b; // blue
 } Pixel;

void ioHeadline(FILE *source_fp, FILE *dest_fp, int *row, int *col, int *newRow, int *newCol);

void ioLine(FILE *source_fp, FILE *dest_fp);

void skipLine(FILE *fp);

void enlarge(FILE *source_fp, FILE *dest_fp, int row, int col, int newRow, int newCol);

void input(FILE *source_fp,FILE *dest_fp, int row, int col, Pixel old[][col]);

void calculate(FILE *dest_fp, int row, int col, int newRow, int newCol, int inputRow,  Pixel old[][col], Pixel new[][newCol]);

void pixelAverage(Pixel *average, Pixel *input1, Pixel *input2);

int main(int argc, char *argv[])
{

	FILE *source_fp, *dest_fp;
	int value, num, i, j, k, row, col, newRow, newCol, grey;

  if (argc != 3) {
    fprintf(stderr, "usage: rotate file1 file2\n");
    exit(EXIT_FAILURE);
  }

  
  if ((source_fp = fopen(argv[1], "rb")) == NULL) {
  
		printf("Can't open %s\n", argv[1]);
		exit(EXIT_FAILURE);
	} else {

		//printf("fopen is successful: %s\n", argv[1]);
		
	}
  
  if ((dest_fp = fopen(argv[2], "w")) == NULL) {

    fprintf(stderr, "Can't open %s\n", argv[2]);
    fclose(source_fp);
    exit(EXIT_FAILURE);
  }

  // switch off buffer
  setbuf(dest_fp, NULL);


  ioHeadline(source_fp, dest_fp, &row, &col, &newRow, &newCol);


  enlarge(source_fp, dest_fp, row, col, newRow, newCol);  // the major function

  fclose(source_fp);
  fclose(dest_fp);

  return 0;
}


void enlarge(FILE *source_fp, FILE *dest_fp, int row, int col, int newRow, int newCol)
{
  
  Pixel old[row][col], new[newRow][newCol];
  int j, k, l, rowLoop;
  int status;

  // read pixel color values from source file
  input(source_fp, dest_fp, row, col, old); 

  for (rowLoop = 0; rowLoop < row - 1 ; rowLoop++) { // j < row - 1
    if (fork() == 0) { // for child
      calculate(dest_fp, row, col, newRow, newCol, rowLoop, old, new);


      exit(0);
    }

    wait(&status);   // parent
    } // for rowLoop

      //calculate last row
      for (j = 0; j < newCol; j += 2) {  //  the columns 0, 2, 4, ... newCol 
          pixelAverage(&new[newRow - 1][j], &old[row - 1][j/2], &old[row - 1][j/2]); // the same value as old[row - 1][j/2];
        }
        
      
      for (j = 1; j < newCol; j += 2) { // the columns 1, 3, 4, ... newCol - 1 
          pixelAverage(&new[newRow - 1][j], &new[newRow - 1][j-1], &new[newRow - 1][j+1]);
        }
      
      
      //output the last row
      for (k = 0; k < newCol; k++) {


        fprintf(dest_fp, "%d %d %d", new[newRow - 1][k].r, new[newRow - 1][k].g, new[newRow - 1][k].b);
        if (k < newCol - 1) {
          fprintf(dest_fp, " ");
          }
        }
      
        fprintf(dest_fp, "\n");  // a new line for each row


}

void calculate(FILE *dest_fp, int row, int col, int newRow, int newCol, int inputRow,  Pixel old[][col], Pixel new[][newCol])
{
  int j, k, l;
  int rowOfNew = 2 * inputRow;
  // 1st row
  for (j = 0; j < newCol; j += 2) {  //  the columns 0, 2, 4, ... newCol 
      pixelAverage(&new[rowOfNew][j], &old[inputRow][j/2], &old[inputRow][j/2]);// the same value as old[row - 1][j/2];
    }
    
  
  for (j = 1; j < newCol; j += 2) { // the columns 1, 3, 4, ... newCol - 1 
      pixelAverage(&new[rowOfNew][j], &new[rowOfNew][j-1], &new[rowOfNew][j+1]);
    }
  // 2nd row
  for (j = 0; j < newCol; j++) { 
     //  the columns 0, 2, 4, ... newCol 
     pixelAverage(&new[rowOfNew + 1][j], &old[inputRow][j/2], &old[inputRow + 1][j/2]);
    } 
    
  for (j = 1; j < newCol; j += 2) { // the columns 1, 3, 4, ... newCol - 1 
      pixelAverage(&new[rowOfNew + 1][j], &new[rowOfNew + 1][j - 1], &new[rowOfNew + 1][j + 1]);
  }


  // output the calculated line
  for (l = rowOfNew; l <= rowOfNew + 1; l++) { // row num for the new matrix
     for (k = 0; k < newCol; k++) {

      fprintf(dest_fp, "%d %d %d", new[l][k].r, new[l][k].g, new[l][k].b);
      if (k < newCol - 1) {
        fprintf(dest_fp, " ");
        }
      }
    
      fprintf(dest_fp, "\n");  // a new line for each row

    }


}


void pixelAverage(Pixel *average, Pixel *input1, Pixel *input2)
{
  average->r = (input1->r + input2->r) / 2;
  average->g = (input1->g + input2->g) / 2;
  average->b = (input1->b + input2->b) / 2;


}



void input(FILE *source_fp, FILE *dest_fp, int row, int col, Pixel old[][col])
{
  int j, k;
  int r, g, b;

  for (j = 0; j < row; j++) {
    for (k = 0; k < col; k++) {
        fscanf(source_fp, "%d %d %d", &(old[j][k].r), &(old[j][k].g), &(old[j][k].b));
      }
    skipLine(source_fp);
  }
 
}


void ioHeadline(FILE *source_fp, FILE *dest_fp, int *row, int *col, int *newRow, int *newCol)
{

  // get input info
  ioLine(source_fp, dest_fp);  // copy and paste 1st line "P3"

  // get the row and column info from 2nd line
  fscanf(source_fp, "%d %d\n", col, row);
  *newRow = *row + *row - 1;
  *newCol = *col + *col - 1;
  fprintf(dest_fp, "%d %d\n", *newCol, *newRow);
  

  ioLine(source_fp, dest_fp);  // copy and paste 3rd line color scale "255"

}


void ioLine(FILE *source_fp, FILE *dest_fp)
{
  int ch;
  while ((ch = fgetc(source_fp)) != '\n' && ch != EOF) {
    fputc(ch, dest_fp);
  }
  fputc('\n', dest_fp);
}

void skipLine(FILE *fp)
{
  int ch;
  while ((ch = fgetc(fp)) != '\n') ;   // skip the line

}





