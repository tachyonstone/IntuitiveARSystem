#include "common.h"
#include "arSystem.h"
#include "bulletSystem.h"


// シャドウマッピング(影付け用)
extern rxShadowMap g_ShadowMap;
extern int g_iShadowMapSize;

char *vconf_name = "Data/WDM_camera_flipV.xml";
char *cparam_name = "Data/camera_para.dat";
char *config_name = "Data/multi/marker.dat";
ARMultiMarkerInfoT *config;
int xsize, ysize;

//! トラックボール(マウスによる視点回転用)
rxTrackball g_tbView;

int arSystem::arInit() {

	if (arVideoOpen(vconf_name) < 0) {
		printf("ビデオデバイスのエラー");
		return -1;
	}

	if (arVideoInqSize(&xsize, &ysize) < 0)	return -1;

	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("カメラパラメータの読み込みに失敗しました\n");
		return -1;
	}

	arParamChangeSize(&wparam, xsize, ysize, &cparam);
	arInitCparam(&cparam);

	if ((config = arMultiReadConfigFile(config_name)) == NULL) {
		printf("multi_marker error\n");
		return -1;
	}

	argInit(&cparam, 1.0, 0, 0, 0, 0);

	arVideoCapStart();

}


void arSystem::Cleanup(void)
{
	arVideoCapStop();
	arVideoClose();
	argCleanup();
	arMultiFreeConfig(config);

}


void arSystem::InitGL(void)
{
	bulletSystem bullet;

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// 光源の初期設定
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, RX_LIGHT_DIFF);
	glLightfv(GL_LIGHT0, GL_SPECULAR, RX_LIGHT_SPEC);
	glLightfv(GL_LIGHT0, GL_AMBIENT, RX_LIGHT_AMBI);
	glLightfv(GL_LIGHT0, GL_POSITION, RX_LIGHT0_POS);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	// 視点の初期化
	g_tbView.SetScaling(-7.0);
	g_tbView.SetTranslation(0.0, -2.0);

	// シャドウマップ初期化
	g_ShadowMap.InitShadow(g_iShadowMapSize, g_iShadowMapSize);

	// Bullet初期化
	bullet.InitBullet();
}
