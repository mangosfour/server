/**
 * MaNGOS is a full featured server for World of Warcraft, supporting
 * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
 *
 * Copyright (C) 2005-2023 MaNGOS <https://getmangos.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <deque>
#include <map>
#include <set>
#include <cstdlib>

#ifdef WIN32
#include "direct.h"
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#include "dbcfile.h"

#include "sl/adt.h"
#include "sl/wdt.h"
#include <fcntl.h>

#include "../shared/ExtractorCommon.h"

#ifndef WIN32
#include <unistd.h>
#endif

#if defined( __GNUC__ )
#define _open   open
#define _close close
#ifndef O_BINARY
#define O_BINARY 0
#endif
#else
#include <io.h>
#endif

#ifdef O_LARGEFILE
#define OPEN_FLAGS  (O_RDONLY | O_BINARY | O_LARGEFILE)
#else
#define OPEN_FLAGS (O_RDONLY | O_BINARY)
#endif

typedef struct
{
    char name[64];
    uint32 id;
} map_id;

typedef std::pair < std::string /*full_filename*/, char const* /*locale_prefix*/ > UpdatesPair;
typedef std::map < int /*build*/, UpdatesPair > Updates;

map_id* pMap_ids;
uint16* pAreas;
uint16* pLiqType;
char output_path[128] = ".";
char input_path[128] = ".";
uint32 uMaxAreaId = 0;
uint32 CONF_max_build = 0;
int iCoreNumber = 0;

//**************************************************
// Extractor options
//**************************************************
enum Extract
{
    EXTRACT_MAP = 1,
    EXTRACT_DBC = 2
};

// Select data for extract
int   CONF_extract = EXTRACT_MAP | EXTRACT_DBC;
// This option allow limit minimum height to some value (Allow save some memory)
// see contrib/mmap/src/Tilebuilder.h, INVALID_MAP_LIQ_HEIGHT
bool  CONF_allow_height_limit = true;
float CONF_use_minHeight = -500.0f;

// This option allow use float to int conversion
bool  CONF_allow_float_to_int   = true;
float CONF_float_to_int8_limit  = 2.0f;      // Max accuracy = val/256
float CONF_float_to_int16_limit = 2048.0f;   // Max accuracy = val/65536
float CONF_flat_height_delta_limit = 0.005f; // If max - min less this value - surface is flat
float CONF_flat_liquid_delta_limit = 0.001f; // If max - min less this value - liquid surface is flat

#define MIN_SUPPORTED_BUILD 18273                           // code expect mpq files and mpq content files structure for this build or later
#define EXPANSION_COUNT 4
#define WORLD_COUNT 1


bool FileExists(const char* FileName)
{
    int fp = _open(FileName, OPEN_FLAGS);
    if (fp != -1)
    {
        _close(fp);
        return true;
    }

    return false;
}

void Usage(char* prg)
{
    printf(
        "Usage:\n"
        "%s -[var] [value]\n"
        "-i set input path\n"
        "-o set output path\n"
        "-e extract only MAP(1)/DBC(2) - standard: both(3)\n"
        "-e extract only MAP(1)/DBC(2) - temporary only: DBC(2)\n"
        "-f height stored as int (less map size but lost some accuracy) 1 by default\n"
        "-b extract data for specific build (at least not greater it from available). Min supported build %u.\n"
        "Example: %s -f 0 -i \"c:\\games\\game\"", prg, MIN_SUPPORTED_BUILD, prg);
    exit(1);
}

void HandleArgs(int argc, char* arg[])
{
    for (int c = 1; c < argc; ++c)
    {
        // i - input path
        // o - output path
        // e - extract only MAP(1)/DBC(2) - standard both(3)
        // f - use float to int conversion
        // h - limit minimum height
        if (arg[c][0] != '-')
        {
            Usage(arg[0]);
        }

        switch (arg[c][1])
        {
            case 'i':
                if (c + 1 < argc)                           // all ok
                {
                    strcpy(input_path, arg[(c++) + 1]);
                }
                else
                {
                    Usage(arg[0]);
                }
                break;
            case 'o':
                if (c + 1 < argc)                           // all ok
                {
                    strcpy(output_path, arg[(c++) + 1]);
                }
                else
                {
                    Usage(arg[0]);
                }
                break;
            case 'f':
                if (c + 1 < argc)                           // all ok
                {
                    CONF_allow_float_to_int = atoi(arg[(c++) + 1]) != 0;
                }
                else
                {
                    Usage(arg[0]);
                }
                break;
            case 'e':
                if (c + 1 < argc)                           // all ok
                {
                    CONF_extract = atoi(arg[(c++) + 1]);
                    if (!(CONF_extract > 0 && CONF_extract < 4))
                    {
                        Usage(arg[0]);
                    }
                }
                else
                {
                    Usage(arg[0]);
                }
                break;
            case 'b':
                if (c + 1 < argc)                           // all ok
                {
                    CONF_max_build = atoi(arg[(c++) + 1]);
                    if (CONF_max_build < MIN_SUPPORTED_BUILD)
                    {
                        Usage(arg[0]);
                    }
                }
                else
                {
                    Usage(arg[0]);
                }
                break;
            default:
                Usage(arg[0]);
                break;
        }
    }
}

void AppendDBCFileListTo(HANDLE mpqHandle, std::set<std::string>& filelist)
{
    SFILE_FIND_DATA findFileData;

    HANDLE searchHandle = SFileFindFirstFile(mpqHandle, "*.dbc", &findFileData, NULL);
    if (!searchHandle)
    {
        return;
    }

    filelist.insert(findFileData.cFileName);

    while (SFileFindNextFile(searchHandle, &findFileData))
        filelist.insert(findFileData.cFileName);

    SFileFindClose(searchHandle);
}

void AppendDB2FileListTo(HANDLE mpqHandle, std::set<std::string>& filelist)
{
    SFILE_FIND_DATA findFileData;

    HANDLE searchHandle = SFileFindFirstFile(mpqHandle, "*.db2", &findFileData, NULL);
    if (!searchHandle)
    {
        return;
    }

    filelist.insert(findFileData.cFileName);

    while (SFileFindNextFile(searchHandle, &findFileData))
        filelist.insert(findFileData.cFileName);

    SFileFindClose(searchHandle);
}

