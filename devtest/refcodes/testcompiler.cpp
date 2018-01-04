//#include "testcase.h"
//#include "baselib.h"
#include "string"
#include "vector"
using namespace std;
#include <string.h>


/////////////////////////////////////////////////////////////////////

int constexpr CONSTLEN(const char* str)
{
    return *str ? 1 + CONSTLEN(str + 1) : 0;
}

#define MAX_CONST_CHAR 32
#define NXMIN(a,b) (a)<(b)?(a):(b)
#define getChr(name, ii) ((NXMIN(ii,MAX_CONST_CHAR))<CONSTLEN(name)?name[ii]:0)

#define NX_STR_TOCHAR(s) \
    getChr(s,0),\
    getChr(s,1),\
    getChr(s,2),\
    getChr(s,3),\
    getChr(s,4),\
    getChr(s,5),\
    getChr(s,6),\
    getChr(s,7),\
    getChr(s,8),\
    getChr(s,9),\
    getChr(s,10),\
    getChr(s,11),\
    getChr(s,12),\
    getChr(s,13),\
    getChr(s,14),\
    getChr(s,15),\
    getChr(s,16),\
    getChr(s,17),\
    getChr(s,18),\
    getChr(s,19),\
    getChr(s,20),\
    getChr(s,21),\
    getChr(s,22),\
    getChr(s,23),\
    getChr(s,24),\
    getChr(s,25),\
    getChr(s,26),\
    getChr(s,27),\
    getChr(s,28),\
    getChr(s,29),\
    getChr(s,30),\
    getChr(s,31)


//////////////////
template<class T ,int line, char... Chars_> struct NXSignal;
//////////////////


template<unsigned int line, char... Chars_>
struct NXSignalBase
{
};

template<int line, char... Chars_>
struct NXSignal<void(*)() ,line, Chars_...>: public NXSignalBase<line, Chars_...>
{
    void postInfo(const char* fileinfo, unsigned int lineinfo)
    {
        //NXSignalBase< NXTuple<>,line, Chars_... >::post_( NXTuple<>(), fileinfo, lineinfo);
    }

    void post()
    {
        //NXSignalBase< NXTuple<>,line, Chars_... >::post_( NXTuple<>() );
    }
};


#define NXSIGNAL(...) NXSignal<void(*)(__VA_ARGS__),(unsigned int)__LINE__, NX_STR_TOCHAR(__FILE__)>


///////////////////////
#include <type_traits>

#define TTT(s) s[0],s[1]

template<char... Chars_>
struct NT
{
    void print()
    {
        std::vector<char> vec = {Chars_...};
        std::string * str = new string(vec.begin(),vec.end());
        char buf[256];
        memcpy(buf, str->c_str(), str->length());
        printf("%s\n", buf);
    }
};


int constexpr length(const char* str)
{
    return *str ? 1 + length(str + 1) : 0;
}

int main() {

 NXSignal<void(*)(),(unsigned int)__LINE__, NX_STR_TOCHAR("a")> x;

NT<TTT("0")> nt;
nt.print();

 return 0;
}