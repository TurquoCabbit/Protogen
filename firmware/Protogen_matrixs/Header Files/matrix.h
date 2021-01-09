#define index_eye		0
#define index_nose		1
#define index_mouth		2

#define Matrix_Scan_time	1  //in ms

//===============Bitmap===================
uint8_t  
eye[][16] = 
{
	{0},														//off
	{0,0,60,102,195,129,129,129,195,102,60,0,0,0,0,0},			//circal
	{16,56,56,108,76,70,198,130,131,1,1,1,0,0,0,0},				//ㄑ
	{0,28,62,127,127,255,254,252,254,255,127,127,62,28,0,0}		//heart shape
},

eye_blink[7][16] = 
{
	0
},
  
nose[][8]
{
	{0},                  //off
	{0,64,48,8,8,48,64,0} // ㄑ
};
//========================================

typedef struct _face
{
	uint8_t nose[8];
	uint8_t eye[16];
	uint8_t mouth[32];
	uint8_t center;
	uint8_t animate_mode;	//Mode
	uint16_t animate_time;	//duration
	uint8_t neo[7];	//Mode, speed_H, speed_L, R, G, B, W
	uint8_t piece;
};

typedef struct _face_ptr
{
	uint8_t *nose;
	uint8_t *eye;
	uint8_t *mouth;
	uint8_t *center;
	uint8_t *animate_mode;	//Mode
	uint16_t *animate_time;	//duration
	uint8_t *neo;
	uint8_t *piece;
};

_face Face_buffer;
_face_ptr Face_index, Face_current;

Adafruit_8x16matrix matrix_eye_L = Adafruit_8x16matrix();
Adafruit_8x16matrix matrix_eye_R = Adafruit_8x16matrix();
Adafruit_8x8matrix matrix_nose_L = Adafruit_8x8matrix();
Adafruit_8x8matrix matrix_nose_R = Adafruit_8x8matrix();
Adafruit_8x16matrix matrix_mouth_L_F = Adafruit_8x16matrix();
Adafruit_8x16matrix matrix_mouth_R_F = Adafruit_8x16matrix();
Adafruit_8x16matrix matrix_mouth_L_B = Adafruit_8x16matrix();
Adafruit_8x16matrix matrix_mouth_R_B = Adafruit_8x16matrix();

void matrix_brightness(uint8_t bright)
{
	matrix_eye_L.setBrightness(bright);
	matrix_eye_R.setBrightness(bright);
	matrix_nose_L.setBrightness(bright);
	matrix_nose_R.setBrightness(bright);
	matrix_mouth_L_F.setBrightness(bright);
	matrix_mouth_R_F.setBrightness(bright);
	matrix_mouth_L_B.setBrightness(bright);
	matrix_mouth_R_B.setBrightness(bright);
}

inline void matrix_init(void)
{
	matrix_eye_L.begin(0x72);  // pass in the address
	matrix_eye_R.begin(0x73);
	matrix_nose_L.begin(0x70);
	matrix_nose_R.begin(0x71);
	matrix_mouth_L_F.begin(0x74);
	matrix_mouth_R_F.begin(0x75);
	matrix_mouth_L_B.begin(0x76);
	matrix_mouth_R_B.begin(0x77);

	Wire.setClock(400000U); //set I2C clock to 400kHZ
}

void matrix_face_set(_face * set_face)
{
	Face_index.nose = set_face->nose;
	Face_index.eye = set_face->eye;
	Face_index.mouth = set_face->mouth;
	Face_index.center = &set_face->center;
	Face_index.animate_mode = &set_face->animate_mode;
	Face_index.animate_time = &set_face->animate_time;
	Face_index.neo = set_face->neo;
	Face_index.piece = &set_face->piece;

	Face_current.eye = Face_index.eye;
	Face_current.nose = Face_index.nose;
	Face_current.mouth = Face_index.mouth;
	Face_current.center = Face_index.center;
	Face_current.animate_mode = Face_index.animate_mode;
	Face_current.animate_time = Face_index.animate_time;
	Face_current.neo = Face_index.neo;
	Face_current.piece = Face_index.piece;

	if (set_face->animate_mode)
	{
		Protogen.animate_on = 1;
		cnt.animate = 0;
	}
	else
	{
		Protogen.animate_on = 0;
		cnt.animate = 0;
	}
	Matrix_refresh();
}

void matrix_show_face(_face_ptr face_left, bool eye = 1, bool nose = 1, bool mouth = 1)
{
	uint8_t i, j;
	_face face_right =
	{
		{0},
		{0},
		{0}
	};

	if (nose)
	{
		for (i = 0; i < 8; i++)
		{
			face_right.nose[7 - i] = face_left.nose[i];
		}

		matrix_nose_L.clear();
		matrix_nose_R.clear();
		matrix_nose_L.drawBitmap(0, 0, face_left.nose, 8, 8, LED_ON);
		matrix_nose_R.drawBitmap(0, 0, face_right.nose, 8, 8, LED_ON);
	}

	if (eye)
	{
		for (i = 0; i < 16; i++)
		{
			for (j = 0; j < 8; j++)
			{
				if ((face_left.eye[i] >> j) & 0x01)
				{
					face_right.eye[i] |= 0x80 >> j;
				}
			}
		}

		matrix_eye_L.clear();
		matrix_eye_R.clear();
		matrix_eye_L.drawBitmap(0, 0, face_left.eye, 8, 16, LED_ON);
		matrix_eye_R.drawBitmap(0, 0, face_right.eye, 8, 16, LED_ON);
	}

	if (mouth)
	{
		for (i = 0; i < 32; i++)
		{
			for (j = 0; j < 8; j++)
			{
				if ((face_left.mouth[i] >> j) & 0x01)
				{
					face_right.mouth[i] |= 0x80 >> j;
				}
			}
		}

		matrix_mouth_L_F.clear();
		matrix_mouth_R_F.clear();
		matrix_mouth_L_B.clear();
		matrix_mouth_R_B.clear();
		matrix_mouth_L_F.setRotation(0);
		matrix_mouth_R_F.setRotation(0);
		matrix_mouth_L_B.setRotation(2);
		matrix_mouth_R_B.setRotation(2);
		matrix_mouth_L_F.drawBitmap(0, 0, face_left.mouth, 8, 32, LED_ON);
		matrix_mouth_R_F.drawBitmap(0, 0, face_right.mouth, 8, 32, LED_ON);
		matrix_mouth_L_B.drawBitmap(0, -16, face_left.mouth, 8, 32, LED_ON);
		matrix_mouth_R_B.drawBitmap(0, -16, face_right.mouth, 8, 32, LED_ON);
	}
	if (eye)
	{
		matrix_eye_L.writeDisplay();
		matrix_eye_R.writeDisplay();
	}

	if (nose)
	{
		matrix_nose_L.writeDisplay();
		matrix_nose_R.writeDisplay();
	}

	if (mouth)
	{
		matrix_mouth_L_F.writeDisplay();
		matrix_mouth_R_F.writeDisplay();
		matrix_mouth_L_B.writeDisplay();
		matrix_mouth_R_B.writeDisplay();
	}
}

