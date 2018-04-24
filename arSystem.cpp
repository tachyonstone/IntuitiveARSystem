#include "common.h"
#include "arSystem.h"
#include "bulletSystem.h"


// �V���h�E�}�b�s���O(�e�t���p)
extern rxShadowMap g_ShadowMap;
extern int g_iShadowMapSize;

char *vconf_name = "Data/WDM_camera_flipV.xml";
char *cparam_name = "Data/camera_para.dat";
char *config_name = "Data/multi/marker.dat";
ARMultiMarkerInfoT *config;
int xsize, ysize;

//! �g���b�N�{�[��(�}�E�X�ɂ�鎋�_��]�p)
rxTrackball g_tbView;

int arSystem::arInit() {

	if (arVideoOpen(vconf_name) < 0) {
		printf("�r�f�I�f�o�C�X�̃G���[");
		return -1;
	}

	if (arVideoInqSize(&xsize, &ysize) < 0)	return -1;

	if (arParamLoad(cparam_name, 1, &wparam) < 0) {
		printf("�J�����p�����[�^�̓ǂݍ��݂Ɏ��s���܂���\n");
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

	// �����̏����ݒ�
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, RX_LIGHT_DIFF);
	glLightfv(GL_LIGHT0, GL_SPECULAR, RX_LIGHT_SPEC);
	glLightfv(GL_LIGHT0, GL_AMBIENT, RX_LIGHT_AMBI);
	glLightfv(GL_LIGHT0, GL_POSITION, RX_LIGHT0_POS);

	glShadeModel(GL_SMOOTH);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	// ���_�̏�����
	g_tbView.SetScaling(-7.0);
	g_tbView.SetTranslation(0.0, -2.0);

	// �V���h�E�}�b�v������
	g_ShadowMap.InitShadow(g_iShadowMapSize, g_iShadowMapSize);

	// Bullet������
	bullet.InitBullet();
}
