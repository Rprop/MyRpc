#include <Rpc.h>
#include <tchar.h>
#include "rpc_h.h"
#pragma comment(lib, "Rpcrt4.lib")

void JoinTask(PRPC_ASYNC_STATE rpcAsyncHandle,
			  /* [string][in] */ unsigned char *pszString,
			  /* [out] */ int *res)
{
	RPC_STATUS rt = RpcServerTestCancel(RpcAsyncGetCallHandle(rpcAsyncHandle));
	if (RPC_S_OK == rt) {
		// RPC_S_CALL_CANCELLED
	} else if (RPC_S_CALL_IN_PROGRESS == rt) {
		*res = 2016;
	} //if

	RpcAsyncCompleteCall(rpcAsyncHandle, NULL);
}

//-------------------------------------------------------------------------

void Shutdown() 
{
	RPC_STATUS status;

	status = RpcMgmtStopServerListening(NULL);

	if (status != RPC_S_OK) exit(status);

	status = RpcServerUnregisterIf(NULL, NULL, FALSE);

	if (status != RPC_S_OK) exit(status);
}

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
	status = RpcServerUseProtseqEpW(reinterpret_cast<RPC_WSTR>(L"ncalrpc"),
									RPC_C_LISTEN_MAX_CALLS_DEFAULT,
									reinterpret_cast<RPC_WSTR>(L"msmpq"),
									NULL);

	if (status != RPC_S_OK) exit(status);

	status = RpcServerRegisterIf(MPQ_v1_0_s_ifspec,
								 NULL,
								 NULL);

	if (status != RPC_S_OK) exit(status);

	status = RpcServerListen(1,
							 RPC_C_LISTEN_MAX_CALLS_DEFAULT,
							 FALSE);

	if (status != RPC_S_OK) exit(status);

	return 0;
}