uint32 ReadBuild(int locale)
{
    // include build info file also
    std::string filename  = std::string("component.wow-") + Locales[locale] + ".txt";
    //printf("Read %s file... ", filename.c_str());

    HANDLE fileHandle;

    if (!OpenNewestFile(filename.c_str(), &fileHandle))
    {
        printf("Fatal error: Not found %s file!\n", filename.c_str());
        exit(1);
    }

    unsigned int data_size = SFileGetFileSize(fileHandle, NULL);

    std::string text;
    text.resize(data_size);

    if (!SFileReadFile(fileHandle, &text[0], data_size, NULL, NULL))
    {
        printf("Fatal error: Can't read %s file!\n", filename.c_str());
        exit(1);
    }

    SFileCloseFile(fileHandle);

    size_t pos = text.find("version=\"");
    size_t pos1 = pos + strlen("version=\"");
    size_t pos2 = text.find("\"", pos1);
    if (pos == text.npos || pos2 == text.npos || pos1 >= pos2)
    {
        printf("Fatal error: Invalid  %s file format!\n", filename.c_str());
        exit(1);
    }

    std::string build_str = text.substr(pos1, pos2 - pos1);

    int build = atoi(build_str.c_str());
    if (build <= 0)
    {
        printf("Fatal error: Invalid  %s file format!\n", filename.c_str());
        exit(1);
    }

    if (build < MIN_SUPPORTED_BUILD)
    {
        printf("Fatal error: tool can correctly extract data only for build %u or later (detected: %u)!\n", MIN_SUPPORTED_BUILD, build);
        exit(1);
    }

    return build;
}

uint32 ReadMapDBC(HANDLE localeMpqHandle, int const locale)
{
    printf("\n Reading maps from Map.dbc... ");

    HANDLE dbcFile;
    if (!SFileOpenFileEx(localeMpqHandle, "DBFilesClient\\Map.dbc", SFILE_OPEN_FROM_MPQ, &dbcFile))
    {
        printf("Fatal error: Cannot find Map.dbc in archive!\n");
        exit(1);
    }

    DBCFile dbc(dbcFile);
    if (!dbc.open())
    {
        printf("Fatal error: Could not read Map.dbc!\n");
        exit(1);
    }

    size_t map_count = dbc.getRecordCount();
    pMap_ids = new map_id[map_count];
    for(uint32 x = 0; x < map_count; ++x)
    {
        pMap_ids[x].id = dbc.getRecord(x).getUInt(0);
        strcpy(pMap_ids[x].name, dbc.getRecord(x).getString(1));
    }

  //  SFileCloseFile(dbcFile);
    printf("Done! (%u maps loaded)\n", uint32(map_count));
    return map_count;
}

void ReadAreaTableDBC(HANDLE localeMpqHandle, int const locale)
{
    printf("Read AreaTable.dbc file...");

    HANDLE dbcFile;
    if (!SFileOpenFileEx(localeMpqHandle, "DBFilesClient\\AreaTable.dbc", SFILE_OPEN_FROM_MPQ, &dbcFile))
    {
        printf("Fatal error: Cannot find AreaTable.dbc in archive!\n");
        exit(1);
    }

    DBCFile dbc(dbcFile);

    if (!dbc.open())
    {
        printf("Fatal error: Could not read AreaTable.dbc!\n");
        exit(1);
    }

    size_t area_count = dbc.getRecordCount();
    size_t maxid = dbc.getMaxId();
    pAreas = new uint16[maxid + 1];
    memset(pAreas, 0xff, (maxid + 1) * sizeof(uint16));

    for (uint32 x = 0; x < area_count; ++x)
    {
        pAreas[dbc.getRecord(x).getUInt(0)] = dbc.getRecord(x).getUInt(3);
    }

    uMaxAreaId = dbc.getMaxId();

    printf("Done! (%lu areas loaded)\n", area_count);
}

void ReadLiquidTypeTableDBC(int const locale)
{
    HANDLE localeFile;
    char localMPQ[512];

    sprintf(localMPQ, "%s/Data/misc.MPQ", input_path);//, langs[locale], langs[locale]);
    if (FileExists(localMPQ)==false)
    {   // Use misc.mpq
        sprintf(localMPQ, "%s/Data/%s/locale-%s.MPQ", input_path, Locales[locale], Locales[locale]);
    }

    if (!SFileOpenArchive(localMPQ, 0, MPQ_OPEN_READ_ONLY, &localeFile))
    {
        exit(1);
    }

    printf("Read LiquidType.dbc file...");

    HANDLE dbcFile;
    if (!SFileOpenFileEx(localeFile, "DBFilesClient\\LiquidType.dbc", SFILE_OPEN_FROM_MPQ, &dbcFile))
    {
        printf("Fatal error: Cannot find LiquidType.dbc in archive!\n");
        exit(1);
    }

    DBCFile dbc(dbcFile);
    if (!dbc.open())
    {
        printf("Fatal error: Could not read LiquidType.dbc!\n");
        exit(1);
    }

    size_t LiqType_count = dbc.getRecordCount();
    size_t LiqType_maxid = dbc.getMaxId();
    pLiqType = new uint16[LiqType_maxid + 1];
    memset(pLiqType, 0xff, (LiqType_maxid + 1) * sizeof(uint16));

    for (uint32 x = 0; x < LiqType_count; ++x)
    {
        pLiqType[dbc.getRecord(x).getUInt(0)] = dbc.getRecord(x).getUInt(3);
    }

    printf("Done! (%lu LiqTypes loaded)\n", LiqType_count);
}

//
// Adt file convertor function and data
//

// Map file format data
static char const* MAP_MAGIC         = "MAPS";
static char const* MAP_VERSION_MAGIC = "p1.4";
static char const* MAP_AREA_MAGIC    = "AREA";
static char const* MAP_HEIGHT_MAGIC  = "MHGT";
static char const* MAP_LIQUID_MAGIC  = "MLIQ";

/**
 * @brief
 *
 */
struct map_fileheader
{
    uint32 mapMagic;        /**< TODO */
    uint32 versionMagic;    /**< TODO */
    uint32 buildMagic;
    uint32 areaMapOffset;   /**< TODO */
    uint32 areaMapSize;     /**< TODO */
    uint32 heightMapOffset; /**< TODO */
    uint32 heightMapSize;   /**< TODO */
    uint32 liquidMapOffset; /**< TODO */
    uint32 liquidMapSize;   /**< TODO */
    uint32 holesOffset;     /**< TODO */
    uint32 holesSize;       /**< TODO */
};

#define MAP_AREA_NO_AREA      0x0001

/**
 * @brief
 *
 */
struct map_areaHeader
{
    uint32 fourcc;          /**< TODO */
    uint16 flags;           /**< TODO */
    uint16 gridArea;        /**< TODO */
};

#define MAP_HEIGHT_NO_HEIGHT  0x0001
#define MAP_HEIGHT_AS_INT16   0x0002
#define MAP_HEIGHT_AS_INT8    0x0004

/**
 * @brief
 *
 */
