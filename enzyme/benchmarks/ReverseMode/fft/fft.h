#ifndef _fft_h_
#define _fft_h_

#include <adept_source.h>
#include <adept.h>
#include <adept_arrays.h>
using adept::adouble;

using adept::aVector;


/*
  A classy FFT and Inverse FFT C++ class library

  Author: Tim Molteno, tim@physics.otago.ac.nz

  Based on the article "A Simple and Efficient FFT Implementation in C++" by Volodymyr Myrnyy
  with just a simple Inverse FFT modification.

  Licensed under the GPL v3.
*/


#include <cmath>

inline void swap(double* a, double* b) {
  double temp=*a;
  *a = *b;
  *b = temp;
}

static void recursiveApply(double* data, int iSign, unsigned N) {
  if (N == 1) return;
  recursiveApply(data, iSign, N/2);
  recursiveApply(data+N, iSign, N/2);

  double wtemp = iSign*sin(M_PI/N);
  double wpi = -iSign*sin(2*M_PI/N);
  double wpr = -2.0*wtemp*wtemp;
  double wr = 1.0;
  double wi = 0.0;

  for (unsigned i=0; i<N; i+=2) {
    int iN = i+N;

    double tempr = data[iN]*wr - data[iN+1]*wi;
    double tempi = data[iN]*wi + data[iN+1]*wr;

    data[iN] = data[i]-tempr;
    data[iN+1] = data[i+1]-tempi;
    data[i] += tempr;
    data[i+1] += tempi;

    wtemp = wr;
    wr += wr*wpr - wi*wpi;
    wi += wi*wpr + wtemp*wpi;
  }
}

