#include "common.h"
#include "UDPSystem.h"

// ネットワークプログラミング用変数
SOCKET sock;
WSAData wsaData;
struct sockaddr_in addr;


void UDPSystem::UDPStart() {

	/* ネットワークプログラミング開始 */
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(sock, (struct sockaddr *)&addr, sizeof(addr));

	// ノンブロッキングに設定
	u_long val = 1;
	ioctlsocket(sock, FIONBIO, &val);

}


void UDPSystem::UDPEnd() {
	/* ネットワークプログラミング終了 */
	closesocket(sock);
	WSACleanup();
}
