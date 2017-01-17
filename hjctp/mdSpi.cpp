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
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "tradingDay", 0, pRspUserLogin->TradingDay);
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "loginTime", 0, pRspUserLogin->LoginTime);
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "userId", 0, pRspUserLogin->UserID);
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "shfeTime", 0, pRspUserLogin->SHFETime);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);

	env->CallVoidMethod(jMdSpi, methodid, rspUserLoginObj, rspInfoObj, nRequestID, bIsLast);

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
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "instrumentId", 0, pDepthMarketData->InstrumentID);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "lastPrice", 1, pDepthMarketData->LastPrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "preSettlementPrice", 1, pDepthMarketData->PreSettlementPrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "preClosePrice", 1, pDepthMarketData->PreClosePrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "preOpenInterest", 1, pDepthMarketData->PreOpenInterest);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "openPrice", 1, pDepthMarketData->OpenPrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "highestPrice", 1, pDepthMarketData->HighestPrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "lowestPrice", 1, pDepthMarketData->LowestPrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "volume", 2, pDepthMarketData->Volume);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "turnover", 1, pDepthMarketData->Turnover);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "openInterest", 1, pDepthMarketData->OpenInterest);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "closePrice", 1, pDepthMarketData->ClosePrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "settlementPrice", 1, pDepthMarketData->SettlementPrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "upperLimitPrice", 1, pDepthMarketData->UpperLimitPrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "lowerLimitPrice", 1, pDepthMarketData->LowerLimitPrice);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "updateTime", 0, pDepthMarketData->UpdateTime);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "tradingDay", 0, pDepthMarketData->TradingDay);
	Common::loadClass(env, &rtnDepthMarketDataCls, &rtnDepthMarketDataObj, "updateMillisec", 2, pDepthMarketData->UpdateMillisec);

	env->CallVoidMethod(jMdSpi, methodid, rtnDepthMarketDataObj);

	mdJvm->DetachCurrentThread();
}