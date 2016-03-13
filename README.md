# Image-enlargement-in-C

The input image(enlarge.ppm) to be transformed is given as PPM ASCII.
The output image(dest.ppm) is in the same PPM ASCII.

Let the given pixel matrix be rows*columns. The enlarged matrix

(i)  add one row between every two neighboring rows in the original matrix where the value of each new pixel is the average of the values of the  two pixels  in the neighboring rows and in the same column; 

(ii) analogously add one column between every two neighboring columns.

Parallel execution:  
For each row i  ( 1  <=  i < rows ), fork() creates a child process to calculate the new row between row i and i+1, and to write row i and this new row into the transformed file. 

The parent process waits for the child process to terminate before processing the next row. 
The original process will write the last row into the transformed file. 

Usage

./executable enlarge.ppm dest.ppm
