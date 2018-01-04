#include "testcase.h"
#include "baselib.h"
#include "string"
#include "vector"
using namespace std;

/////////////////////////////////////////////////////////////////////////

#define MAX_CONST_CHAR 32
#define MIN(a,b) (a)<(b)?(a):(b)
#define getChr(name, ii) ((MIN(ii,MAX_CONST_CHAR))<strlen(name)?name[ii]:0)

#define _T(s) \
getChr(s,0), \
getChr(s,1), \
getChr(s,2), \
getChr(s,3), \
getChr(s,4), \
getChr(s,5), \
getChr(s,6), \
getChr(s,7), \
getChr(s,8), \
getChr(s,9), \
getChr(s,10), \
getChr(s,11), \
getChr(s,12), \
getChr(s,13), \
getChr(s,14), \
getChr(s,15), \
getChr(s,16), \
getChr(s,17), \
getChr(s,18), \
getChr(s,19), \
getChr(s,20), \
getChr(s,21), \
getChr(s,22), \
getChr(s,23), \
getChr(s,24), \
getChr(s,25), \
getChr(s,26), \
getChr(s,27), \
getChr(s,28), \
getChr(s,29), \
getChr(s,30), \
getChr(s,31), \
getChr(s,32)

template <char... Chars_>
class E {
public:
	E(){
		std::vector<char> vec = {Chars_...};
		str = new string(vec.begin(),vec.end());
		memcpy(array, str->c_str(), str->length());
	}
	~E()
	{
		delete str;
	}

	char array[256];
	string *str;

};


/////////////////////////////////////////////////////////////////////////

#define NXSIGNAL(...) \
	NXSignalBase< void(*)(__VA_ARGS__), _T("SignalObjName")>


template<class T, int line, char... Chars_>
class NXSignalBase
{
public:
	NXSignalBase()
	{
		std::vector<char> vec = {Chars_...};
		str = new string(vec.begin(),vec.end());
	}
	void func()
	{
		printf(" line(%d) str(%s)\n", line, str->c_str());
	}
	string *str;
};


#define NXSIGNALDEF(obj, ...) \
	NXSignalBase< void(*)(__VA_ARGS__), __LINE__, _T(__FILE__"#"#obj)> obj


/////////////////////////////////////////////////////////////////////////


template<class T, int line, char... Chars_> 
struct NXSignal;


template<class T0, class T1, int L, char... Chars_>
struct NXSignal<void(*)(T0, T1), L, Chars_...>: public NXSignalBase< void(*)(T0, T1)>
{
 void func()
 {
  std::vector<char> vec = {Chars_...};
  string *str = new string(vec.begin(),vec.end());
  printf(" line(%d) string(%s)\n", L,str->c_str());
 }
};

/////////////////////////////////////////////////////////////////////////

int test() {


	NXSIGNALDEF(objCls1, int, int);
	objCls1.func();


	NXSignal<void(*)(int, int), 1, _T("avsdfasdfsdf")> objClssig;
	objClssig.func();

	return 0;
}