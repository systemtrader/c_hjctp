#include "ThostFtdcMdApi.h"
#include "jni.h"

class MdSpi:public CThostFtdcMdSpi{
private:
	CThostFtdcMdApi* mdApi;
	JNIEnv *env; 
	jobject jspi;
	jmethodID methodid;
public:

	MdSpi(CThostFtdcMdApi* pMdApi,JNIEnv *jenv, jobject jobj, jmethodID jmethodid):mdApi(pMdApi),env(jenv),jspi(jobj),methodid(jmethodid){}

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(int nReason);

	/*virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);*/
};