//2021/01/12 09:37:40

void * struct_init_dummy;

_sheet megalovania_sheet[13] = 
{
	{0, 4, 16, 0}, 
	{3, 4, 16, 0},
	{3, 4, 16, 0},
	{3, 5, 8, 0},
	{10, 4, 8, 0},
	{9, 4, 16, 0},
	{0, 4, 16, 0},
	{8, 4, 8, 0},
	{6, 4, 16, 1},
	{6, 4, 16, 0},
	{3, 4, 16, 0},
	{6, 4, 16, 0},
	{8, 4, 16, 0},
};
_music megalovania = 
{
	megalovania_sheet,
	120,
	13,
	{
		2000,		//whole
		1000,		//half
		500,		//quarter
		250,		//8th
		125,		//16th
		62,			//32th
	},
	100,
};

_music * music_ptr_rack[] =
{
	(_music *)(&megalovania),		//0x01
};