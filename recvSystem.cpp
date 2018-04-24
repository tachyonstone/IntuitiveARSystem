#include "common.h"
#include "recvSystem.h"


/* ジェスチャによる仮想物体のサイズ指定*/
extern int create_stat;
extern int dir_index;
extern int dir_min;
extern int dif;
extern int create_num;
extern bool isInitFinger;

static bool isInitFinger = false;

double finger_x[FINGERR_N] = { 0.0 };
double finger_y[FINGERR_N] = { 0.0 };
double finger_z[FINGERR_N] = { 0.0 };
double distal_x[FINGERR_N] = { 0.0 };
double distal_y[FINGERR_N] = { 0.0 };
double distal_z[FINGERR_N] = { 0.0 };
double intermediate_x[FINGERR_N] = { 0.0 };
double intermediate_y[FINGERR_N] = { 0.0 };
double intermediate_z[FINGERR_N] = { 0.0 };
double proximal_x[FINGERR_N] = { 0.0 };
double proximal_y[FINGERR_N] = { 0.0 };
double proximal_z[FINGERR_N] = { 0.0 };
double metacarpal_x[FINGERR_N] = { 0.0 };
double metacarpal_y[FINGERR_N] = { 0.0 };
double metacarpal_z[FINGERR_N] = { 0.0 };

double finger2_x[FINGERR_N] = { 0.0 };
double finger2_y[FINGERR_N] = { 0.0 };
double finger2_z[FINGERR_N] = { 0.0 };

int finger_no[FINGERR_N] = { 0 };


int Min(int min1, int min2)
{
	return min1 < min2 ? min1 : min2;

}


void InitFinger() {
	for (int i = 0; i < 5; i++) {
		finger_x[i] = 10;
		finger_y[i] = 10;
		finger_z[i] = 10;
	}
}


void recvSystem::RecvHandData() {

	char buf[2048];
	int n;
	int tmp;

	if (isInitFinger == false) {
		InitFinger();
		isInitFinger = true;
	}

	memset(buf, 0, sizeof(buf));

	n = recv(sock, buf, sizeof(buf), 0);

	if (n < 1) {
		if (WSAGetLastError() == WSAEWOULDBLOCK) {
			// まだ来ない。
			printf("MADA KONAI\n");

		}
		else {
			printf("error : 0x%x\n", WSAGetLastError());
			//break;
		}
	}
	else {

		printf("%s\n", buf);
		int count = 0;  // 片手か両手かを受信したサイズで取得
		sscanf(buf, "%d", &count);

		if (count < 100) {  // 右手だけ

			if (create_stat == 1) {
				create_stat++;
			}

			printf("%s\n", buf);
			sscanf(buf, "%d %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf\
		    %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\
		    %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\
		    %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\
		    %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf",
				&count,
				&finger_no[0], &finger_x[0], &finger_y[0], &finger_z[0],
				&finger_no[1], &finger_x[1], &finger_y[1], &finger_z[1],
				&finger_no[2], &finger_x[2], &finger_y[2], &finger_z[2],
				&finger_no[3], &finger_x[3], &finger_y[3], &finger_z[3],
				&finger_no[4], &finger_x[4], &finger_y[4], &finger_z[4],
				&distal_x[0], &distal_y[0], &distal_z[0],
				&distal_x[1], &distal_y[1], &distal_z[1],
				&distal_x[2], &distal_y[2], &distal_z[2],
				&distal_x[3], &distal_y[3], &distal_z[3],
				&distal_x[4], &distal_y[4], &distal_z[4],
				&intermediate_x[0], &intermediate_y[0], &intermediate_z[0],
				&intermediate_x[1], &intermediate_y[1], &intermediate_z[1],
				&intermediate_x[2], &intermediate_y[2], &intermediate_z[2],
				&intermediate_x[3], &intermediate_y[3], &intermediate_z[3],
				&intermediate_x[4], &intermediate_y[4], &intermediate_z[4],
				&proximal_x[0], &proximal_y[0], &proximal_z[0],
				&proximal_x[1], &proximal_y[1], &proximal_z[1],
				&proximal_x[2], &proximal_y[2], &proximal_z[2],
				&proximal_x[3], &proximal_y[3], &proximal_z[3],
				&proximal_x[4], &proximal_y[4], &proximal_z[4],
				&metacarpal_x[0], &metacarpal_y[0], &metacarpal_z[0],
				&metacarpal_x[1], &metacarpal_y[1], &metacarpal_z[1],
				&metacarpal_x[2], &metacarpal_y[2], &metacarpal_z[2],
				&metacarpal_x[3], &metacarpal_y[3], &metacarpal_z[3]);

			for (int i = 0; i < 5; i++) {
				if (i < 4) {
					tmp = metacarpal_y[i];
					metacarpal_y[i] = -metacarpal_z[i];
					metacarpal_z[i] = tmp;
				}
				tmp = finger_y[i];
				finger_y[i] = -finger_z[i];
				finger_z[i] = tmp;
				tmp = distal_y[i];
				distal_y[i] = -distal_z[i];
				distal_z[i] = tmp;
				tmp = intermediate_y[i];
				intermediate_y[i] = -intermediate_z[i];
				intermediate_z[i] = tmp;
				tmp = proximal_y[i];
				proximal_y[i] = -proximal_z[i];
				proximal_z[i] = tmp;
			}

		}
		else { // 両手

			sscanf(buf, "%d %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf %d %lf %lf %lf",
				&count,
				&finger_no[0], &finger_x[0], &finger_y[0], &finger_z[0],
				&finger_no[1], &finger_x[1], &finger_y[1], &finger_z[1],
				&finger_no[2], &finger_x[2], &finger_y[2], &finger_z[2],
				&finger_no[3], &finger_x[3], &finger_y[3], &finger_z[3],
				&finger_no[4], &finger_x[4], &finger_y[4], &finger_z[4],
				&finger_no[5], &finger_x[5], &finger_y[5], &finger_z[5],
				&finger_no[6], &finger_x[6], &finger_y[6], &finger_z[6],
				&finger_no[7], &finger_x[7], &finger_y[7], &finger_z[7],
				&finger_no[8], &finger_x[8], &finger_y[8], &finger_z[8],
				&finger_no[9], &finger_x[9], &finger_y[9], &finger_z[9]
			);

			tmp = dir_min;
			dir_index = abs(finger_x[1] - finger_x[6]);
			dir_min = Min(dir_index, tmp);

			if (create_stat == 0) {
				create_stat++;
			}
		}

		/* ARToolKitの座標系に変換 */
		int k;

		/* 隠面処理用 */
		for (k = 0; k < 5; k++) {
			finger2_x[k] = finger_x[k];
			finger2_y[k] = finger_y[k] + 2;
			finger2_z[k] = finger_z[k] + 1;
		}

		/* bullet用 */
		for (k = 0; k < 5; k++) {
			finger_x[k] = finger_x[k] / ENLARGE;
			finger_y[k] = finger_y[k] / ENLARGE + 10.0;
			finger_z[k] = finger_z[k] / ENLARGE;
		}

	}

}


