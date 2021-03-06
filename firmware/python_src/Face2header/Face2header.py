##Face xlsx to header file convertor
import os
from datetime import datetime
from openpyxl import load_workbook
from shutil import copyfile
from shutil import rmtree
from PIL import Image
from PIL import ImageDraw

try:
    file_list = open('../output/Protogen_list.txt', 'r')
except:
    print('Run New_Gens.exe to create protogen first!!')
    os.system('pause')
    os._exit(0)

protogen_list = file_list.readlines()
file_list.close()
print('Choose Protogen：')
for i in protogen_list:
    print('\t' + i.split('\n')[0].split('\t')[0])

protogen = input('please enter Protogen number：')
while(True):
    if protogen.isdigit():
        protogen = (int)(protogen)
    else:
        protogen = input('ENTER NUMBER：')
        continue
    if protogen < len(protogen_list):
        pixel_color = 'rgb' + protogen_list[protogen].split('\t')[1]
        protogen_path = protogen_list[protogen].split('\t')[0] + '/'
        protogen_name = protogen_list[protogen].split('\t')[0].split('. ')[1]
        protogen_ID = protogen_list[protogen].split('.')[0]
        remote_theme_path = protogen_list[protogen].split('\t')[2].split('\n')[0] + '/'
        break
    else:
        print("We don't have that boi here yet")
        protogen = input('please enter again：')

file_face = open('../output/Face.h', 'w')

time = datetime.now().strftime("%Y/%m/%d %H:%M:%S")
file_face.write('//' + time + '\t\t' + protogen_ID + '_' + protogen_name + '\n\n')

wb = load_workbook(filename = '../Configure/{}LED_Matrix.xlsx'.format(protogen_path), data_only = True, read_only = True)
sheet_num = len(wb.worksheets)
start_sheet = 11
face_off_num = 0
for ws in range(start_sheet ,sheet_num):
    value = wb[wb.worksheets[ws].title]['E33'].value
    if not value:
        continue

    file_face.write(value)
    file_face.write('\n')

file_face.write('\n')
file_face.write('_face * face_ptr_rack[] =\n{\n')

for ws in range(start_sheet ,sheet_num):
    value = wb[wb.worksheets[ws].title]['E33'].value
    if not value:
        continue
    value = wb[wb.worksheets[ws].title]['E32'].value
    file_face.write('\t(_face *)(&Face_{}),\t\t//{}\n'.format(value, hex(ws - start_sheet)))
        
file_face.write('\n};')
file_face.close()

eye_array = []
nose_array = []
mouth_array = []

if os.path.isdir('../../image/Protogen/' + protogen_path):
    rmtree('../../image/Protogen/' + protogen_path)

os.mkdir('../../image/Protogen/' + protogen_path)

