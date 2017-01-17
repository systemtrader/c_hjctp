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
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "tradingDay", 0, pRspUserLogin->TradingDay);
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "loginTime", 0, pRspUserLogin->LoginTime);
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "userId", 0, pRspUserLogin->UserID);
	Common::loadClass(env, &rspUserLoginCls, &rspUserLoginObj, "shfeTime", 0, pRspUserLogin->SHFETime);

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

	jclass settlementInfoConfirmCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcSettlementInfoConfirmField");
	jobject settlementInfoConfirmObj = env->AllocObject(settlementInfoConfirmCls); 
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &settlementInfoConfirmCls, &settlementInfoConfirmObj, "brokerID", 0, pSettlementInfoConfirm->BrokerID);
	Common::loadClass(env, &settlementInfoConfirmCls, &settlementInfoConfirmObj, "investorID", 0, pSettlementInfoConfirm->InvestorID);
	Common::loadClass(env, &settlementInfoConfirmCls, &settlementInfoConfirmObj, "confirmDate", 0, pSettlementInfoConfirm->ConfirmDate);
	Common::loadClass(env, &settlementInfoConfirmCls, &settlementInfoConfirmObj, "confirmTime", 0, pSettlementInfoConfirm->ConfirmTime);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, settlementInfoConfirmObj, rspInfoObj, nRequestID, bIsLast);

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
	
	jclass tradingAccountCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcTradingAccountField");
	jobject tradingAccountObj = env->AllocObject(tradingAccountCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "brokerID", 0, pTradingAccount->BrokerID);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "accountID", 0, pTradingAccount->AccountID);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "preCredit", 1, pTradingAccount->PreCredit);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "preDeposit", 1, pTradingAccount->PreDeposit);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "preBalance", 1, pTradingAccount->PreBalance);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "preMargin", 1, pTradingAccount->PreMargin);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "deposit", 1, pTradingAccount->Deposit);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "withdraw", 1, pTradingAccount->Withdraw);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "frozenMargin", 1, pTradingAccount->FrozenMargin);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "frozenCash", 1, pTradingAccount->FrozenCash);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "frozenCommission", 1, pTradingAccount->FrozenCommission);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "currMargin", 1, pTradingAccount->CurrMargin);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "cashIn", 1, pTradingAccount->CashIn);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "commission", 1, pTradingAccount->Commission);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "closeProfit", 1, pTradingAccount->CloseProfit);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "positionProfit", 1, pTradingAccount->PositionProfit);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "balance", 1, pTradingAccount->Balance);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "available", 1, pTradingAccount->Available);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "withdrawQuota", 1, pTradingAccount->WithdrawQuota);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "reserve", 1, pTradingAccount->Reserve);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "tradingDay", 0, pTradingAccount->TradingDay);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "settlementID", 2, pTradingAccount->SettlementID);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "credit", 1, pTradingAccount->Credit);
	Common::loadClass(env, &tradingAccountCls, &tradingAccountObj, "exchangeMargin", 1, pTradingAccount->ExchangeMargin);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, tradingAccountObj, rspInfoObj, nRequestID, bIsLast);

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

	jclass investorPositionCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcInvestorPositionField");
	jobject investorPositionObj = env->AllocObject(investorPositionCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "instrumentID", 0, pInvestorPosition->InstrumentID);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "brokerID", 0, pInvestorPosition->BrokerID);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "investorID", 0, pInvestorPosition->InvestorID);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "posiDirection", 0, pInvestorPosition->PosiDirection);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "hedgeFlag", 0, pInvestorPosition->HedgeFlag);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "positionDate", 0, pInvestorPosition->PositionDate);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "ydPosition", 2, pInvestorPosition->YdPosition);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "position", 2, pInvestorPosition->Position);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "longFrozen", 2, pInvestorPosition->LongFrozen);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "shortFrozen", 2, pInvestorPosition->ShortFrozen);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "longFrozenAmount", 1, pInvestorPosition->LongFrozenAmount);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "shortFrozenAmount", 1, pInvestorPosition->ShortFrozenAmount);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "openVolume", 2, pInvestorPosition->OpenVolume);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "closeVolume", 2, pInvestorPosition->CloseVolume);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "openAmount", 1, pInvestorPosition->OpenAmount);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "closeAmount", 1, pInvestorPosition->CloseAmount);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "positionCost", 1, pInvestorPosition->PositionCost);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "preMargin", 1, pInvestorPosition->PreMargin);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "useMargin", 1, pInvestorPosition->UseMargin);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "frozenMargin", 1, pInvestorPosition->FrozenMargin);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "frozenCash", 1, pInvestorPosition->FrozenCash);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "frozenCommission", 1, pInvestorPosition->FrozenCommission);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "commission", 1, pInvestorPosition->Commission);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "closeProfit", 1, pInvestorPosition->CloseProfit);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "positionProfit", 1, pInvestorPosition->PositionProfit);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "preSettlementPrice", 1, pInvestorPosition->PreSettlementPrice);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "settlementPrice", 1, pInvestorPosition->SettlementPrice);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "tradingDay", 0, pInvestorPosition->TradingDay);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "settlementID", 2, pInvestorPosition->SettlementID);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "openCost", 1, pInvestorPosition->OpenCost);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "exchangeMargin", 1, pInvestorPosition->ExchangeMargin);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "closeProfitByDate", 1, pInvestorPosition->CloseProfitByDate);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "closeProfitByTrade", 1, pInvestorPosition->CloseProfitByTrade);
	Common::loadClass(env, &investorPositionCls, &investorPositionObj, "todayPosition", 2, pInvestorPosition->TodayPosition);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, investorPositionObj, rspInfoObj, nRequestID, bIsLast);

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
	
	jclass investorPositionDetailCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcInvestorPositionDetailField");
	jobject investorPositionDetailObj = env->AllocObject(investorPositionDetailCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "instrumentID", 0, pInvestorPositionDetail->InstrumentID);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "brokerID", 0, pInvestorPositionDetail->BrokerID);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "investorID", 0, pInvestorPositionDetail->InvestorID);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "hedgeFlag", 2, pInvestorPositionDetail->HedgeFlag);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "direction", 2,  pInvestorPositionDetail->Direction);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "openDate", 0, pInvestorPositionDetail->OpenDate);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "tradeID", 0, pInvestorPositionDetail->TradeID);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "volume", 2,  pInvestorPositionDetail->Volume);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "openPrice", 1, pInvestorPositionDetail->OpenPrice);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "tradingDay", 0, pInvestorPositionDetail->TradingDay);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "settlementID", 2,  pInvestorPositionDetail->SettlementID);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "tradeType", 0,  pInvestorPositionDetail->TradeType);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "combInstrumentID", 0, pInvestorPositionDetail->CombInstrumentID);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "exchangeID", 0, pInvestorPositionDetail->ExchangeID);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "closeProfitByDate", 1, pInvestorPositionDetail->CloseProfitByDate);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "closeProfitByTrade", 1, pInvestorPositionDetail->CloseProfitByTrade);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "positionProfitByDate", 1, pInvestorPositionDetail->PositionProfitByDate);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "positionProfitByTrade", 1, pInvestorPositionDetail->PositionProfitByTrade);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "margin", 1, pInvestorPositionDetail->Margin);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "exchMargin", 1, pInvestorPositionDetail->ExchMargin);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "marginRateByMoney", 1, pInvestorPositionDetail->MarginRateByMoney);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "marginRateByVolume", 1, pInvestorPositionDetail->MarginRateByVolume);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "lastSettlementPrice", 1, pInvestorPositionDetail->LastSettlementPrice);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "settlementPrice", 1, pInvestorPositionDetail->SettlementPrice);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "closeVolume", 2,  pInvestorPositionDetail->CloseVolume);
	Common::loadClass(env, &investorPositionDetailCls, &investorPositionDetailObj, "closeAmount", 1, pInvestorPositionDetail->CloseAmount);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, investorPositionDetailObj, rspInfoObj, nRequestID, bIsLast);

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

	jclass orderCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcOrderField");
	jobject orderObj = env->AllocObject(orderCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &orderCls, &orderObj, "brokerID", 0, pOrder->BrokerID);
	Common::loadClass(env, &orderCls, &orderObj, "investorID", 0, pOrder->InvestorID);
	Common::loadClass(env, &orderCls, &orderObj, "instrumentID", 0, pOrder->InstrumentID);
	Common::loadClass(env, &orderCls, &orderObj, "orderRef", 0, pOrder->OrderRef);
	Common::loadClass(env, &orderCls, &orderObj, "userID", 0, pOrder->UserID);
	Common::loadClass(env, &orderCls, &orderObj, "orderPriceType", 0, pOrder->OrderPriceType);
	Common::loadClass(env, &orderCls, &orderObj, "direction", 0, pOrder->Direction);
	Common::loadClass(env, &orderCls, &orderObj, "combOffsetFlag", 0, pOrder->CombOffsetFlag);
	Common::loadClass(env, &orderCls, &orderObj, "combHedgeFlag", 0, pOrder->CombHedgeFlag);
	Common::loadClass(env, &orderCls, &orderObj, "limitPrice", 1, pOrder->LimitPrice);
	Common::loadClass(env, &orderCls, &orderObj, "volumeTotalOriginal", 2, pOrder->VolumeTotalOriginal);
	Common::loadClass(env, &orderCls, &orderObj, "timeCondition", 0, pOrder->TimeCondition);
	Common::loadClass(env, &orderCls, &orderObj, "gTDDate", 0, pOrder->GTDDate);
	Common::loadClass(env, &orderCls, &orderObj, "volumeCondition", 2, pOrder->VolumeCondition);
	Common::loadClass(env, &orderCls, &orderObj, "minVolume", 2,  pOrder->MinVolume);
	Common::loadClass(env, &orderCls, &orderObj, "contingentCondition", 0, pOrder->ContingentCondition);
	Common::loadClass(env, &orderCls, &orderObj, "stopPrice", 1, pOrder->StopPrice);
	Common::loadClass(env, &orderCls, &orderObj, "forceCloseReason", 0, pOrder->ForceCloseReason);
	Common::loadClass(env, &orderCls, &orderObj, "isAutoSuspend", 2,  pOrder->IsAutoSuspend);
	Common::loadClass(env, &orderCls, &orderObj, "businessUnit", 0, pOrder->BusinessUnit);
	Common::loadClass(env, &orderCls, &orderObj, "requestID", 2,  pOrder->RequestID);
	Common::loadClass(env, &orderCls, &orderObj, "orderLocalID", 0, pOrder->OrderLocalID);
	Common::loadClass(env, &orderCls, &orderObj, "exchangeID", 0, pOrder->ExchangeID);
	Common::loadClass(env, &orderCls, &orderObj, "participantID", 0, pOrder->ParticipantID);
	Common::loadClass(env, &orderCls, &orderObj, "clientID", 0, pOrder->ClientID);
	Common::loadClass(env, &orderCls, &orderObj, "exchangeInstID", 0, pOrder->ExchangeInstID);
	Common::loadClass(env, &orderCls, &orderObj, "traderID", 0, pOrder->TraderID);
	Common::loadClass(env, &orderCls, &orderObj, "installID", 2,  pOrder->InstallID);
	Common::loadClass(env, &orderCls, &orderObj, "orderSubmitStatus", 0, pOrder->OrderSubmitStatus);
	Common::loadClass(env, &orderCls, &orderObj, "notifySequence", 2,  pOrder->NotifySequence);
	Common::loadClass(env, &orderCls, &orderObj, "tradingDay", 0, pOrder->TradingDay);
	Common::loadClass(env, &orderCls, &orderObj, "settlementID", 2,  pOrder->SettlementID);
	Common::loadClass(env, &orderCls, &orderObj, "orderSysID", 0, pOrder->OrderSysID);
	Common::loadClass(env, &orderCls, &orderObj, "orderSource", 0, pOrder->OrderSource);
	Common::loadClass(env, &orderCls, &orderObj, "orderStatus", 0, pOrder->OrderStatus);
	Common::loadClass(env, &orderCls, &orderObj, "orderType", 0, pOrder->OrderType);
	Common::loadClass(env, &orderCls, &orderObj, "volumeTraded", 2,  pOrder->VolumeTraded);
	Common::loadClass(env, &orderCls, &orderObj, "volumeTotal", 2,  pOrder->VolumeTotal);
	Common::loadClass(env, &orderCls, &orderObj, "insertDate", 0, pOrder->InsertDate);
	Common::loadClass(env, &orderCls, &orderObj, "insertTime", 0, pOrder->InsertTime);
	Common::loadClass(env, &orderCls, &orderObj, "activeTime", 0, pOrder->ActiveTime);
	Common::loadClass(env, &orderCls, &orderObj, "suspendTime", 0, pOrder->SuspendTime);
	Common::loadClass(env, &orderCls, &orderObj, "updateTime", 0, pOrder->UpdateTime);
	Common::loadClass(env, &orderCls, &orderObj, "cancelTime", 0, pOrder->CancelTime);
	Common::loadClass(env, &orderCls, &orderObj, "activeTraderID", 0, pOrder->ActiveTraderID);
	Common::loadClass(env, &orderCls, &orderObj, "clearingPartID", 0, pOrder->ClearingPartID);
	Common::loadClass(env, &orderCls, &orderObj, "sequenceNo", 2,  pOrder->SequenceNo);
	Common::loadClass(env, &orderCls, &orderObj, "frontID", 2,  pOrder->FrontID);
	Common::loadClass(env, &orderCls, &orderObj, "sessionID", 2,  pOrder->SessionID);
	Common::loadClass(env, &orderCls, &orderObj, "userProductInfo", 0, pOrder->UserProductInfo);
	Common::loadClass(env, &orderCls, &orderObj, "statusMsg", 0, pOrder->StatusMsg);
	Common::loadClass(env, &orderCls, &orderObj, "userForceClose", 2,  pOrder->UserForceClose);
	Common::loadClass(env, &orderCls, &orderObj, "activeUserID", 0, pOrder->ActiveUserID);
	Common::loadClass(env, &orderCls, &orderObj, "brokerOrderSeq", 2,  pOrder->BrokerOrderSeq);
	Common::loadClass(env, &orderCls, &orderObj, "relativeOrderSysID", 0, pOrder->RelativeOrderSysID);
	Common::loadClass(env, &orderCls, &orderObj, "zCETotalTradedVolume", 2,  pOrder->ZCETotalTradedVolume);
	Common::loadClass(env, &orderCls, &orderObj, "isSwapOrder", 2,  pOrder->IsSwapOrder);
	Common::loadClass(env, &orderCls, &orderObj, "branchID", 0, pOrder->BranchID);
	Common::loadClass(env, &orderCls, &orderObj, "investUnitID", 0, pOrder->InvestUnitID);
	Common::loadClass(env, &orderCls, &orderObj, "accountID", 0, pOrder->AccountID);
	Common::loadClass(env, &orderCls, &orderObj, "iPAddress", 0, pOrder->IPAddress);
	Common::loadClass(env, &orderCls, &orderObj, "macAddress", 0, pOrder->MacAddress);

	env->CallVoidMethod(jTraderSpi, methodid, orderObj);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspOrderInsert", "(Lorg/zhps/hjctp/entity/CThostFtdcInputOrderField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");	

	if(pInputOrder == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL, nRequestID, bIsLast);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass inputOrderCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcInputOrderField");
	jobject inputOrderObj = env->AllocObject(inputOrderCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "brokerID", 0, pInputOrder->BrokerID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "investorID", 0, pInputOrder->InvestorID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "instrumentID", 0, pInputOrder->InstrumentID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "orderRef", 0, pInputOrder->OrderRef);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "userID", 0, pInputOrder->UserID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "orderPriceType", 0, pInputOrder->OrderPriceType);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "direction", 0, pInputOrder->Direction);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "combOffsetFlag", 0, pInputOrder->CombOffsetFlag);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "combHedgeFlag", 0, pInputOrder->CombHedgeFlag);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "limitPrice", 1, pInputOrder->LimitPrice);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "volumeTotalOriginal", 2, pInputOrder->VolumeTotalOriginal);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "timeCondition", 0, pInputOrder->TimeCondition);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "gTDDate", 0, pInputOrder->GTDDate);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "volumeCondition", 2, pInputOrder->VolumeCondition);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "minVolume", 2, pInputOrder->MinVolume);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "contingentCondition", 0, pInputOrder->ContingentCondition);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "stopPrice", 1, pInputOrder->StopPrice);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "forceCloseReason", 0, pInputOrder->ForceCloseReason);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "isAutoSuspend", 2, pInputOrder->IsAutoSuspend);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "businessUnit", 0, pInputOrder->BusinessUnit);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "requestID", 2, pInputOrder->RequestID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "userForceClose", 2, pInputOrder->UserForceClose);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "isSwapOrder", 2, pInputOrder->IsSwapOrder);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "exchangeID", 0, pInputOrder->ExchangeID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "investUnitID", 0, pInputOrder->InvestUnitID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "accountID", 0, pInputOrder->AccountID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "clientID", 0, pInputOrder->ClientID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "iPAddress", 0, pInputOrder->IPAddress);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "macAddress", 0, pInputOrder->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, inputOrderObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onErrRtnOrderInsert", "(Lorg/zhps/hjctp/entity/CThostFtdcInputOrderField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField)V");	

	if(pInputOrder == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass inputOrderCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcInputOrderField");
	jobject inputOrderObj = env->AllocObject(inputOrderCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "brokerID", 0, pInputOrder->BrokerID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "investorID", 0, pInputOrder->InvestorID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "instrumentID", 0, pInputOrder->InstrumentID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "orderRef", 0, pInputOrder->OrderRef);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "userID", 0, pInputOrder->UserID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "orderPriceType", 0, pInputOrder->OrderPriceType);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "direction", 0, pInputOrder->Direction);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "combOffsetFlag", 0, pInputOrder->CombOffsetFlag);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "combHedgeFlag", 0, pInputOrder->CombHedgeFlag);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "limitPrice", 1, pInputOrder->LimitPrice);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "volumeTotalOriginal", 2, pInputOrder->VolumeTotalOriginal);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "timeCondition", 0, pInputOrder->TimeCondition);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "gTDDate", 0, pInputOrder->GTDDate);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "volumeCondition", 2, pInputOrder->VolumeCondition);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "minVolume", 2, pInputOrder->MinVolume);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "contingentCondition", 0, &pInputOrder->ContingentCondition);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "stopPrice", 1, pInputOrder->StopPrice);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "forceCloseReason", 0, pInputOrder->ForceCloseReason);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "isAutoSuspend", 2, pInputOrder->IsAutoSuspend);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "businessUnit", 0, pInputOrder->BusinessUnit);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "requestID", 2, pInputOrder->RequestID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "userForceClose", 2, pInputOrder->UserForceClose);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "isSwapOrder", 2, pInputOrder->IsSwapOrder);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "exchangeID", 0, pInputOrder->ExchangeID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "investUnitID", 0, pInputOrder->InvestUnitID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "accountID", 0, pInputOrder->AccountID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "clientID", 0, pInputOrder->ClientID);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "iPAddress", 0, pInputOrder->IPAddress);
	Common::loadClass(env, &inputOrderCls, &inputOrderObj, "macAddress", 0, pInputOrder->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, inputOrderObj, rspInfoObj);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRtnTrade", "(Lorg/zhps/hjctp/entity/CThostFtdcTradeField)V");	

	if(pTrade == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass tradeCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcTradeField");
	jobject tradeObj = env->AllocObject(tradeCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &tradeCls, &tradeObj, "brokerID", 0, pTrade->BrokerID);
	Common::loadClass(env, &tradeCls, &tradeObj, "investorID", 0, pTrade->InvestorID);
	Common::loadClass(env, &tradeCls, &tradeObj, "instrumentID", 0, pTrade->InstrumentID);
	Common::loadClass(env, &tradeCls, &tradeObj, "orderRef", 0, pTrade->OrderRef);
	Common::loadClass(env, &tradeCls, &tradeObj, "userID", 0, pTrade->UserID);
	Common::loadClass(env, &tradeCls, &tradeObj, "exchangeID", 0, pTrade->ExchangeID);
	Common::loadClass(env, &tradeCls, &tradeObj, "tradeID", 0, pTrade->TradeID);
	Common::loadClass(env, &tradeCls, &tradeObj, "direction", 0, pTrade->Direction);
	Common::loadClass(env, &tradeCls, &tradeObj, "orderSysID", 0, pTrade->OrderSysID);
	Common::loadClass(env, &tradeCls, &tradeObj, "participantID", 0, pTrade->ParticipantID);
	Common::loadClass(env, &tradeCls, &tradeObj, "clientID", 0, pTrade->ClientID);
	Common::loadClass(env, &tradeCls, &tradeObj, "tradingRole", 0, pTrade->TradingRole);
	Common::loadClass(env, &tradeCls, &tradeObj, "exchangeInstID", 0, pTrade->ExchangeInstID);
	Common::loadClass(env, &tradeCls, &tradeObj, "offsetFlag", 0, pTrade->OffsetFlag);
	Common::loadClass(env, &tradeCls, &tradeObj, "hedgeFlag", 0, pTrade->HedgeFlag);
	Common::loadClass(env, &tradeCls, &tradeObj, "price", 1, pTrade->Price);
	Common::loadClass(env, &tradeCls, &tradeObj, "volume", 2, pTrade->Volume);
	Common::loadClass(env, &tradeCls, &tradeObj, "tradeDate", 0, pTrade->TradeDate);
	Common::loadClass(env, &tradeCls, &tradeObj, "tradeTime", 0, pTrade->TradeTime);
	Common::loadClass(env, &tradeCls, &tradeObj, "tradeType", 0, pTrade->TradeType);
	Common::loadClass(env, &tradeCls, &tradeObj, "priceSource", 0, pTrade->PriceSource);
	Common::loadClass(env, &tradeCls, &tradeObj, "traderID", 0, pTrade->TraderID);
	Common::loadClass(env, &tradeCls, &tradeObj, "orderLocalID", 0, pTrade->OrderLocalID);
	Common::loadClass(env, &tradeCls, &tradeObj, "clearingPartID", 0, pTrade->ClearingPartID);
	Common::loadClass(env, &tradeCls, &tradeObj, "businessUnit", 0, pTrade->BusinessUnit);
	Common::loadClass(env, &tradeCls, &tradeObj, "sequenceNo", 2, pTrade->SequenceNo);
	Common::loadClass(env, &tradeCls, &tradeObj, "tradingDay", 0, pTrade->TradingDay);
	Common::loadClass(env, &tradeCls, &tradeObj, "settlementID", 2, pTrade->SettlementID);
	Common::loadClass(env, &tradeCls, &tradeObj, "brokerOrderSeq", 2, pTrade->BrokerOrderSeq);
	Common::loadClass(env, &tradeCls, &tradeObj, "tradeSource", 0, pTrade->TradeSource);

	env->CallVoidMethod(jTraderSpi, methodid, tradeObj);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspOrderAction", "(Lorg/zhps/hjctp/entity/CThostFtdcInputOrderActionField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");	

	if(pInputOrderAction == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL, nRequestID, bIsLast);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass inputOrderActionCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcInputOrderActionField");
	jobject inputOrderActionObj = env->AllocObject(inputOrderActionCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "brokerID", 0, pInputOrderAction->BrokerID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "investorID", 0, pInputOrderAction->InvestorID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "orderActionRef", 2, pInputOrderAction->OrderActionRef);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "orderRef", 0, pInputOrderAction->OrderRef);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "requestID", 2, pInputOrderAction->RequestID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "frontID", 2, pInputOrderAction->FrontID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "sessionID", 2, pInputOrderAction->SessionID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "exchangeID", 0, pInputOrderAction->ExchangeID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "orderSysID", 0, pInputOrderAction->OrderSysID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "actionFlag", 0, pInputOrderAction->ActionFlag);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "limitPrice", 1, pInputOrderAction->LimitPrice);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "volumeChange", 2, pInputOrderAction->VolumeChange);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "userID", 0, pInputOrderAction->UserID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "instrumentID", 0, pInputOrderAction->InstrumentID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "investUnitID", 0, pInputOrderAction->InvestUnitID);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "iPAddress", 0, pInputOrderAction->IPAddress);
	Common::loadClass(env, &inputOrderActionCls, &inputOrderActionObj, "macAddress", 0, pInputOrderAction->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, inputOrderActionObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction, CThostFtdcRspInfoField *pRspInfo){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onErrRtnOrderAction", "(Lorg/zhps/hjctp/entity/CThostFtdcOrderActionField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField)V");	

	if(pOrderAction == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass orderActionCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcOrderActionField");
	jobject orderActionObj = env->AllocObject(orderActionCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &orderActionCls, &orderActionObj, "brokerID", 0, pOrderAction->BrokerID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "investorID", 0, pOrderAction->InvestorID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "orderActionRef", 2, pOrderAction->OrderActionRef);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "orderRef", 0, pOrderAction->OrderRef);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "requestID", 2, pOrderAction->RequestID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "frontID", 2, pOrderAction->FrontID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "sessionID", 2, pOrderAction->SessionID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "exchangeID", 0, pOrderAction->ExchangeID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "orderSysID", 0, pOrderAction->OrderSysID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "actionFlag", 0, pOrderAction->ActionFlag);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "limitPrice", 1, pOrderAction->LimitPrice);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "volumeChange", 2, pOrderAction->VolumeChange);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "actionDate", 0, pOrderAction->ActionDate);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "actionTime", 0, pOrderAction->ActionTime);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "traderID", 0, pOrderAction->TraderID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "installID", 2, pOrderAction->InstallID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "orderLocalID", 0, pOrderAction->OrderLocalID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "actionLocalID", 0, pOrderAction->ActionLocalID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "participantID", 0, pOrderAction->ParticipantID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "clientID", 0, pOrderAction->ClientID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "businessUnit", 0, pOrderAction->BusinessUnit);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "orderActionStatus", 0, pOrderAction->OrderActionStatus);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "userID", 0, pOrderAction->UserID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "statusMsg", 0, pOrderAction->StatusMsg);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "instrumentID", 0, pOrderAction->InstrumentID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "branchID", 0, pOrderAction->BranchID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "investUnitID", 0, pOrderAction->InvestUnitID);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "iPAddress", 0, pOrderAction->IPAddress);
	Common::loadClass(env, &orderActionCls, &orderActionObj, "macAddress", 0, pOrderAction->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, orderActionObj, rspInfoObj);

	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnRspQryOrder(CThostFtdcOrderField *pOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);
	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspQryOrder", "(Lorg/zhps/hjctp/entity/CThostFtdcOrderField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");	

	if(pOrder == 0){
		env->CallVoidMethod(jTraderSpi, methodid, NULL, NULL, nRequestID, bIsLast);	
		traderJvm->DetachCurrentThread();
		return;
	}

	jclass orderCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcOrderField");
	jobject orderObj = env->AllocObject(orderCls);
	//type: 0.string, 1.double, 2.int
	Common::loadClass(env, &orderCls, &orderObj, "brokerID", 0, pOrder->BrokerID);
	Common::loadClass(env, &orderCls, &orderObj, "investorID", 0, pOrder->InvestorID);
	Common::loadClass(env, &orderCls, &orderObj, "instrumentID", 0, pOrder->InstrumentID);
	Common::loadClass(env, &orderCls, &orderObj, "orderRef", 0, pOrder->OrderRef);
	Common::loadClass(env, &orderCls, &orderObj, "userID", 0, pOrder->UserID);
	Common::loadClass(env, &orderCls, &orderObj, "orderPriceType", 0, pOrder->OrderPriceType);
	Common::loadClass(env, &orderCls, &orderObj, "direction", 0, pOrder->Direction);
	Common::loadClass(env, &orderCls, &orderObj, "combOffsetFlag", 0, pOrder->CombOffsetFlag);
	Common::loadClass(env, &orderCls, &orderObj, "combHedgeFlag", 0, pOrder->CombHedgeFlag);
	Common::loadClass(env, &orderCls, &orderObj, "limitPrice", 1, pOrder->LimitPrice);
	Common::loadClass(env, &orderCls, &orderObj, "volumeTotalOriginal", 2, pOrder->VolumeTotalOriginal);
	Common::loadClass(env, &orderCls, &orderObj, "timeCondition", 0, pOrder->TimeCondition);
	Common::loadClass(env, &orderCls, &orderObj, "gTDDate", 0, pOrder->GTDDate);
	Common::loadClass(env, &orderCls, &orderObj, "volumeCondition", 2, pOrder->VolumeCondition);
	Common::loadClass(env, &orderCls, &orderObj, "minVolume", 2, pOrder->MinVolume);
	Common::loadClass(env, &orderCls, &orderObj, "contingentCondition", 0, pOrder->ContingentCondition);
	Common::loadClass(env, &orderCls, &orderObj, "stopPrice", 1, pOrder->StopPrice);
	Common::loadClass(env, &orderCls, &orderObj, "forceCloseReason", 0, pOrder->ForceCloseReason);
	Common::loadClass(env, &orderCls, &orderObj, "isAutoSuspend", 2, pOrder->IsAutoSuspend);
	Common::loadClass(env, &orderCls, &orderObj, "businessUnit", 0, pOrder->BusinessUnit);
	Common::loadClass(env, &orderCls, &orderObj, "requestID", 2, pOrder->RequestID);
	Common::loadClass(env, &orderCls, &orderObj, "orderLocalID", 0, pOrder->OrderLocalID);
	Common::loadClass(env, &orderCls, &orderObj, "exchangeID", 0, pOrder->ExchangeID);
	Common::loadClass(env, &orderCls, &orderObj, "participantID", 0, pOrder->ParticipantID);
	Common::loadClass(env, &orderCls, &orderObj, "clientID", 0, pOrder->ClientID);
	Common::loadClass(env, &orderCls, &orderObj, "exchangeInstID", 0, pOrder->ExchangeInstID);
	Common::loadClass(env, &orderCls, &orderObj, "traderID", 0, pOrder->TraderID);
	Common::loadClass(env, &orderCls, &orderObj, "installID", 2, pOrder->InstallID);
	Common::loadClass(env, &orderCls, &orderObj, "orderSubmitStatus", 0, pOrder->OrderSubmitStatus);
	Common::loadClass(env, &orderCls, &orderObj, "notifySequence", 2, pOrder->NotifySequence);
	Common::loadClass(env, &orderCls, &orderObj, "tradingDay", 0, pOrder->TradingDay);
	Common::loadClass(env, &orderCls, &orderObj, "settlementID", 2, pOrder->SettlementID);
	Common::loadClass(env, &orderCls, &orderObj, "orderSysID", 0, pOrder->OrderSysID);
	Common::loadClass(env, &orderCls, &orderObj, "orderSource", 0, pOrder->OrderSource);
	Common::loadClass(env, &orderCls, &orderObj, "orderStatus", 0, pOrder->OrderStatus);
	Common::loadClass(env, &orderCls, &orderObj, "orderType", 0, pOrder->OrderType);
	Common::loadClass(env, &orderCls, &orderObj, "volumeTraded", 2, pOrder->VolumeTraded);
	Common::loadClass(env, &orderCls, &orderObj, "volumeTotal", 2, pOrder->VolumeTotal);
	Common::loadClass(env, &orderCls, &orderObj, "insertDate", 0, pOrder->InsertDate);
	Common::loadClass(env, &orderCls, &orderObj, "insertTime", 0, pOrder->InsertTime);
	Common::loadClass(env, &orderCls, &orderObj, "activeTime", 0, pOrder->ActiveTime);
	Common::loadClass(env, &orderCls, &orderObj, "suspendTime", 0, pOrder->SuspendTime);
	Common::loadClass(env, &orderCls, &orderObj, "updateTime", 0, pOrder->UpdateTime);
	Common::loadClass(env, &orderCls, &orderObj, "cancelTime", 0, pOrder->CancelTime);
	Common::loadClass(env, &orderCls, &orderObj, "activeTraderID", 0, pOrder->ActiveTraderID);
	Common::loadClass(env, &orderCls, &orderObj, "clearingPartID", 0, pOrder->ClearingPartID);
	Common::loadClass(env, &orderCls, &orderObj, "sequenceNo", 2, pOrder->SequenceNo);
	Common::loadClass(env, &orderCls, &orderObj, "frontID", 2, pOrder->FrontID);
	Common::loadClass(env, &orderCls, &orderObj, "sessionID", 2, pOrder->SessionID);
	Common::loadClass(env, &orderCls, &orderObj, "userProductInfo", 0, pOrder->UserProductInfo);
	Common::loadClass(env, &orderCls, &orderObj, "statusMsg", 0, pOrder->StatusMsg);
	Common::loadClass(env, &orderCls, &orderObj, "userForceClose", 2, pOrder->UserForceClose);
	Common::loadClass(env, &orderCls, &orderObj, "activeUserID", 0, pOrder->ActiveUserID);
	Common::loadClass(env, &orderCls, &orderObj, "brokerOrderSeq", 2, pOrder->BrokerOrderSeq);
	Common::loadClass(env, &orderCls, &orderObj, "relativeOrderSysID", 0, pOrder->RelativeOrderSysID);
	Common::loadClass(env, &orderCls, &orderObj, "zCETotalTradedVolume", 2, pOrder->ZCETotalTradedVolume);
	Common::loadClass(env, &orderCls, &orderObj, "isSwapOrder", 2, pOrder->IsSwapOrder);
	Common::loadClass(env, &orderCls, &orderObj, "branchID", 0, pOrder->BranchID);
	Common::loadClass(env, &orderCls, &orderObj, "investUnitID", 0, pOrder->InvestUnitID);
	Common::loadClass(env, &orderCls, &orderObj, "accountID", 0, pOrder->AccountID);
	Common::loadClass(env, &orderCls, &orderObj, "iPAddress", 0, pOrder->IPAddress);
	Common::loadClass(env, &orderCls, &orderObj, "macAddress", 0, pOrder->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, orderObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}






