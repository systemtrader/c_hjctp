#include "StdAfx.h"
#include "mdSpi.h"
#include <iostream>
#include "ThostFtdcMdApi.h"
#include "org_zhps_hjctp_jni_NativeLoader.h"
#include "jni.h"
using namespace std;

extern TThostFtdcBrokerIDType BROKER_ID;						
extern TThostFtdcInvestorIDType INVESTOR_ID;						
extern TThostFtdcPasswordType  PASSWORD;

int i = 0;

extern int iRequestID;
extern char* ppInstrumentID[];	
extern int nCount;

extern jobject jspi;

void MdSpi::OnFrontConnected(){
	//cerr << "--->>> " << "connecting..." << endl;
	notifyFrontConnected();
	reqUserLogin();
}

void MdSpi::OnFrontDisconnected(int nReason){
	//cerr << "--->>> Reason = " << nReason << endl;
}

void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){	
	notifyRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast);

	subScribeMarketData();
}

void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	/*cerr << "--->>> " << "sub market data: " << pSpecificInstrument->InstrumentID << endl;*/
}

void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	notifyRtnDepthMarketData(pDepthMarketData);
}

void MdSpi::reqUserLogin(){
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = mdApi->ReqUserLogin(&req, ++iRequestID);
	//cerr << "--->>> send login: " << ((iResult == 0) ? "success" : "fail") << " iResult = " << iResult << endl;
}

void MdSpi::subScribeMarketData(){
	int iResult = mdApi->SubscribeMarketData(ppInstrumentID, nCount);
	//cerr << "--->>> send market data: " << ((iResult == 0) ? "success" : "failed") << endl;
}

void MdSpi::notifyFrontConnected(){
	JNIEnv *env; 
	jvm->AttachCurrentThread((void **)&env, NULL);
	jclass cls = env->GetObjectClass(jspi); 
	jmethodID methodid = env->GetMethodID(cls, "onFrontConnected", "()V");
	env->CallVoidMethod(jspi, methodid);
	jvm->DetachCurrentThread();
}

void MdSpi::notifyRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	jvm->AttachCurrentThread((void **)&env, NULL);

	const char* TradingDay = pRspUserLogin->TradingDay;
	const char* LoginTime = pRspUserLogin->LoginTime;
	const char* UserID = pRspUserLogin->UserID;
	const char* SHFETime = pRspUserLogin->SHFETime;

	jobject tradingDay = env->NewStringUTF(TradingDay);
	jobject loginTime = env->NewStringUTF(LoginTime);
	jobject userId = env->NewStringUTF(UserID);
	jobject shfeTime = env->NewStringUTF(SHFETime);

	//cerr << "--->>> trading day " << jstrTmp << endl;

	jclass rspUserLoginCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspUserLoginField");
	jobject rspUserLoginObj = env->AllocObject(rspUserLoginCls); 
	jfieldID tradingDayField = env->GetFieldID(rspUserLoginCls,"tradingDay","Ljava/lang/String;"); 
	jfieldID loginTimeField = env->GetFieldID(rspUserLoginCls, "loginTime", "Ljava/lang/String;");
	jfieldID userIdField = env->GetFieldID(rspUserLoginCls, "userId", "Ljava/lang/String;");
	jfieldID shfeTimeField = env->GetFieldID(rspUserLoginCls, "shfeTime", "Ljava/lang/String;");

	env->SetObjectField(rspUserLoginObj, tradingDayField, tradingDay);
	env->SetObjectField(rspUserLoginObj, loginTimeField, loginTime);
	env->SetObjectField(rspUserLoginObj, userIdField, userId);
	env->SetObjectField(rspUserLoginObj, shfeTimeField, shfeTime);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);

	jclass mdSpiCls = env->GetObjectClass(jspi);
	jmethodID methodid = env->GetMethodID(mdSpiCls, "onRspUserLogin", "(Lorg/zhps/hjctp/entity/CThostFtdcRspUserLoginField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");
	env->CallVoidMethod(jspi, methodid, rspUserLoginObj, rspInfoObj, nRequestID, bIsLast);
}

