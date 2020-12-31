import os
import uuid
import shutil
from PIL import ImageDraw

def color_threshold(color_in):
    if color_in.isdigit():
        if (int)(color_in) > 255:
            return '255'
        else:
            return color_in
    else:
        return '255'

def do_rename(protogen, new_name):
    #delete jpg file
    #modify protogen_list
    protogen_number = protogen.split('.')[0]
    list = open('../Protogen_list.txt', 'r')
    list_new = open('../Protogen_list_bak.txt', 'w')
    for i in list:
        if i.split('.')[0] == protogen_number:
            i = i.replace(protogen.split('. ')[1], new_name)
        
        list_new.write(i)
    
    list_new.close()
    list.close()
    
    os.remove('../Protogen_list.txt')
    os.rename('../Protogen_list_bak.txt', '../Protogen_list.txt')

    if os.path.isdir('../../image/Jpg/' + protogen):
        shutil.rmtree('../../image/Jpg/' + protogen)

    return True

def do_recolor(protogen):
    #delete jpg file
    #modify protogen_list
    
    shutil.rmtree('../../image/Jpg/' + protogen)
    print('do_recolor')
    os.system("PAUSE")
    return True

def do_retheme(protogen):
    #delete jpg file
    #modify protogen_list
    
    shutil.rmtree('../../image/Jpg/' + protogen)
    print('do_retheme')
    os.system("PAUSE")
    return True

def do_delete(protogen):
    #delete jpg file   (shift protogen number)
    #modify protogen_list   (shift protogen number)
    #modify UUID file   (shift protogen number)
    
    shutil.rmtree('../../image/Jpg/' + protogen)
    print('do_delete')
    os.system("PAUSE")
    return True

cmd = '-i'

protogen_name = input('Hellowo!!\nplease enter New Protogen name\nor enter -E to edit Protogens：')
## General adding ######
if protogen_name != '-E' and protogen_name != '-e':
    color_R = input('please enter faceture color R：')
    color_G = input('please enter faceture color G：')
    color_B = input('please enter faceture color B：')

    print('Color Theme : ')
    remote_theme_list = os.listdir('../../image/color_theme')
    for i in remote_theme_list:
        print('\t' + i)
    remote_theme_path = input('Enter color Theme number of the remote')

    color_R = color_threshold(color_R)
    color_G = color_threshold(color_G)
    color_B = color_threshold(color_B)

    if remote_theme_path.isdigit():
        if (int)(remote_theme_path) > ((int)(len(remote_theme_list)) - 1):
            remote_theme_path = (int)(len(remote_theme_list)) - 1
    else:
        remote_theme_path = 0
    remote_theme_path = remote_theme_list[(int)(remote_theme_path)]

    folder_list = os.listdir('../../image/Jpg')
    if len(folder_list):
        protogen_ID = (int)(folder_list[-1].split('.')[0]) + 1
    else:
        protogen_ID = 0

    protogen = '{:0>2d}'.format(protogen_ID) + '. ' + protogen_name

    root_path = '../../image/Jpg/'
    if not os.path.isdir(root_path + protogen):
        os.mkdir(root_path + protogen)


    file_list = open('../Protogen_list.txt', 'a')
    file_list.write(protogen + '\t(' + color_R + ',' + color_G + ',' + color_B + ')\t' + remote_theme_path +'\n')
    file_list.close()


    if not os.path.isdir('../output'):
        os.mkdir('../output')
    file_ID = open('../output/UUID.txt', 'a')
    file_ID.write('#elif (ProtoGen_ID == ' + '{:0>2d}'.format(protogen_ID) + ')\n\t')
    file_ID.write('#define SERVICE_UUID "' + (str)(uuid.uuid4()) + '"\n\t')
    file_ID.write('#define CHARACTERISTIC_UUID "' + (str)(uuid.uuid4()) + '"\n\t')
    file_ID.write('#define ProtoGun_Name "ProtoBlaster_' + '{:0>2d}'.format(protogen_ID) + '"\n')
    file_ID.close()

    print('\nProtogen  ' + protogen.split(' ')[1] + '  has been created!!')
    os.system("PAUSE")
