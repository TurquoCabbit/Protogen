//2020/12/15 14:28:03		00_Spoon

_face Face_general = {{0,24,62,112,96,96,64,0}, {56,124,126,127,63,31,31,31,30,30,60,60,120,112,64,128}, {48,48,56,24,24,112,192,128,96,60,14,14,28,28,56,56,56,56,24,24,28,28,28,12,14,30,62,47,39,51,31,6}, 4, 0, 0, {2, 3, 232, 0, 255, 100, 0}};
_face Face_chill = {{0,0,24,48,32,16,0,0}, {24,60,126,254,254,254,254,126,62,62,30,30,14,6,2,2}, {16,16,32,32,32,32,64,64,128,224,16,8,16,32,32,32,32,32,32,32,16,16,16,16,8,8,8,4,4,0,0,0}, 1, 0, 0, {12, 3, 232, 0, 255, 100, 0}};
_face Face_Full = {{255,255,255,255,255,255,255,255}, {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255}, {255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255}, 4, 0, 0, {0, 3, 232, 10, 100, 50, 20}};

_face * face_ptr_rack[] =
{
	(_face *)(&Face_general),		//0x0
	(_face *)(&Face_chill),		//0x1
	(_face *)(&Face_Full),		//0x2

};