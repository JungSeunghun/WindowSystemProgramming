#pragma once
#include <process.h>
#include "Define.h"
#include "OverlappedEx.h"
#include "SocketAddress.h"
#include "BaseSocket.h"
#include "TCPListenSocket.h"
#include "TCPConnectSocket.h"

struct ThreadArgs {
	DWORD	 threadId;
	HANDLE	 hIOCP;

	ThreadArgs() :threadId(0), hIOCP(INVALID_HANDLE_VALUE) {}
};

static unsigned __stdcall threadProc(PVOID pParam)
{
	ThreadArgs* args = (ThreadArgs*)pParam;
	OverlappedEx* overlappedEx = NULL;
	DWORD bytes = 0;
	ULONG_PTR completionKey = 0;

	while (true) {
		try	{
			BOOL ret = GetQueuedCompletionStatus (
				args->hIOCP, &bytes, &completionKey, (LPOVERLAPPED*)&overlappedEx, INFINITE
			);
			if (ret == FALSE)	{
				if (overlappedEx != NULL)
					throw (int)overlappedEx->Internal;

				int error = WSAGetLastError();
				if (error != ERROR_ABANDONED_WAIT_0)
					cout << "GQCS failed: " << error << endl;
				break;
			}

			TCPConnectSocket* connectSocket = static_cast<TCPConnectSocket*>(overlappedEx->baseSocket);

			switch (completionKey) {
				case IOKEY_LISTEN: {
					if (connectSocket->IsCreateIOCP() == false) {
						HANDLE ret = CreateIoCompletionPort((HANDLE)connectSocket->GetSocket(), args->hIOCP, IOKEY_CHILD, 0);
						if (ret == nullptr)
							throw WSAGetLastError();
						connectSocket->SetIsCreateIOCP(true);
					}
					
					cout << " ==> New client " << connectSocket->GetSocket() << " connected..." << endl;
					int error = connectSocket->AsyncRecv();
					if (error != NO_ERROR)
						throw error;

					PostThreadMessage(args->threadId, TM_SOCK_CONNECTED, 0, (LPARAM)overlappedEx);
					break;
				}
				case IOKEY_CHILD: {
					if (bytes == 0)
						throw (INT)ERROR_SUCCESS;

					if (overlappedEx->state == State::RECVED) {
						int error = connectSocket->AsyncSend(bytes);
						if (error != NO_ERROR)
							throw error;
					}
					else if (overlappedEx->state == State::SENDED) {
						int error = connectSocket->AsyncRecv();
						if (error != NO_ERROR)
							throw error;
					}

					break;
				}
			}			
		}
		catch (int ex)
		{
			if (ex == STATUS_LOCAL_DISCONNECT || ex == STATUS_CANCELLED)
			{
				cout << " ==> Child socket closed." << endl;
				continue;
			}
			if (ex == ERROR_SUCCESS || ex == STATUS_REMOTE_DISCONNECT) {
				cout << " ==> Client " << overlappedEx->baseSocket->GetSocket() << " disconnected..." << endl;
			}
			else if (ex == STATUS_CONNECTION_RESET) {
				cout << " ==> Pending Client " << overlappedEx->baseSocket->GetSocket() << " disconnected..." << endl;
			}
			else {
				cout << " ==> Client " << overlappedEx->baseSocket->GetSocket() << " has error " << ex << endl;
			}

			PostThreadMessage(args->threadId, TM_SOCK_DISCONNECTED, ex, (LPARAM)overlappedEx);
		}
	}
	return 0;
}

class IOCPServer
{
private:
	TCPListenSocket listenSocket;
	HANDLE hIOCP;
	ThreadArgs args;
	DWORD numOfWorkingThreads;
	HANDLE* workingThreads;
	WSAEVENT hEvent;

	set<TCPConnectSocket*> waitingPool;
	set<TCPConnectSocket*> connectedPool;

	int SetListenSocket(short portNum, int backlog);
	int IncreaseConnectSockets(int increase);
	void connectionProc();

public:
	IOCPServer();
	~IOCPServer();

	int Init(short portNum, int backlog);
	void Start(DWORD threadId);
	void Close();
};

