//AUTHOR: THEWILDJARVI
//DATE DEC-25-2021
//MERRY FUKN XMAS IT WORKS


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <io.h>




//256x256 8bit RGB color(2-3-2) display from ram location 'display' to 'display + 0x10000'
// adding 0x00100(0d256) increments in Y
// adding 0x00001(0d001) increments in X
//
// 65280...................................  65535
//
//
//
//
//
//
// 256..257..258...............................511
// 0....1....2....3....4....5....6.............255
// (x,y) = (3, 2) = RAM[X + 256*Y] = RAM[515] 


void drawline(int x0, int y0, int x1, int y1, int screen[65536])  // draws line from (x0,y0) to (x1,y1) and stores line in array "screen" of 256x256 pixels
{
    int dx = (x1-x0);
    int dy = (y1-y0);
    int incX = 1;
    int incY = 256;
    int XaY = 0;
    int cmpt = 0;
    int incD, incS, err, X, Y;
    //screen = 0;
    if (dx < 0) {incX = -1;}                //incX = sgn(dx)
    if (dy < 0) {incY = -256;}              //incY = sgn(dy)
    
    dx = abs(dx);                           // dx = abs(x1-x0)
    dy = abs(dy);                           // dy = abs(y1-y0)
    
    if (dx > dy) { XaY = 1, cmpt = dx, incS = dy<<1;}
    else {cmpt = dy, incS = dx<<1;}

    incD = -((abs(dx-dy)) * 2);
    err = incD + cmpt;
    X = x0;
    Y = y0*256;

while(cmpt >= 0)
    {
    //printf("(x,y) = (%d, %d)", X, Y>>8);
    //printf("\n");
    screen[X + Y] = 255 ; //putpixel(x,y)
    cmpt -= 1;
    if(err >=0 || XaY) {X += incX;}
    if(err >=0 || !XaY) {Y += incY;}
    if(err >=0) {err+=incD;}
    else {err+=incS;}
    }
}


void disp_screen(int display[65536]) //256 x 256 image held in display[]
    {
    //set up unicode mode
    _setmode(_fileno(stdout), _O_U16TEXT);

    int i, j;
    int temp;
    for(j = 0; j<245; j++)
        {
        wprintf(L"\n");
        for(i=0; i<256; i++)
            {   
                temp = display[i+(j*256)];
                if( temp > 0 )
                {
                    wprintf(L"\x25A1");
                }
                else    
                    wprintf(L"\x25A0");  
            }
        wprintf(L" %d", j);
        }  
    }

void render_pgm(int iteration, int image[65536])
    {
    int i, j, w = 244, h = 256;
    FILE* pgmimg;
    char buffer[300];
    snprintf(buffer, 300,  "img%d.pgm", iteration); //make it so i can iterate the fucking name, why is c garbage?
    pgmimg = fopen(buffer, "wb"); //write the file in binary mode
    fprintf(pgmimg, "P2\n"); // Writing Magic Number to the File
    fprintf(pgmimg, "%d %d\n", w, h); // Writing Width and Height into the file
    fprintf(pgmimg, "255\n"); // Writing the maximum gray value
    int count = 0;
    for (i = 0; i < h; i++)
        {
        for (j = 0; j < w; j++) 
            {
                fprintf(pgmimg, "%d ", image[j + (i*256)]); //Copy gray value from array to file
            }
        fprintf(pgmimg, "\n");
        }
    fclose(pgmimg);          
    }




float* vector_by_matrix (float in_vector[4], float in_matrix[4][4])
{/*                                                 i      i      i      i
out_vector  [0:x'] in_vector  [0:x]   in_matrix 0 [00:a | 01:b | 02:c | 03:d]
            [1:y']            [1:y]             1 [04:e | 05:f | 06:g | 07:h]
            [2:z']            [2:z]             2 [08:i | 09:j | 10:k | 11:l]
            [3:w']            [3:w]             3 [12:m | 13:n | 14:o | 15:p]     */
    static float out_vector[4];
    int i;

    for(i=0; i<4; i++)
    {
        out_vector[0] = in_vector[i]*in_matrix[0][i] + out_vector[0];
        out_vector[1] = in_vector[i]*in_matrix[1][i] + out_vector[1];
        out_vector[2] = in_vector[i]*in_matrix[2][i] + out_vector[2];
        out_vector[3] = in_vector[i]*in_matrix[3][i] + out_vector[3];
    }

//printf("\n");
//printf("%f %f %f %f", out_vector[0], out_vector[1], out_vector[2], out_vector[3]);
return out_vector;
}

