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

void TraderSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspQryInvestorPosition", "(Lorg/zhps/hjctp/entity/CThostFtdcInvestorPositionField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");

	if(pInvestorPosition == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL, nRequestID, bIsLast);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass rspQryInvestorPositionCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcInvestorPositionField");
	jobject rspQryInvestorPositionObj = env->AllocObject(rspQryInvestorPositionCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "instrumentID", 0, env->NewStringUTF(pInvestorPosition->InstrumentID), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "brokerID", 0, env->NewStringUTF(pInvestorPosition->BrokerID), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "investorID", 0, env->NewStringUTF(pInvestorPosition->InvestorID), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "posiDirection", 0, env->NewStringUTF(&pInvestorPosition->PosiDirection), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "hedgeFlag", 0, env->NewStringUTF(&pInvestorPosition->HedgeFlag), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "positionDate", 0, env->NewStringUTF(&pInvestorPosition->PositionDate), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "ydPosition", 2, NULL, 0, pInvestorPosition->YdPosition);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "position", 2, NULL, 0, pInvestorPosition->Position);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "longFrozen", 2, NULL, 0, pInvestorPosition->LongFrozen);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "shortFrozen", 2, NULL, 0, pInvestorPosition->ShortFrozen);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "longFrozenAmount", 1, NULL, pInvestorPosition->LongFrozenAmount, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "shortFrozenAmount", 1, NULL, pInvestorPosition->ShortFrozenAmount, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "openVolume", 2, NULL, 0, pInvestorPosition->OpenVolume);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "closeVolume", 2, NULL, 0, pInvestorPosition->CloseVolume);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "openAmount", 1, NULL, pInvestorPosition->OpenAmount, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "closeAmount", 1, NULL, pInvestorPosition->CloseAmount, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "positionCost", 1, NULL, pInvestorPosition->PositionCost, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "preMargin", 1, NULL, pInvestorPosition->PreMargin, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "useMargin", 1, NULL, pInvestorPosition->UseMargin, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "frozenMargin", 1, NULL, pInvestorPosition->FrozenMargin, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "frozenCash", 1, NULL, pInvestorPosition->FrozenCash, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "frozenCommission", 1, NULL, pInvestorPosition->FrozenCommission, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "commission", 1, NULL, pInvestorPosition->Commission, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "closeProfit", 1, NULL, pInvestorPosition->CloseProfit, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "positionProfit", 1, NULL, pInvestorPosition->PositionProfit, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "preSettlementPrice", 1, NULL, pInvestorPosition->PreSettlementPrice, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "settlementPrice", 1, NULL, pInvestorPosition->SettlementPrice, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "tradingDay", 0, env->NewStringUTF(pInvestorPosition->TradingDay), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "settlementID", 2, NULL, 0, pInvestorPosition->SettlementID);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "openCost", 1, NULL, pInvestorPosition->OpenCost, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "exchangeMargin", 1, NULL, pInvestorPosition->ExchangeMargin, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "closeProfitByDate", 1, NULL, pInvestorPosition->CloseProfitByDate, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "closeProfitByTrade", 1, NULL, pInvestorPosition->CloseProfitByTrade, 0);
	Common::loadClass(env, &rspQryInvestorPositionCls, &rspQryInvestorPositionObj, "todayPosition", 2, NULL, 0, pInvestorPosition->TodayPosition);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, rspQryInvestorPositionObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnRspQryInvestorPositionDetail(CThostFtdcInvestorPositionDetailField *pInvestorPositionDetail, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspQryInvestorPositionDetail", "(Lorg/zhps/hjctp/entity/CThostFtdcInvestorPositionDetailField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");

	if(pInvestorPositionDetail == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL, nRequestID, bIsLast);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass rspQryInvestorPositionDetailCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcInvestorPositionDetailField");
	jobject rspQryInvestorPositionDetailObj = env->AllocObject(rspQryInvestorPositionDetailCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "instrumentID", 0, env->NewStringUTF(pInvestorPositionDetail->InstrumentID), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "brokerID", 0, env->NewStringUTF(pInvestorPositionDetail->BrokerID), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "investorID", 0, env->NewStringUTF(pInvestorPositionDetail->InvestorID), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "hedgeFlag", 0, env->NewStringUTF(&pInvestorPositionDetail->HedgeFlag), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "direction", 0, env->NewStringUTF(&pInvestorPositionDetail->Direction), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "openDate", 0, env->NewStringUTF(pInvestorPositionDetail->OpenDate), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "tradeID", 0, env->NewStringUTF(pInvestorPositionDetail->TradeID), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "volume", 2, NULL, 0, pInvestorPositionDetail->Volume);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "openPrice", 1, NULL, pInvestorPositionDetail->OpenPrice, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "tradingDay", 0, env->NewStringUTF(pInvestorPositionDetail->TradingDay), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "settlementID", 2, NULL, 0, pInvestorPositionDetail->SettlementID);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "tradeType", 0, env->NewStringUTF(&pInvestorPositionDetail->TradeType), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "combInstrumentID", 0, env->NewStringUTF(pInvestorPositionDetail->CombInstrumentID), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "exchangeID", 0, env->NewStringUTF(pInvestorPositionDetail->ExchangeID), 0, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "closeProfitByDate", 1, NULL, pInvestorPositionDetail->CloseProfitByDate, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "closeProfitByTrade", 1, NULL, pInvestorPositionDetail->CloseProfitByTrade, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "positionProfitByDate", 1, NULL, pInvestorPositionDetail->PositionProfitByDate, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "positionProfitByTrade", 1, NULL, pInvestorPositionDetail->PositionProfitByTrade, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "margin", 1, NULL, pInvestorPositionDetail->Margin, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "exchMargin", 1, NULL, pInvestorPositionDetail->ExchMargin, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "marginRateByMoney", 1, NULL, pInvestorPositionDetail->MarginRateByMoney, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "marginRateByVolume", 1, NULL, pInvestorPositionDetail->MarginRateByVolume, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "lastSettlementPrice", 1, NULL, pInvestorPositionDetail->LastSettlementPrice, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "settlementPrice", 1, NULL, pInvestorPositionDetail->SettlementPrice, 0);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "closeVolume", 2, NULL, 0, pInvestorPositionDetail->CloseVolume);
	Common::loadClass(env, &rspQryInvestorPositionDetailCls, &rspQryInvestorPositionDetailObj, "closeAmount", 1, NULL, pInvestorPositionDetail->CloseAmount, 0);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, rspQryInvestorPositionDetailObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRtnOrder", "(Lorg/zhps/hjctp/entity/CThostFtdcOrderField)V");

	if(pOrder == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass rtnOrderCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcOrderField");
	jobject rtnOrderObj = env->AllocObject(rtnOrderCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "brokerID", 0, env->NewStringUTF(pOrder->BrokerID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "investorID", 0, env->NewStringUTF(pOrder->InvestorID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "instrumentID", 0, env->NewStringUTF(pOrder->InstrumentID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "orderRef", 0, env->NewStringUTF(pOrder->OrderRef), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "userID", 0, env->NewStringUTF(pOrder->UserID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "orderPriceType", 0, env->NewStringUTF(&pOrder->OrderPriceType), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "direction", 0, env->NewStringUTF(&pOrder->Direction), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "combOffsetFlag", 0, env->NewStringUTF(pOrder->CombOffsetFlag), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "combHedgeFlag", 0, env->NewStringUTF(pOrder->CombHedgeFlag), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "limitPrice", 1, NULL, pOrder->LimitPrice, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "volumeTotalOriginal", 2, NULL, 0, pOrder->VolumeTotalOriginal);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "timeCondition", 0, env->NewStringUTF(&pOrder->TimeCondition), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "gTDDate", 0, env->NewStringUTF(pOrder->GTDDate), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "volumeCondition", 2, NULL, 0, pOrder->VolumeCondition);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "minVolume", 2, NULL, 0, pOrder->MinVolume);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "contingentCondition", 0, env->NewStringUTF(&pOrder->ContingentCondition), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "stopPrice", 1, NULL, pOrder->StopPrice, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "forceCloseReason", 0, env->NewStringUTF(&pOrder->ForceCloseReason), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "isAutoSuspend", 2, NULL, 0, pOrder->IsAutoSuspend);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "businessUnit", 0, env->NewStringUTF(pOrder->BusinessUnit), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "requestID", 2, NULL, 0, pOrder->RequestID);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "orderLocalID", 0, env->NewStringUTF(pOrder->OrderLocalID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "exchangeID", 0, env->NewStringUTF(pOrder->ExchangeID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "participantID", 0, env->NewStringUTF(pOrder->ParticipantID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "clientID", 0, env->NewStringUTF(pOrder->ClientID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "exchangeInstID", 0, env->NewStringUTF(pOrder->ExchangeInstID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "traderID", 0, env->NewStringUTF(pOrder->TraderID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "installID", 2, NULL, 0, pOrder->InstallID);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "orderSubmitStatus", 0, env->NewStringUTF(&pOrder->OrderSubmitStatus), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "notifySequence", 2, NULL, 0, pOrder->NotifySequence);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "tradingDay", 0, env->NewStringUTF(pOrder->TradingDay), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "settlementID", 2, NULL, 0, pOrder->SettlementID);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "orderSysID", 0, env->NewStringUTF(pOrder->OrderSysID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "orderSource", 0, env->NewStringUTF(&pOrder->OrderSource), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "orderStatus", 0, env->NewStringUTF(&pOrder->OrderStatus), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "orderType", 0, env->NewStringUTF(&pOrder->OrderType), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "volumeTraded", 2, NULL, 0, pOrder->VolumeTraded);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "volumeTotal", 2, NULL, 0, pOrder->VolumeTotal);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "insertDate", 0, env->NewStringUTF(pOrder->InsertDate), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "insertTime", 0, env->NewStringUTF(pOrder->InsertTime), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "activeTime", 0, env->NewStringUTF(pOrder->ActiveTime), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "suspendTime", 0, env->NewStringUTF(pOrder->SuspendTime), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "updateTime", 0, env->NewStringUTF(pOrder->UpdateTime), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "cancelTime", 0, env->NewStringUTF(pOrder->CancelTime), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "activeTraderID", 0, env->NewStringUTF(pOrder->ActiveTraderID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "clearingPartID", 0, env->NewStringUTF(pOrder->ClearingPartID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "sequenceNo", 2, NULL, 0, pOrder->SequenceNo);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "frontID", 2, NULL, 0, pOrder->FrontID);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "sessionID", 2, NULL, 0, pOrder->SessionID);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "userProductInfo", 0, env->NewStringUTF(pOrder->UserProductInfo), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "statusMsg", 0, env->NewStringUTF(pOrder->StatusMsg), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "userForceClose", 2, NULL, 0, pOrder->UserForceClose);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "activeUserID", 0, env->NewStringUTF(pOrder->ActiveUserID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "brokerOrderSeq", 2, NULL, 0, pOrder->BrokerOrderSeq);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "relativeOrderSysID", 0, env->NewStringUTF(pOrder->RelativeOrderSysID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "zCETotalTradedVolume", 2, NULL, 0, pOrder->ZCETotalTradedVolume);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "isSwapOrder", 2, NULL, 0, pOrder->IsSwapOrder);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "branchID", 0, env->NewStringUTF(pOrder->BranchID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "investUnitID", 0, env->NewStringUTF(pOrder->InvestUnitID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "accountID", 0, env->NewStringUTF(pOrder->AccountID), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "iPAddress", 0, env->NewStringUTF(pOrder->IPAddress), 0, 0);
	Common::loadClass(env, &rtnOrderCls, &rtnOrderObj, "macAddress", 0, env->NewStringUTF(pOrder->MacAddress), 0, 0);

	env->CallVoidMethod(jTraderSpi, methodid, rtnOrderObj);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspOrderInsert", "(Lorg/zhps/hjctp/entity/CThostFtdcInputOrderField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");	

	if(pInputOrder == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass rspOrderInsertCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcInputOrderField");
	jobject rspOrderInsertObj = env->AllocObject(rspOrderInsertCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "brokerID", 0, env->NewStringUTF(pInputOrder->BrokerID), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "investorID", 0, env->NewStringUTF(pInputOrder->InvestorID), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "instrumentID", 0, env->NewStringUTF(pInputOrder->InstrumentID), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "orderRef", 0, env->NewStringUTF(pInputOrder->OrderRef), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "userID", 0, env->NewStringUTF(pInputOrder->UserID), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "orderPriceType", 0, env->NewStringUTF(&pInputOrder->OrderPriceType), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "direction", 0, env->NewStringUTF(&pInputOrder->Direction), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "combOffsetFlag", 0, env->NewStringUTF(pInputOrder->CombOffsetFlag), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "combHedgeFlag", 0, env->NewStringUTF(pInputOrder->CombHedgeFlag), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "limitPrice", 1, NULL, pInputOrder->LimitPrice, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "volumeTotalOriginal", 2, NULL, 0, pInputOrder->VolumeTotalOriginal);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "timeCondition", 0, env->NewStringUTF(&pInputOrder->TimeCondition), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "gTDDate", 0, env->NewStringUTF(pInputOrder->GTDDate), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "volumeCondition", 2, NULL, 0, pInputOrder->VolumeCondition);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "minVolume", 2, NULL, 0, pInputOrder->MinVolume);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "contingentCondition", 0, env->NewStringUTF(&pInputOrder->ContingentCondition), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "stopPrice", 1, NULL, pInputOrder->StopPrice, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "forceCloseReason", 0, env->NewStringUTF(&pInputOrder->ForceCloseReason), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "isAutoSuspend", 2, NULL, 0, pInputOrder->IsAutoSuspend);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "businessUnit", 0, env->NewStringUTF(pInputOrder->BusinessUnit), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "requestID", 2, NULL, 0, pInputOrder->RequestID);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "userForceClose", 2, NULL, 0, pInputOrder->UserForceClose);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "isSwapOrder", 2, NULL, 0, pInputOrder->IsSwapOrder);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "exchangeID", 0, env->NewStringUTF(pInputOrder->ExchangeID), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "investUnitID", 0, env->NewStringUTF(pInputOrder->InvestUnitID), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "accountID", 0, env->NewStringUTF(pInputOrder->AccountID), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "clientID", 0, env->NewStringUTF(pInputOrder->ClientID), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "iPAddress", 0, env->NewStringUTF(pInputOrder->IPAddress), 0, 0);
	Common::loadClass(env, &rspOrderInsertCls, &rspOrderInsertObj, "macAddress", 0, env->NewStringUTF(pInputOrder->MacAddress), 0, 0);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, rspOrderInsertObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}







