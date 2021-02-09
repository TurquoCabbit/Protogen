##Face xlsx to header file convertor
import os
from datetime import datetime
from openpyxl import load_workbook
from shutil import copyfile

loading_bar = ["\\", "|", "/", "—"]

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
try:
    wb = load_workbook(filename = '../Configure/Music_Sheet.xlsx', data_only = True, read_only = True)
except:
    print('Missing Music_Sheet.xlsx')
    os.system('pause')
    os._exit(0)
work_sheet_num = len(wb.worksheets)

file_music.write('void * struct_init_dummy;\n\n')
for ws in range(2 ,work_sheet_num):
    work_sheet = wb[wb.worksheets[ws].title]
    if work_sheet['K12'].value == 'OFF':
        continue
    music_name = work_sheet['J1'].value
    tempo = work_sheet['L2'].value
    ring_time = work_sheet['L3'].value
    length = work_sheet['L4'].value

    file_music.write('_sheet {}_sheet[{}] = \n'.format(music_name, length + 1))
    file_music.write('{\n\t')
    file_music.write('{0, 4, 16, 0}, \n')
    
    for i in range(2, length + 2):
        print('\r{}'.format(loading_bar[i % 4]), end = '')
        file_music.write('\t{')
        file_music.write('{}, '.format(pitch_conv(work_sheet['B{}'.format(i)].value)))
        file_music.write('{}, '.format(work_sheet['C{}'.format(i)].value))
        file_music.write('{}, '.format(work_sheet['D{}'.format(i)].value))
        file_music.write('{}'.format(work_sheet['E{}'.format(i)].value))
        file_music.write('},\n')
    file_music.write('};\n')
    file_music.write('_music {}'.format(music_name))
    file_music.write(' = \n{\n')
    file_music.write('\t{}_sheet,\n'.format(music_name))
    file_music.write('\t{},\n'.format(tempo))
    file_music.write('\t{},\n'.format(length + 1))
    file_music.write('\t{\n')
    file_music.write('\t\t{},\t\t//whole\n'.format(240000 // tempo))
    file_music.write('\t\t{},\t\t//half\n'.format(120000 // tempo))
    file_music.write('\t\t{},\t\t//quarter\n'.format(60000 // tempo))
    file_music.write('\t\t{},\t\t//8th\n'.format(30000 // tempo))
    file_music.write('\t\t{},\t\t//16th\n'.format(15000 // tempo))
    file_music.write('\t\t{},\t\t\t//32th\n'.format(7500 // tempo))
    file_music.write('\t},\n')
    file_music.write('\t{},\n'.format(ring_time))
    file_music.write('};\n\n')

file_music.write('_music * music_ptr_rack[] =\n{\n')
ser = 0
for ws in range(2 ,work_sheet_num):
    work_sheet = wb[wb.worksheets[ws].title]
    if work_sheet['K12'].value == 'OFF':
        continue
    music_name = work_sheet['J1'].value
    ser += 1
    file_music.write('\t(_music *)(&{}),\t\t//0x{:0>2d}\n'.format(music_name, ser))
file_music.write('};')

file_music.close()
wb.close()

copyfile('../output/music.h', '../../Protogen_matrixs/Header Files/music.h')

print('Music convert successfully!!')
os.system('pause')