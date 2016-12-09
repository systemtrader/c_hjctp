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

void MdSpi::OnFrontConnected(){
	cerr << "--->>> " << "connecting..." << endl;

	cerr << "--->>> " << "test..." << endl;
	//jclass jcla = env->GetObjectClass(jspi);
	//jmethodID methodid = env->GetMethodID(jcla, "OnFrontConnected", "()V");
	env->CallVoidMethod(jspi, methodid);

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

//void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
//	cerr << "--->>> 获取当前交易日 = " << pMdApi->GetTradingDay() << endl;
//
//	int iResult = pMdApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
//	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
//}
//
//void MdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
//	cerr << "--->>> " << "订阅行情品种：" <<pSpecificInstrument->InstrumentID<< endl;
//}
//
//void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData){
//	double NewPrice = pDepthMarketData->LastPrice;
//	cerr << "--->>>" << " new price " << NewPrice << endl;
//}