struct map_heightHeader
{
    uint32 fourcc;          /**< TODO */
    uint32 flags;           /**< TODO */
    float  gridHeight;      /**< TODO */
    float  gridMaxHeight;   /**< TODO */
};

#define MAP_LIQUID_TYPE_NO_WATER    0x00
#define MAP_LIQUID_TYPE_WATER       0x01
#define MAP_LIQUID_TYPE_OCEAN       0x02
#define MAP_LIQUID_TYPE_MAGMA       0x04
#define MAP_LIQUID_TYPE_SLIME       0x08

#define MAP_LIQUID_TYPE_DARK_WATER  0x10
#define MAP_LIQUID_TYPE_WMO_WATER   0x20


#define MAP_LIQUID_NO_TYPE    0x0001
#define MAP_LIQUID_NO_HEIGHT  0x0002

/**
 * @brief
 *
 */
struct map_liquidHeader
{
    uint32 fourcc;          /**< TODO */
    uint16 flags;           /**< TODO */
    uint16 liquidType;      /**< TODO */
    uint8  offsetX;         /**< TODO */
    uint8  offsetY;         /**< TODO */
    uint8  width;           /**< TODO */
    uint8  height;          /**< TODO */
    float  liquidLevel;     /**< TODO */
};

/**
 * @brief
 *
 * @param maxDiff
 * @return float
 */
float selectUInt8StepStore(float maxDiff)
{
    return 255 / maxDiff;
}

/**
 * @brief
 *
 * @param maxDiff
 * @return float
 */
float selectUInt16StepStore(float maxDiff)
{
    return 65535 / maxDiff;
}

uint16 area_flags[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];      /**< Temporary grid data store */

float V8[ADT_GRID_SIZE][ADT_GRID_SIZE];                         /**< TODO */
float V9[ADT_GRID_SIZE + 1][ADT_GRID_SIZE + 1];                 /**< TODO */
uint16 uint16_V8[ADT_GRID_SIZE][ADT_GRID_SIZE];                 /**< TODO */
uint16 uint16_V9[ADT_GRID_SIZE + 1][ADT_GRID_SIZE + 1];         /**< TODO */
uint8  uint8_V8[ADT_GRID_SIZE][ADT_GRID_SIZE];                  /**< TODO */
uint8  uint8_V9[ADT_GRID_SIZE + 1][ADT_GRID_SIZE + 1];          /**< TODO */

uint16 liquid_entry[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];    /**< TODO */
uint8 liquid_flags[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];     /**< TODO */
bool  liquid_show[ADT_GRID_SIZE][ADT_GRID_SIZE];                /**< TODO */
float liquid_height[ADT_GRID_SIZE + 1][ADT_GRID_SIZE + 1];      /**< TODO */
uint16 holes[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];

/**
 * @brief
 *
 * @param filename
 * @param filename2
 * @param build
 * @return bool
 */
