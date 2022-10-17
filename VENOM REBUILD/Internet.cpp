#include "plugin.h"

enum eRequestMethod
{
	eRequestMethod_HTTP,
	eRequestMethod_HTTPS
};

std::string Dark::Internet::Sync(std::string url)
{
	char _url[768];
	ZeroMemory(_url, sizeof(_url));
	char host[256];
	ZeroMemory(host, sizeof(host));
	char request[512];
	ZeroMemory(request, sizeof(request));

	char data[1024];
	memset(data, 0, 1024);

	std::string result;

	int method = 0;
	if (Dark::Match(url.c_str(), _url, "https://"))		method = eRequestMethod::eRequestMethod_HTTPS;
	else if (Dark::Match(url.c_str(), _url, "http://"))	method = eRequestMethod::eRequestMethod_HTTP;

	Dark::Match(_url, host, "", "/");
	Dark::Match(_url, request, "/");


	HINTERNET hInternet = InternetOpen("WinInet", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	if (hInternet != NULL)
	{
		INTERNET_PORT port = NULL;
		DWORD flags = NULL;

		switch (method)
		{
		case eRequestMethod_HTTP:
			port = INTERNET_DEFAULT_HTTP_PORT;
			flags = INTERNET_FLAG_KEEP_CONNECTION;
			break;
		case eRequestMethod_HTTPS:
			port = INTERNET_DEFAULT_HTTPS_PORT;
			flags = INTERNET_FLAG_SECURE;
			break;
		}

		HINTERNET hConnect = InternetConnect(hInternet, host, port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);
		if (hConnect != NULL)
		{
			HINTERNET hRequest = HttpOpenRequest(hConnect, "GET", request, NULL, NULL, 0, flags, 1);
			if (hRequest != NULL)
			{
				char r[128];
				memset(r, 0, 128);
				BOOL isSend = HttpSendRequestA(hRequest, r, strlen(r), r, strlen(r));

				if (isSend)
				{
					DWORD dwByteRead;
					BOOL isRead;

					for (;;)
					{
						// reading data
						isRead = ::InternetReadFile(hRequest, data, sizeof(data) - 1, &dwByteRead);

						if (isRead == FALSE || dwByteRead == 0) break;

						result.append(data, dwByteRead);
						data[dwByteRead] = 0;
					}
				}
				InternetCloseHandle(hRequest);
			}
			else Dark::Log("Error: HttpOpenRequest()");
			InternetCloseHandle(hConnect);
		}
		else Dark::Log("Error: InternetConnect()");
		InternetCloseHandle(hInternet);
	}
	else Dark::Log("Error: InternetOpen()");
	
	return result;
}

void request_async(InternetCallback v, std::string url)
{
	std::string result = Dark::Internet::Sync(url);
	v(result);
	return _endthread();
}

void Dark::Internet::aSync(InternetCallback v, std::string url)
{
	std::thread th(request_async, v, url);
	th.detach();
}
