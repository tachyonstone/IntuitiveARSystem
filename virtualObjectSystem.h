#ifndef VIRTUAL_OBJECT_SYSTEM_H
#define VIRTUAL_OBJECT_SYSTEM_H


// ���̃I�u�W�F�N�g����
extern btRigidBody* body[2];
extern btSoftBody* sphere[2];

// �W�F�X�`���ɂ�鉼�z���̂̃T�C�Y�w��
extern int create_stat;
extern int dir_index;
extern int dir_min;
extern int create_num;

class virtualObjectSystem {
public :
	void CreateModel(void);
	void SetRigidBodies(void);
	void DrawBulletObjects(void);

};


#endif