bool ConvertADT(HANDLE hWorldMPQ, char *filename, char *filename2, int /*cell_y*/, int /*cell_x*/, uint32 build)
{
    ChunkedFile adt;
    if (!adt.loadFile(hWorldMPQ, filename))
    {
        return false;
    }

    // Prepare map header
    map_fileheader map;
    map.mapMagic = *(uint32 const*)MAP_MAGIC;
    map.versionMagic = *(uint32 const*)MAP_VERSION_MAGIC;
    map.buildMagic = build;

    // Get area flags data
    memset(area_flags, 0xFF, sizeof(area_flags));
    memset(V9, 0, sizeof(V9));
    memset(V8, 0, sizeof(V8));

    memset(liquid_show, 0, sizeof(liquid_show));
    memset(liquid_flags, 0, sizeof(liquid_flags));
    memset(liquid_entry, 0, sizeof(liquid_entry));

    memset(holes, 0, sizeof(holes));

    bool hasHoles = false;

    for (std::multimap<std::string, FileChunk*>::const_iterator itr = adt.chunks.lower_bound("MCNK"); itr != adt.chunks.upper_bound("MCNK"); ++itr)
    {
        adt_MCNK* mcnk = itr->second->As<adt_MCNK>();

        // Area data
        if (mcnk->areaid <= uMaxAreaId && pAreas[mcnk->areaid] != 0xFFFF)
        {
            area_flags[mcnk->iy][mcnk->ix] = pAreas[mcnk->areaid];
        }

        // Height
        // Height values for triangles stored in order:
        // 1     2     3     4     5     6     7     8     9
        //    10    11    12    13    14    15    16    17
        // 18    19    20    21    22    23    24    25    26
        //    27    28    29    30    31    32    33    34
        // . . . . . . . .
        // For better get height values merge it to V9 and V8 map
        // V9 height map:
        // 1     2     3     4     5     6     7     8     9
        // 18    19    20    21    22    23    24    25    26
        // . . . . . . . .
        // V8 height map:
        //    10    11    12    13    14    15    16    17
        //    27    28    29    30    31    32    33    34
        // . . . . . . . .

        // Set map height as grid height
        for (int y = 0; y <= ADT_CELL_SIZE; y++)
        {
            int cy = mcnk->iy * ADT_CELL_SIZE + y;
            for (int x = 0; x <= ADT_CELL_SIZE; x++)
            {
                int cx = mcnk->ix * ADT_CELL_SIZE + x;
                V9[cy][cx] = mcnk->ypos;
            }
        }

        for (int y = 0; y < ADT_CELL_SIZE; y++)
        {
            int cy = mcnk->iy * ADT_CELL_SIZE + y;
            for (int x = 0; x < ADT_CELL_SIZE; x++)
            {
                int cx = mcnk->ix * ADT_CELL_SIZE + x;
                V8[cy][cx] = mcnk->ypos;
            }
        }

        // Get custom height
        if (FileChunk* chunk = itr->second->GetSubChunk("MCVT"))
        {
            adt_MCVT* mcvt = chunk->As<adt_MCVT>();
            // get V9 height map
            for (int y = 0; y <= ADT_CELL_SIZE; y++)
            {
                int cy = mcnk->iy * ADT_CELL_SIZE + y;
                for (int x = 0; x <= ADT_CELL_SIZE; x++)
                {
                    int cx = mcnk->ix * ADT_CELL_SIZE + x;
                    V9[cy][cx] += mcvt->height_map[y*(ADT_CELL_SIZE * 2 + 1) + x];
                }
            }
            // get V8 height map
            for (int y = 0; y < ADT_CELL_SIZE; y++)
            {
                int cy = mcnk->iy * ADT_CELL_SIZE + y;
                for (int x = 0; x < ADT_CELL_SIZE; x++)
                {
                    int cx = mcnk->ix * ADT_CELL_SIZE + x;
                    V8[cy][cx] += mcvt->height_map[y*(ADT_CELL_SIZE * 2 + 1) + ADT_CELL_SIZE + 1 + x];
                }
            }
        }

        // Liquid data
        if (mcnk->sizeMCLQ > 8)
        {
            if (FileChunk* chunk = itr->second->GetSubChunk("MCLQ"))
            {
                adt_MCLQ* liquid = chunk->As<adt_MCLQ>();
                int count = 0;
                for (int y = 0; y < ADT_CELL_SIZE; ++y)
                {
                    int cy = mcnk->iy * ADT_CELL_SIZE + y;
                    for (int x = 0; x < ADT_CELL_SIZE; ++x)
                    {
                        int cx = mcnk->ix * ADT_CELL_SIZE + x;
                        if (liquid->flags[y][x] != 0x0F)
                        {
                            liquid_show[cy][cx] = true;
                            if (liquid->flags[y][x] & (1 << 7))
                            {
                                liquid_flags[mcnk->iy][mcnk->ix] |= MAP_LIQUID_TYPE_DARK_WATER;
                            }
                            ++count;
                        }
                    }
                }

                uint32 c_flag = mcnk->flags;
                if (c_flag & (1 << 2))
                {
                    liquid_entry[mcnk->iy][mcnk->ix] = 1;
                    liquid_flags[mcnk->iy][mcnk->ix] |= MAP_LIQUID_TYPE_WATER;            // water
                }
                if (c_flag & (1 << 3))
                {
                    liquid_entry[mcnk->iy][mcnk->ix] = 2;
                    liquid_flags[mcnk->iy][mcnk->ix] |= MAP_LIQUID_TYPE_OCEAN;            // ocean
                }
                if (c_flag & (1 << 4))
                {
                    liquid_entry[mcnk->iy][mcnk->ix] = 3;
                    liquid_flags[mcnk->iy][mcnk->ix] |= MAP_LIQUID_TYPE_MAGMA;            // magma/slime
                }

                if (!count && liquid_flags[mcnk->iy][mcnk->ix])
                {
                    fprintf(stderr, "Wrong liquid detect in MCLQ chunk");
                }

                for (int y = 0; y <= ADT_CELL_SIZE; ++y)
                {
                    int cy = mcnk->iy * ADT_CELL_SIZE + y;
                    for (int x = 0; x <= ADT_CELL_SIZE; ++x)
                    {
                        int cx = mcnk->ix * ADT_CELL_SIZE + x;
                        liquid_height[cy][cx] = liquid->liquid[y][x].height;
                    }
                }
            }
        }

        // Hole data
        if (!(mcnk->flags & 0x10000))
        {
            if (uint16 hole = mcnk->holes)
            {
                holes[mcnk->iy][mcnk->ix] = mcnk->holes;
                hasHoles = true;
            }
        }
    }

    // Get liquid map for grid (in WOTLK used MH2O chunk)
    if (FileChunk* chunk = adt.GetChunk("MH2O"))
    {
        adt_MH2O* h2o = chunk->As<adt_MH2O>();
        for (int i = 0; i < ADT_CELLS_PER_GRID; i++)
        {
            for (int j = 0; j < ADT_CELLS_PER_GRID; j++)
            {
                adt_liquid_header *h = h2o->getLiquidData(i, j);
                if (!h)
                {
                    continue;
                }

                int count = 0;
                uint64 show = h2o->getLiquidShowMap(h);
                for (int y = 0; y < h->height; y++)
                {
                    int cy = i * ADT_CELL_SIZE + y + h->yOffset;
                    for (int x = 0; x < h->width; x++)
                    {
                        int cx = j * ADT_CELL_SIZE + x + h->xOffset;
                        if (show & 1)
                        {
                            liquid_show[cy][cx] = true;
                            ++count;
                        }
                        show >>= 1;
                    }
                }

                liquid_entry[i][j] = h->liquidType;
                switch (pLiqType[h->liquidType])
                {
                    case LIQUID_TYPE_WATER: liquid_flags[i][j] |= MAP_LIQUID_TYPE_WATER; break;
                    case LIQUID_TYPE_OCEAN: liquid_flags[i][j] |= MAP_LIQUID_TYPE_OCEAN; break;
                    case LIQUID_TYPE_MAGMA: liquid_flags[i][j] |= MAP_LIQUID_TYPE_MAGMA; break;
                    case LIQUID_TYPE_SLIME: liquid_flags[i][j] |= MAP_LIQUID_TYPE_SLIME; break;
                    default:
                        printf("\nCan't find Liquid type %u for map %s\nchunk %d,%d\n", h->liquidType, filename, i, j);
                        break;
                }
                // Dark water detect
                if (pLiqType[h->liquidType] == LIQUID_TYPE_OCEAN)
                {
                    uint8* lm = h2o->getLiquidLightMap(h);
                    if (!lm)
                    {
                        liquid_flags[i][j] |= MAP_LIQUID_TYPE_DARK_WATER;
                    }
                }

                if (!count && liquid_flags[i][j])
                {
                    printf("Wrong liquid detect in MH2O chunk");
                }

                float* height = h2o->getLiquidHeightMap(h);
                int pos = 0;
                for (int y = 0; y <= h->height; y++)
                {
                    int cy = i * ADT_CELL_SIZE + y + h->yOffset;
                    for (int x = 0; x <= h->width; x++)
                    {
                        int cx = j * ADT_CELL_SIZE + x + h->xOffset;

                        if (height)
                        {
                            liquid_height[cy][cx] = height[pos];
                        }
                        else
                        {
                            liquid_height[cy][cx] = h->heightLevel1;
                        }

                        pos++;
                    }
                }
            }
        }
    }

    //============================================
    // Try pack area data
    //============================================
    bool fullAreaData = false;
    uint32 areaflag = area_flags[0][0];
    for (int y = 0; y < ADT_CELLS_PER_GRID; y++)
    {
        for (int x = 0; x < ADT_CELLS_PER_GRID; x++)
        {
            if (area_flags[y][x] != areaflag)
            {
                fullAreaData = true;
                break;
            }
        }
    }

    map.areaMapOffset = sizeof(map);
    map.areaMapSize   = sizeof(map_areaHeader);

    map_areaHeader areaHeader;
    areaHeader.fourcc = *(uint32 const*)MAP_AREA_MAGIC;
    areaHeader.flags = 0;
    if (fullAreaData)
    {
        areaHeader.gridArea = 0;
        map.areaMapSize += sizeof(area_flags);
    }
    else
    {
        areaHeader.flags |= MAP_AREA_NO_AREA;
        areaHeader.gridArea = (uint16)areaflag;
    }

    //============================================
    // Try pack height data
    //============================================
    float maxHeight = -20000;
    float minHeight =  20000;
    for (int y = 0; y < ADT_GRID_SIZE; y++)
    {
        for (int x = 0; x < ADT_GRID_SIZE; x++)
        {
            float h = V8[y][x];
            if (maxHeight < h)
            {
                maxHeight = h;
            }
            if (minHeight > h)
            {
                minHeight = h;
            }
        }
    }
    for (int y = 0; y <= ADT_GRID_SIZE; y++)
    {
        for (int x = 0; x <= ADT_GRID_SIZE; x++)
        {
            float h = V9[y][x];
            if (maxHeight < h)
            {
                maxHeight = h;
            }
            if (minHeight > h)
            {
                minHeight = h;
            }
        }
    }

    // Check for allow limit minimum height (not store height in deep ochean - allow save some memory)
    if (CONF_allow_height_limit && minHeight < CONF_use_minHeight)
    {
        for (int y = 0; y < ADT_GRID_SIZE; y++)
            for (int x = 0; x < ADT_GRID_SIZE; x++)
                if (V8[y][x] < CONF_use_minHeight)
                {
                    V8[y][x] = CONF_use_minHeight;
                }
        for (int y = 0; y <= ADT_GRID_SIZE; y++)
            for (int x = 0; x <= ADT_GRID_SIZE; x++)
                if (V9[y][x] < CONF_use_minHeight)
                {
                    V9[y][x] = CONF_use_minHeight;
                }
        if (minHeight < CONF_use_minHeight)
        {
            minHeight = CONF_use_minHeight;
        }
        if (maxHeight < CONF_use_minHeight)
        {
            maxHeight = CONF_use_minHeight;
        }
    }

    map.heightMapOffset = map.areaMapOffset + map.areaMapSize;
    map.heightMapSize = sizeof(map_heightHeader);

    map_heightHeader heightHeader;
    heightHeader.fourcc = *(uint32 const*)MAP_HEIGHT_MAGIC;
    heightHeader.flags = 0;
    heightHeader.gridHeight    = minHeight;
    heightHeader.gridMaxHeight = maxHeight;

    if (maxHeight == minHeight)
    {
        heightHeader.flags |= MAP_HEIGHT_NO_HEIGHT;
    }

    // Not need store if flat surface
    if (CONF_allow_float_to_int && (maxHeight - minHeight) < CONF_flat_height_delta_limit)
    {
        heightHeader.flags |= MAP_HEIGHT_NO_HEIGHT;
    }

    // Try store as packed in uint16 or uint8 values
    if (!(heightHeader.flags & MAP_HEIGHT_NO_HEIGHT))
    {
        float step = 0;
        // Try Store as uint values
        if (CONF_allow_float_to_int)
        {
            float diff = maxHeight - minHeight;
            if (diff < CONF_float_to_int8_limit)      // As uint8 (max accuracy = CONF_float_to_int8_limit/256)
            {
                heightHeader.flags |= MAP_HEIGHT_AS_INT8;
                step = selectUInt8StepStore(diff);
            }
            else if (diff < CONF_float_to_int16_limit) // As uint16 (max accuracy = CONF_float_to_int16_limit/65536)
            {
                heightHeader.flags |= MAP_HEIGHT_AS_INT16;
                step = selectUInt16StepStore(diff);
            }
        }

        // Pack it to int values if need
        if (heightHeader.flags & MAP_HEIGHT_AS_INT8)
        {
            for (int y = 0; y < ADT_GRID_SIZE; y++)
                for (int x = 0; x < ADT_GRID_SIZE; x++)
                {
                    uint8_V8[y][x] = uint8((V8[y][x] - minHeight) * step + 0.5f);
                }
            for (int y = 0; y <= ADT_GRID_SIZE; y++)
                for (int x = 0; x <= ADT_GRID_SIZE; x++)
                {
                    uint8_V9[y][x] = uint8((V9[y][x] - minHeight) * step + 0.5f);
                }
            map.heightMapSize += sizeof(uint8_V9) + sizeof(uint8_V8);
        }
        else if (heightHeader.flags & MAP_HEIGHT_AS_INT16)
        {
            for (int y = 0; y < ADT_GRID_SIZE; y++)
                for (int x = 0; x < ADT_GRID_SIZE; x++)
                {
                    uint16_V8[y][x] = uint16((V8[y][x] - minHeight) * step + 0.5f);
                }
            for (int y = 0; y <= ADT_GRID_SIZE; y++)
                for (int x = 0; x <= ADT_GRID_SIZE; x++)
                {
                    uint16_V9[y][x] = uint16((V9[y][x] - minHeight) * step + 0.5f);
                }
            map.heightMapSize += sizeof(uint16_V9) + sizeof(uint16_V8);
        }
        else
        {
            map.heightMapSize += sizeof(V9) + sizeof(V8);
        }
    }

    //============================================
    // Pack liquid data
    //============================================
    uint8 type = liquid_flags[0][0];
    bool fullType = false;
    for (int y = 0; y < ADT_CELLS_PER_GRID; y++)
    {
        for (int x = 0; x < ADT_CELLS_PER_GRID; x++)
        {
            if (liquid_flags[y][x] != type)
            {
                fullType = true;
                y = ADT_CELLS_PER_GRID;
                break;
            }
        }
    }

    map_liquidHeader liquidHeader;

    // no water data (if all grid have 0 liquid type)
    if (type == 0 && !fullType)
    {
        // No liquid data
        map.liquidMapOffset = 0;
        map.liquidMapSize   = 0;
    }
    else
    {
        int minX = 255, minY = 255;
        int maxX = 0, maxY = 0;
        maxHeight = -20000;
        minHeight = 20000;
        for (int y = 0; y < ADT_GRID_SIZE; y++)
        {
            for (int x = 0; x < ADT_GRID_SIZE; x++)
            {
                if (liquid_show[y][x])
                {
                    if (minX > x)
                    {
                        minX = x;
                    }
                    if (maxX < x)
                    {
                        maxX = x;
                    }
                    if (minY > y)
                    {
                        minY = y;
                    }
                    if (maxY < y)
                    {
                        maxY = y;
                    }
                    float h = liquid_height[y][x];
                    if (maxHeight < h)
                    {
                        maxHeight = h;
                    }
                    if (minHeight > h)
                    {
                        minHeight = h;
                    }
                }
                else
                {
                    liquid_height[y][x] = CONF_use_minHeight;
                }
            }
        }
        map.liquidMapOffset = map.heightMapOffset + map.heightMapSize;
        map.liquidMapSize = sizeof(map_liquidHeader);
        liquidHeader.fourcc = *(uint32 const*)MAP_LIQUID_MAGIC;
        liquidHeader.flags = 0;
        liquidHeader.liquidType = 0;
        liquidHeader.offsetX = minX;
        liquidHeader.offsetY = minY;
        liquidHeader.width   = maxX - minX + 1 + 1;
        liquidHeader.height  = maxY - minY + 1 + 1;
        liquidHeader.liquidLevel = minHeight;

        if (maxHeight == minHeight)
        {
            liquidHeader.flags |= MAP_LIQUID_NO_HEIGHT;
        }

        // Not need store if flat surface
        if (CONF_allow_float_to_int && (maxHeight - minHeight) < CONF_flat_liquid_delta_limit)
        {
            liquidHeader.flags |= MAP_LIQUID_NO_HEIGHT;
        }

        if (!fullType)
        {
            liquidHeader.flags |= MAP_LIQUID_NO_TYPE;
        }

        if (liquidHeader.flags & MAP_LIQUID_NO_TYPE)
        {
            liquidHeader.liquidType = type;
        }
        else
        {
            map.liquidMapSize += sizeof(liquid_entry) + sizeof(liquid_flags);
        }

        if (!(liquidHeader.flags & MAP_LIQUID_NO_HEIGHT))
        {
            map.liquidMapSize += sizeof(float) * liquidHeader.width * liquidHeader.height;
        }
    }

    if (map.liquidMapOffset)
    {
        map.holesOffset = map.liquidMapOffset + map.liquidMapSize;
    }
    else
    {
        map.holesOffset = map.heightMapOffset + map.heightMapSize;
    }

    if (hasHoles)
    {
        map.holesSize = sizeof(holes);
    }
    else
    {
        map.holesSize = 0;
    }

    // Ok all data prepared - store it
    FILE* output = fopen(filename2, "wb");
    if (!output)
    {
        printf("Can not create the output file '%s'\n", filename2);
        return false;
    }
    fwrite(&map, sizeof(map), 1, output);
    // Store area data
    fwrite(&areaHeader, sizeof(areaHeader), 1, output);
    if (!(areaHeader.flags & MAP_AREA_NO_AREA))
    {
        fwrite(area_flags, sizeof(area_flags), 1, output);
    }

    // Store height data
    fwrite(&heightHeader, sizeof(heightHeader), 1, output);
    if (!(heightHeader.flags & MAP_HEIGHT_NO_HEIGHT))
    {
        if (heightHeader.flags & MAP_HEIGHT_AS_INT16)
        {
            fwrite(uint16_V9, sizeof(uint16_V9), 1, output);
            fwrite(uint16_V8, sizeof(uint16_V8), 1, output);
        }
        else if (heightHeader.flags & MAP_HEIGHT_AS_INT8)
        {
            fwrite(uint8_V9, sizeof(uint8_V9), 1, output);
            fwrite(uint8_V8, sizeof(uint8_V8), 1, output);
        }
        else
        {
            fwrite(V9, sizeof(V9), 1, output);
            fwrite(V8, sizeof(V8), 1, output);
        }
    }

    // Store liquid data if need
    if (map.liquidMapOffset)
    {
        fwrite(&liquidHeader, sizeof(liquidHeader), 1, output);
        if (!(liquidHeader.flags & MAP_LIQUID_NO_TYPE))
        {
            fwrite(liquid_entry, sizeof(liquid_entry), 1, output);
            fwrite(liquid_flags, sizeof(liquid_flags), 1, output);
        }

        if (!(liquidHeader.flags & MAP_LIQUID_NO_HEIGHT))
        {
            for (int y = 0; y < liquidHeader.height; y++)
            {
                fwrite(&liquid_height[y + liquidHeader.offsetY][liquidHeader.offsetX], sizeof(float), liquidHeader.width, output);
            }
        }
    }

    // store hole data
    if (hasHoles)
    {
        fwrite(holes, map.holesSize, 1, output);
    }

    fclose(output);

    return true;
}

