 /*******************************************************************
   RTN SPLINE: Fits cubic smoothing spline to time series
 
   Derived from IMSL routines by Edward R Cook, Tree Ring Laboratory,
   Lamont-Doherty Earth Observatory, Palisades, New York, USA
 
   Four routines combined into one by
   Richard L Holmes, University of Arizona, Tucson, Arizona, USA
   Modified copyright (C) 10 AUG 1998

 ********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415926535897935

/* DYNAMICALLY ALLOCATE A 2-D ARRAY */
/* Assumption:  nrows*ncols*element_size, rounded up to a multiple   */
/* of sizeof(long double), must fit in a long type.  If not, then    */
/* the "i += ..." step could overflow.                               */
 
void **MATRIX(int nrows, int ncols, int first_row, int first_col,int element_size)
{
    void **p;
    int alignment;
    long i;
 
    if(nrows < 1 || ncols < 1) return(NULL);
    i = nrows*sizeof(void *);
    /* align the addr of the data to be a multiple of sizeof(long double) */
    alignment = i % sizeof(long double);
    if(alignment != 0) alignment = sizeof(long double) - alignment;
    i += nrows*ncols*element_size+alignment;
    if((p = (void **)malloc((size_t)i)) != NULL)
    {
        /* compute the address of matrix[first_row][0] */
        p[0] = (char *)(p+nrows)+alignment-first_col*element_size;
        for(i = 1; i < nrows; i++)
            /* compute the address of matrix[first_row+i][0] */
            p[i] = (char *)(p[i-1])+ncols*element_size;
            /* compute the address of matrix[0][0] */
        p -= first_row;
    }
    return(p);
}

/* This function is called from SPLINE when :  */
/* 1. Series is too short to compute spline    */
/* 2. Matrix A is not positive definite        */

void errorAction(int N, double *Y, float *ZF)
{
    int k;
    double ZN;

    ZN = 0.0;
    for(k = 1; k <= N; k++)
        ZN = ZN + Y[k];

    if (N > 0)
    {
        ZN = ZN/(float)N;
        for(k = 1; k <= N; k++)
            ZF[k-1] = ZN;
    }

    return;
}

/* Function  SPLINE: Fits cubic smoothing spline to time series               */
/* Arguments:                                                                 */
/*                                                                            */
/* N:   Number of values in time series                                       */
/* Z:   Time series array to be modeled with spline                           */
/* ZF:  Computed cubic spline function array fit to time series               */
/* ZSP: Length (rigidity) of spline to be used to model series                */
/* ZPV: Portion of variance at wavelength ZSP contained in spline (0<ZPV<1)   */
/*                                                                            */
/* Arguments Z, ZF, ZSP and ZPV are single precision;                         */
/* computation is done entirely in double-precision arithmetic                */

