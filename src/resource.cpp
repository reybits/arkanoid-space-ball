////////////////////////////////////////////////////////////////
// This class allow to read data from resource file
// Andrey A. Ugolnik / 'WE' Group
// Nov 17, 2006
////////////////////////////////////////////////////////////////

#include "resource.h"

CResource::CResource() {
	m_pchResource	= 0;
}

CResource::~CResource() {
	delete[]	m_pchResource;
}

bool CResource::Open(const char *pchResource) {
	printf("Opening resource file '%s'...", pchResource);
	FILE	*pFile	= fopen(pchResource, "rb");
   if(pFile) {
		unsigned int	dwCount;
		// read signature
		fread(&dwCount, 1, sizeof(dwCount), pFile);
		if(_GetUInt((unsigned char*)&dwCount) == RES_SIGNATURE) {
			// read files count
			fread(&dwCount, 1, sizeof(dwCount), pFile);
			EncodeData(&dwCount, sizeof(dwCount));
			dwCount	= _GetUInt((unsigned char*)&dwCount);
			m_listFiles.reserve(dwCount);
			RES_FILE_HEADER	file;
			for(int i = 0; i < dwCount; i++) {
				// read and store files list
				if(sizeof(file) == fread(&file, 1, sizeof(file), pFile)) {
					EncodeData(&file, sizeof(file));
					//printf("file '%s' %d bytes\n", file.achName, file.nDataLen);
					m_listFiles.push_back(file);
				}
				else {
					printf("  incorrect resource file size.\n");
					fclose(pFile);
					return false;
				}
			}
			m_pchResource	= new char[strlen(pchResource) + 1];
			strcpy(m_pchResource, pchResource);
			printf("  done, contain %d files.\n", dwCount);
			return true;
		}
		printf("  broken resource file.\n");
		fclose(pFile);
		return false;
	}
	printf("  not found.\n");
	return false;
}

unsigned int CResource::_GetUInt(unsigned char *pbyData) {
        return (unsigned int)(pbyData[0] | (pbyData[1] << 8) | (pbyData[2] << 16) | (pbyData[3] << 24));
}

unsigned char *CResource::GetDataAllocMem(const char *pchName, unsigned int &nDataLen) {
	if(m_pchResource != 0) {
		printf("Opening '%s' resource...", pchName);
		for(int i = 0; i < m_listFiles.size(); i++) {
			if(strcmp(m_listFiles[i].achName, pchName) == 0) {
				nDataLen	= _GetUInt((unsigned char*)m_listFiles[i].nDataLen);
				unsigned char	*pbyData	= new unsigned char[nDataLen];
				FILE *pResFile	= fopen(m_pchResource, "rb");
				if(pResFile) {
					fseek(pResFile, _GetUInt((unsigned char*)m_listFiles[i].nDataPos), SEEK_SET);
					fread(pbyData, 1, nDataLen, pResFile);
					fclose(pResFile);
					EncodeData(pbyData, nDataLen);
					return pbyData;
				}
			}
		}
		printf("\n  file '%s' not found\n", pchName);
	}
	else {
		printf("Resource file not loaded\n");
	}

	return 0;
}

void CResource::FreeMem(unsigned char *pbyData) {
	delete[]	pbyData;
}

void CResource::EncodeData(const void *pbyData, int nLen) {
	unsigned char	*p	= (unsigned char*)pbyData;
	while(nLen--)	*p++	^= RES_XOR;
}

// SDL_RWops *CResource::GetFile(const char *pchFile) {
// 	register SDL_RWops	*pRWops;
//
// 	if(m_pchResource == 0) {
// 		return SDL_RWFromFile(pchFile, "rb");
// 	}
//
// 	FILE *pResFile	= fopen(m_pchResource, "rb");
// 	if(pResFile) {
// 		for(int i = 0; i < m_listFiles.size(); i++) {
// 			if(strcmp(m_listFiles[i].achName, pchFile) == 0) {
// 				//fseek(pResFile, m_listFiles[i].nDataPos, SEEK_SET);
// 				if(0 == (pRWops = SDL_AllocRW())) {
// 					SDL_SetError("No memory for RWops\n");
// 					//errno	= ENOMEM;
// 					return 0;
// 				}
// 				pRWops->seek	= _SeekFn;
// 				pRWops->read	= _ReadFn;
// 				pRWops->write	= _WriteFn;
// 				pRWops->close	= _CloseFn;
// 				pRWops->type	= 0xdeadbeef;
// 				RES_DATA	*pData	= (RES_DATA*)malloc(sizeof(RES_DATA));
// 				pData->pFile			= pResFile;
// 				pData->nPos				= m_listFiles[i].nDataPos;
// 				pData->nLen				= m_listFiles[i].nDataLen;
// 				pData->nCurrentPos	= m_listFiles[i].nDataPos;
// 				pRWops->hidden.unknown.data1	= pData;
// 				printf("Opening '%s' resource...\n", m_listFiles[i].achName);
// 				return pRWops;
// 			}
// 		}
// 		SDL_SetError("File '%s' not found\n", pchFile);
// 		//errno	= ENOTFOUND;
// 	}
// 	SDL_SetError("Can't open resource file '%s'\n", m_pchResource);
// 	//errno	= ECANTOPEN;
//
// 	return 0;
// }
//
// // These functions shouldE not be used except from pointers in a RWops
// int CResource::_SeekFn(SDL_RWops *context, int offset, int whence) {
// 	//printf("seek %d, %d\n", offset, whence);
// 	FILE	*pFile	= ((RES_DATA*)context->hidden.unknown.data1)->pFile;
// 	long	nPos		= ((RES_DATA*)context->hidden.unknown.data1)->nPos;
// 	long	nLen		= ((RES_DATA*)context->hidden.unknown.data1)->nLen;
// 	switch(whence) {
// 	case SEEK_SET:
// 		((RES_DATA*)context->hidden.unknown.data1)->nCurrentPos	= nPos + offset;
// 		break;
// 	case SEEK_END:
// 		((RES_DATA*)context->hidden.unknown.data1)->nCurrentPos	= nPos + nLen - offset;
// 		break;
// 	case SEEK_CUR:
// 		((RES_DATA*)context->hidden.unknown.data1)->nCurrentPos	+= offset;
// 		break;
// 	}
// 	return fseek(pFile, ((RES_DATA*)context->hidden.unknown.data1)->nCurrentPos, SEEK_SET);
// }
//
// int CResource::_ReadFn(SDL_RWops *context, void *ptr, int size, int maxnum) {
// 	//printf("read: %d, %d\n", size, maxnum);
// 	return fread(ptr, size, maxnum, ((RES_DATA*)context->hidden.unknown.data1)->pFile);
// }
//
// // not supported now
// int CResource::_WriteFn(SDL_RWops *context, const void *ptr, int size, int num) {
// 	//printf("write\n");
// 	return num;
// }
//
// int CResource::_CloseFn(SDL_RWops *context) {
// 	//printf("close\n");
// 	if(context->type != 0xdeadbeef)  {
// 		SDL_SetError("Wrong kind of RWops for _CloseFn()");
// 		return -1;
// 	}
//
// 	fclose(((RES_DATA*)context->hidden.unknown.data1)->pFile);
// 	free(context->hidden.unknown.data1);
// 	SDL_FreeRW(context);
//
// 	return 0;
// }
