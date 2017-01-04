#ifndef _SHARE_MEMORY_H
#define _SHARE_MEMORY_H

//�����ڴ�ӿ�API
//add by freeeyes

#include <stdio.h>
#ifdef WIN32
#include <Windows.h> 
#else 
#include <unistd.h> 
#include <sys/stat.h>  
#include <sys/types.h>  
#include <sys/ipc.h>  
#include <sys/shm.h> 
#include <errno.h> 
#endif

typedef unsigned int shm_key;

#ifdef WIN32
typedef HANDLE shm_id;
#else
typedef int shm_id;
#define PERMS_IPC 0600
#endif

//��һ�������ڴ�,blIsCreateΪtrue���Ǳ�������,Ϊfalse�Ǵ򿪵ġ�
char* Open_Share_Memory_API(shm_key obj_key, size_t obj_shm_size, shm_id& obj_shm_id, bool& blIsCreate);

//�ر�һ�������ڴ�
void Close_Share_Memory_API(const char* pBase, shm_id& obj_shm_id, size_t stPoolSize);

//ɾ��һ�������ڴ�
void Delete_Share_Memory_API(shm_key obj_key);

#endif
