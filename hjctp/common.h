#include "jni.h"

//market
extern JavaVM *mdJvm;
extern jobject jMdSpi;
extern int iMdRequestID;
//trader
extern JavaVM *traderJvm;
extern jobject jTraderSpi;
extern int iTdRequestID;
using namespace std;

class Common{
public:
	template <typename T> static void loadClass(JNIEnv *env, jclass *cls, jobject *obj, const char* filedName, int type, T val){
		if(type == 0){//string
			jfieldID field = env->GetFieldID(*cls, filedName, "Ljava/lang/String;");
			env->SetObjectField(*obj, field, (jobject)val);
		}else if(type == 1){//double
			jfieldID field = env->GetFieldID(*cls, filedName, "D");
			env->SetDoubleField(*obj, field, (jdouble)val);
		}else if(type == 2){//int
			jfieldID field = env->GetFieldID(*cls, filedName, "I");
			env->SetIntField(*obj, field, (jint)val);
		}else if(type == 3){//char
			jfieldID field = env->GetFieldID(*cls, filedName, "C");
			env->SetCharField(*obj, field, (jchar)val);		
		}
	}
};