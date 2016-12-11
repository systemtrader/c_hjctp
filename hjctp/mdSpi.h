#include "ThostFtdcMdApi.h"
#include "jni.h"

class MdSpi:public CThostFtdcMdSpi{
private:
	CThostFtdcMdApi* mdApi;
public:

	MdSpi(CThostFtdcMdApi* pMdApi):mdApi(pMdApi){}

	virtual void OnFrontConnected();

	virtual void OnFrontDisconnected(int nReason);

	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
private:
	void reqUserLogin();

	void subScribeMarketData();

	void notifyFrontConnected();

	void notifyRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	void notifyRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);
};