#include "soft-fp.h"
#include "double.h"
#include "single.h"

//strong_alias(__eqdf2, __nedf2);
//strong_alias(__eqsf2, __nesf2);
//strong_alias(__lesf2, __ltsf2);
//strong_alias(__gesf2, __gtsf2);
//strong_alias(__gedf2, __gtdf2);
//strong_alias(__ledf2, __ltdf2);

typedef int word_type __attribute__ ((mode (__word__)));

struct DWstruct {
	int low, high;
};

typedef union {
	struct DWstruct s;
	long long ll;
} DWunion;

long long __ashldi3(long long u, word_type b)
{
  DWunion uu, w;
  word_type bm;

  if (b == 0)
          return u;

  uu.ll = u;
  bm = 32 - b;

  if (bm <= 0) {
    w.s.low = 0;
    w.s.high = (unsigned int) uu.s.low << -bm;
  } else {
    const unsigned int carries = (unsigned int) uu.s.low >> bm;

    w.s.low = (unsigned int) uu.s.low << b;
    w.s.high = ((unsigned int) uu.s.high << b) | carries;
  }

  return w.ll;
}

long long __lshrdi3(long long u, word_type b)
{
	DWunion uu, w;
	word_type bm;
	if (b == 0)
		return u;
	uu.ll = u;
	bm = 32 - b;
	if (bm <= 0) {
		w.s.high = 0;
		w.s.low = (unsigned int) uu.s.high >> -bm;
	} else {
		const unsigned int carries = (unsigned int) uu.s.high << bm;
		w.s.high = (unsigned int) uu.s.high >> b;
		w.s.low = ((unsigned int) uu.s.low >> b) | carries;
	}
	return w.ll;
}

