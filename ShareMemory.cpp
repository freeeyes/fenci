#include "ShareMemory.h"

char* Open_Share_Memory_API(shm_key obj_key, size_t obj_shm_size, shm_id& obj_shm_id, bool& blIsCreate)
{
#ifdef WIN32
	char sz_Key[50] = {'\0'};
	sprintf_s(sz_Key, 50, "%d", obj_key);

	blIsCreate = true;

	//��������
	SECURITY_DESCRIPTOR sd = {0};
	SECURITY_ATTRIBUTES sa = {0};
	::InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl(&sd,TRUE,NULL,FALSE);
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = &sd;
	sa.bInheritHandle = TRUE;

	//�������ڴ��ļ�������Ϊwindows�����ڴ����������̵���ʧ����ʧ
	//���ļ��Ͳ������������
	//�����ж��ļ��Ƿ���ڣ����û���򴴽����������Ƚ��ڴ��С�Ƿ�һ��
	//���һ�����
	shm_id obj_local_id;   //�����ļ����
	DWORD access      = GENERIC_READ | GENERIC_WRITE;
	DWORD shared_mode = 7;
	DWORD creation    = OPEN_ALWAYS;
	 
	LPSECURITY_ATTRIBUTES sa_local = {0};
	obj_local_id = ::CreateFileA(sz_Key, 
							   access,
							   shared_mode,
							   sa_local,
							   creation,
							   0, 
							   0);

	//�õ��ļ���С
	DWORD dwFileSize = ::GetFileSize(obj_local_id, NULL);
	if(dwFileSize > 0 && dwFileSize != obj_shm_size)
	{
		//��������ڴ��ļ���С��ƥ�䣬��Ҫɾ���ļ��ؽ�
		::CloseHandle(obj_local_id);
		::DeleteFileA(sz_Key);
		obj_local_id = ::CreateFileA(sz_Key, 
			access,
			shared_mode,
			sa_local,
			creation,
			0, 
			0);
	}
	else if(dwFileSize > 0)
	{
		//�򿪹����ڴ�
		blIsCreate = false;
	}
	

	//�����µĹ����ڴ�
	obj_shm_id = ::CreateFileMappingA(obj_local_id, &sa, PAGE_READWRITE, 0, (DWORD)obj_shm_size, (LPCSTR)sz_Key);
	if(obj_shm_id != 0)
	{
		char* pBase = (char* )MapViewOfFile(obj_shm_id, FILE_MAP_ALL_ACCESS, 0, 0, 0);
		//FlushViewOfFile(pBase, obj_shm_size); 
		return pBase;
	}
#else
	blIsCreate = true;

	//��Ѱ��ָ���Ĺ����ڴ��Ƿ����
	obj_shm_id = shmget(obj_key, 0, PERMS_IPC);
	if(obj_shm_id > 0)
	{
		//��õ�ǰ�����ڴ��С
		struct shmid_ds buf;
		shmctl(obj_shm_id, IPC_STAT, &buf);
		int n_Curr_Shm_Size = buf.shm_segsz;
		if(n_Curr_Shm_Size != (int)obj_shm_size)
		{
			//�����ڴ��С��һ�£��رչ����ڴ棬���´���
			int nRet = shmctl(obj_shm_id, IPC_RMID, &buf);
			if(0 != nRet)
			{
				printf("[Open_Share_Memory_API](%d)(%u)delete shm fail(%d).\n", 
					n_Curr_Shm_Size, (unsigned int)obj_shm_size, errno);
				return NULL;
			}
		}
		else
		{
			blIsCreate = false;
			return (char *)shmat(obj_shm_id, (char *)0, 0);
		}
	}

	//�����µĹ����ڴ�
	obj_shm_id = shmget(obj_key, obj_shm_size, PERMS_IPC|IPC_CREAT);
	if(obj_shm_id > 0)
	{
		return (char *)shmat(obj_shm_id, (char *)0, 0);
	}
#endif
	return NULL;
}

void Close_Share_Memory_API(const char* pBase, shm_id& obj_shm_id, size_t stPoolSize)
{
#ifdef WIN32
	FlushViewOfFile(pBase, stPoolSize); 
	UnmapViewOfFile(pBase);
	CloseHandle(obj_shm_id); 
#else
	if(NULL != pBase)
	{
		printf("[Close_Share_Memory_API]Close obj_shm_id=%d, size=%u.\n", obj_shm_id, (unsigned int)stPoolSize);
	}
#endif
}

void Delete_Share_Memory_API(shm_key obj_key)
{
	shm_id obj_shm_id;
	char* buf;
#ifdef WIN32
	//windows�ݲ�ʵ��
#else	
	//��Ѱ��ָ���Ĺ����ڴ��Ƿ����
	obj_shm_id = shmget(obj_key, 0, PERMS_IPC);
	if(obj_shm_id > 0)
	{
		struct shmid_ds buf;
		//�����ڴ��С��һ�£��رչ����ڴ棬���´���
		int nRet = shmctl(obj_shm_id, IPC_RMID, &buf);
		if(0 != nRet)
		{
			printf("[Delete_Share_Memory_API]err=%d", errno);
		}		
	}
#endif	
}

