#include "common.h"
#include "bulletSystem.h"
#include "virtualObjectSystem.h"


/*!
* Bullet初期化
*/
void bulletSystem::InitBullet(void)
{
	virtualObjectSystem virtualObject;

	// 衝突検出方法の選択(デフォルトを選択)
	btDefaultCollisionConfiguration *config = new btSoftBodyRigidBodyCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(config);

	btRigidBody* rigidbody = NULL;

	// ブロードフェーズ法の設定(Dynamic AABB tree method)
#if 0
	btDbvtBroadphase *broadphase = new btDbvtBroadphase();
#endif

	// スイープ＆プルーン
#if 1
	btAxisSweep3 *broadphase;
	broadphase = new btAxisSweep3(btVector3(-100, -100, -100),
		btVector3(100, 100, 100),
		10,
		0,
		false
	);
#endif

	// 拘束(剛体間リンク)のソルバ設定
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

	// Bulletのワールド作成
	g_pDynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, config, 0);

	// btSoftBodyWorldInfoの初期化・設定
	g_softBodyWorldInfo.m_dispatcher = dispatcher;
	g_softBodyWorldInfo.m_broadphase = broadphase;
	g_softBodyWorldInfo.m_sparsesdf.Initialize();
	g_softBodyWorldInfo.m_gravity.setValue(0, 0, -4.0); //	g_softBodyWorldInfo.m_gravity.setValue(0, -9.8, 0);
	g_softBodyWorldInfo.air_density = 1.2;

	virtualObject.SetRigidBodies();

}


/*!
* 設定したBulletの剛体オブジェクト，ワールドの破棄
*/
void bulletSystem::CleanBullet(void)
{
	// 衝突オブジェクトの破棄
	for (int i = g_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject* obj = g_pDynamicsWorld->getCollisionObjectArray()[i];

		// オブジェクトがRigid Bodyの場合の破棄
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}

		// オブジェクトがSoft Bodyの場合の破棄
		btSoftBody* softBody = btSoftBody::upcast(obj);
		if (softBody) {
			static_cast<btSoftRigidDynamicsWorld*>(g_pDynamicsWorld)->removeSoftBody(softBody);
		}
		else {
			static_cast<btSoftRigidDynamicsWorld*>(g_pDynamicsWorld)->removeCollisionObject(obj);
		}

		g_pDynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	// 形状の破棄
	for (int j = 0; j < (int)g_vCollisionShapes.size(); ++j) {
		btCollisionShape* shape = g_vCollisionShapes[j];
		g_vCollisionShapes[j] = 0;
		delete shape;
	}
	g_vCollisionShapes.clear();

	// ワールド破棄
	delete g_pDynamicsWorld;

}


/*!
* Bullet剛体(btRigidBody)の作成
* @param[in] mass 質量
* @param[in] init_tras 初期位置・姿勢
* @param[in] shape 形状
* @param[in] index オブジェクト固有の番号
* @return 作成したbtRigidBody
*/
btRigidBody* bulletSystem::CreateRigidBody(double mass, const btTransform& init_trans, btCollisionShape* shape, int index)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	// 質量が0ならば静的な(static)オブジェクトとして設定，
	bool isDynamic = (mass != 0.0);

	// 形状から慣性テンソルを計算
	btVector3 inertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, inertia);

	// 初期位置，姿勢の設定
	btDefaultMotionState* motion_state = new btDefaultMotionState(init_trans);

	// 質量，慣性テンソル(回転しやすさ)，形状，位置・姿勢情報を一つの変数にまとめる
	btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state, shape, inertia);

	// 剛体オブジェクト(btRigidBody)の生成
	btRigidBody* body = new btRigidBody(rb_info);

	// 剛体オブジェクトに番号を設定
	body->setUserPointer(&index);

	// Bulletワールドに剛体オブジェクトを追加
	g_pDynamicsWorld->addRigidBody(body);

	return body;
}


/**
*  柔軟物体をワールドに追加する
*/
void bulletSystem::addSoftBody(btSoftBody* sb)
{
	((btSoftRigidDynamicsWorld*)g_pDynamicsWorld)->addSoftBody(sb);
}


/*!
* btSoftBodyの描画
* @param[in] sbody btSoftBodyオブジェクト
*/
void bulletSystem::DrawBulletSoftBody(btSoftBody* sbody)
{
	// btSoftBodyに面(face)がない場合は線として描画
	if (sbody->m_faces.size() == 0) {
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < sbody->m_nodes.size(); ++i) {
			const btSoftBody::Node &node = sbody->m_nodes[i]; // 頂点ノード(質点)
			btVector3 p = node.m_x; // 頂点座標
			glVertex3f(p.x(), p.y(), p.z());
		}
		glEnd();
	}

	// 面(face)がある場合はポリゴンとして描画
	for (int i = 0; i < sbody->m_faces.size(); ++i) {
		const btSoftBody::Face &face = sbody->m_faces[i];

		glBegin(GL_POLYGON);
		for (int j = 0; j < 3; ++j) {
			const btSoftBody::Node* node = face.m_n[j]; // 面を構成する頂点ノード
			btVector3 p = node->m_x; // 頂点座標
			btVector3 n = node->m_n; // 頂点法線

			glNormal3f(n.x(), n.y(), n.z());
			glVertex3f(p.x(), p.y(), p.z());

		}
		glEnd();
	}

}


