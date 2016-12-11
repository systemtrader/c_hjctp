// hjctp.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "hjctp.h"
#include "jni.h"
#include "jni_md.h"
#include "org_hjctp_jni_NativeLoader.h"
#include "ThostFtdcMdApi.h"
#include "mdSpi.h"
#include <iostream>
using namespace std;

// This is an example of an exported variable
HJCTP_API int nhjctp=0;

// This is an example of an exported function.
HJCTP_API int fnhjctp(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see hjctp.h for the class definition
Chjctp::Chjctp()
{
	return;
}

TThostFtdcBrokerIDType	BROKER_ID;//"9999";						
TThostFtdcInvestorIDType INVESTOR_ID;// "077580";						
TThostFtdcPasswordType  PASSWORD;//"hp120416";TThostFtdcBrokerIDType

int iRequestID;

char *ppInstrumentID[40];// = {"rb1701"};							
int nCount;	

JavaVM *jvm;
jobject jspi;
CThostFtdcMdApi* pMdApi;

JNIEXPORT void JNICALL Java_org_hjctp_jni_NativeLoader_createMdApi
  (JNIEnv *jenv, jclass, jstring pszFlowPath, jboolean bIsUsingUdp, jboolean bIsMulticast){
	const char *c_pszFlowPath = jenv->GetStringUTFChars(pszFlowPath , NULL);
	pMdApi = CThostFtdcMdApi::CreateFtdcMdApi(c_pszFlowPath, bIsUsingUdp, bIsMulticast);
}

JNIEXPORT void JNICALL Java_org_hjctp_jni_NativeLoader_registerSpi
	(JNIEnv *jenv, jclass, jobject jobj){
	jenv->GetJavaVM(&jvm);
	jspi = jenv->NewGlobalRef(jobj); 
}

JNIEXPORT void JNICALL Java_org_hjctp_jni_NativeLoader_registerFront
  (JNIEnv *jenv, jclass, jstring front){
	char *c_front = (char*)jenv->GetStringUTFChars(front , NULL);
	pMdApi->RegisterFront(c_front);
}

JNIEXPORT void JNICALL Java_org_hjctp_jni_NativeLoader_registerLoginInfo
  (JNIEnv *jenv, jclass, jstring brokerId, jstring investorId, jstring password){
	const char *c_brokerId = jenv->GetStringUTFChars(brokerId , NULL);
	const char *c_investorId = jenv->GetStringUTFChars(investorId , NULL);
	const char *c_password = jenv->GetStringUTFChars(password , NULL);
	strcpy(BROKER_ID, c_brokerId);
	strcpy(INVESTOR_ID, c_brokerId);
	strcpy(PASSWORD, c_brokerId);
}

JNIEXPORT void JNICALL Java_org_hjctp_jni_NativeLoader_registerSubMarketData
	(JNIEnv *jenv, jclass, jobjectArray contractsArray, jint subNum){	
	jsize length = jenv->GetArrayLength(contractsArray);	
	for(int i = 0; i < length; i++){
		jstring str = (jstring)(jenv)->GetObjectArrayElement(contractsArray, i);
		char *contracts = (char*)jenv->GetStringUTFChars(str , NULL);
		ppInstrumentID[i] = contracts;
	}
	nCount = subNum;
}

JNIEXPORT void JNICALL Java_org_hjctp_jni_NativeLoader_connect
	(JNIEnv *, jclass){
	MdSpi mdSpi(pMdApi);
	pMdApi->RegisterSpi(&mdSpi);
	pMdApi->Init();
	pMdApi->Join();
}
