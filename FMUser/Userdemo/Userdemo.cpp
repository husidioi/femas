#include "stdafx.h"
#include <stdio.h> 
#include <string.h>
#include <float.h>
#include "UstpFtdcMduserApi.h"

class CSimpleHandler : public CUstpFtdcMduserSpi
{
public:
	CSimpleHandler(CUstpFtdcMduserApi *pUserApi) : m_pUserApi(pUserApi) {} 

	~CSimpleHandler() {} 
	
	void OnFrontConnected()
	{
		CUstpFtdcReqUserLoginField reqUserLogin;
		strcpy_s(reqUserLogin.TradingDay, m_pUserApi->GetTradingDay());
		strcpy_s(reqUserLogin.BrokerID, "1004");
		strcpy_s(reqUserLogin.UserID, "0193763201");
		strcpy_s(reqUserLogin.Password, "ba8700525eafa378b569774b6ae4678f");
		m_pUserApi->ReqUserLogin(&reqUserLogin, 0);  
	}   

	void OnFrontDisconnected()
	{
		printf("OnFrontDisconnected. \n");
	}

	void OnRspUserLogin(CUstpFtdcRspUserLoginField *pRspUserLogin, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)   
	{
		printf("OnRspUserLogin:\n");
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg); 
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast);  
		if (pRspInfo->ErrorID != 0)    
		{
			printf("Failed to login, errorcode=%d errormsg=%s requestid=%d chain=%d", pRspInfo->ErrorID, pRspInfo->ErrorMsg, nRequestID, bIsLast);
		}
		char * contracts[3]={"","",""};   
		contracts[0]="IF1509";   
		contracts[1]="IF1510";   
		contracts[2]="IF1512";   
		m_pUserApi->SubMarketData(contracts, 3);  
		char * uncontracts[2]={"",""};   
		uncontracts[0]="IF1511";   
		uncontracts[1]="IF1601";   
		m_pUserApi->UnSubMarketData(uncontracts, 2);  
	}

	void OnRtnDepthMarketData(CUstpFtdcDepthMarketDataField *pMarketData)   
	{
		printf("%s,%s,%d,",pMarketData->InstrumentID,pMarketData->UpdateTime,pMarketData->UpdateMillisec);
		if (pMarketData->AskPrice1==DBL_MAX)
			printf("%s,","");   
		else    
			printf("%f,",pMarketData->AskPrice1);      
		if (pMarketData->BidPrice1==DBL_MAX)   
			printf("%s \n","");   
		else    
			printf("%f \n",pMarketData->BidPrice1);  
	}

	void OnRspError(CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)   
	{
		printf("OnRspError:\n");   
		printf("ErrorCode=[%d], ErrorMsg=[%s]\n", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
		printf("RequestID=[%d], Chain=[%d]\n", nRequestID, bIsLast); 
	}

	void OnRspSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)  
	{
		printf("Sub 返回订阅合约：%s \n",pSpecificInstrument->InstrumentID); 
	}

	void OnRspUnSubMarketData(CUstpFtdcSpecificInstrumentField *pSpecificInstrument, CUstpFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)  
	{
		printf("UnSub 返回订阅合约：%s \n",pSpecificInstrument->InstrumentID); 
	}

private:
	CUstpFtdcMduserApi *m_pUserApi;
};


int main()
{
	CUstpFtdcMduserApi *pUserApi = CUstpFtdcMduserApi::CreateFtdcMduserApi();
	CSimpleHandler sh(pUserApi);
	pUserApi->RegisterSpi(&sh);
	//pUserApi-> SubscribeMarketDataTopic (101, TERT_RESUME);  
	pUserApi-> SubscribeMarketDataTopic (110, USTP_TERT_RESTART); 
	pUserApi->RegisterFront("tcp://117.185.125.4:17199"); 
	pUserApi->Init();
	pUserApi->Release();
	return 0;
}