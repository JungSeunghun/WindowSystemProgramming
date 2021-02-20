#include "IOCPServer.h"

DWORD threadId;

BOOL CtrlHandler(DWORD fdwCtrlType)
{
	PostThreadMessage(threadId, TM_PROG_EXIT, 0, 0);
	return TRUE;
}

int _tmain()
{
	setlocale(LC_ALL, "KOREAN");

	threadId = GetCurrentThreadId();

	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE)) {
		cout << "SetConsoleCtrlHandler failed, code : " << GetLastError() << endl;
		return -1;
	}
	
	IOCPServer server;
	if (server.Init(9001, 5) != NO_ERROR)
		return -1;
	server.Start(threadId);
	server.Close();

	return 0;
}

/*
#include "Define.h"
#include "Session.h"
#include <process.h>

SOCKET GetListenSocket(short shPortNo, int nBacklog = SOMAXCONN)
{
	SOCKET hsoListen = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (hsoListen == INVALID_SOCKET)
	{
		cout << "socket failed, code : " << WSAGetLastError() << endl;
		return INVALID_SOCKET;
	}

	SOCKADDR_IN	sa;
	memset(&sa, 0, sizeof(SOCKADDR_IN));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(shPortNo);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	LONG lSockRet = bind(hsoListen, (PSOCKADDR)&sa, sizeof(SOCKADDR_IN));
	if (lSockRet == SOCKET_ERROR)
	{
		cout << "bind failed, code : " << WSAGetLastError() << endl;
		closesocket(hsoListen);
		return INVALID_SOCKET;
	}

	lSockRet = listen(hsoListen, nBacklog);
	if (lSockRet == SOCKET_ERROR)
	{
		cout << "listen failed, code : " << WSAGetLastError() << endl;
		closesocket(hsoListen);
		return INVALID_SOCKET;
	}

	return hsoListen;
}

struct ThreadArgs
{
	HANDLE	 handleOfIOCP;
	DWORD	 threadId;
};


unsigned __stdcall IOCPProc(PVOID pParam)
{
	ThreadArgs* args = (ThreadArgs*)pParam;
	OverlappedEx* overlappedEx = NULL;
	DWORD bytes = 0;
	ULONG_PTR completionKey = 0;

	while (true)
	{
		try
		{
			BOOL bIsOK = GetQueuedCompletionStatus
			(
				args->handleOfIOCP, &bytes, &completionKey, (LPOVERLAPPED*)&overlappedEx, INFINITE
			);
			if (bIsOK == FALSE)
			{
				if (overlappedEx != NULL)
					throw (int)overlappedEx->Internal;

				int nErrCode = WSAGetLastError();
				if (nErrCode != ERROR_ABANDONED_WAIT_0)
					cout << "GQCS failed: " << nErrCode << endl;
				break;
			}

			switch (completionKey) {
				case IOKEY_LISTEN: {
					bool ret = overlappedEx->session->setIOCP(args->handleOfIOCP);
					if (ret == false)
						throw WSAGetLastError();

					cout << " ==> New client " << overlappedEx->session->getSocket() << " connected..." << endl;
					PostThreadMessage(args->threadId, TM_SOCK_CONNECTED, 0, (LPARAM)overlappedEx);
					break;
				}
				case IOKEY_CHILD: {
					if (bytes == 0)
						throw (INT)ERROR_SUCCESS;

					bool ret = overlappedEx->session->asyncSend(bytes);
					if (ret == false)
						throw WSAGetLastError();
					break;
				}
			}

			bool ret = overlappedEx->session->asyncRecv();
			if (ret == false) {
				int error = WSAGetLastError();
				if (error != WSA_IO_PENDING)
					throw error;
			}
		}
		catch (int ex)
		{
			if (ex == STATUS_LOCAL_DISCONNECT || ex == STATUS_CANCELLED)
			{
				cout << " ==> Child socket closed." << endl;
				continue;
			}
			if (ex == ERROR_SUCCESS || ex == STATUS_REMOTE_DISCONNECT)
				cout << " ==> Client " << overlappedEx->session->getSocket() << " disconnected..." << endl;
			else if (ex == STATUS_CONNECTION_RESET)
				cout << " ==> Pending Client " << overlappedEx->session->getSocket() << " disconnected..." << endl;
			else
				cout << " ==> Client " << overlappedEx->session->getSocket() << " has error " << ex << endl;

			PostThreadMessage(args->threadId, TM_SOCK_DISCONNECTED, ex, (LPARAM)overlappedEx);
		}
	}
	return 0;
}

int IncreaseAcceptSockets(SOCKET listenSocket, int increase, set<Session*>& waitingPool)
{
	int poolCount = 0;
	for (; poolCount < increase; poolCount++)
	{
		Session* session = new Session();
		if (session->init(listenSocket) == false) {
			delete session;
			break;
		}
		if (session->asyncAccept() == false) {
			delete session;
			break;
		}
		waitingPool.insert(session);
	}
	printf("Increase AcceptSockets: %d\n", poolCount);
	return poolCount;
}

DWORD mainThreadId = 0;
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	PostThreadMessage(mainThreadId, TM_PROG_EXIT, 0, 0);
	return TRUE;
}

int _tmain()
{
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	DWORD numOfCpu = systemInfo.dwNumberOfProcessors;
	DWORD numOfWorkingThreads = numOfCpu * 2 + 1;

	WSADATA	wsd;
	int nErrCode = WSAStartup(MAKEWORD(2, 2), &wsd);
	if (nErrCode)
	{
		cout << "WSAStartup failed with error : " << nErrCode << endl;
		return -1;
	}
	//-------------------
	if (!SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		cout << "SetConsoleCtrlHandler failed, code : " << GetLastError() << endl;
		return -1;
	}
	//-------------------
	SOCKET listenSocket = GetListenSocket(9001);
	if (listenSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return -1;
	}
	cout << " ==> Waiting for client's connection......" << endl;

	ThreadArgs env;
	env.threadId = mainThreadId = GetCurrentThreadId();
	env.handleOfIOCP = CreateIoCompletionPort((HANDLE)listenSocket, NULL, IOKEY_LISTEN, numOfCpu);

	HANDLE* workingThreads = new HANDLE[numOfWorkingThreads];
	for (DWORD i = 0; i < numOfWorkingThreads; i++)
	{
		unsigned int threadId;
		workingThreads[i] = (HANDLE)_beginthreadex(NULL, 0, IOCPProc, &env, 0, &threadId);
	}

	set<Session*> waitingPool;
	set<Session*> connectedPool;
	IncreaseAcceptSockets(listenSocket, POOL_MIN_SIZE, waitingPool);

	WSAEVENT hEvent = WSACreateEvent();
	WSAEventSelect(listenSocket, hEvent, FD_ACCEPT);
	while (true)
	{
		DWORD dwWaitRet = MsgWaitForMultipleObjectsEx
		(
			1, &hEvent, 2000, QS_POSTMESSAGE, MWMO_INPUTAVAILABLE
		);
		if (dwWaitRet == WAIT_FAILED)
			break;

		if (dwWaitRet == WAIT_TIMEOUT)
		{
			if (waitingPool.size() > POOL_MIN_SIZE)
			{
				set<Session*>::iterator it = waitingPool.begin();
				Session* session = *it;
				waitingPool.erase(it);
				delete session;

				printf("...Timeout expired, WaitingPool=%d, ConnectedPool=%d\n",
					waitingPool.size(), connectedPool.size());
			}
			continue;
		}

		if (dwWaitRet == 1)
		{
			MSG msg;
			if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				continue;

			if (msg.message == TM_PROG_EXIT)
				break;

			Session* session = (Session*)msg.lParam;
			if (msg.message == TM_SOCK_CONNECTED)
			{
				waitingPool.erase(session);
				connectedPool.insert(session);
				printf("...Connection established, WaitingPool=%d, ConnectedPool=%d\n",
					waitingPool.size(), connectedPool.size());
			}
			else if (msg.message == TM_SOCK_DISCONNECTED)
			{
				connectedPool.erase(session);								
				session->asyncDisconnect();
				session->asyncAccept();
				waitingPool.insert(session);
				printf("...Connection released, WaitingPool=%d, ConnectedPool=%d\n",
					waitingPool.size(), connectedPool.size());
			}
		}
		else
		{
			WSANETWORKEVENTS ne;
			WSAEnumNetworkEvents(listenSocket, hEvent, &ne);
			if (ne.lNetworkEvents & FD_ACCEPT)
			{
				if (connectedPool.size() < POOL_MAX_SIZE)
					IncreaseAcceptSockets(listenSocket, PERMIT_INC_CNT, waitingPool);
				printf("...Listen event signaled, WaitingPool=%d, ConnectedPool=%d\n",
					waitingPool.size(), connectedPool.size());
			}
		}
	}
	CloseHandle(env.handleOfIOCP);
	WaitForMultipleObjects(numOfWorkingThreads, workingThreads, TRUE, INFINITE);
	delete[] workingThreads;
	closesocket(listenSocket);
	CloseHandle(hEvent);
	for (set<Session*>::iterator it = connectedPool.begin(); it != connectedPool.end(); it++)
	{
		Session* session = *it;
		delete session;
	}
	for (set<Session*>::iterator it = waitingPool.begin(); it != waitingPool.end(); it++)
	{
		Session* session = *it;
		delete session;
	}

	cout << "==== Server terminates... ==========================" << endl;
	WSACleanup();

	return 0;
}
*/