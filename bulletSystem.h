#ifndef BULLET_SYSTEM_H
#define BULLET_SYSTEM_H


class bulletSystem
{
public:
	btRigidBody* CreateRigidBody(
		double mass,
		const btTransform& init_trans,
		btCollisionShape* shape,
		int index
	);

	void addSoftBody(btSoftBody* sb);
	void DrawBulletSoftBody(btSoftBody* sbody);
	void InitBullet(void);
	void CleanBullet(void);

};

#endif


