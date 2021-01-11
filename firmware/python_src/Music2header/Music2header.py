##Face xlsx to header file convertor
import os
from datetime import datetime
from openpyxl import load_workbook


pitch_array = ['0', 'C', 'C#', 'D', 'Eb', 'E', 'F', 'F#', 'G', 'G#', 'A', 'Bb', 'B']
def pitch_conv(input):
    i = 0
    for i in range(len(pitch_array)):
        if input == pitch_array[i]:
            return i
    return 0

file_music = open('../output/music.h', 'w')

time = datetime.now().strftime("%Y/%m/%d %H:%M:%S")
file_music.write('//{}\n\n'.format(time))
wb = load_workbook(filename = '../Configure/Music_Sheet.xlsx', data_only = True, read_only = True)
work_sheet_num = len(wb.worksheets)

file_music.write('void * struct_init_dummy;\n\n')
for ws in range(2 ,work_sheet_num):
    work_sheet = wb[wb.worksheets[ws].title]
    music_name = work_sheet['G1'].value
    tempo = work_sheet['I2'].value
    ring_time = work_sheet['I3'].value
    length = work_sheet['I4'].value

    file_music.write('_sheet {}_sheet[{}] = \n'.format(music_name, length + 1))
    file_music.write('{\n\t')
    file_music.write('{00, 04, 16, 00}, \n')
    
    for i in range(2, length + 2):
        file_music.write('\t{')
        file_music.write('{:0>2d}, '.format(pitch_conv(work_sheet['B{}'.format(i)].value)))
        file_music.write('{:0>2d}, '.format(work_sheet['C{}'.format(i)].value))
        file_music.write('{:0>2d}, '.format(work_sheet['D{}'.format(i)].value))
        file_music.write('{:0>2d}'.format(work_sheet['E{}'.format(i)].value))
        file_music.write('},\n')
    file_music.write('};\n')