for ws in range(start_sheet ,sheet_num):
    value = wb[wb.worksheets[ws].title]['E33'].value
    if not value:
        face_off_num += 1
        continue
    face_name = wb[wb.worksheets[ws].title]['E32'].value
    save_path = '../../image/Protogen/' + protogen_path + '{:0>2d}_'.format(ws - start_sheet - face_off_num) + face_name + '.png'
    eye_array = wb[wb.worksheets[ws].title]['X12'].value.split(',', 16)
    nose_array = wb[wb.worksheets[ws].title]['N10'].value.split(',', 8)
    mouth_array = wb[wb.worksheets[ws].title]['E30'].value.split(',', 32)
    
    im_eye = Image.new('RGBA', (48, 24))
    im_nose = Image.new('RGBA', (16, 16))
    im_mouth = Image.new('RGBA', (64, 16))
    draw_eye = ImageDraw.Draw(im_eye)
    draw_nose = ImageDraw.Draw(im_nose)
    draw_mouth = ImageDraw.Draw(im_mouth)

    for x in range(48):
        for y in range(24):
            if ((int)(eye_array[x // 3]) >> y // 3) & 0x01:
                draw_eye.point((x, y),fill = pixel_color)

    for x in range(16):
        for y in range(16):
            if ((int)(nose_array[y // 2]) << x // 2) & 0x80:
                draw_nose.point((x, y),fill = pixel_color)
                
    for x in range(64):
        for y in range(16):
            if ((int)(mouth_array[x // 2]) >> y // 2) & 0x01:
                draw_mouth.point((x, y),fill = pixel_color)
                
    im_eye = im_eye.rotate(30, resample = Image.BICUBIC, expand = True)
    im_nose = im_nose.rotate(20, resample = Image.BICUBIC, expand = True)
    im_mouth = im_mouth.rotate(10, resample = Image.BICUBIC, expand = True)

    # try:
    #     im_empty = Image.open('../../image/color_theme/' + remote_theme_path + 'Face/Face_empty.png')
    # except:
    #     print('Face_empty.png image file is missing')
    #     os.system('pause')
    #     os._exit(0)
    try:
        im_list = os.listdir('../../image/color_theme/' + remote_theme_path + 'Face/')
    except:
        print('Face_empty image file is missing')
        os.system('pause')
        os._exit(0)
    im_empty = Image.open('../../image/color_theme/' + remote_theme_path + 'Face/' + im_list[0])
    #im_empty = Image.open('../../image/color_theme/' + remote_theme_path + 'Face/Face_empty.png')
    im_empty.paste(im_eye, (55,30), mask = im_eye)
    im_empty.paste(im_nose, (8,78), mask = im_nose)
    im_empty.paste(im_mouth, (24,96), mask = im_mouth)
    im_empty.save(save_path, dpi = (72,72), compress_level = 0)
    im_empty.close()
    im_eye.close()
    im_nose.close()
    im_mouth.close()

wb.close()

##################################################################################################
try:
    jpg_list_Batt = os.listdir('../../image/color_theme/' + remote_theme_path + 'Batt/')
    for i in jpg_list_Batt:
        im = Image.open('../../image/color_theme/' + remote_theme_path + 'Batt/' + i)
        im.close()
except:
    print('Battery image file is missing or wrong name')
    os.system('pause')
    os._exit(0)

try:
    jpg_list_BLE = os.listdir('../../image/color_theme/' + remote_theme_path + 'BLE/')
    im = Image.open('../../image/color_theme/' + remote_theme_path + 'BLE/Connected.jpg')
    im.close()
except:
    print('BLE image file is missing or wrong name')
    os.system('pause')
    os._exit(0)

try:
    jpg_list_Lit = os.listdir('../../image/color_theme/' + remote_theme_path + 'Lit/')
    im = Image.open('../../image/color_theme/' + remote_theme_path + 'Lit/Max_lit.jpg') 
    im.close()
    im = Image.open('../../image/color_theme/' + remote_theme_path + 'Lit/general_lit.jpg')
    im.close()
except:
    print('Lit image file is missing or wrong name')
    os.system('pause')
    os._exit(0)

try:
    jpg_list_Sig_bg = os.listdir('../../image/color_theme/' + remote_theme_path + 'Sig/')
    im = Image.open('../../image/color_theme/' + remote_theme_path + 'Sig/Signature_bg.jpg')
    im.close()
except:
    print('Signature_bg image file is missing or wrong name')
    os.system('pause')
    os._exit(0)

if os.path.isdir('../Configure/' + protogen_path + 'Sig/'):
    jpg_list_Sig = os.listdir('../Configure/' + protogen_path + 'Sig')
    if len(jpg_list_Sig) > 0:
        has_sig = 1
    else:
        has_sig = 0
else:
    has_sig = 0

jpg_list_Face = os.listdir('../../image/Protogen/' + protogen_path)

file_image = open('../output/image.h', 'w')

time = datetime.now().strftime("%Y/%m/%d %H:%M:%S")
file_image.write('//' + time + '\t\t' + protogen_ID + '_' + protogen_name + '\n\n')

##################################################################################################
file_image.write('const uint16_t Batt[][6750] = {')
for j in jpg_list_Batt:
    im = Image.open('../../image/color_theme/' + remote_theme_path + 'Batt/' + j)
    im = im.transpose(Image.FLIP_LEFT_RIGHT)
    im = im.transpose(Image.ROTATE_90)

    pixels = im.load() # this is not a list, nor is it list()'able
    width, height = im.size
 
    file_image.write('\n\t{\n\t')
    line = 0
    for x in range(width):
        for y in range(height):
            line += 1
            R = pixels[x, y][0] >> 3
            G = pixels[x, y][1] >> 2
            B = pixels[x, y][2] >> 3
            file_image.write((hex((R << 11) + (G << 5) + B)) + ', ')
            if line == 16:
                file_image.write('\n\t')
                line = 0
    file_image.write('},')
    im.close()
file_image.write('\n};\n\n')

##################################################################################################
file_image.write('const uint16_t Connected[6750] = {')
im = Image.open('../../image/color_theme/' + remote_theme_path + 'BLE/Connected.jpg')
im = im.transpose(Image.FLIP_LEFT_RIGHT)
im = im.transpose(Image.ROTATE_90)

pixels = im.load() # this is not a list, nor is it list()'able
width, height = im.size
 
line = 0
for x in range(width):
    for y in range(height):
        line += 1
        R = pixels[x, y][0] >> 3
        G = pixels[x, y][1] >> 2
        B = pixels[x, y][2] >> 3
        file_image.write((hex((R << 11) + (G << 5) + B)) + ', ')
        if line == 16:
            file_image.write('\n\t')
            line = 0
im.close()
file_image.write('};\n')

##################################################################################################
file_image.write('const uint16_t Unconnected[6750] = {')
im = Image.open('../../image/color_theme/' + remote_theme_path + 'BLE/Unconnected.jpg')
im = im.transpose(Image.FLIP_LEFT_RIGHT)
im = im.transpose(Image.ROTATE_90)
pixels = im.load() # this is not a list, nor is it list()'able
width, height = im.size
 
line = 0
for x in range(width):
    for y in range(height):
        line += 1
        R = pixels[x, y][0] >> 3
        G = pixels[x, y][1] >> 2
        B = pixels[x, y][2] >> 3
        file_image.write((hex((R << 11) + (G << 5) + B)) + ', ')
        if line == 16:
            file_image.write('\n\t')
            line = 0
im.close()
file_image.write('};\n\n')

##################################################################################################
file_image.write('const uint16_t Max_lit[6750] = {')
im = Image.open('../../image/color_theme/' + remote_theme_path + 'Lit/Max_lit.jpg')
im = im.transpose(Image.FLIP_LEFT_RIGHT)
im = im.transpose(Image.ROTATE_90)
pixels = im.load() # this is not a list, nor is it list()'able
width, height = im.size
 
line = 0
for x in range(width):
    for y in range(height):
        line += 1
        R = pixels[x, y][0] >> 3
        G = pixels[x, y][1] >> 2
        B = pixels[x, y][2] >> 3
        file_image.write((hex((R << 11) + (G << 5) + B)) + ', ')
        if line == 16:
            file_image.write('\n\t')
            line = 0
im.close()
file_image.write('};\n\n')

##################################################################################################
file_image.write('const uint16_t general_lit[6750] = {')
im = Image.open('../../image/color_theme/' + remote_theme_path + 'Lit/general_lit.jpg')
im = im.transpose(Image.FLIP_LEFT_RIGHT)
im = im.transpose(Image.ROTATE_90)
pixels = im.load() # this is not a list, nor is it list()'able
width, height = im.size
 
line = 0
for x in range(width):
    for y in range(height):
        line += 1
        R = pixels[x, y][0] >> 3
        G = pixels[x, y][1] >> 2
        B = pixels[x, y][2] >> 3
        file_image.write((hex((R << 11) + (G << 5) + B)) + ', ')
        if line == 16:
            file_image.write('\n\t')
            line = 0
im.close()
file_image.write('};\n\n')

##################################################################################################
file_image.write('const uint16_t Signature_BG[][32400] = {')
im = Image.open('../../image/color_theme/' + remote_theme_path + 'Sig/Signature_bg.jpg')
im = im.transpose(Image.FLIP_LEFT_RIGHT)
im = im.transpose(Image.ROTATE_90)

pixels = im.load() # this is not a list, nor is it list()'able
width, height = im.size

file_image.write('\n\t{')
line = 0
for x in range(width):
    for y in range(height):
        line += 1
        R = pixels[x, y][0] >> 3
        G = pixels[x, y][1] >> 2
        B = pixels[x, y][2] >> 3
        file_image.write((hex((R << 11) + (G << 5) + B)) + ', ')
        if line == 16:
            file_image.write('\n\t')
            line = 0
file_image.write('},')
im.close()
file_image.write('\n};\n')

if has_sig == 1:
    file_image.write('const uint16_t Signature[][32400] = {')
    for j in jpg_list_Sig:
        im = Image.open('../Configure/' + protogen_path + 'Sig/' + j)
        im = im.transpose(Image.FLIP_LEFT_RIGHT)
        im = im.transpose(Image.ROTATE_90)

        pixels = im.load() # this is not a list, nor is it list()'able
        width, height = im.size
    
        file_image.write('\n\t{')
        line = 0
        for x in range(width):
            for y in range(height):
                line += 1
                R = pixels[x, y][0] >> 3
                G = pixels[x, y][1] >> 2
                B = pixels[x, y][2] >> 3
                file_image.write((hex((R << 11) + (G << 5) + B)) + ', ')
                if line == 16:
                    file_image.write('\n\t')
                    line = 0
        file_image.write('},')
        im.close()
    file_image.write('\n};\n')
    file_image.write('#define\thas_sig\t{}\n'.format(len(jpg_list_Sig)))
file_image.write('\n')

##################################################################################################
file_image.write('const uint16_t Face[][18900] = {')
for j in range(len(jpg_list_Face)):
    im = Image.open('../../image/Protogen/' + protogen_path + jpg_list_Face[j])
    im = im.transpose(Image.FLIP_LEFT_RIGHT)
    im = im.transpose(Image.ROTATE_90)

    pixels = im.load() # this is not a list, nor is it list()'able
    width, height = im.size
 
    file_image.write('\n\t{')
    line = 0
    for x in range(width):
        for y in range(height):
            line += 1
            R = pixels[x, y][0] >> 3
            G = pixels[x, y][1] >> 2
            B = pixels[x, y][2] >> 3
            file_image.write((hex((R << 11) + (G << 5) + B)) + ', ')
            if line == 16:
                file_image.write('\n\t')
                line = 0
    file_image.write('},')
    im.close()
file_image.write('\n};\n\n')

file_image.write('#define Face_index_top\t\t' + hex(len(jpg_list_Face) - 1) + '\n')
for j in range(len(jpg_list_Face)):
    Face = jpg_list_Face[j].split('.')[0]
    Face = Face.split('_')[1]
    file_image.write('#define Face_index_' + Face + '\t' + hex(j) + '\n')
file_image.write('\n')

file_image.write('const uint16_t Face_cmd[] = {\n')
for j in range(len(jpg_list_Face)):    
    file_image.write('\t' + hex(512 + j) + ',\n')
file_image.write('};\n')

file_image.close()

##################################################################################################
file_ID = open('../output/UUID.txt', 'r')
UUID_content = file_ID.readlines()
file_ID.close()

file_BLEID = open('../output/BLEID.h', 'w')

time = datetime.now().strftime("%Y/%m/%d %H:%M:%S")
file_BLEID.write('//' + time + '\t\t' + protogen_ID + '_' + protogen_name + '\n\n')

file_BLEID.write('#define ProtoGen_ID\t' + protogen_ID)
file_BLEID.write('\n#if 0\n')
file_BLEID.writelines(UUID_content)
file_BLEID.seek(0,2)
file_BLEID.write('#endif\n')
file_BLEID.close()
##################################################################################################

##################################################################################################


##################################################################################################

copyfile('../output/Face.h', '../../Protogen_matrixs/Header Files/Face.h')
copyfile('../output/image.h', '../../ProtoGun/Header Files/image.h')
copyfile('../output/BLEID.h', '../../Protogen_matrixs/Header Files/BLEID.h')
copyfile('../output/BLEID.h', '../../ProtoGun/Header Files/BLEID.h')

print("Convert Done!!")
os.system("PAUSE")

   

