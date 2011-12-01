//Open Tuner: Copyright 2008, 2009 Mark Billington.

//This file is part of "Open Tuner".
//
//"Open Tuner" is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//"Open Tuner" is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with "Open Tuner".  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////////////////////////
// stdafx.cpp : source file that includes just the standard includes
//    OpenTuner.pch will be the pre-compiled header
//    stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
#include <fftw3.h>
#include "_kiss_fft_guts.h"
#include "kiss_fft.h"
#include "kiss_fftr.h"
#include <map>
#include <vector>

static std::vector<kiss_fft_cpx> aR2HCO, aHC2RI;
static std::vector<kiss_fft_scalar > aR2HCI, aHC2RO;
static kiss_fftr_cfg cfgR2HC = kiss_fftr_alloc(3072,0,0, 0);
static kiss_fftr_cfg cfgHC2R = kiss_fftr_alloc(3072,1,0, 0); //###is this why frequencies are wrong? I think these 3072/2048 values are correct, although I don't entirely understand them!


extern "C" fftwf_plan fftwf_plan_r2r_1d(int n, float *in, float *out, fftwf_r2r_kind kind/*FFTW_R2HC or FFTW_HC2R*/, unsigned flags /*FFTW_ESTIMATE*/);
extern "C" void fftwf_execute(const fftwf_plan p);
extern "C" void fftwf_destroy_plan(fftwf_plan p);

fftwf_plan fftwf_plan_r2r_1d(int n, float *in, float *out, fftwf_r2r_kind kind/*FFTW_R2HC or FFTW_HC2R*/, unsigned flags /*FFTW_ESTIMATE*/)
{
    kiss_fft_cpx tmp = {0,0};
    aR2HCO .resize(n,tmp);
    aHC2RI .resize(n,tmp);
    aR2HCI  .resize(n,0);
    aHC2RO  .resize(n,0);

    return 0;
}


void * fftwf_malloc(size_t n){return malloc(n);}


void fftwf_free(void *p){free(p);}


void fftwf_destroy_plan(fftwf_plan p)
{
    if (cfgR2HC) free(cfgR2HC); cfgR2HC = 0;
    if (cfgHC2R) free(cfgHC2R); cfgHC2R = 0;
}


#if FIXED_POINT==32
static long const maxrange = LONG_MAX;
#else
static long const maxrange = SHRT_MAX;/* works fine for float too*/
#endif
static long const maxrangeSHIFT1 = maxrange>>1;

#define sq(x) (x*x) //I'm wondering if the effort of a function call is slowing stuff down; hence a #define?

extern "C" double stdAutoCorr(const kiss_fft_scalar *input, float *output, unsigned long const tSize, unsigned long const pBiscN, unsigned long const tK);
double stdAutoCorr(const kiss_fft_scalar *input, float *output, unsigned long const tSize, unsigned long const pBiscN, unsigned long const tK)
{
    unsigned long const SizeOver2 = tSize/2;

    {for (unsigned long i = 0; i < pBiscN; ++i) //###Slow: but: I need to copy 2048 input elements into 3072 buf
#ifdef FIXED_POINT
        aR2HCI[i] = input[i] * 32768;
#else
        aR2HCI[i] = input[i] / 32768.0f;
#endif
    }

    //Do a forward FFT
    kiss_fftr(cfgR2HC, &aR2HCI[0], &*aR2HCO.begin());

    //calculate the (real*real + ima*imag) for each coefficient
    {for(unsigned int j=1; j<SizeOver2; j++) 
    {
#ifdef FIXED_POINT
        aHC2RI[j].r = ((((__int64)aR2HCO[j].r) * ((__int64)aR2HCO[j].r)) / maxrange) + ((((__int64)aR2HCO[j].i) * ((__int64)aR2HCO[j].i)) / maxrange);
#else
        aHC2RI[j].r = sq(aR2HCO[j].r) + sq(aR2HCO[j].i); //Imag part already zero..
#endif
    }}

    //Square the 1st real element, and the last
#ifdef FIXED_POINT
    aHC2RI[0].r = (maxrangeSHIFT1)*sq(aR2HCO[0].r/ (float)maxrange); //!!!Can probably speed up a tiny bit more by removing FP maths here
    aHC2RI[SizeOver2].r = (maxrangeSHIFT1)*sq(aR2HCO[SizeOver2].r/ (float)maxrange);
#else
    aHC2RI[0].r = sq(aR2HCO[0].r);
    aHC2RI[SizeOver2].r = sq(aR2HCO[SizeOver2].r);
#endif

    //Do an inverse FFT
    kiss_fftri(cfgHC2R, &*aHC2RI.begin(), &*aHC2RO.begin());

    //extract the wanted elements out, and normalise
    float * p1 = 0; kiss_fft_scalar *p2 = 0;
    {for(p1=output, p2=(&*aHC2RO.begin())+1; p1<output+tK;)
    {
        //###Will this be better if I divide the int by 64 then make a fload? Or will i lose too much precision
#ifdef FIXED_POINT
        *p1++ = ((float)*p2++) / 64; //Magic number: began with * 54 or so from s_tmpdMax comparison with float, and then wound down to /= until it worked.
#else
        *p1++ = ((float)*p2++) / tSize;
#endif
    }}

#ifdef FIXED_POINT
    return ((float)aHC2RO[0]) / 64;
#else
    return ((float)aHC2RO[0]) / tSize;
#endif
}
