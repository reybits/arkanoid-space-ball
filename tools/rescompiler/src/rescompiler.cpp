/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "resource.h"

#include <cstdio>
#include <cstdlib>

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        printf("no input ini file\n");
        return -1;
    }

    CResource resource;
    CResource::FileList listFiles;

    printf("Reading files from file '%s'\n", argv[1]);

    FILE* descFile = fopen(argv[1], "r");
    if(descFile)
    {
        fseek(descFile, 0, SEEK_END);
        int length = ftell(descFile);
        rewind(descFile);

        std::vector<char> buffer(length + 1);

        fread(&buffer[0], 1, length, descFile);
        fclose(descFile);

        char* filename = strtok(&buffer[0], "\n");
        while(filename)
        {
            if(strncmp(filename, "//", 2) != 0)
            {
                FILE* infile = fopen(filename, "rb");
                if(infile)
                {
                    CResource::sItemHeader strFile;
                    strncpy(strFile.name, filename, sizeof(strFile.name));
                    listFiles.push_back(strFile);
                    fclose(infile);
                }
            }
            filename = strtok(NULL, "\n");
        }

        unsigned filesCount = listFiles.size();
        printf("loaded %u files\n", filesCount);

        char achS[PATH_MAX];
        sprintf(achS, "%s.pak", argv[1]);
        if((descFile = fopen(achS, "wb")))
        {
            // header
            resource.EncodeData(&filesCount, sizeof(filesCount));
            fwrite(&CResource::RES_SIGNATURE, sizeof(CResource::RES_SIGNATURE), 1, descFile);
            fwrite(&filesCount, sizeof(filesCount), 1, descFile);

            filesCount = listFiles.size();

            // skip header info
            CResource::sItemHeader file;
            for(unsigned i = 0; i < filesCount; i++)
            {
                fwrite(&file, 1, sizeof(CResource::sItemHeader), descFile);
            }

            for(int pos = 0; pos < filesCount; pos++)
            {
                CResource::sItemHeader& item = listFiles[pos];

                sprintf(achS, "./%s", item.name);
                printf("adding '%s' file...", item.name);

                FILE* infile = fopen(item.name, "rb");
                if(infile)
                {
                    // get data length
                    fseek(infile, 0, SEEK_END);

                    item.length = ftell(infile);
                    rewind(infile);
                    item.position = ftell(descFile); // data position
                    // write data
                    do
                    {
                        char buffer[10000];
                        unsigned readed = fread(buffer, 1, sizeof(buffer), infile);
                        if(readed > 0)
                        {
                            resource.EncodeData(buffer, readed);
                            fwrite(buffer, 1, readed, descFile);
                        }
                    } while(!feof(infile));
                    fclose(infile);
                    printf(" len %u [ OK ]\n", item.length);
                }
                else
                {
                    printf(" [ ERROR ]\n");
                }
            }

            fseek(descFile, 2 * sizeof(unsigned), SEEK_SET);
            for(unsigned i = 0; i < filesCount; i++)
            {
                CResource::sItemHeader& item = listFiles[i];

                resource.EncodeData(&item, sizeof(CResource::sItemHeader));
                fwrite(&item, 1, sizeof(CResource::sItemHeader), descFile);
            }
            fclose(descFile);
        }

        printf("Done, '%u' files combined\n", filesCount);
    }

    return 0;
}