DFtype __adddf3(DFtype a, DFtype b)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(B); FP_DECL_D(R);
  DFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_SEMIRAW_D(A, a);
  FP_UNPACK_SEMIRAW_D(B, b);
  FP_ADD_D(R, A, B);
  FP_PACK_SEMIRAW_D(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

SFtype __addsf3(SFtype a, SFtype b)
{
  FP_DECL_EX;
  FP_DECL_S(A); FP_DECL_S(B); FP_DECL_S(R);
  SFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_SEMIRAW_S(A, a);
  FP_UNPACK_SEMIRAW_S(B, b);
  FP_ADD_S(R, A, B);
  FP_PACK_SEMIRAW_S(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

// Returns: the number of leading 0-bits
// Precondition: a != 0
unsigned int __clzsi2(unsigned int a)
{
    unsigned int x = a;
    int t = ((x & 0xFFFF0000) == 0) << 4;  // if (x is small) t = 16 else 0
    x >>= 16 - t;      // x = [0 - 0xFFFF]
    unsigned int r = t;       // r = [0, 16]
    // return r + clz(x)
    t = ((x & 0xFF00) == 0) << 3;
    x >>= 8 - t;       // x = [0 - 0xFF]
    r += t;            // r = [0, 8, 16, 24]
    // return r + clz(x)
    t = ((x & 0xF0) == 0) << 2;
    x >>= 4 - t;       // x = [0 - 0xF]
    r += t;            // r = [0, 4, 8, 12, 16, 20, 24, 28]
    // return r + clz(x)
    t = ((x & 0xC) == 0) << 1;
    x >>= 2 - t;       // x = [0 - 3]
    r += t;            // r = [0 - 30] and is even
    // return r + clz(x)
//     switch (x)
//     {
//     case 0:
//         return r + 2;
//     case 1:
//         return r + 1;
//     case 2:
//     case 3:
//         return r;
//     }
    return r + ((2 - x) & -((x & 2) == 0));
}

DFtype __divdf3(DFtype a, DFtype b)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(B); FP_DECL_D(R);
  DFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_D(A, a);
  FP_UNPACK_D(B, b);
  FP_DIV_D(R, A, B);
  FP_PACK_D(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

SFtype __divsf3(SFtype a, SFtype b)
{
  FP_DECL_EX;
  FP_DECL_S(A); FP_DECL_S(B); FP_DECL_S(R);
  SFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_S(A, a);
  FP_UNPACK_S(B, b);
  FP_DIV_S(R, A, B);
  FP_PACK_S(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

CMPtype __eqdf2(DFtype a, DFtype b)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(B);
  CMPtype r;

  FP_UNPACK_RAW_D(A, a);
  FP_UNPACK_RAW_D(B, b);
  FP_CMP_EQ_D(r, A, B);
  if (r && (FP_ISSIGNAN_D(A) || FP_ISSIGNAN_D(B)))
    FP_SET_EXCEPTION(FP_EX_INVALID);
  FP_HANDLE_EXCEPTIONS;

  return r;
}


CMPtype __eqsf2(SFtype a, SFtype b)
{
  FP_DECL_EX;
  FP_DECL_S(A); FP_DECL_S(B);
  CMPtype r;

  FP_UNPACK_RAW_S(A, a);
  FP_UNPACK_RAW_S(B, b);
  FP_CMP_EQ_S(r, A, B);
  if (r && (FP_ISSIGNAN_S(A) || FP_ISSIGNAN_S(B)))
    FP_SET_EXCEPTION(FP_EX_INVALID);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

/*   
   Return a converted to IEEE double
       */

DFtype __extendsfdf2(SFtype a)
{
  FP_DECL_EX;
  FP_DECL_S(A);
  FP_DECL_D(R);
  DFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_RAW_S(A, a);
#if _FP_W_TYPE_SIZE < _FP_FRACBITS_D
  FP_EXTEND(D,S,2,1,R,A);
#else
  FP_EXTEND(D,S,1,1,R,A);
#endif
  FP_PACK_RAW_D(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Convert a to 64bit signed integer
    
       */

DItype __fixdfdi(DFtype a)
{
  FP_DECL_EX;
  FP_DECL_D(A);
  UDItype r;

  FP_UNPACK_RAW_D(A, a);
  FP_TO_INT_D(r, A, DI_BITS, 1);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Convert a to 32bit signed integer
     
     
     
     
     
       */


SItype __fixdfsi(DFtype a)
{
  FP_DECL_EX;
  FP_DECL_D(A);
  USItype r;

  FP_UNPACK_RAW_D(A, a);
  FP_TO_INT_D(r, A, SI_BITS, 1);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Convert a to 64bit signed integer
     
       */

DItype __fixsfdi(SFtype a)
{
  FP_DECL_EX;
  FP_DECL_S(A);
  UDItype r;

  FP_UNPACK_RAW_S(A, a);
  FP_TO_INT_S(r, A, DI_BITS, 1);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Convert a to 32bit signed integer
     
     
       */

SItype __fixsfsi(SFtype a)
{
  FP_DECL_EX;
  FP_DECL_S(A);
  USItype r;

  FP_UNPACK_RAW_S(A, a);
  FP_TO_INT_S(r, A, SI_BITS, 1);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

/*   
   Convert a to 64bit unsigned integer

       */

UDItype __fixunsdfdi(DFtype a)
{
  FP_DECL_EX;
  FP_DECL_D(A);
  UDItype r;

  FP_UNPACK_RAW_D(A, a);
  FP_TO_INT_D(r, A, DI_BITS, 0);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Convert a to 32bit unsigned integer
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

USItype __fixunsdfsi(DFtype a)
{
  FP_DECL_EX;
  FP_DECL_D(A);
  USItype r;

  FP_UNPACK_RAW_D(A, a);
  FP_TO_INT_D(r, A, SI_BITS, 0);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Convert a to 64bit unsigned integer
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

UDItype __fixunssfdi(SFtype a)
{
  FP_DECL_EX;
  FP_DECL_S(A);
  UDItype r;

  FP_UNPACK_RAW_S(A, a);
  FP_TO_INT_S(r, A, DI_BITS, 0);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Convert a to 32bit unsigned integer
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

USItype __fixunssfsi(SFtype a)
{
  FP_DECL_EX;
  FP_DECL_S(A);
  USItype r;

  FP_UNPACK_RAW_S(A, a);
  FP_TO_INT_S(r, A, SI_BITS, 0);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Convert a 64bit signed integer to IEEE double
     
     
     
       */

  
  

DFtype __floatdidf(DItype i)
{
  FP_DECL_EX;
  FP_DECL_D(A);
  DFtype a;

  FP_FROM_INT_D(A, i, DI_BITS, UDItype);
  FP_PACK_RAW_D(a, A);
  FP_HANDLE_EXCEPTIONS;

  return a;
}
/*   
   Convert a 64bit signed integer to IEEE single
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

SFtype __floatdisf(DItype i)
{
  FP_DECL_EX;
  FP_DECL_S(A);
  SFtype a;

  FP_FROM_INT_S(A, i, DI_BITS, UDItype);
  FP_PACK_RAW_S(a, A);
  FP_HANDLE_EXCEPTIONS;

  return a;
}
/*   
   Convert a 32bit signed integer to IEEE double
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

DFtype __floatsidf(SItype i)
{
  FP_DECL_EX;
  FP_DECL_D(A);
  DFtype a;

  FP_FROM_INT_D(A, i, SI_BITS, USItype);
  FP_PACK_RAW_D(a, A);
  FP_HANDLE_EXCEPTIONS;

  return a;
}
/*   
   Convert a 32bit signed integer to IEEE single
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

SFtype __floatsisf(SItype i)
{
  FP_DECL_EX;
  FP_DECL_S(A);
  SFtype a;

  FP_FROM_INT_S(A, i, SI_BITS, USItype);
  FP_PACK_RAW_S(a, A);
  FP_HANDLE_EXCEPTIONS;

  return a;
}
/*   
   Convert a 64bit unsigned integer to IEEE double
   Copyright (C) 1997, 1999, 2006, 2007 Free Software Foundation, Inc.
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

DFtype __floatundidf(UDItype i)
{
  FP_DECL_EX;
  FP_DECL_D(A);
  DFtype a;

  FP_FROM_INT_D(A, i, DI_BITS, UDItype);
  FP_PACK_RAW_D(a, A);
  FP_HANDLE_EXCEPTIONS;

  return a;
}
/*   
   Convert a 64bit unsigned integer to IEEE single
   Copyright (C) 1997, 1999, 2006, 2007 Free Software Foundation, Inc.
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

SFtype __floatundisf(UDItype i)
{
  FP_DECL_EX;
  FP_DECL_S(A);
  SFtype a;

  FP_FROM_INT_S(A, i, DI_BITS, UDItype);
  FP_PACK_RAW_S(a, A);
  FP_HANDLE_EXCEPTIONS;

  return a;
}
/*   
   Convert a 32bit unsigned integer to IEEE double
   Copyright (C) 1997, 1999, 2006, 2007 Free Software Foundation, Inc.
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

DFtype __floatunsidf(USItype i)
{
  FP_DECL_EX;
  FP_DECL_D(A);
  DFtype a;

  FP_FROM_INT_D(A, i, SI_BITS, USItype);
  FP_PACK_RAW_D(a, A);
  FP_HANDLE_EXCEPTIONS;

  return a;
}
/*   
   Convert a 32bit unsigned integer to IEEE single
   Copyright (C) 1997, 1999, 2006, 2007 Free Software Foundation, Inc.
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

SFtype __floatunsisf(USItype i)
{
  FP_DECL_EX;
  FP_DECL_S(A);
  SFtype a;

  FP_FROM_INT_S(A, i, SI_BITS, USItype);
  FP_PACK_RAW_S(a, A);
  FP_HANDLE_EXCEPTIONS;

  return a;
}
/*   
   Return 0 iff a == b, 1 iff a > b, -2 iff a ? b, -1 iff a < b
   Copyright (C) 1997,1999,2006,2007 Free Software Foundation, Inc.
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

CMPtype __gedf2(DFtype a, DFtype b)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(B);
  CMPtype r;

  FP_UNPACK_RAW_D(A, a);
  FP_UNPACK_RAW_D(B, b);
  FP_CMP_D(r, A, B, -2);
  if (r == -2 && (FP_ISSIGNAN_D(A) || FP_ISSIGNAN_D(B)))
    FP_SET_EXCEPTION(FP_EX_INVALID);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

/*   
   Return 0 iff a == b, 1 iff a > b, -2 iff a ? b, -1 iff a < b
       */

  
  

CMPtype __gesf2(SFtype a, SFtype b)
{
  FP_DECL_EX;
  FP_DECL_S(A); FP_DECL_S(B);
  CMPtype r;

  FP_UNPACK_RAW_S(A, a);
  FP_UNPACK_RAW_S(B, b);
  FP_CMP_S(r, A, B, -2);
  if (r == -2 && (FP_ISSIGNAN_S(A) || FP_ISSIGNAN_S(B)))
    FP_SET_EXCEPTION(FP_EX_INVALID);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

/*   
   Return 0 iff a == b, 1 iff a > b, 2 iff a ? b, -1 iff a < b
     
     
       */

  
  

CMPtype __ltdf2(DFtype a, DFtype b)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(B);
  CMPtype r;

  FP_UNPACK_RAW_D(A, a);
  FP_UNPACK_RAW_D(B, b);
  FP_CMP_D(r, A, B, 2);
  if (r == 2 && (FP_ISSIGNAN_D(A) || FP_ISSIGNAN_D(B)))
    FP_SET_EXCEPTION(FP_EX_INVALID);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
//==
CMPtype __ledf2(DFtype a, DFtype b)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(B);
  CMPtype r;

  FP_UNPACK_RAW_D(A, a);
  FP_UNPACK_RAW_D(B, b);
  FP_CMP_D(r, A, B, 2);
  if (r == 2 && (FP_ISSIGNAN_D(A) || FP_ISSIGNAN_D(B)))
    FP_SET_EXCEPTION(FP_EX_INVALID);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

/*   
   Return 0 iff a == b, 1 iff a > b, 2 iff a ? b, -1 iff a < b
   Copyright (C) 1997,1999,2006,2007 Free Software Foundation, Inc.
     
     
       */

  
  

CMPtype __lesf2(SFtype a, SFtype b)
{
  FP_DECL_EX;
  FP_DECL_S(A); FP_DECL_S(B);
  CMPtype r;

  FP_UNPACK_RAW_S(A, a);
  FP_UNPACK_RAW_S(B, b);
  FP_CMP_S(r, A, B, 2);
  if (r == 2 && (FP_ISSIGNAN_S(A) || FP_ISSIGNAN_S(B)))
    FP_SET_EXCEPTION(FP_EX_INVALID);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

/*   
   Return a * b

     
     
       */

  
  

DFtype __muldf3(DFtype a, DFtype b)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(B); FP_DECL_D(R);
  DFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_D(A, a);
  FP_UNPACK_D(B, b);
  FP_MUL_D(R, A, B);
  FP_PACK_D(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Return a * b
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

SFtype __mulsf3(SFtype a, SFtype b)
{
  FP_DECL_EX;
  FP_DECL_S(A); FP_DECL_S(B); FP_DECL_S(R);
  SFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_S(A, a);
  FP_UNPACK_S(B, b);
  FP_MUL_S(R, A, B);
  FP_PACK_S(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Return -a
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

DFtype __negdf2(DFtype a)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(R);
  DFtype r;

  FP_UNPACK_D(A, a);
  FP_NEG_D(R, A);
  FP_PACK_D(r, R);
  FP_CLEAR_EXCEPTIONS;
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Return -a
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

SFtype __negsf2(SFtype a)
{
  FP_DECL_EX;
  FP_DECL_S(A); FP_DECL_S(R);
  SFtype r;

  FP_UNPACK_S(A, a);
  FP_NEG_S(R, A);
  FP_PACK_S(r, R);
  FP_CLEAR_EXCEPTIONS;
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Return sqrt(a)
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

DFtype __sqrtdf2(DFtype a)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(R);
  DFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_D(A, a);
  FP_SQRT_D(R, A);
  FP_PACK_D(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Return sqrt(a)
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

SFtype __sqrtsf2(SFtype a)
{
  FP_DECL_EX;
  FP_DECL_S(A); FP_DECL_S(R);
  SFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_S(A, a);
  FP_SQRT_S(R, A);
  FP_PACK_S(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Return a - b
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

DFtype __subdf3(DFtype a, DFtype b)
{
  FP_DECL_EX;
  FP_DECL_D(A); FP_DECL_D(B); FP_DECL_D(R);
  DFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_SEMIRAW_D(A, a);
  FP_UNPACK_SEMIRAW_D(B, b);
  FP_SUB_D(R, A, B);
  FP_PACK_SEMIRAW_D(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Return a - b
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  

SFtype __subsf3(SFtype a, SFtype b)
{
  FP_DECL_EX;
  FP_DECL_S(A); FP_DECL_S(B); FP_DECL_S(R);
  SFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_SEMIRAW_S(A, a);
  FP_UNPACK_SEMIRAW_S(B, b);
  FP_SUB_S(R, A, B);
  FP_PACK_SEMIRAW_S(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}
/*   
   Truncate IEEE double into IEEE single
     
     
     
		    

     
     
     
     

     
     
     
     
     
     
     
     

     
     
     
     

     
     
       */

  
  
  

SFtype __truncdfsf2(DFtype a)
{
  FP_DECL_EX;
  FP_DECL_D(A);
  FP_DECL_S(R);
  SFtype r;

  FP_INIT_ROUNDMODE;
  FP_UNPACK_SEMIRAW_D(A, a);
#if _FP_W_TYPE_SIZE < _FP_FRACBITS_D
  FP_TRUNC(S,D,1,2,R,A);
#else
  FP_TRUNC(S,D,1,1,R,A);
#endif
  FP_PACK_SEMIRAW_S(r, R);
  FP_HANDLE_EXCEPTIONS;

  return r;
}

/*   
   Return 1 iff a or b is a NaN, 0 otherwise.
       */
CMPtype __unorddf2(DFtype a, DFtype b)
{
  FP_DECL_D(A); FP_DECL_D(B);
  CMPtype r;

  FP_UNPACK_RAW_D(A, a);
  FP_UNPACK_RAW_D(B, b);
  FP_CMP_UNORD_D(r, A, B);

  return r;
}
/*   
   Return 1 iff a or b is a NaN, 0 otherwise.
       */

CMPtype __unordsf2(SFtype a, SFtype b)
{
  FP_DECL_S(A);
  FP_DECL_S(B);
  CMPtype r;

  FP_UNPACK_RAW_S(A, a);
  FP_UNPACK_RAW_S(B, b);
  FP_CMP_UNORD_S(r, A, B);

  return r;
}

