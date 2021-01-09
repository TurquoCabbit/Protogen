#define music_duty 511	//10bit PWM

/*
void BZ_pwm(int freq, uint16_t duty)
{
	duty = duty * 1023 / 100;
	ledcSetup(BZ_channel, freq, 10);
	ledcWrite(BZ_channel, duty);
}*/

uint8_t rand_beep(void)
{
	uint8_t note, oct, oct_range, oct_offest;
	oct_range = Protogen.Beep_pitch >> 4;
	if (oct_range > 0 && oct_range < 8)
	{
		oct_offest = oct_range - 1;
		oct_range = 3;
	}
	else if (oct_range == 0)
	{
		oct_offest = 0;
		oct_range = 2;
	}
	else if (oct_range == 8)
	{
		oct_offest = 7;
		oct_range = 2;
	}
	note = (rand() % 12) + 1;	//1~12
	oct = (rand() % oct_range) + oct_offest;
	return (oct << 4) + (note & 0x0F);
}

void BZ_music(uint8_t chan, uint8_t note, uint8_t octave = 4)
{
	const uint16_t noteFrequencyBase[13] = {
		//   0        1        2        3        4        5        6       7         8       9         10      11        12
		//            C        C#       D        Eb       E        F       F#        G       G#        A       Bb        B
			 0,  	 4186,    4435,    4699,    4978,    5274,    5588,    5920,    6272,    6645,    7040,    7459,    7902
	};
	if (note == 0)
	{
		ledcWrite(BZ_channel, 0);
	}
	else
	{
		ledcSetup(BZ_channel, noteFrequencyBase[note] / (1 << (8 - octave)), 10);
		ledcWrite(BZ_channel, music_duty);
	}
}
