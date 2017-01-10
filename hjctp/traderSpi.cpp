#include "StdAfx.h"
#include "traderSpi.h"
#include <iostream>
#include "ThostFtdcTraderApi.h"
#include "org_zhps_hjctp_jni_NativeLoader.h"
#include "jni.h"
using namespace std;

extern TThostFtdcBrokerIDType BROKER_ID;						
extern TThostFtdcInvestorIDType INVESTOR_ID;						
extern TThostFtdcPasswordType  PASSWORD;

int iTdRequestID;

extern jobject jTraderSpi;

void TraderSpi::OnFrontConnected(){
	notifyFrontConnected();
	execReqUserLogin();
}

void TraderSpi::notifyFrontConnected(){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass cls = env->GetObjectClass(jTraderSpi); 
	jmethodID methodid = env->GetMethodID(cls, "onFrontConnected", "()V");
	env->CallVoidMethod(jTraderSpi, methodid);
	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnFrontDisconnected(int nReason){
	cerr << "disconted" << nReason << endl;
}

void TraderSpi::execReqUserLogin(){
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = traderApi->ReqUserLogin(&req, ++iTdRequestID);
}

void TraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){	
	//cerr << "login info " << pRspInfo->ErrorID << endl;
	notifyRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast);
	execReqSettlementInfoConfirm();
}

void TraderSpi::notifyRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);

	const char* TradingDay = pRspUserLogin->TradingDay;
	const char* LoginTime = pRspUserLogin->LoginTime;
	const char* UserID = pRspUserLogin->UserID;
	const char* SHFETime = pRspUserLogin->SHFETime;

	jobject tradingDay = env->NewStringUTF(TradingDay);
	jobject loginTime = env->NewStringUTF(LoginTime);
	jobject userId = env->NewStringUTF(UserID);
	jobject shfeTime = env->NewStringUTF(SHFETime);

	jclass rspUserLoginCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspUserLoginField");
	jfieldID tradingDayField = env->GetFieldID(rspUserLoginCls,"tradingDay","Ljava/lang/String;"); 
	jfieldID loginTimeField = env->GetFieldID(rspUserLoginCls, "loginTime", "Ljava/lang/String;");
	jfieldID userIdField = env->GetFieldID(rspUserLoginCls, "userId", "Ljava/lang/String;");
	jfieldID shfeTimeField = env->GetFieldID(rspUserLoginCls, "shfeTime", "Ljava/lang/String;");

	jobject rspUserLoginObj = env->AllocObject(rspUserLoginCls); 
	env->SetObjectField(rspUserLoginObj, tradingDayField, tradingDay);
	env->SetObjectField(rspUserLoginObj, loginTimeField, loginTime);
	env->SetObjectField(rspUserLoginObj, userIdField, userId);
	env->SetObjectField(rspUserLoginObj, shfeTimeField, shfeTime);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);

	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspUserLogin", "(Lorg/zhps/hjctp/entity/CThostFtdcRspUserLoginField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");
	env->CallVoidMethod(jTraderSpi, methodid, rspUserLoginObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::execReqSettlementInfoConfirm(){
	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.InvestorID, INVESTOR_ID);
	int iResult = traderApi->ReqSettlementInfoConfirm(&req, ++iTdRequestID);	
}

void TraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	notifyRspSettlementInfoConfirm(pSettlementInfoConfirm, pRspInfo, nRequestID, bIsLast);
}

void TraderSpi::notifyRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);

	const char* BrokerID = pSettlementInfoConfirm->BrokerID;
	const char* InvestorID = pSettlementInfoConfirm->InvestorID;
	const char* ConfirmDate = pSettlementInfoConfirm->ConfirmDate;
	const char* ConfirmTime = pSettlementInfoConfirm->ConfirmTime;

	jobject brokerID = env->NewStringUTF(BrokerID);
	jobject investorID = env->NewStringUTF(InvestorID);
	jobject confirmDate = env->NewStringUTF(ConfirmDate);
	jobject confirmTime = env->NewStringUTF(ConfirmTime);

	jclass rspSettlementInfoConfirmCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcSettlementInfoConfirmField");
	jfieldID brokerIDField = env->GetFieldID(rspSettlementInfoConfirmCls,"brokerID","Ljava/lang/String;"); 
	jfieldID investorIDField = env->GetFieldID(rspSettlementInfoConfirmCls, "investorID", "Ljava/lang/String;");
	jfieldID confirmDateField = env->GetFieldID(rspSettlementInfoConfirmCls, "confirmDate", "Ljava/lang/String;");
	jfieldID confirmTimeField = env->GetFieldID(rspSettlementInfoConfirmCls, "confirmTime", "Ljava/lang/String;");

	jobject rspSettlementInfoConfirmObj = env->AllocObject(rspSettlementInfoConfirmCls); 
	env->SetObjectField(rspSettlementInfoConfirmObj, brokerIDField, brokerID);
	env->SetObjectField(rspSettlementInfoConfirmObj, investorIDField, investorID);
	env->SetObjectField(rspSettlementInfoConfirmObj, confirmDateField, confirmDate);
	env->SetObjectField(rspSettlementInfoConfirmObj, confirmTimeField, confirmTime);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);

	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspSettlementInfoConfirm", "(Lorg/zhps/hjctp/entity/CThostFtdcSettlementInfoConfirmField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");
	env->CallVoidMethod(jTraderSpi, methodid, rspSettlementInfoConfirmObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::queryTradingAccount(){
	CThostFtdcQryTradingAccountField tradingAccount;
	memset(&tradingAccount, 0, sizeof(tradingAccount));
	strcpy(tradingAccount.BrokerID, BROKER_ID);
	strcpy(tradingAccount.InvestorID, INVESTOR_ID);
	int iResult = traderApi->ReqQryTradingAccount(&tradingAccount, ++iTdRequestID);
}

void TraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
}