static void scramble(double* data, unsigned N) {
  int j=1;
  for (int i=1; i<2*N; i+=2) {
    if (j>i) {
      swap(&data[j-1], &data[i-1]);
      swap(&data[j], &data[i]);
    }
    int m = N;
    while (m>=2 && j>m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
}

static void rescale(double* data, unsigned N) {
  double scale = ((double)1)/N;
  for (unsigned i=0; i<2*N; i++) {
    data[i] *= scale;
  }
}

static void fft(double* data, unsigned N) {
  scramble(data, N);
  recursiveApply(data,1, N);
}

static void ifft(double* data, unsigned N) {
  scramble(data, N);
  recursiveApply(data,-1, N);
  rescale(data, N);
}



inline void swapad(adept::ActiveReference<double> a, adept::ActiveReference<double> b) {
  adouble temp=a;
  a = b;
  b = temp;
}

static void recursiveApply(aVector data, int iSign, unsigned N) {
  if (N == 1) return;
  recursiveApply(data, iSign, N/2);
  recursiveApply(data(adept::range(N,adept::end)), iSign, N/2);

  adouble wtemp = iSign*std::sin(M_PI/N);
  adouble wpi = -iSign*std::sin(2*M_PI/N);
  adouble wpr = -2.0*wtemp*wtemp;
  adouble wr = 1.0;
  adouble wi = 0.0;

  for (unsigned i=0; i<N; i+=2) {
    int iN = i+N;

    adouble tempr = data(iN)*wr - data(iN+1)*wi;
    adouble tempi = data(iN)*wi + data(iN+1)*wr;

    data(iN) = data(i)-tempr;
    data(iN+1) = data(i+1)-tempi;
    data(i) += tempr;
    data(i+1) += tempi;

    wtemp = wr;
    wr += wr*wpr - wi*wpi;
    wi += wi*wpr + wtemp*wpi;
  }
}

static void scramble(aVector data, unsigned N) {
  int j=1;
  for (int i=1; i<2*N; i+=2) {
    if (j>i) {
      swapad(data(j-1), data(i-1));
      swapad(data(j), data(i));
    }
    int m = N;
    while (m>=2 && j>m) {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
}

static void rescale(aVector data, unsigned N) {
  adouble scale = ((double)1)/N;
  for (unsigned i=0; i<2*N; i++) {
    data[i] *= scale;
  }
}

static void fft(aVector data, unsigned N) {
  scramble(data, N);
  recursiveApply(data,1, N);
}

static void ifft(aVector data, unsigned N) {
  scramble(data, N);
  recursiveApply(data,-1, N);
  rescale(data, N);
}

//! Tapenade
extern "C" {

/*        Generated by TAPENADE     (INRIA, Ecuador team)
    Tapenade 3.15 (master) - 15 Apr 2020 11:54
*/
#include <adBuffer.h>

/*
  Differentiation of recursiveApply in reverse (adjoint) mode (with options context):
   gradient     of useful results: *data
   with respect to varying inputs: *data
   Plus diff mem management of: data:in
*/
static void recursiveApply_b(double *data, double *datab, int iSign, unsigned 
        int N) {
    int arg1;
    double *arg10;
    double *arg10b;
    int arg2;
    if (N != 1) {
        arg1 = N/2;
        arg10b = datab + N;
        arg10 = data + N;
        arg2 = N/2;
        double wtemp = iSign*sin(3.1415926536/N);
        double wpi = -iSign*sin(2*3.1415926536/N);
        double wpr = -2.0*wtemp*wtemp;
        double wr = 1.0;
        double wi = 0.0;
        for (int i = 0; i <= N-1; i += 2) {
            int iN = i + N;
            double tempr = data[iN]*wr - data[iN+1]*wi;
            double tempi = data[iN]*wi + data[iN+1]*wr;
            double tmp;
            double tmp0;
            wtemp = wr;
            pushReal8(wr);
            wr = wr + (wr*wpr - wi*wpi);
            pushReal8(wi);
            wi = wi + (wi*wpr + wtemp*wpi);
            pushInteger4(iN);
        }
        pushPointer8(arg10b);
        pushInteger4(arg2);
        pushInteger4(arg1);
        popInteger4(&arg1);
        popInteger4(&arg2);
        popPointer8((void **)&arg10b);
        for (int i = N-(N-1)%2-1; i >= 0; i -= 2) {
            int iN;
            double tempr;
            double temprb = 0.0;
            double tempi;
            double tempib = 0.0;
            double tmpb;
            double tmpb0;
            popInteger4(&iN);
            tmpb0 = datab[iN + 1];
            popReal8(&wi);
            popReal8(&wr);
            tempib = datab[i + 1] - tmpb0;
            temprb = datab[i];
            datab[iN + 1] = 0.0;
            datab[i + 1] = datab[i + 1] + tmpb0;
            tmpb = datab[iN];
            datab[iN] = 0.0;
            datab[i] = datab[i] + tmpb;
            temprb = temprb - tmpb;
            datab[iN + 1] = datab[iN + 1] + wr*tempib - wi*temprb;
            datab[iN] = datab[iN] + wi*tempib + wr*temprb;
        }
        recursiveApply_b(arg10, arg10b, iSign, arg2);
        recursiveApply_b(data, datab, iSign, arg1);
    }
}

static void recursiveApply_nodiff(double *data, int iSign, unsigned int N) {
    int arg1;
    double *arg10;
    int arg2;
    if (N == 1)
        return;
    else {
        arg1 = N/2;
        recursiveApply_nodiff(data, iSign, arg1);
        arg10 = data + N;
        arg2 = N/2;
        recursiveApply_nodiff(arg10, iSign, arg2);
        double wtemp = iSign*sin(3.1415926536/N);
        double wpi = -iSign*sin(2*3.1415926536/N);
        double wpr = -2.0*wtemp*wtemp;
        double wr = 1.0;
        double wi = 0.0;
        for (int i = 0; i <= N-1; i += 2) {
            int iN = i + N;
            double tempr = data[iN]*wr - data[iN+1]*wi;
            double tempi = data[iN]*wi + data[iN+1]*wr;
            data[iN] = data[i] - tempr;
            data[iN + 1] = data[i + 1] - tempi;
            data[i] += tempr;
            data[i + 1] += tempi;
            wtemp = wr;
            wr += wr*wpr - wi*wpi;
            wi += wi*wpr + wtemp*wpi;
        }
    }
}

/*
  Differentiation of swap in reverse (adjoint) mode (with options context):
   gradient     of useful results: *a *b
   with respect to varying inputs: *a *b
   Plus diff mem management of: a:in b:in
*/
static void swap_b(double *a, double *ab, double *b, double *bb) {
    double temp = *a;
    double tempb = 0.0;
    tempb = *bb;
    *bb = *ab;
    *ab = tempb;
}

static void swap_nodiff(double *a, double *b) {
    double temp = *a;
    *a = *b;
    *b = temp;
}

/*
  Differentiation of scramble in reverse (adjoint) mode (with options context):
   gradient     of useful results: *data
   with respect to varying inputs: *data
   Plus diff mem management of: data:in
*/
static void scramble_b(double *data, double *datab, unsigned int N) {
    int j = 1;
    int branch;
    for (int i = 1; i <= 2*N-1; i += 2) {
        int adCount;
        if (j > i)
            pushControl1b(0);
        else
            pushControl1b(1);
        int m = N;
        adCount = 0;
        while(m >= 2 && j > m) {
            pushInteger4(j);
            j = j - m;
            m = m >> 1;
            adCount = adCount + 1;
        }
        pushInteger4(adCount);
        pushInteger4(j);
        j = j + m;
    }
    for (int i = 2*N-(2*N-2)%2-1; i >= 1; i -= 2) {
        int m;
        int adCount;
        int i0;
        popInteger4(&j);
        popInteger4(&adCount);
        for (i0 = 1; i0 < adCount+1; ++i0)
            popInteger4(&j);
        popControl1b(&branch);
        if (branch == 0) {
            swap_b(&(data[j]), &(datab[j]), &(data[i]), &(datab[i]));
            swap_b(&(data[j - 1]), &(datab[j - 1]), &(data[i - 1]), &(datab[i 
                   - 1]));
        }
    }
}

static void scramble_nodiff(double *data, unsigned int N) {
    int j = 1;
    for (int i = 1; i <= 2*N-1; i += 2) {
        if (j > i) {
            swap_nodiff(&(data[j - 1]), &(data[i - 1]));
            swap_nodiff(&(data[j]), &(data[i]));
        }
        int m = N;
        while(m >= 2 && j > m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
}

/*
  Differentiation of rescale in reverse (adjoint) mode (with options context):
   gradient     of useful results: *data
   with respect to varying inputs: *data
   Plus diff mem management of: data:in
*/
static void rescale_b(double *data, double *datab, unsigned int N) {
    double scale = (double)1/N;
    pushReal8(scale);
    popReal8(&scale);
    for (int i = 2*N-1; i > -1; --i)
        datab[i] = scale*datab[i];
}

static void rescale_nodiff(double *data, unsigned int N) {
    double scale = (double)1/N;
    for (int i = 0; i < 2*N; ++i)
        data[i] *= scale;
}

/*
  Differentiation of fft in reverse (adjoint) mode (with options context):
   gradient     of useful results: *data
   with respect to varying inputs: *data
   Plus diff mem management of: data:in
*/
static void fft_b(double *data, double *datab, unsigned int N) {
    recursiveApply_b(data, datab, 1, N);
    scramble_b(data, datab, N);
}

static void fft_nodiff(double *data, unsigned int N) {
    scramble_nodiff(data, N);
    recursiveApply_nodiff(data, 1, N);
}

/*
  Differentiation of ifft in reverse (adjoint) mode (with options context):
   gradient     of useful results: *data
   with respect to varying inputs: *data
   Plus diff mem management of: data:in
*/
static void ifft_b(double *data, double *datab, unsigned int N) {
    rescale_b(data, datab, N);
    recursiveApply_b(data, datab, -1, N);
    scramble_b(data, datab, N);
}

static void ifft_nodiff(double *data, unsigned int N) {
    scramble_nodiff(data, N);
    recursiveApply_nodiff(data, -1, N);
    rescale_nodiff(data, N);
}

/*
  Differentiation of foobar in reverse (adjoint) mode (with options context):
   gradient     of useful results: *data
   with respect to varying inputs: *data
   RW status of diff variables: *data:in-out
   Plus diff mem management of: data:in
*/
void foobar_b(double *data, double *datab, unsigned int len) {
    double chksum = 0.0;
    int i;
    ifft_b(data, datab, len);
    fft_b(data, datab, len);
}

}


#endif /* _fft_h_ */