void ExtractMapsFromMpq(HANDLE hWorldMPQ, HANDLE localeMpqHandle, uint32 build, uint32 locale)
{
    char mpq_filename[1024];
    char output_filename[1024];
    char mpq_map_name[1024];

    setvbuf (stdout, NULL, _IONBF, 0);
    printf("\n Extracting maps...\n");

    uint32 map_count = ReadMapDBC(localeMpqHandle, locale);

    ReadAreaTableDBC(localeMpqHandle, locale);
    ReadLiquidTypeTableDBC(locale);

    std::string path = output_path;
    path += "/maps/";
    CreateDir(path);
    uint32 bFailedToLoad = 0;

        setvbuf (stdout, NULL, _IONBF, 0);
    printf("  Converting map files\n\n");
    for (uint32 z = 0; z < map_count; ++z)
    {
        setvbuf (stdout, NULL, _IONBF, 0);
        printf("\n Extract MapId: %04u Name: %s (%d/%u)  \n", pMap_ids[z].id, pMap_ids[z].name, z+1, map_count);
        // Loadup map grid data
        sprintf(mpq_map_name, "World\\Maps\\%s\\%s.wdt", pMap_ids[z].name, pMap_ids[z].name);
        ChunkedFile wdt;
        if (!wdt.loadFile(hWorldMPQ, mpq_map_name, true))
        {
            bFailedToLoad++;
            continue;
        }

        FileChunk* chunk = wdt.GetChunk("MAIN");
        for (uint32 y = 0; y < WDT_MAP_SIZE; ++y)
        {
            for (uint32 x = 0; x < WDT_MAP_SIZE; ++x)
            {
                if (!(chunk->As<wdt_MAIN>()->adt_list[y][x].exist & 0x1))
                {
                    continue;
                }
                sprintf(mpq_filename, "World\\Maps\\%s\\%s_%u_%u.adt", pMap_ids[z].name, pMap_ids[z].name, x, y);
                sprintf(output_filename, "%s/maps/%04u%02u%02u.map", output_path, pMap_ids[z].id, y, x);
                ConvertADT(hWorldMPQ, mpq_filename, output_filename, y, x, build);
            }

            // draw progress bar
        setvbuf (stdout, NULL, _IONBF, 0);
            printf(" Processing........................%d%%\r", (100 * (y + 1)) / WDT_MAP_SIZE);
        }
    }

        setvbuf (stdout, NULL, _IONBF, 0);
    printf("Failed to load: %u\n", bFailedToLoad);

    printf("\n");
    delete [] pAreas;
    delete [] pMap_ids;
}

