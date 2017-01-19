#include "jni.h"
#include <cstdarg>

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
		jstring str = env->NewStringUTF(sv);
		env->SetObjectField(*obj, field, str);

	/*	env->ReleaseStringUTFChars(str, sv);*/
		env->DeleteLocalRef(str);
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
		jstring str = charToJstring(env, csv);
		env->SetObjectField(*obj, field, charToJstring(env, csv));
		env->DeleteLocalRef(str);
	}
////////////////////////////////////////////READ//////////////////////////////////////////////////////////
	static int readDouble(JNIEnv *env, jobject obj, const char* filedName){
		jclass cls = env->GetObjectClass(obj);	
		if(cls == NULL){
			return 0;
		}
		jfieldID fieldID = env->GetFieldID(cls, filedName,"D");
		return env->GetIntField(obj , fieldID);
	}

	static int readInt(JNIEnv *env, jobject obj, const char* filedName){
		jclass cls = env->GetObjectClass(obj);	
		if(cls == NULL){
			return 0;
		}
		jfieldID fieldID = env->GetFieldID(cls, filedName,"I");
		return env->GetIntField(obj , fieldID);
	}

	static int readChar(JNIEnv *env, jobject obj, const char* filedName){
		jclass cls = env->GetObjectClass(obj);	
		if(cls == NULL){
			return 0;
		}
		jfieldID fieldID = env->GetFieldID(cls, filedName,"C");
		return env->GetIntField(obj , fieldID);
	}

	static const char* readString(JNIEnv *env, jobject obj, const char* filedName){
		jclass cls = env->GetObjectClass(obj);
		if(cls == NULL){
			return 0;
		}
		jfieldID fieldID = env->GetFieldID(cls, filedName, "Ljava/lang/String;"); 
		jstring value = (jstring)env->GetObjectField(obj , fieldID);
		if(value == NULL){
			return 0;
		}
		const char * c_value = env->GetStringUTFChars(value ,NULL);

		env->DeleteLocalRef(cls);
		env->DeleteLocalRef(value);
		return c_value;
	}

	static void releaseUTFChar(JNIEnv *env, const char* value){
		if(value == 0){
			return;
		}
		jstring str = env->NewStringUTF(value);
		env->ReleaseStringUTFChars(str, value);
		env->DeleteLocalRef(str);
	}

	static void releaseLocalRef(JNIEnv *env, int count, ...){
		va_list arg_ptr;
		va_start(arg_ptr, count);
		for(int i = 0; i < count; i++){
			//cerr << va_arg(arg_ptr, jobject) << endl;
			env->DeleteLocalRef(va_arg(arg_ptr, jobject));
		}
		va_end(arg_ptr);
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