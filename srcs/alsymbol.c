
//------------------------------------
// ALLEGRO
//------------------------------------

 // MAIN & MISC
 void *screen;
 void *os_type;
 void *allegro_error;
 void install_allegro()         { }
 void override_config_data()    { }
 void set_uformat()             { }

 // VIDEO
 void set_color_depth()         { }
 void set_gfx_mode()            { }
 void scroll_screen()           { }
 void set_clip()                { }

 // MOUSE
 void *mouse_x;
 void *mouse_y;
 void *mouse_b;
 void *_mouse_type;
 void install_mouse()           { }
 void show_mouse()              { }
 void set_mouse_sprite()        { }
 void set_mouse_sprite_focus()  { }
 void set_mouse_range()         { }
 void set_mouse_speed()         { }
 void get_mouse_mickeys()       { }
 void position_mouse()          { }

 // KEYBOARD
 void *key;
 void *key_shifts;
 void install_keyboard()        { }
 void remove_keyboard()         { }
 void readkey()                 { }

 // BITMAP
 void create_bitmap()           { }
 void create_bitmap_ex()        { }
 void create_sub_bitmap()       { }
 void create_video_bitmap()     { }
 void clear_bitmap()            { }
 void show_video_bitmap()       { }
 void destroy_bitmap()          { }
 void save_bitmap()             { }
 void load_pcx()                { }

 // BLIT & VSYNC
 void blit()                    { }
 void stretch_blit()            { }
 void vsync()                   { }

 // DRAWING PRIMITIVES
 void rect()                    { }
 void circlefill()              { }

 // TEXT DRAWING
 void text_mode()               { }
 void textout()                 { }
 void text_length()             { }
 void text_height()             { }

 // PALETTE
 void set_color()               { }
 void get_palette()             { }
 void set_palette_range()       { }

 // TIMER
 void install_timer()           { }
 void install_int()             { }
 void install_int_ex()          { }
 void remove_int()              { }
 void rest()                    { }

 // FILE
 void file_exists()             { }

 // DATAFILE
 void packfile_password()       { }
 void load_datafile()           { }

 // JOYSTICK
 void *joy;
 void *num_joysticks;
 void install_joystick()        { }
 void poll_joystick()           { }
 void calibrate_joystick()      { }
 void calibrate_joystick_name() { }

//------------------------------------
// SEAL
//------------------------------------

 // MAIN
 void AInitialize()             { }
 void AOpenAudio()              { }
 void ACloseAudio()             { }
 void AUpdateAudio()            { }
 void AGetAudioDevCaps()        { }
 void AGetAudioNumDevs()        { }

 // AUDIO
 void ACreateAudioVoice()       { }
 void ADestroyAudioVoice()      { }
 void ACreateAudioData()        { }
 void ADestroyAudioData()       { }
 void AWriteAudioData()         { }
 void ASetAudioMixerValue()     { }

 // VOICES
 void AOpenVoices()             { }
 void APrimeVoice()             { }
 void APlayVoice()              { }
 void AStartVoice()             { }
 void ACloseVoices()            { }
 void AStopVoice()              { }
 void AGetVoiceStatus()         { }
 void AGetVoicePosition()       { }
 void ASetVoicePanning()        { }
 void ASetVoiceVolume()         { }
 void ASetVoiceFrequency()      { }

