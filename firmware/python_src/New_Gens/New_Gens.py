import os
import uuid
from PIL import ImageDraw

protogen_name = input('Hellowo!!\nplease enter New Protogen name：')
color_R = input('please enter faceture color R：')
color_G = input('please enter faceture color G：')
color_B = input('please enter faceture color B：')

folder_list = os.listdir('../../image/Jpg')
if len(folder_list):
    protogen_ID = (int)(folder_list[-1].split('.')[0]) + 1
else:
    protogen_ID = 0

protogen = '{:0>2d}'.format(protogen_ID) + '. ' + protogen_name

root_path = '../../image/Jpg/'
if not os.path.isdir(root_path + protogen):
    os.mkdir(root_path + protogen)
    os.mkdir(root_path + protogen + '/Batt')
    os.mkdir(root_path + protogen + '/BLE')
    os.mkdir(root_path + protogen + '/Face')
    os.mkdir(root_path + protogen + '/Lit')
    os.mkdir(root_path + protogen + '/Sig')

if color_R.isdigit():
    if (int)(color_R) > 255:
        color_R = '255'
else:
    color_R = '255'

if color_G.isdigit():
    if (int)(color_G) > 255:
        color_G = '255'
else:
    color_G = '255'

if color_B.isdigit():
    if (int)(color_B) > 255:
        color_B = '255'
else:
    color_B = '255'

file_list = open('../Protogen_list.txt', 'a')
file_list.write(protogen + '\t(' + color_R + ',' + color_G + ',' + color_B + ')\n')
file_list.close()

file_ID = open('../output/UUID.txt', 'a')
file_ID.write('#elif (ProtoGen_ID == ' + '{:0>2d}'.format(protogen_ID) + ')\n\t')
file_ID.write('#define SERVICE_UUID "' + (str)(uuid.uuid4()) + '"\n\t')
file_ID.write('#define CHARACTERISTIC_UUID "' + (str)(uuid.uuid4()) + '"\n\t')
file_ID.write('#define ProtoGun_Name "ProtoBlaster_' + '{:0>2d}'.format(protogen_ID) + '"\n')
file_ID.close()
