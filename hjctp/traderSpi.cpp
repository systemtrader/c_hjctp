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

	//cerr << cls << endl;
	Common::releaseLocalRef(env, 1, cls);
	//cerr << cls << endl;
	traderJvm->DetachCurrentThread();
}

void TraderSpi::OnFrontDisconnected(int nReason){
	//cerr << "disconted" << nReason << endl;
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
	Common::assemString(env, &rspUserLoginCls, &rspUserLoginObj, "tradingDay", pRspUserLogin->TradingDay);
	Common::assemString(env, &rspUserLoginCls, &rspUserLoginObj, "loginTime", pRspUserLogin->LoginTime);
	Common::assemString(env, &rspUserLoginCls, &rspUserLoginObj, "userId", pRspUserLogin->UserID);
	Common::assemString(env, &rspUserLoginCls, &rspUserLoginObj, "shfeTime", pRspUserLogin->SHFETime);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	Common::assemInt(env, &rspInfoCls, &rspInfoObj, "errorID", pRspInfo->ErrorID);
	Common::assemCstring(env, &rspInfoCls, &rspInfoObj, "errorMsg", pRspInfo->ErrorMsg);

	env->CallVoidMethod(jTraderSpi, methodid, rspUserLoginObj, rspInfoObj, nRequestID, bIsLast);

	Common::releaseLocalRef(env, 5, traderSpiCls, rspUserLoginCls, rspUserLoginObj, rspInfoCls, rspInfoObj);
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
	Common::assemString(env, &settlementInfoConfirmCls, &settlementInfoConfirmObj, "brokerID", pSettlementInfoConfirm->BrokerID);
	Common::assemString(env, &settlementInfoConfirmCls, &settlementInfoConfirmObj, "investorID", pSettlementInfoConfirm->InvestorID);
	Common::assemString(env, &settlementInfoConfirmCls, &settlementInfoConfirmObj, "confirmDate", pSettlementInfoConfirm->ConfirmDate);
	Common::assemString(env, &settlementInfoConfirmCls, &settlementInfoConfirmObj, "confirmTime", pSettlementInfoConfirm->ConfirmTime);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	Common::assemInt(env, &rspInfoCls, &rspInfoObj, "errorID", pRspInfo->ErrorID);
	Common::assemCstring(env, &rspInfoCls, &rspInfoObj, "errorMsg", pRspInfo->ErrorMsg);
	
	env->CallVoidMethod(jTraderSpi, methodid, settlementInfoConfirmObj, rspInfoObj, nRequestID, bIsLast);

	Common::releaseLocalRef(env, 5, traderSpiCls, settlementInfoConfirmCls, settlementInfoConfirmObj, rspInfoCls, rspInfoObj);
	traderJvm->DetachCurrentThread();
}

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
	Common::assemString(env, &tradingAccountCls, &tradingAccountObj, "brokerID", pTradingAccount->BrokerID);
	Common::assemString(env, &tradingAccountCls, &tradingAccountObj, "accountID", pTradingAccount->AccountID);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "preCredit", pTradingAccount->PreCredit);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "preDeposit", pTradingAccount->PreDeposit);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "preBalance", pTradingAccount->PreBalance);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "preMargin", pTradingAccount->PreMargin);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "deposit", pTradingAccount->Deposit);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "withdraw", pTradingAccount->Withdraw);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "frozenMargin", pTradingAccount->FrozenMargin);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "frozenCash", pTradingAccount->FrozenCash);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "frozenCommission", pTradingAccount->FrozenCommission);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "currMargin", pTradingAccount->CurrMargin);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "cashIn", pTradingAccount->CashIn);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "commission", pTradingAccount->Commission);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "closeProfit", pTradingAccount->CloseProfit);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "positionProfit", pTradingAccount->PositionProfit);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "balance", pTradingAccount->Balance);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "available", pTradingAccount->Available);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "withdrawQuota", pTradingAccount->WithdrawQuota);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "reserve", pTradingAccount->Reserve);
	Common::assemString(env, &tradingAccountCls, &tradingAccountObj, "tradingDay", pTradingAccount->TradingDay);
	Common::assemInt(env, &tradingAccountCls, &tradingAccountObj, "settlementID", pTradingAccount->SettlementID);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "credit", pTradingAccount->Credit);
	Common::assemDouble(env, &tradingAccountCls, &tradingAccountObj, "exchangeMargin", pTradingAccount->ExchangeMargin);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, tradingAccountObj, rspInfoObj, nRequestID, bIsLast);

	Common::releaseLocalRef(env, 5, traderSpiCls, tradingAccountCls, tradingAccountObj, rspInfoCls, rspInfoObj);
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
	Common::assemString(env, &investorPositionCls, &investorPositionObj, "instrumentID", pInvestorPosition->InstrumentID);
	Common::assemString(env, &investorPositionCls, &investorPositionObj, "brokerID", pInvestorPosition->BrokerID);
	Common::assemString(env, &investorPositionCls, &investorPositionObj, "investorID", pInvestorPosition->InvestorID);
	Common::assemChar(env, &investorPositionCls, &investorPositionObj, "posiDirection", pInvestorPosition->PosiDirection);
	Common::assemChar(env, &investorPositionCls, &investorPositionObj, "hedgeFlag", pInvestorPosition->HedgeFlag);
	Common::assemChar(env, &investorPositionCls, &investorPositionObj, "positionDate", pInvestorPosition->PositionDate);
	Common::assemInt(env, &investorPositionCls, &investorPositionObj, "ydPosition", pInvestorPosition->YdPosition);
	Common::assemInt(env, &investorPositionCls, &investorPositionObj, "position", pInvestorPosition->Position);
	Common::assemInt(env, &investorPositionCls, &investorPositionObj, "longFrozen", pInvestorPosition->LongFrozen);
	Common::assemInt(env, &investorPositionCls, &investorPositionObj, "shortFrozen", pInvestorPosition->ShortFrozen);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "longFrozenAmount", pInvestorPosition->LongFrozenAmount);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "shortFrozenAmount", pInvestorPosition->ShortFrozenAmount);
	Common::assemInt(env, &investorPositionCls, &investorPositionObj, "openVolume", pInvestorPosition->OpenVolume);
	Common::assemInt(env, &investorPositionCls, &investorPositionObj, "closeVolume", pInvestorPosition->CloseVolume);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "openAmount", pInvestorPosition->OpenAmount);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "closeAmount", pInvestorPosition->CloseAmount);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "positionCost", pInvestorPosition->PositionCost);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "preMargin", pInvestorPosition->PreMargin);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "useMargin", pInvestorPosition->UseMargin);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "frozenMargin", pInvestorPosition->FrozenMargin);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "frozenCash", pInvestorPosition->FrozenCash);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "frozenCommission", pInvestorPosition->FrozenCommission);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "commission", pInvestorPosition->Commission);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "closeProfit", pInvestorPosition->CloseProfit);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "positionProfit", pInvestorPosition->PositionProfit);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "preSettlementPrice", pInvestorPosition->PreSettlementPrice);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "settlementPrice", pInvestorPosition->SettlementPrice);
	Common::assemString(env, &investorPositionCls, &investorPositionObj, "tradingDay", pInvestorPosition->TradingDay);
	Common::assemInt(env, &investorPositionCls, &investorPositionObj, "settlementID", pInvestorPosition->SettlementID);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "openCost", pInvestorPosition->OpenCost);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "exchangeMargin", pInvestorPosition->ExchangeMargin);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "closeProfitByDate", pInvestorPosition->CloseProfitByDate);
	Common::assemDouble(env, &investorPositionCls, &investorPositionObj, "closeProfitByTrade", pInvestorPosition->CloseProfitByTrade);
	Common::assemInt(env, &investorPositionCls, &investorPositionObj, "todayPosition", pInvestorPosition->TodayPosition);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, investorPositionObj, rspInfoObj, nRequestID, bIsLast);

	Common::releaseLocalRef(env, 5, traderSpiCls, investorPositionCls, investorPositionObj, rspInfoCls, rspInfoObj);
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
	Common::assemString(env, &investorPositionDetailCls, &investorPositionDetailObj, "instrumentID", pInvestorPositionDetail->InstrumentID);
	Common::assemString(env, &investorPositionDetailCls, &investorPositionDetailObj, "brokerID", pInvestorPositionDetail->BrokerID);
	Common::assemString(env, &investorPositionDetailCls, &investorPositionDetailObj, "investorID", pInvestorPositionDetail->InvestorID);
	Common::assemChar(env, &investorPositionDetailCls, &investorPositionDetailObj, "hedgeFlag", pInvestorPositionDetail->HedgeFlag);
	Common::assemChar(env, &investorPositionDetailCls, &investorPositionDetailObj, "direction", pInvestorPositionDetail->Direction);
	Common::assemString(env, &investorPositionDetailCls, &investorPositionDetailObj, "openDate", pInvestorPositionDetail->OpenDate);
	Common::assemString(env, &investorPositionDetailCls, &investorPositionDetailObj, "tradeID", pInvestorPositionDetail->TradeID);
	Common::assemInt(env, &investorPositionDetailCls, &investorPositionDetailObj, "volume", pInvestorPositionDetail->Volume);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "openPrice", pInvestorPositionDetail->OpenPrice);
	Common::assemString(env, &investorPositionDetailCls, &investorPositionDetailObj, "tradingDay", pInvestorPositionDetail->TradingDay);
	Common::assemInt(env, &investorPositionDetailCls, &investorPositionDetailObj, "settlementID", pInvestorPositionDetail->SettlementID);
	Common::assemChar(env, &investorPositionDetailCls, &investorPositionDetailObj, "tradeType", pInvestorPositionDetail->TradeType);
	Common::assemString(env, &investorPositionDetailCls, &investorPositionDetailObj, "combInstrumentID", pInvestorPositionDetail->CombInstrumentID);
	Common::assemString(env, &investorPositionDetailCls, &investorPositionDetailObj, "exchangeID", pInvestorPositionDetail->ExchangeID);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "closeProfitByDate", pInvestorPositionDetail->CloseProfitByDate);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "closeProfitByTrade", pInvestorPositionDetail->CloseProfitByTrade);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "positionProfitByDate", pInvestorPositionDetail->PositionProfitByDate);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "positionProfitByTrade", pInvestorPositionDetail->PositionProfitByTrade);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "margin", pInvestorPositionDetail->Margin);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "exchMargin", pInvestorPositionDetail->ExchMargin);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "marginRateByMoney", pInvestorPositionDetail->MarginRateByMoney);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "marginRateByVolume", pInvestorPositionDetail->MarginRateByVolume);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "lastSettlementPrice", pInvestorPositionDetail->LastSettlementPrice);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "settlementPrice", pInvestorPositionDetail->SettlementPrice);
	Common::assemInt(env, &investorPositionDetailCls, &investorPositionDetailObj, "closeVolume", pInvestorPositionDetail->CloseVolume);
	Common::assemDouble(env, &investorPositionDetailCls, &investorPositionDetailObj, "closeAmount", pInvestorPositionDetail->CloseAmount);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, investorPositionDetailObj, rspInfoObj, nRequestID, bIsLast);

	Common::releaseLocalRef(env, 5, traderSpiCls, investorPositionDetailCls, investorPositionDetailObj, rspInfoCls, rspInfoObj);
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
	Common::assemString(env, &orderCls, &orderObj, "brokerID", pOrder->BrokerID);
	Common::assemString(env, &orderCls, &orderObj, "investorID", pOrder->InvestorID);
	Common::assemString(env, &orderCls, &orderObj, "instrumentID", pOrder->InstrumentID);
	Common::assemString(env, &orderCls, &orderObj, "orderRef", pOrder->OrderRef);
	Common::assemString(env, &orderCls, &orderObj, "userID", pOrder->UserID);
	Common::assemChar(env, &orderCls, &orderObj, "orderPriceType", pOrder->OrderPriceType);
	Common::assemChar(env, &orderCls, &orderObj, "direction", pOrder->Direction);
	Common::assemString(env, &orderCls, &orderObj, "combOffsetFlag", pOrder->CombOffsetFlag);
	Common::assemString(env, &orderCls, &orderObj, "combHedgeFlag", pOrder->CombHedgeFlag);
	Common::assemDouble(env, &orderCls, &orderObj, "limitPrice", pOrder->LimitPrice);
	Common::assemInt(env, &orderCls, &orderObj, "volumeTotalOriginal", pOrder->VolumeTotalOriginal);
	Common::assemChar(env, &orderCls, &orderObj, "timeCondition", pOrder->TimeCondition);
	Common::assemString(env, &orderCls, &orderObj, "gTDDate", pOrder->GTDDate);
	Common::assemInt(env, &orderCls, &orderObj, "volumeCondition", pOrder->VolumeCondition);
	Common::assemInt(env, &orderCls, &orderObj, "minVolume", pOrder->MinVolume);
	Common::assemChar(env, &orderCls, &orderObj, "contingentCondition", pOrder->ContingentCondition);
	Common::assemDouble(env, &orderCls, &orderObj, "stopPrice", pOrder->StopPrice);
	Common::assemChar(env, &orderCls, &orderObj, "forceCloseReason", pOrder->ForceCloseReason);
	Common::assemInt(env, &orderCls, &orderObj, "isAutoSuspend", pOrder->IsAutoSuspend);
	Common::assemString(env, &orderCls, &orderObj, "businessUnit", pOrder->BusinessUnit);
	Common::assemInt(env, &orderCls, &orderObj, "requestID", pOrder->RequestID);
	Common::assemString(env, &orderCls, &orderObj, "orderLocalID", pOrder->OrderLocalID);
	Common::assemString(env, &orderCls, &orderObj, "exchangeID", pOrder->ExchangeID);
	Common::assemString(env, &orderCls, &orderObj, "participantID", pOrder->ParticipantID);
	Common::assemString(env, &orderCls, &orderObj, "clientID", pOrder->ClientID);
	Common::assemString(env, &orderCls, &orderObj, "exchangeInstID", pOrder->ExchangeInstID);
	Common::assemString(env, &orderCls, &orderObj, "traderID", pOrder->TraderID);
	Common::assemInt(env, &orderCls, &orderObj, "installID", pOrder->InstallID);
	Common::assemChar(env, &orderCls, &orderObj, "orderSubmitStatus", pOrder->OrderSubmitStatus);
	Common::assemInt(env, &orderCls, &orderObj, "notifySequence", pOrder->NotifySequence);
	Common::assemString(env, &orderCls, &orderObj, "tradingDay", pOrder->TradingDay);
	Common::assemInt(env, &orderCls, &orderObj, "settlementID", pOrder->SettlementID);
	Common::assemString(env, &orderCls, &orderObj, "orderSysID", pOrder->OrderSysID);
	Common::assemChar(env, &orderCls, &orderObj, "orderSource", pOrder->OrderSource);
	Common::assemChar(env, &orderCls, &orderObj, "orderStatus", pOrder->OrderStatus);
	Common::assemChar(env, &orderCls, &orderObj, "orderType", pOrder->OrderType);
	Common::assemInt(env, &orderCls, &orderObj, "volumeTraded", pOrder->VolumeTraded);
	Common::assemInt(env, &orderCls, &orderObj, "volumeTotal", pOrder->VolumeTotal);
	Common::assemString(env, &orderCls, &orderObj, "insertDate", pOrder->InsertDate);
	Common::assemString(env, &orderCls, &orderObj, "insertTime", pOrder->InsertTime);
	Common::assemString(env, &orderCls, &orderObj, "activeTime", pOrder->ActiveTime);
	Common::assemString(env, &orderCls, &orderObj, "suspendTime", pOrder->SuspendTime);
	Common::assemString(env, &orderCls, &orderObj, "updateTime", pOrder->UpdateTime);
	Common::assemString(env, &orderCls, &orderObj, "cancelTime", pOrder->CancelTime);
	Common::assemString(env, &orderCls, &orderObj, "activeTraderID", pOrder->ActiveTraderID);
	Common::assemString(env, &orderCls, &orderObj, "clearingPartID", pOrder->ClearingPartID);
	Common::assemInt(env, &orderCls, &orderObj, "sequenceNo", pOrder->SequenceNo);
	Common::assemInt(env, &orderCls, &orderObj, "frontID", pOrder->FrontID);
	Common::assemInt(env, &orderCls, &orderObj, "sessionID", pOrder->SessionID);
	Common::assemString(env, &orderCls, &orderObj, "userProductInfo", pOrder->UserProductInfo);
	Common::assemCstring(env, &orderCls, &orderObj, "statusMsg", pOrder->StatusMsg);
	Common::assemInt(env, &orderCls, &orderObj, "userForceClose", pOrder->UserForceClose);
	Common::assemString(env, &orderCls, &orderObj, "activeUserID", pOrder->ActiveUserID);
	Common::assemInt(env, &orderCls, &orderObj, "brokerOrderSeq", pOrder->BrokerOrderSeq);
	Common::assemString(env, &orderCls, &orderObj, "relativeOrderSysID", pOrder->RelativeOrderSysID);
	Common::assemInt(env, &orderCls, &orderObj, "zCETotalTradedVolume", pOrder->ZCETotalTradedVolume);
	Common::assemInt(env, &orderCls, &orderObj, "isSwapOrder", pOrder->IsSwapOrder);
	Common::assemString(env, &orderCls, &orderObj, "branchID", pOrder->BranchID);
	Common::assemString(env, &orderCls, &orderObj, "investUnitID", pOrder->InvestUnitID);
	Common::assemString(env, &orderCls, &orderObj, "accountID", pOrder->AccountID);
	Common::assemString(env, &orderCls, &orderObj, "iPAddress", pOrder->IPAddress);
	Common::assemString(env, &orderCls, &orderObj, "macAddress", pOrder->MacAddress);

	env->CallVoidMethod(jTraderSpi, methodid, orderObj);

	Common::releaseLocalRef(env, 3, traderSpiCls, orderCls, orderObj);
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
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "brokerID", pInputOrder->BrokerID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "investorID", pInputOrder->InvestorID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "instrumentID", pInputOrder->InstrumentID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "orderRef", pInputOrder->OrderRef);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "userID", pInputOrder->UserID);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "orderPriceType", pInputOrder->OrderPriceType);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "direction", pInputOrder->Direction);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "combOffsetFlag", pInputOrder->CombOffsetFlag);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "combHedgeFlag", pInputOrder->CombHedgeFlag);
	Common::assemDouble(env, &inputOrderCls, &inputOrderObj, "limitPrice", pInputOrder->LimitPrice);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "volumeTotalOriginal", pInputOrder->VolumeTotalOriginal);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "timeCondition", pInputOrder->TimeCondition);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "gTDDate", pInputOrder->GTDDate);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "volumeCondition", pInputOrder->VolumeCondition);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "minVolume", pInputOrder->MinVolume);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "contingentCondition", pInputOrder->ContingentCondition);
	Common::assemDouble(env, &inputOrderCls, &inputOrderObj, "stopPrice", pInputOrder->StopPrice);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "forceCloseReason", pInputOrder->ForceCloseReason);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "isAutoSuspend", pInputOrder->IsAutoSuspend);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "businessUnit", pInputOrder->BusinessUnit);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "requestID", pInputOrder->RequestID);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "userForceClose", pInputOrder->UserForceClose);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "isSwapOrder", pInputOrder->IsSwapOrder);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "exchangeID", pInputOrder->ExchangeID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "investUnitID", pInputOrder->InvestUnitID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "accountID", pInputOrder->AccountID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "clientID", pInputOrder->ClientID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "iPAddress", pInputOrder->IPAddress);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "macAddress", pInputOrder->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, inputOrderObj, rspInfoObj, nRequestID, bIsLast);

	Common::releaseLocalRef(env, 5, traderSpiCls, inputOrderCls, inputOrderObj, rspInfoCls, rspInfoObj);
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
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "brokerID", pInputOrder->BrokerID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "investorID", pInputOrder->InvestorID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "instrumentID", pInputOrder->InstrumentID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "orderRef", pInputOrder->OrderRef);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "userID", pInputOrder->UserID);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "orderPriceType", pInputOrder->OrderPriceType);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "direction", pInputOrder->Direction);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "combOffsetFlag", pInputOrder->CombOffsetFlag);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "combHedgeFlag", pInputOrder->CombHedgeFlag);
	Common::assemDouble(env, &inputOrderCls, &inputOrderObj, "limitPrice", pInputOrder->LimitPrice);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "volumeTotalOriginal", pInputOrder->VolumeTotalOriginal);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "timeCondition", pInputOrder->TimeCondition);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "gTDDate", pInputOrder->GTDDate);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "volumeCondition", pInputOrder->VolumeCondition);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "minVolume", pInputOrder->MinVolume);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "contingentCondition", pInputOrder->ContingentCondition);
	Common::assemDouble(env, &inputOrderCls, &inputOrderObj, "stopPrice", pInputOrder->StopPrice);
	Common::assemChar(env, &inputOrderCls, &inputOrderObj, "forceCloseReason", pInputOrder->ForceCloseReason);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "isAutoSuspend", pInputOrder->IsAutoSuspend);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "businessUnit", pInputOrder->BusinessUnit);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "requestID", pInputOrder->RequestID);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "userForceClose", pInputOrder->UserForceClose);
	Common::assemInt(env, &inputOrderCls, &inputOrderObj, "isSwapOrder", pInputOrder->IsSwapOrder);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "exchangeID", pInputOrder->ExchangeID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "investUnitID", pInputOrder->InvestUnitID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "accountID", pInputOrder->AccountID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "clientID", pInputOrder->ClientID);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "iPAddress", pInputOrder->IPAddress);
	Common::assemString(env, &inputOrderCls, &inputOrderObj, "macAddress", pInputOrder->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, inputOrderObj, rspInfoObj);

	Common::releaseLocalRef(env, 5, traderSpiCls, inputOrderCls, inputOrderObj, rspInfoCls, rspInfoObj);
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
	Common::assemString(env, &tradeCls, &tradeObj, "brokerID", pTrade->BrokerID);
	Common::assemString(env, &tradeCls, &tradeObj, "investorID", pTrade->InvestorID);
	Common::assemString(env, &tradeCls, &tradeObj, "instrumentID", pTrade->InstrumentID);
	Common::assemString(env, &tradeCls, &tradeObj, "orderRef", pTrade->OrderRef);
	Common::assemString(env, &tradeCls, &tradeObj, "userID", pTrade->UserID);
	Common::assemString(env, &tradeCls, &tradeObj, "exchangeID", pTrade->ExchangeID);
	Common::assemString(env, &tradeCls, &tradeObj, "tradeID", pTrade->TradeID);
	Common::assemChar(env, &tradeCls, &tradeObj, "direction", pTrade->Direction);
	Common::assemString(env, &tradeCls, &tradeObj, "orderSysID", pTrade->OrderSysID);
	Common::assemString(env, &tradeCls, &tradeObj, "participantID", pTrade->ParticipantID);
	Common::assemString(env, &tradeCls, &tradeObj, "clientID", pTrade->ClientID);
	Common::assemChar(env, &tradeCls, &tradeObj, "tradingRole", pTrade->TradingRole);
	Common::assemString(env, &tradeCls, &tradeObj, "exchangeInstID", pTrade->ExchangeInstID);
	Common::assemChar(env, &tradeCls, &tradeObj, "offsetFlag", pTrade->OffsetFlag);
	Common::assemChar(env, &tradeCls, &tradeObj, "hedgeFlag", pTrade->HedgeFlag);
	Common::assemDouble(env, &tradeCls, &tradeObj, "price", pTrade->Price);
	Common::assemInt(env, &tradeCls, &tradeObj, "volume", pTrade->Volume);
	Common::assemString(env, &tradeCls, &tradeObj, "tradeDate", pTrade->TradeDate);
	Common::assemString(env, &tradeCls, &tradeObj, "tradeTime", pTrade->TradeTime);
	Common::assemChar(env, &tradeCls, &tradeObj, "tradeType", pTrade->TradeType);
	Common::assemChar(env, &tradeCls, &tradeObj, "priceSource", pTrade->PriceSource);
	Common::assemString(env, &tradeCls, &tradeObj, "traderID", pTrade->TraderID);
	Common::assemString(env, &tradeCls, &tradeObj, "orderLocalID", pTrade->OrderLocalID);
	Common::assemString(env, &tradeCls, &tradeObj, "clearingPartID", pTrade->ClearingPartID);
	Common::assemString(env, &tradeCls, &tradeObj, "businessUnit", pTrade->BusinessUnit);
	Common::assemInt(env, &tradeCls, &tradeObj, "sequenceNo", pTrade->SequenceNo);
	Common::assemString(env, &tradeCls, &tradeObj, "tradingDay", pTrade->TradingDay);
	Common::assemInt(env, &tradeCls, &tradeObj, "settlementID", pTrade->SettlementID);
	Common::assemInt(env, &tradeCls, &tradeObj, "brokerOrderSeq", pTrade->BrokerOrderSeq);
	Common::assemChar(env, &tradeCls, &tradeObj, "tradeSource", pTrade->TradeSource);

	env->CallVoidMethod(jTraderSpi, methodid, tradeObj);

	Common::releaseLocalRef(env, 3, traderSpiCls, tradeCls, tradeObj);
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
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "brokerID", pInputOrderAction->BrokerID);
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "investorID", pInputOrderAction->InvestorID);
	Common::assemInt(env, &inputOrderActionCls, &inputOrderActionObj, "orderActionRef", pInputOrderAction->OrderActionRef);
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "orderRef", pInputOrderAction->OrderRef);
	Common::assemInt(env, &inputOrderActionCls, &inputOrderActionObj, "requestID", pInputOrderAction->RequestID);
	Common::assemInt(env, &inputOrderActionCls, &inputOrderActionObj, "frontID", pInputOrderAction->FrontID);
	Common::assemInt(env, &inputOrderActionCls, &inputOrderActionObj, "sessionID", pInputOrderAction->SessionID);
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "exchangeID", pInputOrderAction->ExchangeID);
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "orderSysID", pInputOrderAction->OrderSysID);
	Common::assemChar(env, &inputOrderActionCls, &inputOrderActionObj, "actionFlag", pInputOrderAction->ActionFlag);
	Common::assemDouble(env, &inputOrderActionCls, &inputOrderActionObj, "limitPrice", pInputOrderAction->LimitPrice);
	Common::assemInt(env, &inputOrderActionCls, &inputOrderActionObj, "volumeChange", pInputOrderAction->VolumeChange);
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "userID", pInputOrderAction->UserID);
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "instrumentID", pInputOrderAction->InstrumentID);
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "investUnitID", pInputOrderAction->InvestUnitID);
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "iPAddress", pInputOrderAction->IPAddress);
	Common::assemString(env, &inputOrderActionCls, &inputOrderActionObj, "macAddress", pInputOrderAction->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, inputOrderActionObj, rspInfoObj, nRequestID, bIsLast);

	Common::releaseLocalRef(env, 5, traderSpiCls, inputOrderActionCls, inputOrderActionObj, rspInfoCls, rspInfoObj);
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
	Common::assemString(env, &orderActionCls, &orderActionObj, "brokerID", pOrderAction->BrokerID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "investorID", pOrderAction->InvestorID);
	Common::assemDouble(env, &orderActionCls, &orderActionObj, "orderActionRef", pOrderAction->OrderActionRef);
	Common::assemString(env, &orderActionCls, &orderActionObj, "orderRef", pOrderAction->OrderRef);
	Common::assemDouble(env, &orderActionCls, &orderActionObj, "requestID", pOrderAction->RequestID);
	Common::assemDouble(env, &orderActionCls, &orderActionObj, "frontID", pOrderAction->FrontID);
	Common::assemDouble(env, &orderActionCls, &orderActionObj, "sessionID", pOrderAction->SessionID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "exchangeID", pOrderAction->ExchangeID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "orderSysID", pOrderAction->OrderSysID);
	Common::assemChar(env, &orderActionCls, &orderActionObj, "actionFlag", pOrderAction->ActionFlag);
	Common::assemDouble(env, &orderActionCls, &orderActionObj, "limitPrice", pOrderAction->LimitPrice);
	Common::assemDouble(env, &orderActionCls, &orderActionObj, "volumeChange", pOrderAction->VolumeChange);
	Common::assemString(env, &orderActionCls, &orderActionObj, "actionDate", pOrderAction->ActionDate);
	Common::assemString(env, &orderActionCls, &orderActionObj, "actionTime", pOrderAction->ActionTime);
	Common::assemString(env, &orderActionCls, &orderActionObj, "traderID", pOrderAction->TraderID);
	Common::assemDouble(env, &orderActionCls, &orderActionObj, "installID", pOrderAction->InstallID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "orderLocalID", pOrderAction->OrderLocalID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "actionLocalID", pOrderAction->ActionLocalID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "participantID", pOrderAction->ParticipantID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "clientID", pOrderAction->ClientID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "businessUnit", pOrderAction->BusinessUnit);
	Common::assemChar(env, &orderActionCls, &orderActionObj, "orderActionStatus", pOrderAction->OrderActionStatus);
	Common::assemString(env, &orderActionCls, &orderActionObj, "userID", pOrderAction->UserID);
	Common::assemCstring(env, &orderActionCls, &orderActionObj, "statusMsg", pOrderAction->StatusMsg);
	Common::assemString(env, &orderActionCls, &orderActionObj, "instrumentID", pOrderAction->InstrumentID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "branchID", pOrderAction->BranchID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "investUnitID", pOrderAction->InvestUnitID);
	Common::assemString(env, &orderActionCls, &orderActionObj, "iPAddress", pOrderAction->IPAddress);
	Common::assemString(env, &orderActionCls, &orderActionObj, "macAddress", pOrderAction->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, orderActionObj, rspInfoObj);

	Common::releaseLocalRef(env, 5, traderSpiCls, orderActionCls, orderActionObj, rspInfoCls, rspInfoObj);
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
	Common::assemString(env, &orderCls, &orderObj, "brokerID", pOrder->BrokerID);
	Common::assemString(env, &orderCls, &orderObj, "investorID", pOrder->InvestorID);
	Common::assemString(env, &orderCls, &orderObj, "instrumentID", pOrder->InstrumentID);
	Common::assemString(env, &orderCls, &orderObj, "orderRef", pOrder->OrderRef);
	Common::assemString(env, &orderCls, &orderObj, "userID", pOrder->UserID);
	Common::assemChar(env, &orderCls, &orderObj, "orderPriceType", pOrder->OrderPriceType);
	Common::assemChar(env, &orderCls, &orderObj, "direction", pOrder->Direction);
	Common::assemString(env, &orderCls, &orderObj, "combOffsetFlag", pOrder->CombOffsetFlag);
	Common::assemString(env, &orderCls, &orderObj, "combHedgeFlag", pOrder->CombHedgeFlag);
	Common::assemDouble(env, &orderCls, &orderObj, "limitPrice", pOrder->LimitPrice);
	Common::assemDouble(env, &orderCls, &orderObj, "volumeTotalOriginal", pOrder->VolumeTotalOriginal);
	Common::assemChar(env, &orderCls, &orderObj, "timeCondition", pOrder->TimeCondition);
	Common::assemString(env, &orderCls, &orderObj, "gTDDate", pOrder->GTDDate);
	Common::assemDouble(env, &orderCls, &orderObj, "volumeCondition", pOrder->VolumeCondition);
	Common::assemDouble(env, &orderCls, &orderObj, "minVolume", pOrder->MinVolume);
	Common::assemChar(env, &orderCls, &orderObj, "contingentCondition", pOrder->ContingentCondition);
	Common::assemDouble(env, &orderCls, &orderObj, "stopPrice", pOrder->StopPrice);
	Common::assemChar(env, &orderCls, &orderObj, "forceCloseReason", pOrder->ForceCloseReason);
	Common::assemDouble(env, &orderCls, &orderObj, "isAutoSuspend", pOrder->IsAutoSuspend);
	Common::assemString(env, &orderCls, &orderObj, "businessUnit", pOrder->BusinessUnit);
	Common::assemDouble(env, &orderCls, &orderObj, "requestID", pOrder->RequestID);
	Common::assemString(env, &orderCls, &orderObj, "orderLocalID", pOrder->OrderLocalID);
	Common::assemString(env, &orderCls, &orderObj, "exchangeID", pOrder->ExchangeID);
	Common::assemString(env, &orderCls, &orderObj, "participantID", pOrder->ParticipantID);
	Common::assemString(env, &orderCls, &orderObj, "clientID", pOrder->ClientID);
	Common::assemString(env, &orderCls, &orderObj, "exchangeInstID", pOrder->ExchangeInstID);
	Common::assemString(env, &orderCls, &orderObj, "traderID", pOrder->TraderID);
	Common::assemDouble(env, &orderCls, &orderObj, "installID", pOrder->InstallID);
	Common::assemChar(env, &orderCls, &orderObj, "orderSubmitStatus", pOrder->OrderSubmitStatus);
	Common::assemDouble(env, &orderCls, &orderObj, "notifySequence", pOrder->NotifySequence);
	Common::assemString(env, &orderCls, &orderObj, "tradingDay", pOrder->TradingDay);
	Common::assemDouble(env, &orderCls, &orderObj, "settlementID", pOrder->SettlementID);
	Common::assemString(env, &orderCls, &orderObj, "orderSysID", pOrder->OrderSysID);
	Common::assemChar(env, &orderCls, &orderObj, "orderSource", pOrder->OrderSource);
	Common::assemChar(env, &orderCls, &orderObj, "orderStatus", pOrder->OrderStatus);
	Common::assemChar(env, &orderCls, &orderObj, "orderType", pOrder->OrderType);
	Common::assemDouble(env, &orderCls, &orderObj, "volumeTraded", pOrder->VolumeTraded);
	Common::assemDouble(env, &orderCls, &orderObj, "volumeTotal", pOrder->VolumeTotal);
	Common::assemString(env, &orderCls, &orderObj, "insertDate", pOrder->InsertDate);
	Common::assemString(env, &orderCls, &orderObj, "insertTime", pOrder->InsertTime);
	Common::assemString(env, &orderCls, &orderObj, "activeTime", pOrder->ActiveTime);
	Common::assemString(env, &orderCls, &orderObj, "suspendTime", pOrder->SuspendTime);
	Common::assemString(env, &orderCls, &orderObj, "updateTime", pOrder->UpdateTime);
	Common::assemString(env, &orderCls, &orderObj, "cancelTime", pOrder->CancelTime);
	Common::assemString(env, &orderCls, &orderObj, "activeTraderID", pOrder->ActiveTraderID);
	Common::assemString(env, &orderCls, &orderObj, "clearingPartID", pOrder->ClearingPartID);
	Common::assemDouble(env, &orderCls, &orderObj, "sequenceNo", pOrder->SequenceNo);
	Common::assemDouble(env, &orderCls, &orderObj, "frontID", pOrder->FrontID);
	Common::assemDouble(env, &orderCls, &orderObj, "sessionID", pOrder->SessionID);
	Common::assemString(env, &orderCls, &orderObj, "userProductInfo", pOrder->UserProductInfo);
	Common::assemCstring(env, &orderCls, &orderObj, "statusMsg", pOrder->StatusMsg);
	Common::assemDouble(env, &orderCls, &orderObj, "userForceClose", pOrder->UserForceClose);
	Common::assemString(env, &orderCls, &orderObj, "activeUserID", pOrder->ActiveUserID);
	Common::assemDouble(env, &orderCls, &orderObj, "brokerOrderSeq", pOrder->BrokerOrderSeq);
	Common::assemString(env, &orderCls, &orderObj, "relativeOrderSysID", pOrder->RelativeOrderSysID);
	Common::assemDouble(env, &orderCls, &orderObj, "zCETotalTradedVolume", pOrder->ZCETotalTradedVolume);
	Common::assemDouble(env, &orderCls, &orderObj, "isSwapOrder", pOrder->IsSwapOrder);
	Common::assemString(env, &orderCls, &orderObj, "branchID", pOrder->BranchID);
	Common::assemString(env, &orderCls, &orderObj, "investUnitID", pOrder->InvestUnitID);
	Common::assemString(env, &orderCls, &orderObj, "accountID", pOrder->AccountID);
	Common::assemString(env, &orderCls, &orderObj, "iPAddress", pOrder->IPAddress);
	Common::assemString(env, &orderCls, &orderObj, "macAddress", pOrder->MacAddress);

	jclass rspInfoCls = env->FindClass("org/zhps/hjctp/entity/CThostFtdcRspInfoField");
	jobject rspInfoObj = env->AllocObject(rspInfoCls);
	
	env->CallVoidMethod(jTraderSpi, methodid, orderObj, rspInfoObj, nRequestID, bIsLast);

	Common::releaseLocalRef(env, 5, traderSpiCls, orderCls, orderObj, rspInfoCls, rspInfoObj);
	traderJvm->DetachCurrentThread();
}