bool ExtractFile(HANDLE fileInArchive, char const* filename)
{
    FILE* output = fopen(filename, "wb");
    if(!output)
    {
        printf("Can't create the output file '%s'\n", filename);
        return false;
    }

    char  buffer[0x10000];
    DWORD readBytes = 1;

    while (readBytes > 0)
    {
        SFileReadFile(fileInArchive, buffer, sizeof(buffer), &readBytes, NULL);
        if (readBytes > 0)
        {
            fwrite(buffer, 1, readBytes, output);
        }
    }

    fclose(output);
    return true;
}

void ExtractDBCFiles(HANDLE localeMpqHandle, int locale, bool basicLocale)
{
    printf(" ___________________________________    \n");
    printf("\n Extracting client database files...\n");

    std::set<std::string> dbcfiles;
    HANDLE dbcFile = NULL;

    // get DBC file list
    ArchiveSetBounds archives = GetArchivesBounds();
    for (ArchiveSet::const_iterator i = archives.first; i != archives.second; ++i)
    {
        AppendDBCFileListTo(*i, dbcfiles);
        AppendDB2FileListTo(*i, dbcfiles);
    }

    std::string path = output_path;
    path += "/dbc/";
    CreateDir(path);
    if (!basicLocale)
    {
        path += Locales[locale];
        path += "/";
        CreateDir(path);
    }

    // extract Build info file
    {
        std::string mpq_name = std::string("component.wow-") + Locales[locale] + ".txt";
        std::string filename = path + mpq_name;

        ExtractFile(mpq_name.c_str(), filename);
    }

    // extract DBCs
    int count = 0;
    // loop through all of the known dbc and db2 files
    for (std::set<std::string>::iterator iter = dbcfiles.begin(); iter != dbcfiles.end(); ++iter)
    {
        std::string filename = path;
        filename += (iter->c_str() + strlen("DBFilesClient\\"));

        if (!SFileOpenFileEx(localeMpqHandle, iter->c_str(), SFILE_OPEN_FROM_MPQ, &dbcFile))
        {
            printf("Unable to open file %s in the archive\n", filename.c_str());
            continue;
        }

        if (ExtractFile(dbcFile, filename.c_str()))
        {
            ++count;
        }

        if (!FileExists(filename.c_str()))
        {
            printf("\nFailed to extract file %u : %s\n", count, filename.c_str());
        }
    }

    printf("\n\n Extracted %u DBC/DB2 files\n\n", count);
}

