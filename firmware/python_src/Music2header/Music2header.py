##Face xlsx to header file convertor
import os
from datetime import datetime
from openpyxl import load_workbook
from shutil import copyfile

def progress_change(item, percent):
    if percent == 'OFF':
        progress = 'OFF'
        for i in range(3, 50):
            progress += ' '
        print('\r[{}]   0% {}'.format(progress, item), end = '')
    else:
        progress = ''
        for i in range(50):
            if i <= percent / 2 - 1:
                progress += '.'
            else:
                progress += ' '
        print('\r[{}] {:>3d}% {}'.format(progress, int(percent), item), end = '')



#pitch_array = ['0', 'C', 'C#', 'D', 'Eb', 'E', 'F', 'F#', 'G', 'G#', 'A', 'Bb', 'B']
#                0    1    2     3    4     5    6    7     8    9     10   11    12
def pitch_conv(input):
    # i = 0
    # for i in range(len(pitch_array)):
    #     if input == pitch_array[i]:
    #         return i
    # return 0
    if input == '0':
        return 0
    elif input == 'C' or input == 'c':
        return 1
    elif input == 'C#' or input == 'c#':
        return 2
    elif input == 'D' or input == 'd':
        return 3
    elif input == 'Eb' or input == 'EB' or input == 'eb' or input == 'eB':
        return 4
    elif input == 'E' or input == 'e':
        return 5
    elif input == 'F' or input == 'f':
        return 6
    elif input == 'F#'  or input == 'f#':
        return 7
    elif input == 'G' or input == 'g':
        return 8
    elif input == 'G#' or input == 'g#':
        return 9
    elif input == 'A' or input == 'a':
        return 10
    elif input == 'Bb' or input == 'BB' or input == 'bb' or input == 'bB':
        return 11
    elif input == 'B' or input == 'b':
        return 12
    else:
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
    music_name = work_sheet['J1'].value
    tempo = work_sheet['L2'].value
    # ring_time = work_sheet['L3'].value
    length = work_sheet['L4'].value

    if work_sheet['K12'].value == 'OFF':
        progress_change(music_name, 'OFF')
        print('')
        continue

    file_music.write('_sheet {}_sheet[{}] = \n'.format(music_name, length + 1))
    file_music.write('{\n\t')
    file_music.write('{0, 4, 16, 0}, \n')
    
    for i in range(2, length + 2):
        progress_change(music_name, (i - 2) / length * 100)
        file_music.write('\t{')
        file_music.write('{}, {}, {}, {}'.format(pitch_conv(work_sheet['B{}'.format(i)].value), work_sheet['C{}'.format(i)].value, work_sheet['D{}'.format(i)].value, work_sheet['E{}'.format(i)].value))
        # file_music.write('{}, '.format(pitch_conv(work_sheet['B{}'.format(i)].value)))
        # file_music.write('{}, '.format(work_sheet['C{}'.format(i)].value))
        # file_music.write('{}, '.format(work_sheet['D{}'.format(i)].value))
        # file_music.write('{}'.format(work_sheet['E{}'.format(i)].value))
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
    # file_music.write('\t{},\n'.format(ring_time))
    file_music.write('};\n\n')
    progress_change(music_name, 100)
    print('')

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