void MdSpi::notifyRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
	JNIEnv *env; 
	jvm->AttachCurrentThread((void **)&env, NULL);

	const char* InstrumentID = pDepthMarketData->InstrumentID;
	double LastPrice = pDepthMarketData->LastPrice;
	double PreSettlementPrice = pDepthMarketData->PreSettlementPrice;
	double PreClosePrice = pDepthMarketData->PreClosePrice;
	double PreOpenInterest = pDepthMarketData->PreOpenInterest;
	double OpenPrice = pDepthMarketData->OpenPrice;
	double HighestPrice = pDepthMarketData->HighestPrice;
	double LowestPrice = pDepthMarketData->LowestPrice;
	int Volume = pDepthMarketData->Volume;
	double Turnover = pDepthMarketData->Turnover;
	double OpenInterest = pDepthMarketData->OpenInterest;
	double ClosePrice = pDepthMarketData->ClosePrice;
	double SettlementPrice = pDepthMarketData->SettlementPrice;
	double UpperLimitPrice = pDepthMarketData->UpperLimitPrice;
	double LowerLimitPrice = pDepthMarketData->LowerLimitPrice;
	const char* UpdateTime = pDepthMarketData->UpdateTime;
	const char* TradingDay = pDepthMarketData->TradingDay;

	jobject instrumentId = env->NewStringUTF(InstrumentID);
	jobject updateTime = env->NewStringUTF(UpdateTime);
	jobject tradingDay = env->NewStringUTF(TradingDay);

	jclass cls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcDepthMarketDataField");
	jobject depthMarketDataObj =env->AllocObject(cls); 

	jfieldID instrumentIdField = env->GetFieldID(cls, "instrumentId", "Ljava/lang/String;");
	jfieldID lastPriceField = env->GetFieldID(cls,"lastPrice","D"); 
	jfieldID preSettlementPriceField = env->GetFieldID(cls,"preSettlementPrice","D");
	jfieldID preClosePriceField = env->GetFieldID(cls,"preClosePrice","D");
	jfieldID preOpenInterestField = env->GetFieldID(cls,"preOpenInterest","D");
	jfieldID openPriceField = env->GetFieldID(cls,"openPrice","D");
	jfieldID highestPriceField = env->GetFieldID(cls,"highestPrice","D");
	jfieldID lowestPriceField = env->GetFieldID(cls,"lowestPrice","D");
	jfieldID volumeField = env->GetFieldID(cls,"volume","I");
	jfieldID turnoverField = env->GetFieldID(cls,"turnover","D");
	jfieldID openInterestField = env->GetFieldID(cls,"openInterest","D");
	jfieldID closePriceField = env->GetFieldID(cls,"closePrice","D");
	jfieldID settlementPriceField = env->GetFieldID(cls,"settlementPrice","D");
	jfieldID upperLimitPriceField = env->GetFieldID(cls,"upperLimitPrice","D");
	jfieldID lowerLimitPriceField = env->GetFieldID(cls,"lowerLimitPrice","D");
	jfieldID updateTimeField = env->GetFieldID(cls, "updateTime", "Ljava/lang/String;");
	jfieldID tradingDayField = env->GetFieldID(cls, "tradingDay", "Ljava/lang/String;");
	
	env->SetObjectField(depthMarketDataObj, instrumentIdField, instrumentId);
	env->SetDoubleField(depthMarketDataObj, lastPriceField, LastPrice);
	env->SetDoubleField(depthMarketDataObj, preSettlementPriceField, PreSettlementPrice);
	env->SetDoubleField(depthMarketDataObj, preClosePriceField, PreClosePrice);
	env->SetDoubleField(depthMarketDataObj, preOpenInterestField, PreOpenInterest);
	env->SetDoubleField(depthMarketDataObj, openPriceField, OpenPrice);
	env->SetDoubleField(depthMarketDataObj, highestPriceField, HighestPrice);
	env->SetDoubleField(depthMarketDataObj, lowestPriceField, LowestPrice);
	env->SetIntField(depthMarketDataObj, volumeField, Volume);
	env->SetDoubleField(depthMarketDataObj, turnoverField, Turnover);
	env->SetDoubleField(depthMarketDataObj, openInterestField, OpenInterest);
	env->SetDoubleField(depthMarketDataObj, closePriceField, ClosePrice);
	env->SetDoubleField(depthMarketDataObj, settlementPriceField, SettlementPrice);
	env->SetDoubleField(depthMarketDataObj, upperLimitPriceField, UpperLimitPrice);
	env->SetDoubleField(depthMarketDataObj, lowerLimitPriceField, LowerLimitPrice);
	env->SetObjectField(depthMarketDataObj, updateTimeField, updateTime);
	env->SetObjectField(depthMarketDataObj, tradingDayField, tradingDay);
	
	jclass mdSpiCls = env->GetObjectClass(jspi);
	jmethodID methodid = env->GetMethodID(mdSpiCls, "onRtnDepthMarketData", "(Lorg/zhps/hjctp/entity/CThostFtdcDepthMarketDataField;)V");
	env->CallVoidMethod(jspi, methodid, depthMarketDataObj);

	jvm->DetachCurrentThread();
}