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
    if (argc < 2)
    {
        printf("no input ini file\n");
        return -1;
    }

    CResource resource;
    CResource::FileList listFiles;

    printf("Reading files from file '%s'\n", argv[1]);

    FILE* in = fopen(argv[1], "r");
    if (in)
    {
        fseek(in, 0, SEEK_END);
        int length = ftell(in);
        rewind(in);

        std::vector<char> buffer(length + 1);

        fread(&buffer[0], 1, length, in);
        fclose(in);

        for (char* name = strtok(&buffer[0], "\n"); name != nullptr; name = strtok(nullptr, "\n"))
        {
            if (strncmp(name, "//", 2) != 0)
            {
                FILE* infile = fopen(name, "rb");
                if (infile)
                {
                    CResource::sItemHeader strFile;
                    strncpy(strFile.name, name, sizeof(strFile.name));
                    listFiles.push_back(strFile);
                    fclose(infile);
                }
            }
        }

        unsigned filesCount = listFiles.size();
        printf("loaded %u files\n", filesCount);

        char achS[4096];
        snprintf(achS, sizeof(achS), "%s.pak", argv[1]);
        FILE* out = fopen(achS, "wb");
        if (out != nullptr)
        {
            // header
            resource.EncodeData(&filesCount, sizeof(filesCount));
            fwrite(&CResource::RES_SIGNATURE, sizeof(CResource::RES_SIGNATURE), 1, out);
            fwrite(&filesCount, sizeof(filesCount), 1, out);

            filesCount = listFiles.size();

            // skip header info
            CResource::sItemHeader file;
            for (unsigned i = 0; i < filesCount; i++)
            {
                fwrite(&file, 1, sizeof(CResource::sItemHeader), out);
            }

            for (int pos = 0; pos < filesCount; pos++)
            {
                CResource::sItemHeader& item = listFiles[pos];

                sprintf(achS, "./%s", item.name);
                printf("adding '%s' file...", item.name);

                FILE* infile = fopen(item.name, "rb");
                if (infile)
                {
                    // get data length
                    fseek(infile, 0, SEEK_END);

                    item.length = ftell(infile);
                    rewind(infile);
                    item.position = ftell(out); // data position
                    // write data
                    do
                    {
                        char buffer[10000];
                        unsigned readed = fread(buffer, 1, sizeof(buffer), infile);
                        if (readed > 0)
                        {
                            resource.EncodeData(buffer, readed);
                            fwrite(buffer, 1, readed, out);
                        }
                    }
                    while (!feof(infile));
                    fclose(infile);
                    printf(" len %u [ OK ]\n", item.length);
                }
                else
                {
                    printf(" [ ERROR ]\n");
                }
            }

            fseek(out, 2 * sizeof(unsigned), SEEK_SET);
            for (unsigned i = 0; i < filesCount; i++)
            {
                CResource::sItemHeader& item = listFiles[i];

                resource.EncodeData(&item, sizeof(CResource::sItemHeader));
                fwrite(&item, 1, sizeof(CResource::sItemHeader), out);
            }
            fclose(out);
        }

        printf("Done, '%u' files combined\n", filesCount);
    }

    return 0;
}