void AppendPatchMPQFilesToList(char const* subdir, char const* suffix, char const* section, Updates& updates)
{
    char dirname[512];
    if (subdir)
    {
        sprintf(dirname, "%s/Data/%s", input_path, subdir);
    }
    else
    {
        sprintf(dirname, "%s/Data", input_path);
    }

    char scanname[512];
    if (suffix)
    {
        sprintf(scanname, "wow-update-%s-%%u.MPQ", suffix);
    }
    else
    {
        sprintf(scanname, "wow-update-%%u.MPQ");
    }

#ifdef WIN32

    char maskname[512];
    if (suffix)
    {
        sprintf(maskname, "%s/wow-update-%s-*.MPQ", dirname, suffix);
    }
    else
    {
        sprintf(maskname, "%s/wow-update-*.MPQ", dirname);
    }

    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(maskname, &ffd);

    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                continue;
            }

            uint32 ubuild = 0;
            if (sscanf(ffd.cFileName, scanname, &ubuild) == 1 && (!CONF_max_build || ubuild <= CONF_max_build))
            {
                updates[ubuild] = UpdatesPair(ffd.cFileName, section);
            }
        }
        while (FindNextFile(hFind, &ffd) != 0);

        FindClose(hFind);
    }

#else

    if (DIR* dp  = opendir(dirname))
    {
        int ubuild = 0;
        dirent* dirp;
        while ((dirp = readdir(dp)) != NULL)
            if (sscanf(dirp->d_name, scanname, &ubuild) == 1 && (!CONF_max_build || ubuild <= CONF_max_build))
            {
                updates[ubuild] = UpdatesPair(dirp->d_name, section);
            }

        closedir(dp);
    }

#endif
}

void LoadLocaleMPQFiles(HANDLE &localeMpqHandle, int const locale)
{
    printf("\n LoadLocaleMPQFiles \n");

    char filename[512];
    char const* prefix = "";

    sprintf(filename,"%s/Data/misc.MPQ", input_path);//, langs[locale], langs[locale]);
    if (!OpenArchive(filename, &localeMpqHandle))
    {
        printf(" Error open archive: %s\n\n", filename);
        return;
    }

    sprintf(filename, "%s/Data/%s/locale-%s.MPQ", input_path, Locales[locale], Locales[locale]);
    if (!SFileOpenPatchArchive(localeMpqHandle, filename, prefix, 0))
    {
        printf("Failed to patch locale to misc\n");
    }

    // prepare sorted list patches in locale dir and Data root
    Updates updates;
    // now update to newer view, locale
    AppendPatchMPQFilesToList(Locales[locale], Locales[locale], NULL, updates);
    // now update to newer view, root
    AppendPatchMPQFilesToList(NULL, NULL, Locales[locale], updates);
    int updateCount = 0;

    // ./Data update-base files
    for (int i = 0; Builds[i] <= CONF_TargetBuild; ++i)
    {
        if (Builds[i] == 0 || Builds[i] > CONF_TargetBuild) // should not need to do this, but it's being naughty!
        {
            continue;
        }
        sprintf(filename, "%s/Data/wow-update-base-%u.MPQ", input_path, Builds[i]);

        printf("\nPatching : %s\n", filename);

        //if (!OpenArchive(filename))
        if (!SFileOpenPatchArchive(localeMpqHandle, filename, prefix, 0))
        {
            printf("Error open patch archive: %s\n\n", filename);
        }
    }

    for (Updates::const_iterator itr = updates.begin(); itr != updates.end(); ++itr)
    {
        if (!itr->second.second)
        {
            sprintf(filename, "%s/Data/%s/%s", input_path, Locales[locale], itr->second.first.c_str());
        }
        else
        {
            sprintf(filename, "%s/Data/%s", input_path, itr->second.first.c_str());
        }

        printf("\nPatching : %s\n", filename);

        //if (!OpenArchive(filename))
        if (!SFileOpenPatchArchive(localeMpqHandle, filename, itr->second.second ? itr->second.second : "", 0))
        {
            printf("Error open patch archive: %s\n\n", filename);
        }
    }

    // ./Data/Cache patch-base files
    for (int i = 0; Builds[i] <= CONF_TargetBuild; ++i)
    {
        if (Builds[i] == 0 || Builds[i] > CONF_TargetBuild) // should not need to do this, but it's being naughty!
        {
            continue;
        }
        sprintf(filename, "%s/Data/Cache/patch-base-%u.MPQ", input_path, Builds[i]);

        printf("\nPatching : %s\n", filename);

        //if (!OpenArchive(filename))
        if (!SFileOpenPatchArchive(localeMpqHandle, filename, prefix, 0))
        {
            printf("Error open patch archive: %s\n\n", filename);
        }
    }

    // ./Data/Cache/<locale> patch files
    for (int i = 0; Builds[i] <= CONF_TargetBuild; ++i)
    {
        if (Builds[i] == 0 || Builds[i] > CONF_TargetBuild) // should not need to do this, but it's being naughty!
        {
            continue;
        }
        sprintf(filename, "%s/Data/Cache/%s/patch-%s-%u.MPQ", input_path, Locales[locale], Locales[locale], Builds[i]);

        printf("\nPatching : %s\n", filename);

        //if (!OpenArchive(filename))
        if (!SFileOpenPatchArchive(localeMpqHandle, filename, prefix, 0))
        {
            printf("Error open patch archive: %s\n\n", filename);
        }
    }
}

