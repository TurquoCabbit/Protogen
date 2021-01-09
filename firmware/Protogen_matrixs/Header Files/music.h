// note,  octave,  note_time,  if joined note
#define newmusic_test
#ifndef newmusic_test
uint8_t megalovania[][4] =
{
	{0, 4, 32, 0}, //start
	{3, 4, 16, 0}, //D, 16
	{3, 4, 16, 0}, //D, 16
	{3, 5, 8, 0},  //D, 16
	{10, 4, 8, 0}, //A, 8
	{9, 4, 16, 0}, //G#, 16
	{0, 4, 16, 0}, //stop, 8
	{8, 4, 8, 0},  //G, 8
	{6, 4, 16, 1}, //F, 16, j
	{6, 4, 16, 0}, //F, 16
	{3, 4, 16, 0}, //D, 16
	{6, 4, 16, 0}, //F, 16
	{8, 4, 16, 0}, //G, 16
};
uint16_t megalovania_length = sizeof(megalovania) / 4;
#define megalovania_tempo  120
uint16_t megalovania_note_time[6] = 
{
	(60000 / megalovania_tempo) - beep_time,      //whole
	(60000 / megalovania_tempo) - beep_time,      //half
	(60000 / megalovania_tempo) - beep_time,      //quarter
	(60000 / megalovania_tempo / 2) - beep_time,  //8th
	(60000 / megalovania_tempo / 4) - beep_time,  //16th
	(60000 / megalovania_tempo / 8) - beep_time   //32th
};

#else
void * struct_init_dummy;

_sheet megalovania_sheet[13] = 
{
	{0, 4, 32, 0}, //start
	{3, 4, 16, 0}, //D, 16
	{3, 4, 16, 0}, //D, 16
	{3, 5, 8, 0},  //D, 16
	{10, 4, 8, 0}, //A, 8
	{9, 4, 16, 0}, //G#, 16
	{0, 4, 16, 0}, //stop, 8
	{8, 4, 8, 0},  //G, 8
	{6, 4, 16, 1}, //F, 16, j
	{6, 4, 16, 0}, //F, 16
	{3, 4, 16, 0}, //D, 16
	{6, 4, 16, 0}, //F, 16
	{8, 4, 16, 0}, //G, 16	
};
_music megalovania = 
{
	megalovania_sheet,
	13,
	120,
	{
		(60000 / 120) * 4 - 100,		//whole
		(60000 / 120) * 2 - 100,		//half
		(60000 / 120) * 1 - 100,		//quarter
		(60000 / 120) / 2 - 100,  	//8th
		(60000 / 120) / 4 - 100,  	//16th
		(60000 / 120) / 8 - 100,  	//32th
	},
	100,
};

_music * music_ptr_rack[] =
{
	(_music *)(&megalovania),		//0x1
};

#endif