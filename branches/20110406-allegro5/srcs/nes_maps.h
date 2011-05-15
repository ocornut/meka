//-----------------------------------------------------------------------------
// MEKA - nes_maps.h
// Nintendo Mappers - Headers
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Data
//-----------------------------------------------------------------------------

struct t_nes_mapper
{
  int           id;
  void          (*Init)(void);
  void          (*Write)(word, byte);
  void          (*Load)(FILE *);
  void          (*Save)(FILE *);
};

extern t_nes_mapper NES_Mappers[];

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

void    NES_Mapper_Set          (int i);
void    NES_Mapper_Load         (FILE *f);
void    NES_Mapper_Save         (FILE *f);

//-----------------------------------------------------------------------------

void    NES_Mapper_0_Init       (void);
void    NES_Mapper_0_Write      (word Addr, byte Value);

//-----------------------------------------------------------------------------

void    NES_Mapper_1_Init       (void);
void    NES_Mapper_1_Write      (word Addr, byte Value);
void    NES_Mapper_1_Load       (FILE *f);
void    NES_Mapper_1_Save       (FILE *f);

//-----------------------------------------------------------------------------

void    NES_Mapper_2_Init       (void);
void    NES_Mapper_2_Write      (word Addr, byte Value);
void    NES_Mapper_2_Load       (FILE *f);
void    NES_Mapper_2_Save       (FILE *f);

//-----------------------------------------------------------------------------

void    NES_Mapper_3_Init       (void);
void    NES_Mapper_3_Write      (word Addr, byte Value);
void    NES_Mapper_3_Load       (FILE *f);
void    NES_Mapper_3_Save       (FILE *f);

//-----------------------------------------------------------------------------