float* offset_cube(float cube_vertices[8][4], float x, float y, float z)
{
    for(int i=0; i<8;i++)
    {
        printf("sum x, vert x, offset x %f %f %f \n",cube_vertices[i][0] + x, cube_vertices[i][0] , x );
        cube_vertices[i][0] = cube_vertices[i][0] + x;
        printf("sum y, vert y, offset y %f %f %f \n",cube_vertices[i][1] + y, cube_vertices[i][1] , y );
        cube_vertices[i][1] = cube_vertices[i][0] + y;
        printf("sum z, vert z, offset z %f %f %f \n\n",cube_vertices[i][2] + z, cube_vertices[i][2] , z );
        cube_vertices[i][2] = cube_vertices[i][0] + z;
    }
}

float* projection_xfrm(float in_matrix[8][4]) // input 8x4 matrix, outputs 8x2 x/z and y/z /10 for scale projection and add 20 for offset
{       
    int i; int x = 256; int y = 256; int z = 512; int scale = 256;
    static float out_matrix[8][2];
    for(i=0; i<8; i++)
    {
        out_matrix[i][0] = ((in_matrix[i][0] + x) / ((in_matrix[i][2] + z) / scale));
        printf("%d  out x, in x , / z/4  %f %f %f \n\n",i, out_matrix[i][0], in_matrix[i][0] , ((in_matrix[i][2]+ z) /scale)  );
        out_matrix[i][1] = ((in_matrix[i][1] + y )/ ((in_matrix[i][2] + z) / scale));
        printf("%d  out y, in y  , / z/4  %f %f %f \n\n",i, out_matrix[i][1], in_matrix[i][1] , ((in_matrix[i][2]+ z) /scale)  );
    }
    return out_matrix;
}


float* cube_xfrm(float cube_vertices[8][4], float in_matrix[4][4])
{   
    float in_vector[4] ={};
    static float cube_vrtx_xfrm[8][4];
    int i;
        for(i=0; i<8; i++) //load in the vertices of the object and apply matrix transform. 
        {
            in_vector[0] = cube_vertices[i][0] ;
            in_vector[1] = cube_vertices[i][1] ;
            in_vector[2] = cube_vertices[i][2] ;
            in_vector[3] = cube_vertices[i][3]; 
    
            float* out_vector = vector_by_matrix(in_vector, in_matrix);  
            cube_vrtx_xfrm[i][0] = out_vector[0];
            cube_vrtx_xfrm[i][1] = out_vector[1];
            cube_vrtx_xfrm[i][2] = out_vector[2];
            cube_vrtx_xfrm[i][3] = out_vector[3];

            out_vector[0] = 0;
            out_vector[1] = 0;
            out_vector[2] = 0;
            out_vector[3] = 0;
        }
return cube_vrtx_xfrm;
}

