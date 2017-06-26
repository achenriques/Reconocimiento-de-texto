/*
 * File:   main.c
 * Author: Luis Francisco Blanco Cadavid
 *         Alex Carballo Henriques
 *
 * Created on 19 de febrero de 2015, 9:14
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <emmintrin.h>
//#include <pthread.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
/*
 * 
 */
//#define THREADS 4                                                                                                                                                  

//Crea un cuadro que indica en que cuadro busca el texto.
void cuadro(uchar *p, int columna,int fila)
{
     if(fila==0||columna==0||fila==35||columna==15)
     {
          *p=0;
     }
}

//Compara dos imagenes usando la libreria SSE2.
int compararImagen(__m128i *pImg1,__m128i  *pImg2)
{
    __m128i pmImg = _mm_loadu_si128 (pImg1);
    __m128i pi = _mm_loadu_si128 (pImg2);
    __m128i t = _mm_sad_epu8(pmImg, pi);
    int *ttt = (int*) &t;

    return ttt[0]+ttt[2];
}

int recorreImagen(IplImage *Img1, IplImage *Img2, IplImage *Img3,int filaT, int columnaT, int posicion, int filaP)
{
    int fila,columna,sumaP=0;
    __m128i *pmImg1;
    __m128i *pmPatr;
    uchar *pImg3;

    //Recorrio del patron de imagenes
                 for(fila=0;fila<=35;fila++)
                {
                    pmImg1 = (__m128i *) (Img1->imageData + fila*Img1->widthStep + columnaT*3+filaT*Img1->widthStep);
                    pmPatr = (__m128i *) (Img2->imageData + fila*Img2->widthStep + 12 + Img2->widthStep*filaP*56+posicion*15*3+2*Img2->widthStep);
                    pImg3 = (uchar*) Img3->imageData + fila*Img3->widthStep + columnaT*3+filaT*Img3->widthStep;

                     for(columna=0;columna<=15;columna++)
                    {
                        cuadro(pImg3, columna, fila);
                        pImg3++;

                        cuadro(pImg3, columna, fila);
                        pImg3++;

                        cuadro(pImg3, columna, fila);
                        pImg3++;

                    }


                    sumaP+=compararImagen(pmImg1,pmPatr);
                    pmPatr++;
                    pmImg1++;

                    sumaP+=compararImagen(pmImg1,pmPatr);
                    pmPatr++;
                    pmImg1++;

                    sumaP+=compararImagen(pmImg1,pmPatr);
                    pmPatr++;
                    pmImg1++;

                }

    return sumaP;
}

//Borra las letras del texto.
void ponerBlanco(IplImage *Img1,int columnaT, int filaT )
{
    int fila,columna;
    uchar *pImg1;

    for(fila=0;fila<=35;fila++)
    {
        pImg1 = (uchar*) Img1->imageData + fila*Img1->widthStep + columnaT*3+filaT*Img1->widthStep; //24

        for(columna=0;columna<=15;columna++)
        {
            *pImg1=255;
            pImg1++;
            *pImg1=255;
            pImg1++;
            *pImg1=255;
            pImg1++;
         }
    }
}

int main(int argc, char** argv)
{
    //variable que controla el bucle.
    int salida=0;
    //Creamos un string co todos los caracteres del patrón.
    char letra[97]={'!','"','#','$','%','&','´','(',')','*','+',',','-','.','/','0','1','2','3','4',
                    '5','6','7','8','9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H',
                    'I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','/',
                    ']','^','`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q',
                    'r','s','t','u','v','w','x','y','z','{','|','}','~','a','e','i','o','u'};

    //Comprobamos que los argumentos aportados coincidan con los necesarios.
    if (argc != 3)
    {
        printf("Usage: %s image_file_name\n", argv[0]);
        return EXIT_FAILURE;
    }

    //Cargamos imagen del texto.
    IplImage* Img1 = cvLoadImage(argv[1], CV_LOAD_IMAGE_UNCHANGED);
    //Cargamos imagen del patron.
    IplImage* Img2 = cvLoadImage(argv[2], CV_LOAD_IMAGE_UNCHANGED);

    //Creamos dos imagenes de soporte.
    IplImage* Img3;
  
    if (!Img1)
    {
        printf("Error: fichero %s no leido\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (!Img2)
    {
        printf("Error: fichero %s no leido\n", argv[2]);
        return EXIT_FAILURE;
    }

    cvNamedWindow("Texto", 1);

    //Funcion de comparación de dos imagenes
    

    int sumaP=0;
    int posicion=0;
    int contador=0;
    int filaP=0;
    int filaT=0;
    int columnaT=0;

    //For de recorrido del texto
    for(filaT=0;filaT<=Img1->height;filaT++)
    {
        
        for(columnaT=0;columnaT<Img1->width-1;columnaT++)
        {
            Img3 = cvCloneImage(Img1);

            sumaP=0;contador=0;
            posicion=0;
            filaP=0;
            salida=0;
            do
            {
                sumaP=recorreImagen(Img1,Img2,Img3,filaT,columnaT,posicion,filaP);

                
                //Comprobacion de si son igules, si lo son sale del While, si no comprueba la siguiente.
                if((sumaP<=3200)||((posicion==4)&&(filaP==3)))
                {
                    salida=1;
                    if(sumaP<3200)
                    {
                        printf("\nSuma: %d \tEncontrada %c\n", sumaP,letra[contador]);
                        ponerBlanco(Img1,columnaT,filaT);
                    }
                    cvShowImage("Texto", Img3);
                    cvWaitKey(1);
                }
                else
                {
                    if(posicion==30)
                    {
                        posicion=0;
                        filaP++;
                        contador++;
                        sumaP=0;
                    }
                    else
                    {
                        posicion++;
                        sumaP=0;
                        contador++;
                    }
                }
            }while(salida==0);
            cvReleaseImage(&Img3);
        }
    }
    cvReleaseImage(&Img1);
    cvReleaseImage(&Img2);
    cvReleaseImage(&Img3);

    return (EXIT_SUCCESS);
}

