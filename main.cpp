/*!
@file main.cpp

@brief 素手による仮想物体操作システム

@author Ishizu Takahiro
@date 2018-02
*/

#include "common.h"
#include "arSystem.h"
#include "hiddenSystem.h"
#include "recvSystem.h"
#include "virtualObjectSystem.h"
#include "bulletSystem.h"
#include "UDPSystem.h"

using namespace cv;
using namespace std;

int thresh = 100;

double trans_cr[3][4];
double trans_rm[3][4];

void DrawObject(void);
void MainLoop(void);
void Display(void);
void KeyEvent(unsigned char key, int x, int y);

double wmat1[3][4];
double wmat2[3][4];

const GLfloat RX_LIGHT0_POS[4] = { 2.0f, 4.0f, 1.0f, 0.0f };
const GLfloat RX_LIGHT1_POS[4] = { -1.0f, -10.0f, -1.0f, 0.0f };

const GLfloat RX_LIGHT_DIFF[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat RX_LIGHT_SPEC[4] = { 0.7f, 0.6f, 0.6f, 1.0f };
const GLfloat RX_LIGHT_AMBI[4] = { 0.3f, 0.3f, 0.3f, 1.0f };

const GLfloat RX_FOV = 45.0f;

const double DT = 0.01;	 // 時間ステップ幅Δt


//! ウィンドウサイズ
int g_iWinW = 720;
int g_iWinH = 720;
bool g_bIdle;				//!< アニメーションフラグ


//! トラックボール(マウスによる視点回転用)
extern rxTrackball g_tbView;

// シャドウマッピング(影付け用)
rxShadowMap g_ShadowMap;
int g_iShadowMapSize = 512;

btAlignedObjectArray<btCollisionShape*>	g_vCollisionShapes;		//!< 剛体オブジェクトの形状を格納する動的配列

btSoftRigidDynamicsWorld* g_pDynamicsWorld;	//!< Bulletワールド
btSoftBodyWorldInfo g_softBodyWorldInfo;

btRigidBody *g_pKinematicBody;
bool init_flag = false;

bool isAnchor = false;  // 柔軟体と剛体を接続する

ARUint8 *image;

double Tcr[16];
double Trm[16];

hiddenSystem hidden;
arSystem ar;
virtualObjectSystem virtualObject;
bulletSystem bullet;


/*!
* 透視投影変換
*/
void Projection(void)
{
	gluPerspective(RX_FOV, (double)g_iWinW / (double)g_iWinH, 0.2f, 1000.0f);
}


// 剛体オブジェクト生成
extern btRigidBody* body[2];
extern btSoftBody* sphere[2];

static int trans_obj = 0;
static float pos_x_tmp = 0;
static float pos_y_tmp = 0;
static int joint_status = 0;


/*!
* シーン描画
*/
void RenderScene(void* x = 0)
{
	// 光源設定
	glLightfv(GL_LIGHT0, GL_POSITION, RX_LIGHT0_POS);
	virtualObject.DrawBulletObjects();
}


/*!
* 描画関数
*/
void Display(void)
{
	static int k = 0;
	
	glScaled(ENLARGE, ENLARGE, ENLARGE);  // bullet拡大
	glScaled(2, 2, 2);  // bullet拡大

	glutInitWindowPosition(100, 100);
	glutInitWindowSize(g_iWinW, g_iWinH);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glClearColor(1, 0.2, 1, 0);

	// フレームバッファとデプスバッファをクリアする
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	g_tbView.Apply();	// マウスによる回転・平行移動の適用

	// シャドウマップを使って影付きでオブジェクト描画
	Vec3 light_pos = Vec3(RX_LIGHT0_POS[0], RX_LIGHT0_POS[1], RX_LIGHT0_POS[2]);
	rxFrustum light = CalFrustum(90, 0.02, 50.0, g_iShadowMapSize, g_iShadowMapSize, light_pos, Vec3(0.0));
	g_ShadowMap.RenderSceneWithShadow(light, RenderScene, 0);

	glPopMatrix();

}


/*!
* リサイズイベント処理関数
* @param[in] w,h キャンバスサイズ
*/
void Resize(int w, int h)
{
	glViewport(0, 0, w, h);
	g_tbView.SetRegion(w, h);

	g_iWinW = w;
	g_iWinH = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(RX_FOV, (float)w / (float)h, 0.01f, 20.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/*!
* タイマーイベント処理関数(ある時間間隔で実行)
*/
void Timer(int value)
{
	static int timer_a = 0;
	if (g_bIdle && g_pDynamicsWorld) {
		// シミュレーションを1ステップ進める
		int numstep = g_pDynamicsWorld->stepSimulation(DT * 1000, 1);
	}

	glutPostRedisplay();
	glutTimerFunc(DT * 1000, Timer, 0);
}


void Keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case '\033':  // '\033' は ESC の ASCII コード
		bullet.CleanBullet();
		exit(0);

	case 'r':	// ワールドリセット
		bullet.CleanBullet();
		bullet.InitBullet();
		break;

	default:
		break;
	}
}


void MainLoop(void)
{
	ARMarkerInfo *marker_info;
	int marker_num;
	int j, k;

	cv::Mat img_src(ysize, xsize, CV_8UC4);

	char buf[2048];
	int n;

	n = 0;

	if ((image = (ARUint8 *)arVideoGetImage()) == NULL) {
		arUtilSleep(2);
		return;
	}

	argDrawMode2D();
	argDispImage(image, 0, 0);

	if (arDetectMarker(image, thresh, &marker_info, &marker_num) < 0) {
		ar.Cleanup();
		exit(0);
	}

	memcpy(img_src.data, image, img_src.cols * img_src.rows * img_src.channels());

	std::vector<cv::Mat> vec_hsv(3);

	cv::Mat extractedImage;

	hidden.colorExtraction(&img_src, &extractedImage, CV_BGR2HSV, 0, 30, 50, 255, 0, 255);

	cv::cvtColor(extractedImage, img_gray, cv::COLOR_BGR2GRAY);
	cv::cvtColor(extractedImage, img_hsv, cv::COLOR_BGR2HSV_FULL);
	cv::split(img_hsv, vec_hsv);
	cv::inRange(img_hsv, cv::Scalar(h_lower, s_lower, v_lower), cv::Scalar(h_upper, s_upper, v_upper), img_bin);

	cv::dilate(img_bin, img_bin, element8, cv::Point(-1, -1), 2); // 膨張
	cv::erode(img_bin, img_bin, element8, cv::Point(-1, -1), 2); // 収縮

	cv::imshow("bin", img_bin);  // 2値画像表示
	cv::imshow("extra", extractedImage);  // 肌色抽出画像表示
	cv::imshow("RGB", img_src);  // RGB画像表示

	for (int i = 0; i < xsize; i++) {
		for (int j = 0; j < ysize; j++) {
			red[i][j] = 0;
			green[i][j] = 0;
			blue[i][j] = 0;
			bin[i][j] = 0;
		}
	}

	for (int j = 0; j < xsize; j++) {
		for (int k = 0; k < ysize; k++) {
			red[j][k] = img_src.at<Vec4b>(k, j)[2];
			green[j][k] = img_src.at<Vec4b>(k, j)[1];
			blue[j][k] = img_src.at<Vec4b>(k, j)[0];
			if (img_bin.at<uchar>(k, j) != 0) {
				bin[j][k] = 1;
			}
		}
	}

	arVideoCapNext();
	if (arMultiGetTransMat(marker_info, marker_num, config) >= 0) {
		DrawObject();
	}

	hidden.HiddenResult();

	argSwapBuffers();

}


void DrawObject(void)
{
	argDrawMode3D();
	argDraw3dCamera(0, 0);

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	argConvGlpara(config->trans, Tcr);
	argConvGlpara(config->marker[4].trans, Trm);  // maeker D

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(Tcr);
	glMultMatrixd(Trm);

	glutDisplayFunc(Display);
	glutReshapeFunc(Resize);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(DT * 10000, Timer, 0);

	if (init_flag == false) {
		g_bIdle = true;
		ar.InitGL();
		init_flag = true;
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	hidden.HiddenPreShow();

}


void KeyEvent(unsigned char key, int x, int y)
{
	if (key == 0x1b) {
		ar.Cleanup();
		exit(0);
	}
}


int main(int argc, char **argv)
{
	UDPSystem udp;

	glutInit(&argc, argv);

	ar.arInit();
	udp.UDPStart();
	hidden.InitCV();

	argMainLoop(NULL, KeyEvent, MainLoop);

	udp.UDPEnd();

	glutMainLoop();

	bullet.CleanBullet();
	hidden.CleanCV();
	ar.Cleanup();

	return 0;

}