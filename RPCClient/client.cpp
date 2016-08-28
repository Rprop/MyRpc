#include <Rpc.h>
#include <tchar.h>
#include <stdio.h>
#include "../RPCServer/rpc_h.h"
#pragma comment(lib, "Rpcrt4.lib")

//-------------------------------------------------------------------------

void __RPC_FAR * __RPC_USER midl_user_allocate(size_t len)
{
	return(malloc(len));
}

//-------------------------------------------------------------------------

void __RPC_USER midl_user_free(void __RPC_FAR * ptr)
{
	free(ptr);
}

//-------------------------------------------------------------------------

int main()
{
	RPC_STATUS status;
	RPC_WSTR pszStringBinding = NULL;
	status = RpcStringBindingComposeW(NULL,
									 reinterpret_cast<RPC_WSTR>(L"ncalrpc"),
									 NULL,
									  reinterpret_cast<RPC_WSTR>(L"msmpq"),
									 NULL,
									 &pszStringBinding);
	if (status != RPC_S_OK) exit(status);

	status = RpcBindingFromStringBinding(pszStringBinding, &MPQ_BindingHandle);

	if (status != RPC_S_OK) exit(status);

	RpcTryExcept
	{
		static RPC_ASYNC_STATE async;
		RpcAsyncInitializeHandle(&async, sizeof(async));
		async.UserInfo              = NULL;
		async.NotificationType      = RpcNotificationTypeCallback;
		async.u.NotificationRoutine = [](struct _RPC_ASYNC_STATE *pAsync,
										 void                    *Context,
										 RPC_ASYNC_EVENT         Event) {
			auto a = RpcAsyncGetCallStatus(pAsync);
			while ((a = RpcAsyncGetCallStatus(pAsync)) == RPC_S_ASYNC_CALL_PENDING) {
				printf("Call Hello() pending, wait 1s.../n");
				Sleep(1000);
			}

			if (a == RPC_S_SERVER_UNAVAILABLE) {
			} //if

			RpcAsyncCompleteCall(pAsync, NULL);
			printf("Server return\n");
		};

		int res;
		JoinTask(&async, reinterpret_cast<unsigned char *>(""), &res);
		Shutdown();
	}
	RpcExcept(1)
	{
		DWORD ulCode = RpcExceptionCode();
		printf("Runtime reported exception 0x%lx = %ld\n", ulCode, ulCode);
	}
	RpcEndExcept;

	status = RpcStringFree(&pszStringBinding);

	if (status != RPC_S_OK) exit(status);

	status = RpcBindingFree(&MPQ_BindingHandle);

	if (status != RPC_S_OK) exit(status);

	return 0;
}