void SPLINE(int N, float *Z, float *ZF, float ZSP, float ZPV)
{
    int i, j, k, l, m;
    int NC, NC1, NCP1, IMNCP1, I1, I2, JM1, IW, KL, N1, K1;
    double RSP, VPV, PD, RN, D1, D2, SUM;
    double **A, *F, *Y, C1[5], C2[4];

    C1[0] =  0.0;
    C1[1] =  1.0;
    C1[2] = -4.0;
    C1[3] =  6.0;
    C1[4] = -2.0;
   
    C2[0] =  0.0;
    C2[1] =  0.0;
    C2[2] =  0.33333333333333;
    C2[3] =  1.33333333333333;
    
    /* Allocate arrays to store intermeediate results */
    A = (double **)MATRIX(N+1, 5, 0, 0, sizeof(double));
    F = (double *)malloc((N+1)*sizeof(double));
    Y = (double *)malloc((N+1)*sizeof(double));
    if (A == NULL || F == NULL || Y == NULL)
    {
        printf("\nSPLINE >> Unable to allocate memory\n");
        return;
    }
    
    /* Check whether series is too short to compute spline */
    if (N < 4)
    {
        errorAction(N, Y, ZF);
        return;
    }

    /* Copy time series into double precision array */
    for(j = 1; j <= N; j++)
        Y[j] = (double)Z[j-1];
         
    /* Compute Lagrange multiplier, which defines frequency response of spline */
    RSP = (double)ZSP;
    VPV = (double)ZPV;
    PD = ((1.0/(1.0-VPV)-1.0)*6.0* pow((cos(PI*2.0/RSP)-1.0),2.0))/(cos(PI*2.0/RSP)+2.0);
    for(i = 1; i <= N-2; i++)
        for(j = 1; j <= 3; j++)
        {
            A[i][j] = C1[j] + PD * C2[j];
            A[i][4] = Y[i] + C1[4] * Y[i+1] + Y[i+2];
        }

    A[1][1] = C2[1];
    A[1][2] = C2[1];
    A[2][1] = C2[1];
    NC = 2;

    /* Begin LUDAPB */
    RN = 1.0/((double)(N-2) * 16.0);
    D1 = 1.0;
    D2 = 0.0;
    NCP1 = NC + 1;

    /* Initialize zero elements */
    if (NC != 0)
    {
        for(i = 1; i <= NC; i++)
            for(j = i; j <= NC; j++)
            {
                k = NCP1 - j;
                A[i][k] = 0.0;
            }
    }

    /* i: row index of element being computed */
    /* j: column index of element being computed */
    /* l: row index of previously computed vector being used to compute inner product */
    /* m: column index */
    for(i = 1; i <= N-2; i++)
    {
        IMNCP1 = i - NCP1;
        I1 = (1 < 1 - IMNCP1? 1 - IMNCP1: 1);
        for(j = I1; j <= NCP1; j++)
        {
            l = IMNCP1 + j;
            I2 = NCP1 - j;
            SUM = A[i][j];
            JM1 = j - 1;

            if (JM1 > 0)
            {
                for(k = 1; k <= JM1; k++)
                {
                    m = I2 + k;
                    SUM = SUM - (A[i][k]*A[l][m]);
                }
            }

            /* Matrix not positive definite */
            if (j == NCP1)
            {
                if (A[i][j]+SUM*RN <= A[i][j])
                {
                    printf("\nSPLINE >> Matrix not positive definite\n");
                    errorAction(N, Y, ZF);
                    return;             
                }
                
                A[i][j] = 1.0/sqrt(SUM);
                
                /* Update determinant */
                D1 = D1*SUM;
                while (fabs(D1) > 1.0)
                {
                    D1 = D1*0.0625;
                    D2 = D2+4.0;
                }
                
                while (fabs(D1) <= 0.0625)
                {
                    D1 = D1*16.0;
                    D2 = D2-4.0;
                }
                continue;
            }
            A[i][j] = SUM*A[l][NCP1];                           
        }
    }
    /* End LUDAPB */

    /* Begin LUELPB */
    /* Solution LY = B */
    NC1 = NC + 1;
    IW = 0;
    l = 0;

    for(i = 1; i <= N-2; i++)
    {
        SUM = A[i][4];
        if (NC > 0) {
            if (IW != 0) {
                l = l + 1;
                if (l > NC)
                {
                    l = NC;
                }
                k = NC1 - l;
                KL = i - l;

                for(j = k; j <= NC; j++)
                {
                    SUM = SUM - (A[KL][4]*A[i][j]);
                    KL = KL + 1;
                }
            } else if (SUM != 0.0)
            {
                IW = 1;
            }
        }
        A[i][4] = SUM*A[i][NC1];
    }

    /* Solution UX = Y */
    A[N-2][4] = A[N-2][4]*A[N-2][NC1];
    N1 = N-2+1;

    for(i = 2; i <= N-2; i++)
    {
        k = N1 - i;
        SUM = A[k][4];
        if (NC > 0)
        {
            KL = k +1;
            K1 = (N-2 < k+NC? N-2: k+NC);
            l = 1;
            for(j = KL; j <= K1; j++)
            {
                SUM = SUM - A[j][4]*A[j][NC1-l];
                l = l + 1;
            }
        }
        A[k][4] = SUM*A[k][NC1];
    }
    /* End LUELPB */

    /* Calculate Spline Curve */
    for(i = 3; i <= N-2; i++)
        F[i] = A[i-2][4]+C1[4]*A[i-1][4]+A[i][4];

    F[1] = A[1][4];
    F[2] = C1[4]*A[1][4]+A[2][4];
    F[N-1] = A[N-2-1][4]+C1[4]*A[N-2][4];
    F[N] = A[N-2][4];

    for(i = 1; i <= N; i++)
        ZF[i-1] = Y[i] - F[i];
 
     return;
 }
