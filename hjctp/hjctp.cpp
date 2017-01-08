// hjctp.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "hjctp.h"
#include "jni.h"
#include "jni_md.h"
#include "org_zhps_hjctp_jni_NativeLoader.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "mdSpi.h"
#include "traderSpi.h"
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

TThostFtdcBrokerIDType	BROKER_ID;						
TThostFtdcInvestorIDType INVESTOR_ID;					
TThostFtdcPasswordType  PASSWORD;

int iRequestID;

char *ppInstrumentID[40];// = {"rb1701"};							
int nCount;	

JavaVM *mdJvm;
jobject jMdSpi;
CThostFtdcMdApi* pMdApi;

JavaVM *traderJvm;
jobject jTraderSpi;
CThostFtdcTraderApi *pTraderApi;

/////////////////////////////////////////////////////////MARKET///////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_createMdApi
  (JNIEnv *jenv, jclass, jstring pszFlowPath, jboolean bIsUsingUdp, jboolean bIsMulticast){
	const char *c_pszFlowPath = jenv->GetStringUTFChars(pszFlowPath , NULL);
	pMdApi = CThostFtdcMdApi::CreateFtdcMdApi(c_pszFlowPath, bIsUsingUdp, bIsMulticast);
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_registerMdSpi
	(JNIEnv *jenv, jclass, jobject jobj){
	jenv->GetJavaVM(&mdJvm);
	jMdSpi = jenv->NewGlobalRef(jobj); 
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_registerMdFront
  (JNIEnv *jenv, jclass, jstring front){
	char *c_front = (char*)jenv->GetStringUTFChars(front , NULL);
	pMdApi->RegisterFront(c_front);
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_registerMdLoginInfo
  (JNIEnv *jenv, jclass, jstring brokerId, jstring investorId, jstring password){
	const char *c_brokerId = jenv->GetStringUTFChars(brokerId , NULL);
	const char *c_investorId = jenv->GetStringUTFChars(investorId , NULL);
	const char *c_password = jenv->GetStringUTFChars(password , NULL);
	strcpy(BROKER_ID, c_brokerId);
	strcpy(INVESTOR_ID, c_brokerId);
	strcpy(PASSWORD, c_brokerId);
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_registerSubMarketData
	(JNIEnv *jenv, jclass, jobjectArray contractsArray, jint subNum){	
	jsize length = jenv->GetArrayLength(contractsArray);	
	for(int i = 0; i < length; i++){
		jstring str = (jstring)(jenv)->GetObjectArrayElement(contractsArray, i);
		char *contracts = (char*)jenv->GetStringUTFChars(str , NULL);
		ppInstrumentID[i] = contracts;
	}
	nCount = subNum;
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_connectMdServer
	(JNIEnv *, jclass){
	MdSpi mdSpi(pMdApi);
	pMdApi->RegisterSpi(&mdSpi);
	pMdApi->Init();
	pMdApi->Join();
}

JNIEXPORT jstring JNICALL Java_org_zhps_hjctp_jni_NativeLoader_getTradingTay
	(JNIEnv *env, jclass){
	const char *TradingDay = pMdApi->GetTradingDay();
	jstring tradingDay = env->NewStringUTF(TradingDay);
	return tradingDay;
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_stop 
	(JNIEnv *, jclass){
	//cerr << ppInstrumentID << endl;
	//pMdApi->UnSubscribeMarketData(ppInstrumentID, nCount);
	pMdApi->Release();
}

/////////////////////////////////////////////////////////TRADER///////////////////////////////////////////////////////////////////
JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_createTraderApi
	(JNIEnv *jenv, jclass, jstring pszFlowPath){
	const char *c_pszFlowPath = jenv->GetStringUTFChars(pszFlowPath , NULL);
	pTraderApi = CThostFtdcTraderApi::CreateFtdcTraderApi(c_pszFlowPath);
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_registerTraderSpi
  (JNIEnv *jenv, jclass, jobject jobj){
	jenv->GetJavaVM(&traderJvm);
	jTraderSpi = jenv->NewGlobalRef(jobj); 
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_registerTraderFront
  (JNIEnv *jenv, jclass, jstring front){
	char *c_front = (char*)jenv->GetStringUTFChars(front , NULL);
	pTraderApi->RegisterFront(c_front);
	pTraderApi->SubscribePrivateTopic(THOST_TERT_QUICK);
	pTraderApi->SubscribePublicTopic(THOST_TERT_QUICK);
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_registerTraderLoginInfo
  (JNIEnv *jenv, jclass, jstring brokerId, jstring investorId, jstring password){
	const char *c_brokerId = jenv->GetStringUTFChars(brokerId , NULL);
	const char *c_investorId = jenv->GetStringUTFChars(investorId , NULL);
	const char *c_password = jenv->GetStringUTFChars(password , NULL);
	strcpy(BROKER_ID, c_brokerId);
	strcpy(INVESTOR_ID, c_brokerId);
	strcpy(PASSWORD, c_brokerId);
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_connectTraderServer
	(JNIEnv *, jclass){
	TraderSpi traderSpi(pTraderApi);
	pTraderApi->RegisterSpi(&traderSpi);
	pTraderApi->Init();
	pTraderApi->Join();
}


