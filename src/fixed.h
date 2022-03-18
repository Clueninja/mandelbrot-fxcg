#ifndef FIXED_H
#define FIXED_H

#define FIX_BITS 32

// I trust you on this one

#ifdef FIX_FBITS
    #if FIX_FBITS>=FIX_BITS
        #error "Too many bits after decimal point"
    #endif
#else
    #define FIX_FBITS 16
#endif

#define FIX_ONE ((long long)1<<FIX_FBITS)

// just use +
#define fix_add(l, r) (l)+(r)

// just use -
#define fix_sub(l, r) (l)-(r)

// shorten stuff a little
#define fix_sq(x) fix_mul((x), (x))

// not tested
#define fix_fromfloat(x) ((long long) ((x) * FIX_ONE))



typedef long long fix;

static inline fix fix_fromint(int a){ return ((long long)a)<<FIX_FBITS; }

static inline int fix_toint(fix a){return a>>FIX_FBITS;}

static inline fix fix_mul(fix l, fix r){return l * r>>FIX_FBITS;}

static inline fix fix_div(fix l, fix r){ return (l << FIX_FBITS) / r;}

static inline float fix_tofloat(fix f){return ((float) f )/FIX_ONE;}

#endif