uint8_t matrix_eye_blink(uint8_t * eye_current, uint8_t center)
{
	uint8_t i, j, range;
	uint8_t temp[16];

	for (j = 0; j < 16; j++)
	{
		temp[j] = *(eye_current + j);
	}

	if (center > 3)
	{
		range = center;
	}
	else
	{
		range = 7 - center;
	}

	for (i = 0; i < range; i++)
	{
		for (j = 0; j < 16; j++)
		{
			if (i < center)
			{
				if ((temp[j] >> i) & 0x01)
				{
					temp[j] &= ~(1 << i);
					if (!((temp[j] >> i + 1) & 0x01))
					{
						temp[j] |= 1 << (i + 1);
					}
				}
			}

			if ((center + i) < 6)
			{
				if ((temp[j] >> (7 - i)) & 0x01)
				{
					temp[j] &= ~(1 << (7 - i));
					if (!((temp[j] >> (7 - i - 1)) & 0x01))
					{
						temp[j] |= 1 << (7 - i - 1);
					}
				}
			}
			eye_blink[i][j] = temp[j];
		}
	}
	return range;
}

void clear_face_buffer(uint8_t feature)
{
	uint8_t i, j, size;
	uint8_t *temp;

	switch (feature)
	{
	case index_nose:
		temp = Face_buffer.nose;
		size = 8;
		break;
	case index_eye:
		temp = Face_buffer.eye;
		size = 16;
		break;
	case index_mouth:
		temp = Face_buffer.mouth;
		size = 32;
		break;
	}

	for (i = 0; i < size; i++)
	{
		temp[i] = 0;
	}
}

void copy_face_buffer(uint8_t * matrix, uint8_t feature)
{
	uint8_t i, j, size;
	uint8_t *temp;

	switch (feature)
	{
	case index_nose:
		temp = Face_buffer.nose;
		size = 8;
		break;
	case index_eye:
		temp = Face_buffer.eye;
		size = 16;
		break;
	case index_mouth:
		temp = Face_buffer.mouth;
		size = 32;
		break;
	}

	for (i = 0; i < size; i++)
	{
		temp[i] = matrix[i];
	}
}

void matrix_rand_dot(uint8_t * matrix, uint8_t feature)
{
	uint8_t i, j, size;
	uint8_t *temp;

	switch (feature)
	{
	case index_nose:
		temp = Face_buffer.nose;
		size = 8;
		break;
	case index_eye:
		temp = Face_buffer.eye;
		size = 16;
		break;
	case index_mouth:
		temp = Face_buffer.mouth;
		size = 32;
		break;
	}

	for (i = 0; i < size; i++)
	{
		for (j = 0; j < 8; j++)
		{
			if (((matrix[i] >> j) & 0x01))
			{
				if ((temp[i] >> j) & 0x01)
				{
					temp[i] |= (1 << j);
				}
				else if ((rand() % 1000) < rand_dot_Probability)
				{
					temp[i] |= (1 << j);
				}
			}
		}
	}
}

void matrix_vibrate(uint8_t * matrix, uint8_t feature)
{
	uint8_t i, j, size, dist, dir;
	int R = rand();
	uint8_t *temp;

	switch (feature)
	{
	case index_nose:
		temp = Face_buffer.nose;
		size = 8;
		break;
	case index_eye:
		temp = Face_buffer.eye;
		size = 16;
		break;
	case index_mouth:
		temp = Face_buffer.mouth;
		size = 32;
		break;
	}
	dist = R % 3;
	dir = R % 4;

	switch (dir)
	{
	case 0:
		for (i = 0; i < size; i++)
		{
			temp[i] = (matrix[i] << dist) & 0xFF;
		}
		break;
	case 1:
		for (i = 0; i < size; i++)
		{
			temp[i] = (matrix[i] >> dist) & 0xFF;
		}
		break;
	case 2:	
		for (i = 0; i < size - dist; i++)
		{
			temp[i] = matrix[i + dist];
		}
		for (i = 0; i < dist; i++)
		{
			temp[size - dist + i] = 0;
		}
		break;
	case 3:
		for (i = size - 1; i > dist; i--)
		{
			temp[i] = matrix[i - dist];
		}
		if (dist)
		{
			for (i = dist - 1; i > 0; i--)
			{
				temp[size - dist + i] = 0;
			}
			temp[0] = 0;
		}
		break;
	}
}
