// 67d7842dbbe25473c3c32b93c0da8047785f30d78e8a024de1b57352245f9689
///*****************************************************************************
// *
// *     Author: Xilinx, Inc.
// *
// *     This text contains proprietary, confidential information of
// *     Xilinx, Inc. , is distributed by under license from Xilinx,
// *     Inc., and may be used, copied and/or disclosed only pursuant to
// *     the terms of a valid license agreement with Xilinx, Inc.
// *
// *     XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS"
// *     AS A COURTESY TO YOU, SOLELY FOR USE IN DEVELOPING PROGRAMS AND
// *     SOLUTIONS FOR XILINX DEVICES.  BY PROVIDING THIS DESIGN, CODE,
// *     OR INFORMATION AS ONE POSSIBLE IMPLEMENTATION OF THIS FEATURE,
// *     APPLICATION OR STANDARD, XILINX IS MAKING NO REPRESENTATION
// *     THAT THIS IMPLEMENTATION IS FREE FROM ANY CLAIMS OF INFRINGEMENT,
// *     AND YOU ARE RESPONSIBLE FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE
// *     FOR YOUR IMPLEMENTATION.  XILINX EXPRESSLY DISCLAIMS ANY
// *     WARRANTY WHATSOEVER WITH RESPECT TO THE ADEQUACY OF THE
// *     IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OR
// *     REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE FROM CLAIMS OF
// *     INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// *     FOR A PARTICULAR PURPOSE.
// *
// *     Xilinx products are not intended for use in life support appliances,
// *     devices, or systems. Use in such applications is expressly prohibited.
// *
// *     (c) Copyright 2012-2013 Xilinx Inc.
// *     All rights reserved.
// *
// *****************************************************************************/
//
//// In recent versions of GCC, the c++ standard headers import the standard library
//// functions (like sin) into the std:: namespace and then import them back into
//// the global namespace. see: https://gcc.gnu.org/ml/libstdc++/2016-01/msg00025.html
//// Basically it looks like:
//// extern double sin(double x); // external declaration
//// namespace std { using ::sin; }
//// using std::sin;
//// Unfortunately, after the second using declaration it's impossible to implement
//// the extern symbol.  It's not clear to me whether this is an oversight in the
//// c++ standard or not.  So we need to define ::sin before including any math.h
//// or cmath headers.  The best solution I see is to forward declare them in yet
//// another namespace and call the forward declarations... then later we can come
//// back and get all the forward declared symbols implemented.
//
//#ifdef _WIN32
//#define NOTHROW
//#endif
//#ifdef __arm__
//#ifdef __linux__
//#define NOTHROW throw()
//#else
//// standalone toolchain doesn't use this.
//#define NOTHROW
//#endif
//#endif
//#ifndef NOTHROW
//#define NOTHROW throw()
//#endif
//
//#ifdef _MSC_VER
//#pragma function(sin, sinf, cos, cosf, atanf, atan2, atan2f, sinh, sinhf, cosh, coshf, tan, tanf)
//#pragma function(floor, floorf, ceil, ceilf)
//#pragma function(log, logf, sqrt, sqrtf, cbrt, cbrtf, exp, expf, log10, log10f, pow, powf)
//#pragma function(nextafter, nextafterf)
//#pragma function(fmod, fmodf, remainder, remainderf, remquo, remquof)
//#endif
//
//namespace hlstmp {
//    static double sin(double x);
//    static double cos(double x);
//    static double sinh(double x);
//    static double cosh(double x);
//    static double atan(double x);
//    static double atan2(double y, double x);
//    static double tan(double x);
//    static double floor(double x);
//    static double ceil(double x);
//#ifndef _MSC_VER
//    static double modf(double x, double *intpart);
//    static double frexp (double x, int* exp);
//    static double ldexp (double x, int exp);
//#endif
//    static double log10(double x);
//    static double log2(double x);
//    static double logb(double x);
//    static double log1p(double x);
////    static double exp10(double x);
//    static double exp2(double x);
//    static double expm1(double x);
//    static double scalbn(double x, int n);
//    static double scalbln(double x, long int n);
//    static int    ilogb(double x);
//    static double fma(double x, double y, double z);
//}
//extern "C" {
//    double sin(double x) NOTHROW {
//        return hlstmp::sin(x);
//    }
//    double cos(double x) NOTHROW {
//        return hlstmp::cos(x);
//    }
//    double sinh(double x) NOTHROW {
//        return hlstmp::sinh(x);
//    }
//    double cosh(double x) NOTHROW {
//        return hlstmp::cosh(x);
//    }
//    double atan(double x) NOTHROW {
//        return hlstmp::atan(x);
//    }
//    double atan2(double y, double x) NOTHROW {
//        return hlstmp::atan2(y, x);
//    }
//    double tan(double x) NOTHROW {
//        return hlstmp::tan(x);
//    }
//    double floor(double x) NOTHROW {
//        return hlstmp::floor(x);
//    }
//    double ceil(double x) NOTHROW {
//        return hlstmp::ceil(x);
//    }
//#ifndef _MSC_VER
//    double modf(double x, double *intpart) NOTHROW {
//        return hlstmp::modf(x, intpart);
//    }
//    double frexp (double x, int* exp) NOTHROW {
//        return hlstmp::frexp(x, exp);
//    }
//    double ldexp (double x, int exp) NOTHROW {
//        return hlstmp::ldexp(x, exp);
//    }
//#endif
//    double log10(double x) NOTHROW {
//        return hlstmp::log10(x);
//    }
//    double log2(double x) NOTHROW {
//        return hlstmp::log2(x);
//    }
//    double logb(double x) NOTHROW {
//        return hlstmp::logb(x);
//    }
//    double log1p(double x) NOTHROW {
//	return hlstmp::log1p(x);
//    }
////    double exp10(double x) NOTHROW {
////        return hlstmp::exp10(x);
////    }
//    double exp2(double x) NOTHROW {
//        return hlstmp::exp2(x);
//    }
//    double expm1(double x) NOTHROW {
//	return hlstmp::expm1(x);
//    }
//    double scalbn(double x, int n) NOTHROW {
//	return hlstmp::scalbn(x,n);
//    }
//    double scalbln(double x, long int n) NOTHROW {
//	return hlstmp::scalbln(x,n);
//    }
//    int ilogb(double x) NOTHROW {
//	return hlstmp::ilogb(x);
//    }
//    double fma(double x, double y, double z) NOTHROW {
//	return hlstmp::fma(x,y,z);
//    }
//}
//
//#include <cmath>
//#include "hlstmp_math.h"
//
//extern "C" {
//
//float sinf(float x) {
//    return hlstmp::sinf(x);
//}
//
////float sinpif(float x) {
////    return hlstmp::sinpif(x);
////}
////
//float cosf(float x) {
//    return hlstmp::cosf(x);
//}
//
////float cospif(float x) {
////    return hlstmp::cospif(x);
////}
////
//float atanf(float x) {
//    return hlstmp::atanf(x);
//}
//
//float atan2f(float y, float x) {
//    return hlstmp::atan2f(y, x);
//}
//
//float sinhf(float x) {
//	return hlstmp::sinhf(x);
//}
//
//float coshf(float x) {
//	return hlstmp::coshf(x);
//}
//
////void sincos(double x, double *sin, double *cos) {
////    hlstmp::sincos(x, sin, cos);
////}
////
////void sincosf(float x, float *sin, float *cos) {
////    hlstmp::sincosf(x, sin, cos);
////}
////
//float tanf(float x) {
//    return hlstmp::tanf(x);
//}
//
//double copysign(double x, double y) {
//    return hlstmp::copysign(x, y);
//}
//
//float copysignf(float x, float y) {
//    return hlstmp::copysignf(x, y);
//}
//
//double fmax(double x, double y) {
//    return hlstmp::fmax(x, y);
//}
//
//float fmaxf(float x, float y) {
//    return hlstmp::fmaxf(x, y);
//}
//
//double fmin(double x, double y) {
//    return hlstmp::fmin(x, y);
//}
//
//float fminf(float x, float y) {
//    return hlstmp::fminf(x, y);
//}
//
//double fdim(double x, double y) {
//    return hlstmp::fdim(x, y);
//}
//
//float fdimf(float x, float y) {
//    return hlstmp::fdimf(x, y);
//}
//
////double maxmag(double x, double y) {
////    return hlstmp::maxmag(x, y);
////}
////
////float maxmagf(float x, float y) {
////    return hlstmp::maxmag(x, y);
////}
////
////double minmag(double x, double y) {
////    return hlstmp::minmag(x, y);
////}
////
////float minmagf(float x, float y) {
////    return hlstmp::minmag(x, y);
////}
////
//double fabs(double x){
//    return hlstmp::fabs(x);
//}
//
//float fabsf(float x) {
//    return hlstmp::fabsf(x);
//}
//float absf(float x) {
//    return hlstmp::absf(x);
//}
//
//float floorf(float x) {
//    return hlstmp::floorf(x);
//}
//
//float ceilf(float x) {
//    return hlstmp::ceilf(x);
//}
//
//double trunc(double x) {
//    return hlstmp::trunc(x);
//}
//
//float truncf(float x) {
//    return hlstmp::truncf(x);
//}
//
//double round(double x) {
//    return hlstmp::round(x);
//}
//
//float roundf(float x) {
//    return hlstmp::roundf(x);
//}
//
//double nearbyint(double x) {
//    return hlstmp::nearbyint(x);
//}
//
//float nearbyintf(float x) {
//    return hlstmp::nearbyintf(x);
//}
//
//double rint(double x) {
//    return hlstmp::rint(x);
//}
//
//float rintf(float x) {
//    return hlstmp::rintf(x);
//}
//
//long int lrint(double x) {
//    return hlstmp::lrint(x);
//}
//
//long long int llrint(double x) {
//    return hlstmp::llrint(x);
//}
//
//long int lrintf(float x) {
//    return hlstmp::lrintf(x);
//}
//
//long long int llrintf(float x) {
//    return hlstmp::llrintf(x);
//}
//
//long int lround(double x) {
//    return hlstmp::lround(x);
//}
//
//long long int llround(double x) {
//    return hlstmp::llround(x);
//}
//
//long int lroundf(float x) {
//    return hlstmp::lroundf(x);
//}
//
//long long int llroundf(float x) {
//    return hlstmp::llroundf(x);
//}
//
//#ifndef _MSC_VER
//float modff(float x, float *intpart) NOTHROW {
//    return hlstmp::modff(x, intpart);
//}
//
////double fract(double x, double *intpart) NOTHROW {
////    return hlstmp::fract(x, intpart);
////}
////
////float fractf(float x, float *intpart) NOTHROW {
////    return hlstmp::fractf(x, intpart);
////}
////
//float frexpf (float x, int* exp) {
//    return hlstmp::frexpf(x, exp);
//}
//
//float ldexpf (float x, int exp) {
//    return hlstmp::ldexpf(x, exp);
//}
//
//#endif
//
//#if (defined AESL_SYN || defined HLS_NO_XIL_FPO_LIB)
//// These functions are now implemented by libm in simulation.
//// These functions are now implemented by intrinsic during synthesis.
//#else
//double sqrt(double x) {
//    return hlstmp::sqrt(x);
//}
//
//float sqrtf(float x) {
//    return hlstmp::sqrtf(x);
//}
//double log(double x) {
//    return hlstmp::log(x);
//}
//float logf(float x) {
//    return hlstmp::logf(x);
//}
//double exp(double x) {
//    return hlstmp::exp(x);
//}
//#endif
//float expf(float x) {
//    return hlstmp::expf(x);
//}
////#endif
//
//double cbrt(double x) {
//    return hlstmp::cbrt(x);
//}
//float cbrtf(float x) {
//    return hlstmp::cbrtf(x);
//}
//
//double hypot(double x, double y) {
//    return hlstmp::hypot(x,y);
//}
//float hypotf(float x, float y) {
//    return hlstmp::hypotf(x,y);
//}
//
////float exp10f(float x) {
////    return hlstmp::exp10f(x);
////}
////
//float exp2f(float x) {
//    return hlstmp::exp2f(x);
//}
//
//float expm1f(float x) {
//    return hlstmp::expm1f(x);
//}
//
//float log10f(float x) {
//    return hlstmp::log10f(x);
//}
//
//float log2f(float x) {
//    return hlstmp::log2f(x);
//}
//
//float logbf(float x) {
//    return hlstmp::logbf(x);
//}
//
//float log1pf(float x) {
//    return hlstmp::log1pf(x);
//}
//
////double recip(double x) {
////    return hlstmp::recip(x);
////}
////
////float recipf(float x) {
////    return hlstmp::recipf(x);
////}
////
////double rsqrt(double x) {
////    return hlstmp::rsqrt(x);
////}
////
////float rsqrtf(float x) {
////    return hlstmp::rsqrtf(x);
////}
////
//double pow(double x, double y) {
//    return hlstmp::pow(x,y);
//}
//float powf(float x, float y) {
//    return hlstmp::pow(x,y);
//}
//
////double powr(double x, double y) {
////    return hlstmp::powr(x,y);
////}
////float powrf(float x, float y) {
////    return hlstmp::powr(x,y);
////}
////
////double pown(double x, int y) {
////    return hlstmp::pown(x,y);
////}
////float pownf(float x, int y) {
////    return hlstmp::pown(x,y);
////}
////
////double rootn(double x, int y) {
////    return hlstmp::rootn(x,y);
////}
////float rootnf(float x, int y) {
////    return hlstmp::rootn(x,y);
////}
////
//int __signbitf(float t_in) {
//    return hlstmp::__signbit(t_in);
//}
//
//int __signbit(double t_in) {
//    return hlstmp::__signbit(t_in);
//}
//
//int __finitef(float t_in) {
//	return hlstmp::__isfinite(t_in);
//}
//
//int __finite(double t_in) {
//	return hlstmp::__isfinite(t_in);
//}
//
//int __isinff(float t_in) {
//	return hlstmp::__isinf(t_in);
//}
//
//int __isinf(double t_in) {
//	return hlstmp::__isinf(t_in);
//}
//#if __GNUC__ < 6
//int isinf(double t_in) {
//	return hlstmp::__isinf(t_in);
//}
//#endif
//
//int __isnanf(float t_in) {
//	return hlstmp::__isnan(t_in);
//}
//
//int __isnan(double t_in) {
//	return hlstmp::__isnan(t_in);
//}
//
//#if __GNUC__ < 6
//int isnan(double t_in) {
//	return hlstmp::__isnan(t_in);
//}
//#endif
//
//int __isnormalf(float t_in) {
//	return hlstmp::__isnormal(t_in);
//}
//
//int __isnormal(double t_in) {
//	return hlstmp::__isnormal(t_in);
//}
//
//int __fpclassifyf(float t_in) NOTHROW {
//	return hlstmp::__fpclassifyf(t_in);
//}
//
//int __fpclassify(double t_in) NOTHROW {
//	return hlstmp::__fpclassify(t_in);
//}
//
//// int __fpclassify(half t_in) NOTHROW {
//// 	return hlstmp::__fpclassify(t_in);
//// }
//
//double nan(const char *p) {
//    return hlstmp::nan(p);
//}
//
//float nanf(const char *p) {
//    return hlstmp::nanf(p);
//}
//
//float scalbnf(float x, int n) {
//    return hlstmp::scalbnf(x,n);
//}
//
//float scalblnf(float x, long n) {
//    return hlstmp::scalblnf(x,n);
//}
//
//int ilogbf(float x) {
//    return hlstmp::ilogbf(x);
//}
//
//float fmaf(float x, float y, float z) {
//    return hlstmp::fmaf(x,y,z);
//}
//
////double mad(double x, double y, double z) {
////    return hlstmp::mad(x,y,z);
////}
////
////float madf(float x, float y, float z) {
////    return hlstmp::madf(x,y,z);
////}
////
//double erf(double x)
//{
//    return hlstmp::erf(x);
//}
//
//double erfc(double x)
//{
//    return hlstmp::erfc(x);
//}
//
//float erff(float x)
//{
//    return hlstmp::erff(x);
//}
//
//float erfcf(float x)
//{
//    return hlstmp::erfcf(x);
//}
//
//double asin(double x)
//{
//    return hlstmp::asin(x);
//}
//double acos(double x)
//{
//    return hlstmp::acos(x);
//}
//float asinf(float x)
//{
//    return hlstmp::asinf(x);
//}
//float acosf(float x)
//{
//    return hlstmp::acosf(x);
//}
//
//double nextafter(double x, double y) {
//    return hlstmp::nextafter(x,y);
//}
//
//float nextafterf(float x, float y) {
//    return hlstmp::nextafter(x,y);
//}
//
//double fmod(double x, double y) {
//    return hlstmp::fmod(x,y);
//}
//
//float fmodf(float x, float y) {
//    return hlstmp::fmod(x,y);
//}
//
//double remainder(double x, double y) {
//    return hlstmp::remainder(x,y);
//}
//
//float remainderf(float x, float y) {
//    return hlstmp::remainder(x,y);
//}
//
//double remquo(double x, double y, int* quo) {
//    return hlstmp::remquo(x,y,quo);
//}
//
//float remquof(float x, float y, int* quo) {
//    return hlstmp::remquo(x,y,quo);
//}
//
//double tanh(double x){
//    return hlstmp::tanh(x);
//}
//
//float tanhf(float x){
//    return hlstmp::tanh(x);
//}
//
//double atanh(double x){
//    return hlstmp::atanh(x);
//}
//
//float atanhf(float x){
//    return hlstmp::atanh(x);
//}
//
//double asinh(double x){
//    return hlstmp::asinh(x);
//}
//
//float asinhf(float x){
//    return hlstmp::asinh(x);
//} 
//
//double acosh(double x){
//    return hlstmp::acosh(x);
//}
//
//float acoshf(float x){
//    return hlstmp::acosh(x);
//} 
//
//
//} // extern "C"
//
//// half functions need c++ linkage
//half half_nan(const char *tagp) {
//    return hlstmp::half_nan(tagp);
//}
//half half_atan(half t) {
//    return hlstmp::half_atan(t);
//}
//half half_atan2(half y, half x) {
//    return hlstmp::half_atan2(y, x);
//}
//half half_copysign(half x, half y) {
//    return hlstmp::half_copysign(x, y);
//}
//half half_fabs(half x) {
//    return hlstmp::half_fabs(x);
//}
//half half_abs(half x) {
//    return hlstmp::half_abs(x);
//}
//half half_frexp(half x, int* exp) {
//    return hlstmp::half_frexp(x, exp);
//}
//half half_ldexp(half x, int exp) {
//    return hlstmp::half_ldexp(x, exp);
//}
//half half_fmax(half x, half y) {
//    return hlstmp::half_fmax(x,y);
//}
//half half_fmin(half x, half y) {
//    return hlstmp::half_fmin(x,y);
//}
//half half_fdim(half x, half y) {
//    return hlstmp::half_fdim(x,y);
//}
////half half_maxmag(half x, half y) {
////    return hlstmp::half_maxmag(x,y);
////}
////half half_minmag(half x, half y) {
////    return hlstmp::half_minmag(x,y);
////}
//half half_sin(half t_in) {
//    return hlstmp::half_sin(t_in);
//}
//half half_cos(half t_in) {
//    return hlstmp::half_cos(t_in);
//}
////void half_sincos(half x, half *sin, half *cos) {
////    hlstmp::half_sincos(x, sin, cos);
////}
//half half_sinh(half t_in) {
//    return hlstmp::half_sinh(t_in);
//}
//half half_cosh(half t_in) {
//    return hlstmp::half_cosh(t_in);
//}
////half half_sinpi(half t_in) {
////    return hlstmp::half_sinpi(t_in);
////}
////half half_cospi(half t_in) {
////    return hlstmp::half_cospi(t_in);
////}
////half half_recip(half x) {
////    return hlstmp::half_recip(x);
////}
//half half_sqrt(half x) {
//    return hlstmp::half_sqrt(x);
//}
////half half_rsqrt(half x) {
////    return hlstmp::half_rsqrt(x);
////}
//half half_cbrt(half x) {
//    return hlstmp::half_cbrt(x);
//}
//half half_hypot(half x, half y) {
//    return hlstmp::half_hypot(x,y);
//}
//half half_log(half x) {
//    return hlstmp::half_log(x);
//}
//half half_log10(half x) {
//    return hlstmp::half_log10(x);
//}
//half half_log2(half x) {
//    return hlstmp::half_log2(x);
//}
//half half_logb(half x) {
//    return hlstmp::half_logb(x);
//}
//half half_log1p(half x) {
//    return hlstmp::half_log1p(x);
//}
//half half_exp(half x) {
//    return hlstmp::half_exp(x);
//}
////half half_exp10(half x) {
////    return hlstmp::half_exp10(x);
////}
//half half_exp2(half x) {
//    return hlstmp::half_exp2(x);
//}
//half half_expm1(half x) {
//    return hlstmp::half_expm1(x);
//}
//half half_floor(half x) {
//    return hlstmp::half_floor(x);
//}
////half floor(half x)
//half half_ceil(half x) {
//    return hlstmp::half_ceil(x);
//}
////half ceil(half x)
//half half_trunc(half x) {
//    return hlstmp::half_trunc(x);
//}
//// half trunc(half x)
//half half_round(half x) {
//    return hlstmp::half_round(x);
//}
////half round(half x)
//half half_nearbyint(half x) {
//    return hlstmp::half_nearbyint(x);
//}
//half half_rint(half x) {
//    return hlstmp::half_rint(x);
//}
//long int half_lrint(half x) {
//    return hlstmp::half_lrint(x);
//}
//long long int half_llrint(half x) {
//    return hlstmp::half_llrint(x);
//}
//long int half_lround(half x) {
//    return hlstmp::half_lround(x);
//}
//long long int half_llround(half x) {
//    return hlstmp::half_llround(x);
//}
//half half_modf(half x, half *intpart) {
//    return hlstmp::half_modf(x, intpart);
//}
//// half modf(half x, half *intpart)
////half half_fract(half x, half *intpart) {
////    return hlstmp::half_fract(x, intpart);
////}
//half half_pow(half x, half y) {
//    return hlstmp::half_pow(x,y);
//}
////half half_powr(half x, half y) {
////    return hlstmp::half_powr(x,y);
////}
////half half_pown(half x, int y) {
////    return hlstmp::half_pown(x,y);
////}
////half half_rootn(half x, int y) {
////    return hlstmp::half_rootn(x,y);
////}
//half half_scalbn(half x, int n) {
//    return hlstmp::half_scalbn(x,n);
//}
//half half_scalbln(half x, long int n) {
//    return hlstmp::half_scalbln(x,n);
//}
//int half_ilogb(half x) {
//    return hlstmp::half_ilogb(x);
//}
//half half_fma(half x, half y, half z) {
//    return hlstmp::half_fma(x,y,z);
//}
////half half_mad(half x, half y, half z) {
////    return hlstmp::half_mad(x,y,z);
////}
//half half_asin(half x)
//{
//    return hlstmp::half_asin(x);
//}
//half half_acos(half x)
//{
//    return hlstmp::half_acos(x);
//}
//
//half half_nextafter(half x, half y) {
//    return hlstmp::half_nextafter(x,y);
//}
//
//half half_fmod(half x, half y) {
//    return hlstmp::half_fmod(x,y);
//}
//
//half half_remainder(half x, half y) {
//    return hlstmp::half_remainder(x,y);
//}
//
//half half_remquo(half x, half y, int* quo) {
//    return hlstmp::half_remquo(x,y,quo);
//}
//
//half half_tanh(half x){
//    return hlstmp::tanh(x);
//}
//
//half half_atanh(half x){
//    return hlstmp::atanh(x);
//}
//
//half half_asinh(half x){
//    return hlstmp::asinh(x);
//}
//
//half half_acosh(half x){
//    return hlstmp::acosh(x);
//}
//
//half half_tan(half x){
//    return hlstmp::tan(x);
//}

