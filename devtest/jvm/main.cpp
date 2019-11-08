#include <iostream>
#include <jni.h>
#include <cstdlib>
using namespace std;

int main()
{
	JavaVM* mJavaVM;
	JNIEnv* env;
	const int size = 3;
	JavaVMOption options[size];
	options[0].optionString = "-Djava.compiler=NONE";
	options[1].optionString = "-Djava.class.path=./"; //指定当前目录为classpath路径
 
	options[2].optionString=  "-verbose:class";          
 
	JavaVMInitArgs initArgs;
	initArgs.version = JNI_VERSION_1_4;
	initArgs.options = options;
	initArgs.nOptions = size;
	initArgs.ignoreUnrecognized = JNI_TRUE;
 
	if (JNI_CreateJavaVM(&mJavaVM, (void **)&env, &initArgs) != 0) {
		cout<<"创建JVM失败!"<<endl;
	}else{
		cout<<"创建JVM成功!"<<endl;
		jclass startClass =env->FindClass("com/hello/HelloWorld");
		jmethodID startMeth;
		if(startClass){
			cout<<"成功找到类!"<<endl;
			startMeth = env->GetStaticMethodID(startClass, "main","([Ljava/lang/String;)V");
			env->CallStaticVoidMethod(startClass, startMeth, NULL);
		}else{
			cout<<"找不到指定的类!"<<endl;
		}
		
	}	
	return 0;
}