// note,  octave,  beat,  if joined note

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
	(60000 / megalovania_tempo) - ring_time,      //whole
	(60000 / megalovania_tempo) - ring_time,      //half
	(60000 / megalovania_tempo) - ring_time,      //quarter
	(60000 / megalovania_tempo / 2) - ring_time,  //8th
	(60000 / megalovania_tempo / 4) - ring_time,  //16th
	(60000 / megalovania_tempo / 8) - ring_time   //32th
};

/*
_sheet init_dummy_sheet;
_note init_dummy_note;


_music megalovania = 
{
	13,
	120,
	&init_dummy_sheet,
	&init_dummy_note

};
_sheet megalovania_sheet[megalovania.length];
_note megalovania_note = 
{
	(60000 / megalovania.tempo) - ring_time,      //whole
	(60000 / megalovania.tempo) - ring_time,      //half
	(60000 / megalovania.tempo) - ring_time,      //quarter
	(60000 / megalovania.tempo / 2) - ring_time,  //8th
	(60000 / megalovania.tempo / 4) - ring_time,  //16th
	(60000 / megalovania.tempo / 8) - ring_time   //32th
};
musix_struct_init(&megalovania, &megalovania_sheet, &megalovania_note);
*/