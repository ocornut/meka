//
// MEKA - VGM.H
// VGM File Creation
//

// VGM 1.01 -------------------------------------------------------------------
#define VGM_MAGIC                       "Vgm "
#define VGM_VERSION                     ((int)0x00000101)
//#define VGM_PSG_SPEED                 (CPU_CLOCK)
//#define VGM_FM_SPEED                  (CPU_CLOCK)
#define VGM_PADDING_SIZE                (24)

// VGM commands
#define VGM_CMD_GG_STEREO               (0x4F)
#define VGM_CMD_PSG                     (0x50)
#define VGM_CMD_YM2413                  (0x51)
#define VGM_CMD_WAIT                    (0x61)
#define VGM_CMD_WAIT_735                (0x62)
#define VGM_CMD_WAIT_882                (0x63)
#define VGM_CMD_EOF                     (0x66)

// VGM logging values
#define VGM_LOGGING_NO                  (0)
#define VGM_LOGGING_ACCURACY_FRAME      (1)
#define VGM_LOGGING_ACCURACY_SAMPLE     (2)

// VGM buffer (unused as of yet)
// #define VGM_BUFFER_SIZE              (131072) // 128 Kb

typedef struct
{
 char           Magic[4];
 int            EOF_Relative;
 int            Version;
 int            PSG_Speed;
 int            FM_Speed;
 int            GD3_Relative;
 int            N_Samples;
 int            Loop_Point_Relative;
 int            Loop_N_Samples;
 int            Rate;
 char           Padding[VGM_PADDING_SIZE];
}               t_vgm_header;

// GD3 1.00 -------------------------------------------------------------------
#define GD3_MAGIC                       "Gd3 "
#define GD3_VERSION                     ((int)0x00000100)

#define GD3_S_NAME_TRACK_ENG            (0)
#define GD3_S_NAME_TRACK_JAP            (1)
#define GD3_S_NAME_GAME_ENG             (2)
#define GD3_S_NAME_GAME_JAP             (3)
#define GD3_S_NAME_SYSTEM_ENG           (4)
#define GD3_S_NAME_SYSTEM_JAP           (5)
#define GD3_S_NAME_AUTHOR_ENG           (6)
#define GD3_S_NAME_AUTHOR_JAP           (7)
#define GD3_S_DATE                      (8)
#define GD3_S_FILE_AUTHOR               (9)
#define GD3_S_NOTES                     (10)
#define GD3_S_MAX                       (11)

typedef struct
{
 // Three first fields must stay at the beginning of the structure!
 char           Magic[4];
 int            Version;
 int            Data_Length;
 word          *Strings[GD3_S_MAX];
}               t_gd3;

typedef struct
{
 int            Logging;                // Currently logging / & type
 FILE *         File;                   // File
 int            DataSize;               // Size for all VGM data (not just a single buffer)
 int            FM_Used;                // FM is used ?
 t_vgm_header   Header;                 // Header
 t_gd3          GD3;                    // GD3 Header
 int            Samples_per_Frame;      //
 int            Cycles_per_Frame;       //
 int            Cycles_Counter;         //
 double         Samples_per_Cycle;      //
 double         Cycles_per_Sample;      //
}               t_vgm;

int             VGM_Start(t_vgm *VGM, char *FileName, int Logging_Accuracy);
void            VGM_Close(t_vgm *VGM);

void            VGM_Update_Timing(t_vgm *VGM);
void            VGM_NewFrame(t_vgm *VGM);

void            VGM_Data_Add_GG_Stereo(t_vgm *VGM, byte Data);
void            VGM_Data_Add_Byte(t_vgm *VGM, byte Data);
void            VGM_Data_Add_PSG(t_vgm *VGM, byte Data);
void            VGM_Data_Add_FM(t_vgm *VGM, int RegData);
void            VGM_Data_Add_Wait(t_vgm *VGM, int Samples);

void            GD3_Header_Init(t_gd3 *h);
void            GD3_Header_Close(t_gd3 *h);
int             GD3_Header_Write(t_gd3 *h, FILE *f);

