#include <cstdio>
#include <cstring>

static char* CheckRegistration(const char* pchRegName)
{
    const char achKeyWord[16] = "oPZmAtk73v4eNjE";
    char achTmp[10];
    //char achRegKey[16 + 3 + 1] = { 0 }; // 16 symbols + 3 dashes + 1 null
    static char achGenerated[16 + 1];
    achGenerated[16] = 0;

    strcpy(achGenerated, achKeyWord);
    unsigned int nLen = strlen(pchRegName);
    for(int i = 1; i < 16; i++)
    {
        for(unsigned int a = 0; a < nLen; a++)
        {
            sprintf(achTmp, "%2X", achKeyWord[i] ^ pchRegName[a]);
            achGenerated[i]	= achTmp[a % 2];
        }
        sprintf(achTmp, "%2X", achGenerated[i - 1] ^ achKeyWord[i] ^ nLen);
        switch(achTmp[1])
        {
        case '0': achTmp[1] = 'H';  break;
        case '1': achTmp[1] = 'Z';  break;
        case '8': achTmp[1] = 'X';  break;
        case 'A': achTmp[1] = 'N';  break;
        case 'B': achTmp[1] = 'K';  break;
        case 'C': achTmp[1] = 'W';  break;
        case 'D': achTmp[1] = 'Y';  break;
        case 'G': achTmp[1] = 'P';  break;
        }
        achGenerated[i - 1] = achTmp[1];
    }

    return achGenerated;
}

static void EncodeDecode(void* pData, int nLen)
{
    unsigned char* p = (unsigned char*)pData;
    unsigned char by = 0xaa;
    while(nLen--)
    {
        *p ^= by;
        *p += 128;
        by += 3;
        p++;
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Arkanoid: Space Ball key generator.\n");
        printf("Use: %s \"User Name\"\n\n", argv[0]);
        return 0;
    }

    char achRegName[100 + 1] = { 0 };
    char achRegKey[16 + 3 + 1] = { 0 }; // 16 symbols + 3 dashes + 1 null

    strncpy(achRegName, argv[1], 100);
    strncpy(achRegKey, CheckRegistration(argv[1]), 16);

    printf("Name: %s\n", achRegName);
    printf("Key: %s\n", achRegKey);
    printf("Mac OS X:\n");
    printf("   Place file 'key' in to ~/Library/Application Support/arkanoidsb/\n");

    EncodeDecode(&achRegName, sizeof(achRegName));
    EncodeDecode(&achRegKey, sizeof(achRegKey));

    FILE* f = fopen("key", "wb");
    if(f)
    {
        fwrite(achRegName, sizeof(achRegName), 1, f);
        fwrite(achRegKey, sizeof(achRegKey), 1, f);
        fclose(f);
    }

    return 0;
}

