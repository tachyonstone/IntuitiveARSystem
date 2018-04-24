#include "common.h"
#include "UDPSystem.h"

// �l�b�g���[�N�v���O���~���O�p�ϐ�
SOCKET sock;
WSAData wsaData;
struct sockaddr_in addr;


void UDPSystem::UDPStart() {

	/* �l�b�g���[�N�v���O���~���O�J�n */
	WSAStartup(MAKEWORD(2, 0), &wsaData);

	sock = socket(AF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_port = htons(12345);
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(sock, (struct sockaddr *)&addr, sizeof(addr));

	// �m���u���b�L���O�ɐݒ�
	u_long val = 1;
	ioctlsocket(sock, FIONBIO, &val);

}


void UDPSystem::UDPEnd() {
	/* �l�b�g���[�N�v���O���~���O�I�� */
	closesocket(sock);
	WSACleanup();
}
