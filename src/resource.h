#if !defined(RESOURCE_FILE_CLASS)
#define RESOURCE_FILE_CLASS

#include <SDL_rwops.h>
#include <SDL_error.h>
#include <vector>
using namespace std;

class CResource {
public:
	CResource();
	~CResource();
	bool Open(const char *pchResource);
	//SDL_RWops *GetFile(const char *pbyFile);
	unsigned char *GetDataAllocMem(const char *pchName, unsigned int &nDataLen);
	void FreeMem(unsigned char *pbyData);
	
protected:
#define RES_SIGNATURE	((unsigned int)0x0abcd)
#define RES_MAX_NAME		99
#define RES_XOR			((unsigned char)0x0aa)
	struct RES_FILE_HEADER {
		char	achName[RES_MAX_NAME + 1];
		long	nDataPos;	// position in resource file (seek)
		long	nDataLen;	// length of data
	};
	std::vector<RES_FILE_HEADER>m_listFiles;
	char *m_pchResource;
	void EncodeData(const void *pbyData, int nLen);
// 	struct RES_DATA {
// 		FILE	*pFile;
// 		long	nPos;
// 		long	nLen;
// 		long	nCurrentPos;
// 	};
// 	static int _SeekFn(SDL_RWops *context, int offset, int whence);
// 	static int _ReadFn(SDL_RWops *context, void *ptr, int size, int maxnum);
// 	static int _WriteFn(SDL_RWops *context, const void *ptr, int size, int num);
// 	static int _CloseFn(SDL_RWops *context);
};

#endif
