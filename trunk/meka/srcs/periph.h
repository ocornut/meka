
void Peripherals_Init();
void Peripherals_MachineReset();
void Peripherals_WritePort3F(u8 old_v, u8 new_v);

void Peripherals_SportsPad_Update(int player, int device_x_rel, int device_y_rel);
void Peripherals_GraphicBoardV2_Update(int player, int x, int y, int buttons);


