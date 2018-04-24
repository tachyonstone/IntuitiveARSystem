#include "common.h"
#include "bulletSystem.h"
#include "virtualObjectSystem.h"


/*!
* Bullet������
*/
void bulletSystem::InitBullet(void)
{
	virtualObjectSystem virtualObject;

	// �Փˌ��o���@�̑I��(�f�t�H���g��I��)
	btDefaultCollisionConfiguration *config = new btSoftBodyRigidBodyCollisionConfiguration();
	btCollisionDispatcher *dispatcher = new btCollisionDispatcher(config);

	btRigidBody* rigidbody = NULL;

	// �u���[�h�t�F�[�Y�@�̐ݒ�(Dynamic AABB tree method)
#if 0
	btDbvtBroadphase *broadphase = new btDbvtBroadphase();
#endif

	// �X�C�[�v���v���[��
#if 1
	btAxisSweep3 *broadphase;
	broadphase = new btAxisSweep3(btVector3(-100, -100, -100),
		btVector3(100, 100, 100),
		10,
		0,
		false
	);
#endif

	// �S��(���̊ԃ����N)�̃\���o�ݒ�
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

	// Bullet�̃��[���h�쐬
	g_pDynamicsWorld = new btSoftRigidDynamicsWorld(dispatcher, broadphase, solver, config, 0);

	// btSoftBodyWorldInfo�̏������E�ݒ�
	g_softBodyWorldInfo.m_dispatcher = dispatcher;
	g_softBodyWorldInfo.m_broadphase = broadphase;
	g_softBodyWorldInfo.m_sparsesdf.Initialize();
	g_softBodyWorldInfo.m_gravity.setValue(0, 0, -4.0); //	g_softBodyWorldInfo.m_gravity.setValue(0, -9.8, 0);
	g_softBodyWorldInfo.air_density = 1.2;

	virtualObject.SetRigidBodies();

}


/*!
* �ݒ肵��Bullet�̍��̃I�u�W�F�N�g�C���[���h�̔j��
*/
void bulletSystem::CleanBullet(void)
{
	// �Փ˃I�u�W�F�N�g�̔j��
	for (int i = g_pDynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject* obj = g_pDynamicsWorld->getCollisionObjectArray()[i];

		// �I�u�W�F�N�g��Rigid Body�̏ꍇ�̔j��
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}

		// �I�u�W�F�N�g��Soft Body�̏ꍇ�̔j��
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

	// �`��̔j��
	for (int j = 0; j < (int)g_vCollisionShapes.size(); ++j) {
		btCollisionShape* shape = g_vCollisionShapes[j];
		g_vCollisionShapes[j] = 0;
		delete shape;
	}
	g_vCollisionShapes.clear();

	// ���[���h�j��
	delete g_pDynamicsWorld;

}


/*!
* Bullet����(btRigidBody)�̍쐬
* @param[in] mass ����
* @param[in] init_tras �����ʒu�E�p��
* @param[in] shape �`��
* @param[in] index �I�u�W�F�N�g�ŗL�̔ԍ�
* @return �쐬����btRigidBody
*/
btRigidBody* bulletSystem::CreateRigidBody(double mass, const btTransform& init_trans, btCollisionShape* shape, int index)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	// ���ʂ�0�Ȃ�ΐÓI��(static)�I�u�W�F�N�g�Ƃ��Đݒ�C
	bool isDynamic = (mass != 0.0);

	// �`�󂩂犵���e���\�����v�Z
	btVector3 inertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(mass, inertia);

	// �����ʒu�C�p���̐ݒ�
	btDefaultMotionState* motion_state = new btDefaultMotionState(init_trans);

	// ���ʁC�����e���\��(��]���₷��)�C�`��C�ʒu�E�p��������̕ϐ��ɂ܂Ƃ߂�
	btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state, shape, inertia);

	// ���̃I�u�W�F�N�g(btRigidBody)�̐���
	btRigidBody* body = new btRigidBody(rb_info);

	// ���̃I�u�W�F�N�g�ɔԍ���ݒ�
	body->setUserPointer(&index);

	// Bullet���[���h�ɍ��̃I�u�W�F�N�g��ǉ�
	g_pDynamicsWorld->addRigidBody(body);

	return body;
}


/**
*  �_��̂����[���h�ɒǉ�����
*/
void bulletSystem::addSoftBody(btSoftBody* sb)
{
	((btSoftRigidDynamicsWorld*)g_pDynamicsWorld)->addSoftBody(sb);
}


/*!
* btSoftBody�̕`��
* @param[in] sbody btSoftBody�I�u�W�F�N�g
*/
void bulletSystem::DrawBulletSoftBody(btSoftBody* sbody)
{
	// btSoftBody�ɖ�(face)���Ȃ��ꍇ�͐��Ƃ��ĕ`��
	if (sbody->m_faces.size() == 0) {
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < sbody->m_nodes.size(); ++i) {
			const btSoftBody::Node &node = sbody->m_nodes[i]; // ���_�m�[�h(���_)
			btVector3 p = node.m_x; // ���_���W
			glVertex3f(p.x(), p.y(), p.z());
		}
		glEnd();
	}

	// ��(face)������ꍇ�̓|���S���Ƃ��ĕ`��
	for (int i = 0; i < sbody->m_faces.size(); ++i) {
		const btSoftBody::Face &face = sbody->m_faces[i];

		glBegin(GL_POLYGON);
		for (int j = 0; j < 3; ++j) {
			const btSoftBody::Node* node = face.m_n[j]; // �ʂ��\�����钸�_�m�[�h
			btVector3 p = node->m_x; // ���_���W
			btVector3 n = node->m_n; // ���_�@��

			glNormal3f(n.x(), n.y(), n.z());
			glVertex3f(p.x(), p.y(), p.z());

		}
		glEnd();
	}

}


