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
	static void assemString(JNIEnv *env, jclass *cls, jobject *obj, const char* filedName, char* sv){
		jfieldID field = env->GetFieldID(*cls, filedName, "Ljava/lang/String;");
		env->SetObjectField(*obj, field, env->NewStringUTF(sv));
	}

	static void assemDouble(JNIEnv *env, jclass *cls, jobject *obj, const char* filedName, double dv){
		jfieldID field = env->GetFieldID(*cls, filedName, "D");
		env->SetDoubleField(*obj, field, dv);
	}

	static void assemInt(JNIEnv *env, jclass *cls, jobject *obj, const char* filedName, int iv){
		jfieldID field = env->GetFieldID(*cls, filedName, "I");
		env->SetIntField(*obj, field, iv);
	}

	static void assemChar(JNIEnv *env, jclass *cls, jobject *obj, const char* filedName, char cv){
		jfieldID field = env->GetFieldID(*cls, filedName, "C");
		env->SetCharField(*obj, field, cv);		
	}

	static void assemCstring(JNIEnv *env, jclass *cls, jobject *obj, const char* filedName, char* csv){
		jfieldID field = env->GetFieldID(*cls, filedName, "Ljava/lang/String;");
		env->SetObjectField(*obj, field, charToJstring(env, csv));
	}

private:
	static jstring charToJstring(JNIEnv *env, char* val){
		jstring rtn = 0;
		int slen = strlen(val);
		int length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)val, slen, NULL, 0);
		unsigned short * buffer = (unsigned short *)malloc(length*2 + 1);
		if(slen == 0){
			rtn = env->NewStringUTF(val);
		}else{
			int length = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)val, slen, NULL, 0);
			buffer = (unsigned short *)malloc(length*2 + 1);
			if(MultiByteToWideChar(CP_ACP, 0, (LPCSTR)val, slen, (LPWSTR)buffer, length) >0){
				rtn = env->NewString((jchar*)buffer, length);
			}
		}
		if(buffer){
			free(buffer);	
		}
        return rtn;
	}
};