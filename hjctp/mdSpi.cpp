#include "StdAfx.h"
#include "mdSpi.h"
#include <iostream>
#include "ThostFtdcMdApi.h"
#include "org_hjctp_jni_NativeLoader.h"
#include "jni.h"
using namespace std;

extern TThostFtdcBrokerIDType BROKER_ID;						
extern TThostFtdcInvestorIDType INVESTOR_ID;						
extern TThostFtdcPasswordType  PASSWORD;

int i = 0;

extern int iRequestID;
extern char* ppInstrumentID[];	
extern int iInstrumentID;

extern JNIEnv *eenv;
extern jobject jjobj;

void MdSpi::OnFrontConnected(){
	cerr << "--->>> " << "connecting..." << endl;

	JNIEnv *env; 
	jvm->AttachCurrentThread((void **)&env, NULL);
	jclass cls = env->GetObjectClass(jjobj); 
	jmethodID methodid = env->GetMethodID(cls, "OnFrontConnected", "()V");
	env->CallVoidMethod(jjobj, methodid);
	jvm->DetachCurrentThread();

	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = mdApi->ReqUserLogin(&req, ++iRequestID);
	cerr << "--->>> send login: " << ((iResult == 0) ? "success" : "fail") << " iResult = " << iResult << endl;
}

void MdSpi::OnFrontDisconnected(int nReason){
	cerr << "--->>> Reason = " << nReason << endl;
}

void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cerr << "--->>> trading day = " << mdApi->GetTradingDay() << endl;

	int iResult = mdApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cerr << "--->>> send market data: " << ((iResult == 0) ? "success" : "failed") << endl;
}

void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	cerr << "--->>> " << "sub market data: " << pSpecificInstrument->InstrumentID << endl;
}

void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	double NewPrice = pDepthMarketData->LastPrice;
	//cerr << "--->>> " << "NewPrice: " << NewPrice << endl;

	JNIEnv *env; 
	jvm->AttachCurrentThread((void **)&env, NULL);

	jclass cls = env->FindClass("org/hjctp/entity/CThostFtdcDepthMarketDataField");
	jobject maketObj =env->AllocObject(cls); 
	jfieldID lastPrice = env->GetFieldID(cls,"LastPrice","D"); 
	env->SetDoubleField(maketObj, lastPrice, NewPrice);

	jclass mdSpiCls = env->GetObjectClass(jjobj);
	jmethodID methodid = env->GetMethodID(mdSpiCls, "OnRtnDepthMarketData", "(Lorg/hjctp/entity/CThostFtdcDepthMarketDataField;)V");
	env->CallVoidMethod(jjobj, methodid, maketObj);

	//cerr << "--->>>" << " new price " << NewPrice << endl;
}