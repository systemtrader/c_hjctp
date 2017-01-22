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
#include "common.h"
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

char *ppInstrumentID[40];// = {"rb1701"};							
int nCount;	

JavaVM *mdJvm;
jobject jMdSpi;
CThostFtdcMdApi* pMdApi;
int iMdRequestID = 1;

JavaVM *traderJvm;
jobject jTraderSpi;
CThostFtdcTraderApi *pTraderApi;
int iTdRequestID = 1;

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
	strcpy(INVESTOR_ID, c_investorId);
	strcpy(PASSWORD, c_password);
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
	strcpy(INVESTOR_ID, c_investorId);
	strcpy(PASSWORD, c_password);
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_connectTraderServer
	(JNIEnv *, jclass){
	TraderSpi traderSpi(pTraderApi);
	pTraderApi->RegisterSpi(&traderSpi);
	pTraderApi->Init();
	pTraderApi->Join();
}

JNIEXPORT jint JNICALL Java_org_zhps_hjctp_jni_NativeLoader_queryTradingAccount
	(JNIEnv *, jclass){
	CThostFtdcQryTradingAccountField tradingAccount;
	memset(&tradingAccount, 0, sizeof(tradingAccount));
	strcpy(tradingAccount.BrokerID, BROKER_ID);
	strcpy(tradingAccount.InvestorID, INVESTOR_ID);
	return pTraderApi->ReqQryTradingAccount(&tradingAccount, ++iTdRequestID);
}

JNIEXPORT jint JNICALL Java_org_zhps_hjctp_jni_NativeLoader_queryInvestorPosition
	(JNIEnv *, jclass){
	CThostFtdcQryInvestorPositionField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	return pTraderApi->ReqQryInvestorPosition(&req, ++iTdRequestID);
}

