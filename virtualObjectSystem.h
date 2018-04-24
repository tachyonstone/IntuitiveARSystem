#ifndef VIRTUAL_OBJECT_SYSTEM_H
#define VIRTUAL_OBJECT_SYSTEM_H


// 剛体オブジェクト生成
extern btRigidBody* body[2];
extern btSoftBody* sphere[2];

// ジェスチャによる仮想物体のサイズ指定
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
