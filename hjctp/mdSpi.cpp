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
	reqUserLogin();
}

void MdSpi::notifyFrontConnected(){
	JNIEnv *env; 
	mdJvm->AttachCurrentThread((void **)&env, NULL);
	jclass cls = env->GetObjectClass(jMdSpi); 
	jmethodID methodid = env->GetMethodID(cls, "onFrontConnected", "()V");
	env->CallVoidMethod(jMdSpi, methodid);

	Common::releaseLocalRef(env, 1, cls);
	mdJvm->DetachCurrentThread();
}

void MdSpi::OnFrontDisconnected(int nReason){
}

void MdSpi::reqUserLogin(){
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = mdApi->ReqUserLogin(&req, ++iMdRequestID);
	//cerr << "--->>> send login: " << ((iResult == 0) ? "success" : "fail") << " iResult = " << iResult << endl;
}

void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){	
	notifyRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast);
	subScribeMarketData();
}

void MdSpi::subScribeMarketData(){
	int iResult = mdApi->SubscribeMarketData(ppInstrumentID, nCount);
	//cerr << "--->>> send market data: " << ((iResult == 0) ? "success" : "failed") << endl;
}

void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
}

void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	notifyRtnDepthMarketData(pDepthMarketData);
}

void MdSpi::notifyRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	mdJvm->AttachCurrentThread((void **)&env, NULL);
	jclass mdSpiCls = env->GetObjectClass(jMdSpi);
	jmethodID methodid = env->GetMethodID(mdSpiCls, "onRspUserLogin", "(Lorg/zhps/hjctp/entity/CThostFtdcRspUserLoginField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");

	if(pRspUserLogin == 0){
		env->CallVoidMethod(jMdSpi, methodid, NULL, NULL, nRequestID, bIsLast);
		mdJvm->DetachCurrentThread();
		return;
	}

	jclass rspUserLoginCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspUserLoginField");
	jobject rspUserLoginObj = env->AllocObject(rspUserLoginCls); 
	Common::assemString(env, &rspUserLoginCls, &rspUserLoginObj, "tradingDay", pRspUserLogin->TradingDay);
	Common::assemString(env, &rspUserLoginCls, &rspUserLoginObj, "loginTime", pRspUserLogin->LoginTime);
	Common::assemString(env, &rspUserLoginCls, &rspUserLoginObj, "userId", pRspUserLogin->UserID);
	Common::assemString(env, &rspUserLoginCls, &rspUserLoginObj, "shfeTime", pRspUserLogin->SHFETime);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);

	env->CallVoidMethod(jMdSpi, methodid, rspUserLoginObj, rspInfoObj, nRequestID, bIsLast);


	Common::releaseLocalRef(env, 5, mdSpiCls, rspUserLoginCls, rspUserLoginObj, rspInfoCls, rspInfoObj);
	mdJvm->DetachCurrentThread();
}

void MdSpi::notifyRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	JNIEnv *env; 
	mdJvm->AttachCurrentThread((void **)&env, NULL);
	jclass mdSpiCls = env->GetObjectClass(jMdSpi);
	jmethodID methodid = env->GetMethodID(mdSpiCls, "onRtnDepthMarketData", "(Lorg/zhps/hjctp/entity/CThostFtdcDepthMarketDataField;)V");

	if(pDepthMarketData == 0){
		env->CallVoidMethod(jMdSpi, methodid, NULL);
		mdJvm->DetachCurrentThread();
		return;
	}

	jclass rtnDepthMarketDataCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcDepthMarketDataField");
	jobject rtnDepthMarketDataObj =env->AllocObject(rtnDepthMarketDataCls); 
	Common::assemString(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "instrumentId", pDepthMarketData->InstrumentID);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "lastPrice", pDepthMarketData->LastPrice);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "preSettlementPrice", pDepthMarketData->PreSettlementPrice);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "preClosePrice", pDepthMarketData->PreClosePrice);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "preOpenInterest", pDepthMarketData->PreOpenInterest);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "openPrice", pDepthMarketData->OpenPrice);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "highestPrice", pDepthMarketData->HighestPrice);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "lowestPrice", pDepthMarketData->LowestPrice);
	Common::assemInt(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "volume", pDepthMarketData->Volume);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "turnover", pDepthMarketData->Turnover);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "openInterest", pDepthMarketData->LastPrice);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "closePrice", pDepthMarketData->ClosePrice);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "settlementPrice", pDepthMarketData->SettlementPrice);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "upperLimitPrice", pDepthMarketData->UpperLimitPrice);
	Common::assemDouble(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "lowerLimitPrice", pDepthMarketData->LowerLimitPrice);
	Common::assemString(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "updateTime", pDepthMarketData->UpdateTime);
	Common::assemString(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "tradingDay", pDepthMarketData->TradingDay);
	Common::assemInt(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "updateMillisec", pDepthMarketData->UpdateMillisec);

	env->CallVoidMethod(jMdSpi, methodid, rtnDepthMarketDataObj);


	Common::releaseLocalRef(env, 3, mdSpiCls, rtnDepthMarketDataCls, rtnDepthMarketDataObj);
	mdJvm->DetachCurrentThread();

	//cerr << "--->>> send market data: " << endl;
}