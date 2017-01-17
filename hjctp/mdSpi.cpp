#include "StdAfx.h"
#include "mdSpi.h"
#include <iostream>
#include "ThostFtdcMdApi.h"
#include "org_zhps_hjctp_jni_NativeLoader.h"
#include "jni.h"
#include "common.h"
using namespace std;

extern TThostFtdcBrokerIDType BROKER_ID;						
extern TThostFtdcInvestorIDType INVESTOR_ID;						
extern TThostFtdcPasswordType  PASSWORD;

extern char* ppInstrumentID[];	
extern int nCount;

//extern jobject jMdSpi;

void MdSpi::OnFrontConnected(){
	notifyFrontConnected();
	//reqUserLogin();
}

void MdSpi::notifyFrontConnected(){
	JNIEnv *env; 
	mdJvm->AttachCurrentThread((void **)&env, NULL);
	jclass cls = env->GetObjectClass(jMdSpi); 
	jmethodID methodid = env->GetMethodID(cls, "onFrontConnected", "()V");
	env->CallVoidMethod(jMdSpi, methodid);
	mdJvm->DetachCurrentThread();
}

void MdSpi::OnFrontDisconnected(int nReason){
}

//void MdSpi::reqUserLogin(){
//	CThostFtdcReqUserLoginField req;
//	memset(&req, 0, sizeof(req));
//	strcpy(req.BrokerID, BROKER_ID);
//	strcpy(req.UserID, INVESTOR_ID);
//	strcpy(req.Password, PASSWORD);
//	int iResult = mdApi->ReqUserLogin(&req, ++iMdRequestID);
//	//cerr << "--->>> send login: " << ((iResult == 0) ? "success" : "fail") << " iResult = " << iResult << endl;
//}
//
//void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){	
//	notifyRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast);
//	subScribeMarketData();
//}
//
//void MdSpi::subScribeMarketData(){
//	int iResult = mdApi->SubscribeMarketData(ppInstrumentID, nCount);
//	//cerr << "--->>> send market data: " << ((iResult == 0) ? "success" : "failed") << endl;
//}
//
//void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
//}
//
//void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
//	notifyRtnDepthMarketData(pDepthMarketData);
//}
//
//void MdSpi::notifyRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
//	JNIEnv *env; 
//	mdJvm->AttachCurrentThread((void **)&env, NULL);
//	jclass mdSpiCls = env->GetObjectClass(jMdSpi);
//	jmethodID methodid = env->GetMethodID(mdSpiCls, "onRspUserLogin", "(Lorg/zhps/hjctp/entity/CThostFtdcRspUserLoginField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");
//
//	if(pRspUserLogin == 0){
//		env->CallVoidMethod(jMdSpi, methodid, NULL, NULL, nRequestID, bIsLast);
//		mdJvm->DetachCurrentThread();
//		return;
//	}
//
//	jclass rspUserLoginCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspUserLoginField");
//	jobject rspUserLoginObj = env->AllocObject(rspUserLoginCls); 
//	//type: 0.string, 1.double, 2.int
//	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "tradingDay", 0, env->NewStringUTF(pRspUserLogin->TradingDay),0, 0);
//	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "loginTime", 0, env->NewStringUTF(pRspUserLogin->LoginTime),0, 0);
//	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "userId", 0, env->NewStringUTF(pRspUserLogin->UserID),0, 0);
//	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "shfeTime", 0, env->NewStringUTF(pRspUserLogin->SHFETime),0, 0);
//
//	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
//	jobject rspInfoObj = env->AllocObject(rspInfoCls);
//
//	env->CallVoidMethod(jMdSpi, methodid, rspUserLoginObj, rspInfoObj, nRequestID, bIsLast);
//
//	mdJvm->DetachCurrentThread();
//}
//
//void MdSpi::notifyRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
//	JNIEnv *env; 
//	mdJvm->AttachCurrentThread((void **)&env, NULL);
//	jclass mdSpiCls = env->GetObjectClass(jMdSpi);
//	jmethodID methodid = env->GetMethodID(mdSpiCls, "onRtnDepthMarketData", "(Lorg/zhps/hjctp/entity/CThostFtdcDepthMarketDataField;)V");
//
//	if(pDepthMarketData == 0){
//		env->CallVoidMethod(jMdSpi, methodid, NULL);
//		mdJvm->DetachCurrentThread();
//		return;
//	}
//
//	jclass rtnDepthMarketDataCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcDepthMarketDataField");
//	jobject rtnDepthMarketDataObj =env->AllocObject(rtnDepthMarketDataCls); 
//	//type: 0.string, 1.double, 2.int
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "instrumentId", 0, env->NewStringUTF(pDepthMarketData->InstrumentID),0, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "lastPrice", 1, NULL,pDepthMarketData->LastPrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "preSettlementPrice", 1, NULL,pDepthMarketData->PreSettlementPrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "preClosePrice", 1, NULL,pDepthMarketData->PreClosePrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "preOpenInterest", 1, NULL,pDepthMarketData->PreOpenInterest, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "openPrice", 1, NULL,pDepthMarketData->OpenPrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "highestPrice", 1, NULL,pDepthMarketData->HighestPrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "lowestPrice", 1, NULL,pDepthMarketData->LowestPrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "volume", 2, NULL,0, pDepthMarketData->Volume);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "turnover", 1, NULL,pDepthMarketData->Turnover, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "openInterest", 1, NULL,pDepthMarketData->OpenInterest, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "closePrice", 1, NULL,pDepthMarketData->ClosePrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "settlementPrice", 1, NULL,pDepthMarketData->SettlementPrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "upperLimitPrice", 1, NULL,pDepthMarketData->UpperLimitPrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "lowerLimitPrice", 1, NULL,pDepthMarketData->LowerLimitPrice, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "updateTime", 0, env->NewStringUTF(pDepthMarketData->UpdateTime),0, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "tradingDay", 0, env->NewStringUTF(pDepthMarketData->TradingDay),0, 0);
//	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "updateMillisec", 2, NULL,0, pDepthMarketData->UpdateMillisec);
//
//	env->CallVoidMethod(jMdSpi, methodid, rtnDepthMarketDataObj);
//
//	mdJvm->DetachCurrentThread();
//}