#include "StdAfx.h"
#include "traderSpi.h"
#include <iostream>
#include "ThostFtdcTraderApi.h"
#include "org_zhps_hjctp_jni_NativeLoader.h"
#include "jni.h"
#include "common.h"
using namespace std;

extern TThostFtdcBrokerIDType BROKER_ID;						
extern TThostFtdcInvestorIDType INVESTOR_ID;						
extern TThostFtdcPasswordType  PASSWORD;

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
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspUserLogin", "(Lorg/zhps/hjctp/entity/CThostFtdcRspUserLoginField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");

	if(pRspUserLogin == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL, nRequestID, bIsLast);
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass rspUserLoginCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspUserLoginField");
	jobject rspUserLoginObj = env->AllocObject(rspUserLoginCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "tradingDay", 0, env->NewStringUTF(pRspUserLogin->TradingDay),0, 0);
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "loginTime", 0, env->NewStringUTF(pRspUserLogin->LoginTime),0, 0);
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "userId", 0, env->NewStringUTF(pRspUserLogin->UserID),0, 0);
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "shfeTime", 0, env->NewStringUTF(pRspUserLogin->SHFETime),0, 0);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);

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
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspSettlementInfoConfirm", "(Lorg/zhps/hjctp/entity/CThostFtdcSettlementInfoConfirmField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");

	if(pSettlementInfoConfirm == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL, nRequestID, bIsLast);	
		traderJvm->DetachCurrentThread();
		return;	
	}

	jclass rspSettlementInfoConfirmCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcSettlementInfoConfirmField");
	jobject rspSettlementInfoConfirmObj = env->AllocObject(rspSettlementInfoConfirmCls); 
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &rspSettlementInfoConfirmCls, &rspSettlementInfoConfirmObj, "brokerID", 0, env->NewStringUTF(pSettlementInfoConfirm->BrokerID),0, 0);
	Common::loadClass(env, &rspSettlementInfoConfirmCls, &rspSettlementInfoConfirmObj, "investorID", 0, env->NewStringUTF(pSettlementInfoConfirm->InvestorID),0, 0);
	Common::loadClass(env, &rspSettlementInfoConfirmCls, &rspSettlementInfoConfirmObj, "confirmDate", 0, env->NewStringUTF(pSettlementInfoConfirm->ConfirmDate),0, 0);
	Common::loadClass(env, &rspSettlementInfoConfirmCls, &rspSettlementInfoConfirmObj, "confirmTime", 0, env->NewStringUTF(pSettlementInfoConfirm->ConfirmTime),0, 0);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, rspSettlementInfoConfirmObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}

//int TraderSpi::queryTradingAccount(){
//	CThostFtdcQryTradingAccountField tradingAccount;
//	memset(&tradingAccount, 0, sizeof(tradingAccount));
//	strcpy(tradingAccount.BrokerID, BROKER_ID);
//	strcpy(tradingAccount.InvestorID, INVESTOR_ID);
//	return traderApi->ReqQryTradingAccount(&tradingAccount, ++iTdRequestID);
//}

void TraderSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspQryTradingAccount", "(Lorg/zhps/hjctp/entity/CThostFtdcTradingAccountField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");

	if(pTradingAccount == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL, nRequestID, bIsLast);	
		traderJvm->DetachCurrentThread();
		return;
	}
	
	jclass rspQryTradingAccountCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcTradingAccountField");
	jobject rspQryTradingAccountObj = env->AllocObject(rspQryTradingAccountCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "brokerID", 0, env->NewStringUTF(pTradingAccount->BrokerID), 0, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "accountID", 0, env->NewStringUTF(pTradingAccount->AccountID), 0, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "preCredit", 1, NULL, pTradingAccount->PreCredit, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "preDeposit", 1, NULL, pTradingAccount->PreDeposit, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "preBalance", 1, NULL, pTradingAccount->PreBalance, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "preMargin", 1, NULL, pTradingAccount->PreMargin, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "deposit", 1, NULL, pTradingAccount->Deposit, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "withdraw", 1, NULL, pTradingAccount->Withdraw, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "frozenMargin", 1, NULL, pTradingAccount->FrozenMargin, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "frozenCash", 1, NULL, pTradingAccount->FrozenCash, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "frozenCommission", 1, NULL, pTradingAccount->FrozenCommission, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "currMargin", 1, NULL, pTradingAccount->CurrMargin, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "cashIn", 1, NULL, pTradingAccount->CashIn, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "commission", 1, NULL, pTradingAccount->Commission, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "closeProfit", 1, NULL, pTradingAccount->CloseProfit, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "positionProfit", 1, NULL, pTradingAccount->PositionProfit, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "balance", 1, NULL, pTradingAccount->Balance, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "available", 1, NULL, pTradingAccount->Available, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "withdrawQuota", 1, NULL, pTradingAccount->WithdrawQuota, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "reserve", 1, NULL, pTradingAccount->Reserve, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "tradingDay", 0, env->NewStringUTF(pTradingAccount->TradingDay), 0, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "settlementID", 2, NULL, 0, pTradingAccount->SettlementID);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "credit", 1, NULL, pTradingAccount->Credit, 0);
	Common::loadClass(env, &rspQryTradingAccountCls, &rspQryTradingAccountObj, "exchangeMargin", 1, NULL, pTradingAccount->ExchangeMargin, 0);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, rspQryTradingAccountObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}