JNIEXPORT jint JNICALL Java_org_zhps_hjctp_jni_NativeLoader_queryInvestorPositionDetail
	(JNIEnv *, jclass){
	CThostFtdcQryInvestorPositionDetailField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	return pTraderApi->ReqQryInvestorPositionDetail(&req, ++iTdRequestID);
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_queryOrder
	(JNIEnv *env, jclass, jobject qOrder){
	CThostFtdcQryOrderField pQryOrder;
	memset(&pQryOrder, 0, sizeof(pQryOrder));
	/*const char* broker_id = Common::readString(env, qOrder, "brokerID");
	const char* investor_id = Common::readString(env, qOrder, "investorID");
	if(broker_id == 0 || investor_id == 0){
		return;
	}*/
	strcpy(pQryOrder.BrokerID, BROKER_ID);
	strcpy(pQryOrder.InvestorID, INVESTOR_ID);	
	pTraderApi->ReqQryOrder(&pQryOrder, ++iTdRequestID);
	/*Common::releaseUTFChar(env, broker_id);
	Common::releaseUTFChar(env, investor_id);*/
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_insertOrder
	(JNIEnv *env, jclass, jobject iOrder){
	const char* instrumentID = Common::readString(env, iOrder, "instrumentID");
	const char* orderRef = Common::readString(env, iOrder, "orderRef");
	char direction = Common::readChar(env, iOrder, "direction");
	char combOffsetFlag = Common::readChar(env, iOrder, "combOffsetFlag");
	char combHedgeFlag = Common::readChar(env, iOrder, "combHedgeFlag");
	int volumeTotalOriginal = Common::readInt(env, iOrder, "volumeTotalOriginal");
	char contingentCondition = Common::readChar(env, iOrder, "contingentCondition");
	char volumeCondition = Common::readChar(env, iOrder, "volumeCondition");
	char timeCondition = Common::readChar(env, iOrder, "timeCondition");
	int minVolume = Common::readInt(env, iOrder, "minVolume");
	char forceCloseReason = Common::readChar(env, iOrder, "forceCloseReason");
	int isAutoSuspend = Common::readInt(env, iOrder, "isAutoSuspend");
	int userForceClose = Common::readInt(env, iOrder, "userForceClose");
	char orderPriceType = Common::readChar(env, iOrder, "orderPriceType");
	double limitPrice = Common::readDouble(env, iOrder, "limitPrice");

	/*cerr << "instrumentID: " << instrumentID << endl;
	cerr << "orderRef: " << orderRef << endl;
	cerr << "direction: " << direction << endl;
	cerr << "combOffsetFlag: " << combOffsetFlag << endl;
	cerr << "combHedgeFlag: " << combHedgeFlag << endl;
	cerr << "volumeTotalOriginal: " << volumeTotalOriginal << endl;
	cerr << "contingentCondition: " << contingentCondition << endl;
	cerr << "volumeCondition: " << volumeCondition << endl;
	cerr << "timeCondition: " << timeCondition << endl;
	cerr << "minVolume: " << minVolume << endl;
	cerr << "forceCloseReason: " << forceCloseReason << endl;
	cerr << "isAutoSuspend: " << isAutoSuspend << endl;
	cerr << "userForceClose: " << userForceClose << endl;
	cerr << "orderPriceType: " << orderPriceType << endl;
	cerr << "limitPrice: " << limitPrice << endl;*/

	CThostFtdcInputOrderField inputOrder;
	memset(&inputOrder, 0, sizeof(inputOrder));
	strcpy(inputOrder.BrokerID, BROKER_ID);
	strcpy(inputOrder.InvestorID, INVESTOR_ID);
	strcpy(inputOrder.InstrumentID, instrumentID);
	strcpy(inputOrder.OrderRef, "");
	inputOrder.Direction = direction;//THOST_FTDC_D_Buy;
	inputOrder.CombOffsetFlag[0] = combOffsetFlag;//THOST_FTDC_OF_Open;
	inputOrder.CombHedgeFlag[0] = combHedgeFlag;//THOST_FTDC_HF_Speculation;
	inputOrder.VolumeTotalOriginal = volumeTotalOriginal;
	inputOrder.ContingentCondition = contingentCondition;//THOST_FTDC_CC_Immediately;
	inputOrder.VolumeCondition = volumeCondition;//THOST_FTDC_VC_AV;
	inputOrder.TimeCondition = timeCondition;//THOST_FTDC_TC_GFD;
	inputOrder.MinVolume = minVolume;
	inputOrder.ForceCloseReason = forceCloseReason;//THOST_FTDC_FCC_NotForceClose;
	inputOrder.IsAutoSuspend = isAutoSuspend;
	inputOrder.UserForceClose = userForceClose;
	inputOrder.OrderPriceType = orderPriceType;//THOST_FTDC_OPT_LimitPrice;
	inputOrder.LimitPrice = limitPrice;
	pTraderApi->ReqOrderInsert(&inputOrder, ++iTdRequestID);

	Common::releaseUTFChar(env, instrumentID);
	Common::releaseUTFChar(env, orderRef);
}

JNIEXPORT void JNICALL Java_org_zhps_hjctp_jni_NativeLoader_killOrder
	(JNIEnv *env, jclass, jobject kOrder){
	const char* instrumentID = Common::readString(env, kOrder, "instrumentID");
	const char* exchangeID = Common::readString(env, kOrder, "exchangeID");
	const char* orderSysID = Common::readString(env, kOrder, "orderSysID");
	char actionFlag = Common::readChar(env, kOrder, "actionFlag");
	
	CThostFtdcInputOrderActionField actionOrder;
	memset(&actionOrder, 0, sizeof(actionOrder));
	strcpy(actionOrder.BrokerID, BROKER_ID);
	strcpy(actionOrder.InvestorID, INVESTOR_ID);
	strcpy(actionOrder.InstrumentID, instrumentID);
	strcpy(actionOrder.ExchangeID, exchangeID);
	strcpy(actionOrder.OrderSysID, orderSysID);
	actionOrder.ActionFlag = actionFlag;
	pTraderApi->ReqOrderAction(&actionOrder, ++iTdRequestID);

	Common::releaseUTFChar(env, instrumentID);
	Common::releaseUTFChar(env, exchangeID);
	Common::releaseUTFChar(env, orderSysID);
}
