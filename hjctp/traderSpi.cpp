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
	reqUserLogin();
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

void TraderSpi::reqUserLogin(){
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = traderApi->ReqUserLogin(&req, ++iTdRequestID);
	//cerr << "login result: " << iResult << endl;
}

void TraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){	
	/*cerr << "login info " << pRspInfo->ErrorID << endl;
	cerr << "tradding day " << pRspUserLogin->TradingDay << endl;
	cerr << "login FrontID " << pRspUserLogin->FrontID << endl;
	cerr << "login SessionID " << pRspUserLogin->SessionID << endl;
	cerr << "login MaxOrderRef " << pRspUserLogin->MaxOrderRef << endl;*/
	notifyRspUserLogin(pRspUserLogin, pRspInfo, nRequestID, bIsLast);
}

void TraderSpi::notifyRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast){
	JNIEnv *env; 
	traderJvm->AttachCurrentThread((void **)&env, NULL);

	const char* TradingDay = pRspUserLogin->TradingDay;
	const char* LoginTime = pRspUserLogin->LoginTime;
	const char* UserID = pRspUserLogin->UserID;
	const char* SHFETime = pRspUserLogin->SHFETime;
	const char* ErrorMsg = pRspInfo->ErrorMsg;

	jobject tradingDay = env->NewStringUTF(TradingDay);
	jobject loginTime = env->NewStringUTF(LoginTime);
	jobject userId = env->NewStringUTF(UserID);
	jobject shfeTime = env->NewStringUTF(SHFETime);
	jobject errorMsg = env->NewStringUTF(ErrorMsg);

	//cerr << "trading day: " << TradingDay << endl;

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
	jfieldID errorMsgField = env->GetFieldID(rspInfoCls,"errorMsg","Ljava/lang/String;"); 

	env->SetObjectField(rspInfoObj, errorMsgField, errorMsg);

	jclass traderSpiCls = env->GetObjectClass(jTraderSpi);
	jmethodID methodid = env->GetMethodID(traderSpiCls, "onRspUserLogin", "(Lorg/zhps/hjctp/entity/CThostFtdcRspUserLoginField;Lorg/zhps/hjctp/entity/CThostFtdcRspInfoField;IZ)V");
	env->CallVoidMethod(jTraderSpi, methodid, rspUserLoginObj, rspInfoObj, nRequestID, bIsLast);

	traderJvm->DetachCurrentThread();
}