void LoadBaseMPQFiles(HANDLE &hWorldMPQ)
{
    char filename[512];
    setvbuf (stdout, NULL, _IONBF, 0);
    printf("Loading MPQ files for map extraction:\n");
    for (int i = 1; i <= WORLD_COUNT; i++)
    {
        sprintf(filename, "%s/Data/world%s.MPQ", input_path, (i == 2 ? "2" : ""));
        printf("%s\n", filename);

        if (!OpenArchive(filename, &hWorldMPQ))
        {
            printf("Error open archive: %s\n\n", filename);
            return;
        }
    }

    // misc.MPQ
    sprintf(filename,"%s/Data/misc.MPQ", input_path);//, langs[locale], langs[locale]);
    if (!SFileOpenPatchArchive(hWorldMPQ, filename, "", 0))
    {
        printf("Fatal error: cannot open archive: %s\n\n", filename);
        printf("Exiting program!\n");
        exit(0);
    }
    setvbuf (stdout, NULL, _IONBF, 0);
    printf("\nWorld loaded\n");

    // expansion MPQ files
    for (int i = 1; i <= EXPANSION_COUNT; i++)
    {
        sprintf(filename, "%s/Data/expansion%i.MPQ", input_path, i);
        setvbuf (stdout, NULL, _IONBF, 0);
        printf("%s\n", filename);

        if (!SFileOpenPatchArchive(hWorldMPQ, filename, "", 0))
        {
            printf("Fatal error: cannot open archive: %s\n\n", filename);
            printf("Exiting program!\n");
            exit(0);
        }
    }
    setvbuf (stdout, NULL, _IONBF, 0);
    printf("Expansions loaded\n");

    // ./Data update-base files
    for (int i = 0; Builds[i] <= CONF_TargetBuild; ++i)
    {
        if (Builds[i] == 0 || Builds[i] > CONF_TargetBuild) // should not need to do this, but it's being naughty!
        {
            continue;
        }
        sprintf(filename, "%s/Data/wow-update-base-%u.MPQ", input_path, Builds[i]);

        //if (!OpenArchive(filename))
        if (!SFileOpenPatchArchive(hWorldMPQ, filename, "", 0))
        {
            printf("Error open patch archive: %s\n\n", filename);
        }
    }

    printf("update-base files loaded\n");

    // ./Data/Cache patch-base files
    for (int i = 0; Builds[i] <= CONF_TargetBuild; ++i)
    {
        if (Builds[i] == 0 || Builds[i] > CONF_TargetBuild) // should not need to do this, but it's being naughty!
        {
            continue;
        }
        sprintf(filename, "%s/Data/Cache/patch-base-%u.MPQ", input_path, Builds[i]);

        //if (!OpenArchive(filename))
        if (!SFileOpenPatchArchive(hWorldMPQ, filename, "", 0))
        {
            printf("Error open patch archive: %s\n\n", filename);
        }
    }
    setvbuf (stdout, NULL, _IONBF, 0);
    printf("  patch-base files loaded\n");
    setvbuf (stdout, NULL, _IONBF, 0);
    printf("  Loaded MPQ files for map extraction\n");
}

int main(int argc, char* arg[])
{
    int thisBuild = getBuildNumber();
    iCoreNumber = getCoreNumberFromBuild(thisBuild);
    showBanner("DBC Extractor & Map Generator", iCoreNumber, thisBuild);

    HandleArgs(argc, arg);

    HANDLE hLocaleMpqHandle, hWorldMPQ;
    int iFirstLocale = -1;
    uint32 uBuild = 0;


            HANDLE dbcFile;

    for (int i = 0; i < LOCALES_COUNT; i++)
    {
        char tmp1[512];
        sprintf(tmp1, "%s/Data/%s/locale-%s.MPQ", input_path, Locales[i], Locales[i]);
        if (FileExists(tmp1))
        {
            printf("Detected locale: %s\n", Locales[i]);

            //Open MPQs
            LoadLocaleMPQFiles(hLocaleMpqHandle, i);

            if (!SFileOpenFileEx(hLocaleMpqHandle, "DBFilesClient\\Map.dbc", SFILE_OPEN_FROM_MPQ, &dbcFile))
            {
                printf(" MAIN: Fatal error: Cannot find Map.dbc! Possible faulty locale archive?\n");
                exit(1);
            }

            if ((CONF_extract & EXTRACT_DBC) == 0)
            {
                iFirstLocale = i;
                uBuild = getBuildNumber();
                printf("    Detected client build: %u\n", uBuild);
                break;
            }

            //Extract DBC files
            if (iFirstLocale < 0)
            {
                iFirstLocale = i;
                uBuild = getBuildNumber();
                printf("    Detected client build: %u\n", uBuild);
                ExtractDBCFiles(hLocaleMpqHandle, i, true);
            }
            else
            {
                ExtractDBCFiles(hLocaleMpqHandle, i, false);
            }
        }
    }

    if (iFirstLocale < 0)
    {
        printf("No locales detected\n");
        return 0;
    }

    if (CONF_extract & EXTRACT_MAP)
    {
        printf("    Using locale: %s\n", Locales[iFirstLocale]);

        // Open MPQs
        LoadBaseMPQFiles(hWorldMPQ);

        // Extract maps
        ExtractMapsFromMpq(hWorldMPQ, hLocaleMpqHandle, uBuild, iFirstLocale);
        printf("\n\nFinished extracting maps !!!\n\n");

    }

    // Close MPQs
    CloseArchives();
        setvbuf (stdout, NULL, _IONBF, 0);
        printf("\n\nExtraction Completed !!!\n\n");

    return 0;
}
