#define _CRT_SECURE_NO_WARNINGS
#define bool {false,true};
#include <stdio.h>
#define MAX_ALLOCATED 100
void* allocated_arrays[MAX_ALLOCATED];
int allocated_count = 0;
void main()
 {
  int *arr=malloc (3*sizeof(int )),*arr2=malloc (4);
 allocated_arrays[allocated_count++] = arr;
allocated_arrays[allocated_count++] = arr2;
int n=3;
 for (int iIndex=0;
 iIndex<4;
 iIndex++)
  {
   int i=arr[iIndex];
  printf ("%d",i);
    }
  float numberArr[]=
 {
  1.6,2,3,4 }
 ;
 for (int numberIndex=0;
 numberIndex<4;
 numberIndex++)
  {
   float number=numberArr[numberIndex];
  printf ("%f",number);
    }
  double arr3[4]=
 {
  1,2,3 }
 ;
 char*s="abcd";
 int l=8;
 if (x>5)
  {
   printf ("x is greater than 5");
    }
  bool flag=true ;
 int x=8,y=9;
 if (6<x&&x<10)printf ("x is%dand y is%dand x+y equals%lf",x,y,x+y);
  else if (y<4||flag)printf ("y<4flag is:%d",flag);
  else printf ("oooops...");
  for (int i = 0; i < allocated_count; i++) {
    free(allocated_arrays[i]);
}
 }
 