void render_cube(float cube_vertices[8][4], int display[65536], int iteration)
{
/*  
    float x = 64; float y = 64; float z = 64;

    for(int i=0; i<8;i++)
    {
        printf("sum x, vert x, offset x %f %f %f \n",cube_vertices[i][0] + x, cube_vertices[i][0] , x );
        cube_vertices[i][0] = cube_vertices[i][0] + x;
        printf("sum y, vert y, offset y %f %f %f \n",cube_vertices[i][1] + y, cube_vertices[i][1] , y );
        cube_vertices[i][1] = cube_vertices[i][0] + y;
        printf("sum z, vert z, offset z %f %f %f \n\n",cube_vertices[i][2] + z, cube_vertices[i][2] , z );
        cube_vertices[i][2] = cube_vertices[i][0] + z;
    }
*/
    float* projected = projection_xfrm(cube_vertices);

    int Ax = projected[0];
    int Ay = projected[1];
    int Bx = projected[2];
    int By = projected[3];
    int Cx = projected[4];
    int Cy = projected[5];
    int Dx = projected[6];
    int Dy = projected[7];    
    int Ex = projected[8];
    int Ey = projected[9];
    int Fx = projected[10];
    int Fy = projected[11];
    int Gx = projected[12];
    int Gy = projected[13];
    int Hx = projected[14];
    int Hy = projected[15];

    drawline(Ax, Ay, Ex, Ey, display);
    drawline(Ax, Ay, Cx, Cy, display);
    drawline(Gx, Gy, Ex, Ey, display);
    drawline(Gx, Gy, Cx, Cy, display);
    drawline(Dx, Dy, Hx, Hy, display);
    drawline(Dx, Dy, Bx, By, display);
    drawline(Fx, Fy, Hx, Hy, display);
    drawline(Fx, Fy, Bx, By, display);
    drawline(Hx, Hy, Gx, Gy, display);
    drawline(Fx, Fy, Ex, Ey, display);
    drawline(Dx, Dy, Cx, Cy, display);
    drawline(Bx, By, Ax, Ay, display);
    render_pgm(iteration, display);
 
}

int main(void)
{
    //need to have object vertices
    //need to have edges
    //need matrix transform (takes in 4x1 input vector and 4x4 matrix transform, returns 4x1 matrix)

    int display[65536], i; //initialize array to be drawn to as a screen x y z w
 /*  
    float cube_vertices[8][4] = {
    {16, 16, 16, 1}, //A 0
    {16, 16,  64, 1}, //B 1
    {16,  64, 16, 1}, //C 2
    {16,  64,  64, 1}, //D 3
    { 64, 16, 16, 1}, //E 4
    { 64, 16,  64, 1}, //F 5
    { 64,  64, 16, 1}, //G 6
    { 64,  64,  64, 1}  //H 7
    };
*/
    float cube_vertices[8][4] = {
    {-48, -48, -48, 1}, //A 0
    {-48, -48,  48, 1}, //B 1
    {-48,  48, -48, 1}, //C 2
    {-48,  48,  48, 1}, //D 3
    { 48, -48, -48, 1}, //E 4
    { 48, -48,  48, 1}, //F 5
    { 48,  48, -48, 1}, //G 6
    { 48,  48,  48, 1}  //H 7
    };
     
    float* cube_vertex_xfrm;
    //float in_vector[4] = {};
    //float* out_vector[4] ;
    //float in_matrix[16] = {1, 0, 0, 128, 0, 1, 0, 128, 0, 0, 1, 128, 0, 0, 0, 1}; // 2x identity
    //float in_matrix[16] = {1, 0, 0, 128, 0, .707, -.707, 128, 0, .707, .707, 128, 0, 0, 0, 1}; // pi/4 rotation in x axis 

    float in_matrix[4][4] = {
                            {.985, -.113, .129, 0 },
                            {.129, .983, -.131, 0},
                            {-.113, .145, .983, 0},  
                            {0, 0, 0, 1}
                            };
/*
    float in_matrix[4][4] = {
                            {1, 0, 0, 0 },
                            {0, .97, -0.26, 0},
                            {0, .26, .97, 0},  
                            {0, 0, 0, 1}
                            };
*/   
    for(i=0;i<48;i++)
    {
        cube_vertex_xfrm = cube_xfrm(cube_vertices, in_matrix);
        
        
        //float* temp = offset_cube(cube_vertices, 64, 64, 64);

        render_cube(cube_vertex_xfrm, display,i); 
        memset(display, 0, sizeof display);
        memcpy(cube_vertices, cube_vertex_xfrm, sizeof cube_vertices);
        
        
    }



    
printf("done");
return 0;
}