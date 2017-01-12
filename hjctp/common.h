#include "jni.h"

//market
extern JavaVM *mdJvm;
extern jobject jMdSpi;
extern int iMdRequestID;
//trader
extern JavaVM *traderJvm;
extern jobject jTraderSpi;
extern int iTdRequestID;

class Common{
public:
	static void loadClass(JNIEnv *env, jclass *cls, jobject *obj, const char* filedName, int type, jobject sv, double dv, int iv){
		if(type == 0){//string
			jfieldID field = env->GetFieldID(*cls, filedName, "Ljava/lang/String;");
			env->SetObjectField(*obj, field, sv);
		}else if(type == 1){//double
			jfieldID field = env->GetFieldID(*cls, filedName, "D");
			env->SetDoubleField(*obj, field, dv);
		}else if(type == 2){//int
			jfieldID field = env->GetFieldID(*cls, filedName, "I");
			env->SetIntField(*obj, field, iv);
		}
	}
};