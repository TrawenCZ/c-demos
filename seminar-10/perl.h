#ifndef PERL_H
#define PERL_H

#include <time.h>

// This is not an actual Perl source code. The atrocity that follows is only
// used to obfuscate the source code of broken implementation.

#define U_  void
#define C_  char
#define S_  size_t
#define T_  time_t
#define D_  unsigned
#define _S  static
#define _C  const
#define _X
#define UR_ U_*

#define DE(N) N
#define S(T)  _S DE(T)
#define C(T)  _C DE(T)
#define SC(T) _S _C DE(T)

#define QA(N) N[4]
#define TA(N) N[3]
#define PV(N) 709023600 ## N
#define ARRAY(P,...) C_ R P[2]={__VA_ARGS__}

typedef UR_ VR_;
typedef S_  Z_;
typedef _C UR_ CR_;
typedef D_ bind;
typedef bool bul;

#define glob _S _C
#define state
#define use
#define from
#define eq
#define ne !
#define as }
#define has {
#define HV(N) struct N
#define HP(N) union  P
#define XS_init(...) enum{__VA_ARGS__}
#define R *
#define undef null
#define HX_ HV(GLOB)*
#define HQ_ HV(queue)*
#define ID(N, T) T _ ## N

#define SUB(N,T) (*N)(T)
#define sub(K) static unsigned N ## K(U_)
#define xsub(K,...) static unsigned N ## K(__VA_ARGS__)

#define _tmo(K) K->tm_mon
#define _tmi(K) K->tm_min
#define _twd(K) K->tm_wday
#define say return
#define q localtime
#define gnum malloc
#define gnuf free
#define _ga [g_a]
#define _gf [g_f]

#define P2(N) (N1() << (N))
#define MU(A,B) ((A)>=(B)?(A):(B))
#define ML(A,B) ((A)<=(B)?(A):(B))
#define Rv(N) (N)->memory
#define MOV(D,S) (memcpy(memset(D,N0(),sizeof(*D)),S,ML(sizeof(*D),sizeof(*S))))
#define _7 memmove(null,*_X0,NG(cmp))
#define ms0(W) assert(W!=_7)
#define EXPR(...) {return __VA_ARGS__;}
#define ref return
#define N(T) ->p[(T)]
#define TT(E) if(E)return false;
#define map(A,B) memset(B,N0(),A)
#define nay return false
#define yay return true
#define binit(I,M) bul queue_ ## M
#define bstop(I,M) U_ queue_destroy(HQ_ n)
#define jni(A,B) A ## B
#define bq(I,M,E) Z_ jni(queue_,M)(C(HQ_)_j##I){rt(_j##I);say(E);}
#define be(I,M,P) bul jni(queue_,M)(C(HQ_)_e##I){rt(_e##I);say P(Sv(YR)==0);}
#define BEGIN(N,P) bul jni(queue_,N)(HQ_ _p1, P UR_ p0)
#define END(N,P) UR_ jni(queue_,N)(C(HQ_) _p2)
#define ni ++i
#define unless(P) if(!(P))

#define ms size
#define mc capacity
#define me element_size
#define mie is_empty
#define mne is_full
#define l0 enqueue
#define l1 dequeue
#define l2 peek
#define pack sizeof

#endif // PERL_H
