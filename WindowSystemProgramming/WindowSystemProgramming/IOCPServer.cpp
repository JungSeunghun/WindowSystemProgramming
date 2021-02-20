#include "IOCPServer.h"

IOCPServer::IOCPServer()
	: hIOCP(INVALID_HANDLE_VALUE),
	numOfWorkingThreads(0),	workingThreads(nullptr),
	hEvent(INVALID_HANDLE_VALUE)
{}

IOCPServer::~IOCPServer() {
}

int IOCPServer::Init(short portNum, int backlog) {
	WSADATA	wsa;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (ret != 0) {
		cout << "WSAStartup failed with error : " << ret << endl;
		return Error::GetLastError();
	}

	int error = SetListenSocket(portNum, backlog);
	if (error != NO_ERROR) {
		WSACleanup();
		return error;
	}

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	DWORD numOfCpu = systemInfo.dwNumberOfProcessors;
	numOfWorkingThreads = numOfCpu * 2 + 1;

	hIOCP = CreateIoCompletionPort((HANDLE)listenSocket.GetSocket(), NULL, IOKEY_LISTEN, numOfCpu);
	if (hIOCP == NULL) {
		listenSocket.CloseSocket();
		WSACleanup();
		return Error::GetLastError();
	}

	error = IncreaseConnectSockets(POOL_MIN_SIZE);
	if (error != NO_ERROR) {
		CloseHandle(hIOCP);
		listenSocket.CloseSocket();
		WSACleanup();
		return error;
	}

	return NO_ERROR;
}

int IOCPServer::SetListenSocket(short portNum, int backlog) {
	SocketAddress sockAddr(INADDR_ANY, portNum);
	
	int error = listenSocket.CreateSocket();
	if (error != NO_ERROR)
		return error;

	error = listenSocket.Bind(sockAddr);
	if (error != NO_ERROR)
		return error;

	error = listenSocket.Listen(backlog);
	if (error != NO_ERROR)
		return error;

	return NO_ERROR;
}

int IOCPServer::IncreaseConnectSockets(int increase) {
	for (int i = 0; i < increase; i++) {
		TCPConnectSocket* connectSocket = new TCPConnectSocket();
		int error = connectSocket->CreateSocket();
		if (error != NO_ERROR) {
			delete connectSocket;
			return error;
		}

		error = connectSocket->AsyncAccept(listenSocket.GetSocket());
		if (error != NO_ERROR) {
			delete connectSocket;
			return error;
		}
		waitingPool.insert(connectSocket);
	}
	return NO_ERROR;
}

void IOCPServer::Start(DWORD threadId) {
	args.threadId = threadId;
	args.hIOCP = hIOCP;

	workingThreads = new HANDLE[numOfWorkingThreads];
	for (DWORD i = 0; i < numOfWorkingThreads; i++) {
		unsigned int id;
		workingThreads[i] = (HANDLE)_beginthreadex(NULL, 0, threadProc, &args, 0, &id);
	}

	hEvent = WSACreateEvent();
	WSAEventSelect(listenSocket.GetSocket(), hEvent, FD_ACCEPT);

	cout << " ==> Waiting for client's connection......" << endl;
	connectionProc();

	return;
}

void IOCPServer::connectionProc() {
	while (true) {
		DWORD dwWaitRet = 
			MsgWaitForMultipleObjectsEx(1, &hEvent, 2000, QS_POSTMESSAGE, MWMO_INPUTAVAILABLE);
		if (dwWaitRet == WAIT_FAILED)
			break;

		if (dwWaitRet == WAIT_TIMEOUT) {
			if (waitingPool.size() > POOL_MIN_SIZE) {
				set<TCPConnectSocket*>::iterator it = waitingPool.begin();
				TCPConnectSocket* session = *it;
				waitingPool.erase(it);
				delete session;

				printf("...Timeout expired, WaitingPool=%d, ConnectedPool=%d\n",
					waitingPool.size(), connectedPool.size());
			}
			continue;
		}

		if (dwWaitRet == 1) {
			MSG msg;
			if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				continue;

			if (msg.message == TM_PROG_EXIT)
				break;

			OverlappedEx* overlappedEx = (OverlappedEx*)msg.lParam;
			TCPConnectSocket* connectSocket = static_cast<TCPConnectSocket*>(overlappedEx->baseSocket);
			
			if (msg.message == TM_SOCK_CONNECTED) {
				waitingPool.erase(connectSocket);
				connectedPool.insert(connectSocket);
				printf("...Connection established, WaitingPool=%d, ConnectedPool=%d\n",
					waitingPool.size(), connectedPool.size());
			}
			else if (msg.message == TM_SOCK_DISCONNECTED) {
				connectedPool.erase(connectSocket);
				waitingPool.insert(connectSocket);
				int error = connectSocket->AsyncDisconnect();				
				error = connectSocket->AsyncAccept(listenSocket.GetSocket());
				if (error != NO_ERROR)
					break;
				printf("...Connection disconnected, WaitingPool=%d, ConnectedPool=%d\n",
					waitingPool.size(), connectedPool.size());
			}
		}
		else {
			WSANETWORKEVENTS ne;
			WSAEnumNetworkEvents(listenSocket.GetSocket(), hEvent, &ne);
			if (ne.lNetworkEvents & FD_ACCEPT) {
				if (connectedPool.size() < POOL_MAX_SIZE)
					IncreaseConnectSockets(PERMIT_INC_CNT);
				printf("...Listen event signaled, WaitingPool=%d, ConnectedPool=%d\n",
					waitingPool.size(), connectedPool.size());
			}
		}
	}
}

void IOCPServer::Close() {
	CloseHandle(hIOCP);
	WaitForMultipleObjects(numOfWorkingThreads, workingThreads, TRUE, INFINITE);
	delete[] workingThreads;
	workingThreads = nullptr;
	CloseHandle(hEvent);
	listenSocket.CloseSocket();

	for (set<TCPConnectSocket*>::iterator it = connectedPool.begin(); it != connectedPool.end(); it++)
	{
		TCPConnectSocket* connectSocket = *it;
		connectSocket->CloseSocket();
		delete connectSocket;
	}
	for (set<TCPConnectSocket*>::iterator it = waitingPool.begin(); it != waitingPool.end(); it++)
	{
		TCPConnectSocket* connectSocket = *it;
		connectSocket->CloseSocket();
		delete connectSocket;
	}

	cout << "==== Server terminates... ==========================" << endl;
	WSACleanup();
}