## Editting ######
else:
    while(True):
        if cmd == '-i' or cmd == '-I':
            file_list = open('../Protogen_list.txt', 'r')
            protogen_list = file_list.readlines()
            file_list.close()
            print('Choose Protogen：')
            for i in protogen_list:
                print('\t' + i.split('\n')[0].split('\t')[0])

            protogen = input('please enter number of whitch Protogen need edit or -e to leave：')

            while(True):
                if protogen.isdigit():
                    protogen = (int)(protogen)
                elif protogen == '-e' or protogen == '-E':
                    os._exit(0)
                else:
                    protogen = input('ENTER NUMBER：')
                    continue

                if protogen < len(protogen_list):
                    pixel_color_R = protogen_list[protogen].split('\t')[1].split(',')[0].split('(')[1]
                    pixel_color_G = protogen_list[protogen].split('\t')[1].split(',')[1]
                    pixel_color_B = protogen_list[protogen].split('\t')[1].split(',')[2].split(')')[0]

                    protogen_path = protogen_list[protogen].split('\t')[0]
                    protogen_name = protogen_list[protogen].split('\t')[0].split('. ')[1]
                    protogen_ID = protogen_list[protogen].split('.')[0]
                    remote_theme_path = protogen_list[protogen].split('\t')[2].split('\n')[0]
                    break
                else:
                    print("We don't have that boi here yet")
                    protogen = input('please enter again：')

            print('Name\t: ' + protogen_name)
            print('Color\t: ' + pixel_color_R + ',' + pixel_color_G + ',' + pixel_color_B)
            print('Themet\t: ' + remote_theme_path)
            print('\nCommand : ')
            print('\t-n\t\tRename')
            print('\t-c\t\tChange faceture color')
            print('\t-t\t\tChange remote Theme')
            print('\t-d\t\tDelete')
            print('\t-i\t\tSelect another Protogen')
            print('\t-e\t\tDiscard and leave')
            
            cmd_error = 0
            cmd = input('What do you wanna do：')
            continue

        elif cmd == '-n' or cmd == '-N':
            print('Enter -b to go back or')
            new_name = input('enter the new name : ')
            if new_name == '-b' or new_name == '-B':
                cmd = '-b'
                continue
            print('Goning to rename ' + protogen_name + ' as ' + new_name)
            confirm = input('Enter any thing to confirm or -b to discard : ')
            if confirm != '-b' and confirm != '-B':
                if do_rename(protogen_path, new_name) != True:
                    print("There's some error occurred during the process")
                else:
                    print('Rename successfully')
            cmd = '-i'
            continue               

        elif cmd == '-c' or cmd == '-C':
            print('Enter -b to go back or')
            new_color_R = input('enter new R color : ')
            if new_color_R == '-b' or new_color_R == '-B':
                cmd = '-b'
                continue
            new_color_G = input('Enter new G color : ')
            if new_color_G == '-b' or new_color_G == '-B':
                cmd = '-b'
                continue
            new_color_B = input('Enter new B color : ')
            if new_color_B == '-b' or new_color_B == '-B':
                cmd = '-b'
                continue
            new_color_R = color_threshold(new_color_R)
            new_color_G = color_threshold(new_color_G)
            new_color_B = color_threshold(new_color_B)

            print('Goning to change faceture color from : ' + pixel_color_R + ',' + pixel_color_G + ',' + pixel_color_B + ' to : ' + new_color_R + ',' + new_color_G + ',' + new_color_B)
            confirm = input('Enter any thing to confirm or -b to discard : ')
            if confirm != '-b' and confirm != '-B':
                if do_recolor(protogen_path) != True:
                    print("There's some error occurred during the process")
                else:
                    print('Change faceture color successfully')
            cmd = '-i'
            continue 

        elif cmd == '-t' or cmd == '-T':
            print('Enter -b to go back or')
            print('entre new color theme number : ')
            remote_theme_list = os.listdir('../../image/color_theme')
            for i in remote_theme_list:
                print('\t' + i)
            new_theme_path = input(' : ')

            if new_theme_path.isdigit():
                if (int)(new_theme_path) > ((int)(len(remote_theme_list)) - 1):
                    new_theme_path = (int)(len(remote_theme_list)) - 1
            else:
                new_theme_path = 0
            new_theme_path = remote_theme_list[(int)(new_theme_path)]

            print('Goning to change color theme from : ' + remote_theme_path + ' to : ' + new_theme_path)
            confirm = input('Enter any thing to confirm or -b to discard : ')
            if confirm != '-b' and confirm != '-B':
                if do_retheme(protogen_path) != True:
                    print("There's some error occurred during the process")
                else:
                    print('Change color theme successfully')
            cmd = '-i'
            continue 

        elif cmd == '-d' or cmd == '-D':
            confirm = input('Do you really want to DELETE ' + protogen_name + '? (y/n)')
            if confirm == 'y' or confirm == 'Y':
                confirm = input('Enter ' + protogen_name + ' to confirm : ')
                if confirm == protogen_name:
                    if do_delete(protogen_path) != True:
                        print("There's some error occurred during the process")
                        cmd = '-i'
                        continue 
                    else:
                        print('DELET Ptotoen successfully')
                        cmd = '-i'
                        continue 
                else:
                    cmd = '-b'
                    continue
            else:
                cmd = '-b'
                continue 

        elif cmd == '-e' or cmd == '-E':
            os._exit(0)
        elif cmd == '-b' or cmd == '-B':
            print('\t-n\t\tRename')
            print('\t-c\t\tChange faceture color')
            print('\t-t\t\tChange remote Theme')
            print('\t-d\t\tDelete')
            print('\t-i\t\tSelect another Protogen')
            print('\t-e\t\tDiscard and leave')
            cmd = input('What do you wanna do：')
        else:
            if cmd_error < 4:
                cmd_error += 1
                print('Unknown command')
                cmd = input('What do you wanna do：')
            else:
                print('Discard and Leave')
                break