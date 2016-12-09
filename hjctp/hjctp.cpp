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

TThostFtdcBrokerIDType	BROKER_ID = "";//"9999";						
TThostFtdcInvestorIDType INVESTOR_ID = "";// "077580";						
TThostFtdcPasswordType  PASSWORD = "";//hp120416";TThostFtdcBrokerIDType

int iRequestID;

JNIEXPORT void JNICALL Java_org_hjctp_jni_NativeLoader_connect
  (JNIEnv *jenv, jclass, jstring pszFlowPath, jboolean bIsUsingUdp, jboolean bIsMulticast, jobject jobj, jstring front){
	const char *c_pszFlowPath = jenv->GetStringUTFChars(pszFlowPath , NULL);
	CThostFtdcMdApi* pMdApi = CThostFtdcMdApi::CreateFtdcMdApi(c_pszFlowPath, bIsUsingUdp, bIsMulticast);
	jclass jcla = jenv->GetObjectClass(jobj);
	jmethodID methodid = jenv->GetMethodID(jcla, "OnFrontConnected", "()V");
	/*jenv->CallVoidMethod(jobj, methodid);*/
	MdSpi mdSpi(pMdApi, jenv, jobj, methodid);
	pMdApi->RegisterSpi(&mdSpi);
	char *c_front = (char*)jenv->GetStringUTFChars(front , NULL);
	pMdApi->RegisterFront(c_front);
	pMdApi->Init();
	pMdApi->Join();
}
