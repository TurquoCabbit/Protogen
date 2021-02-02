import os
import uuid
from shutil import rmtree
from shutil import copy
from PIL import ImageDraw

for_testing = False
cmd = '-i'

def test_pause():
    if for_testing:
        os.system("PAUSE")

def color_threshold(color_in):
    if color_in.isdigit():
        if (int)(color_in) > 255:
            return '255'
        else:
            return color_in
    else:
        return '255'

def remove_and_shift_folder(root_path, protogen_serial_to_del):
    folder_list = []
    folder_list_row = os.listdir(root_path)
    for i in folder_list_row:
        if i.split('. ')[0].isdigit():
            folder_list.append(i)

    for k in range(len(folder_list)):
        if folder_list[k].split('. ')[0].isnumeric():
            folder_serial = (int)(folder_list[k].split('. ')[0])
            if folder_serial == protogen_serial_to_del:
                rmtree(root_path + folder_list[k])
            elif folder_serial > protogen_serial_to_del:
                os.rename(root_path + folder_list[k], root_path + '{:0>2d}{}'.format(folder_serial - 1, folder_list[k][2:]))
        else:
            break

def do_rename(protogen, new_name):
    #rename jpg folder
    #modify protogen_list
    try:
        protogen_number = protogen.split('.')[0]
        list = open('../output/Protogen_list.txt', 'r')
        list_new = open('../output/Protogen_list_bak.txt', 'w')
        for i in list:
            if i.split('.')[0] == protogen_number:
                i = i.replace(i.split('\t', 2)[0], protogen_number + '. ' + new_name)
            
            list_new.write(i)
        
        list_new.close()
        list.close()

        test_pause()
        os.remove('../output/Protogen_list.txt')
        os.rename('../output/Protogen_list_bak.txt', '../output/Protogen_list.txt')

        if os.path.isdir('../../image/Protogen/' + protogen):
            rmtree('../../image/Protogen/' + protogen)
            os.mkdir('../../image/Protogen/' + protogen_number + '. ' + new_name)

        if os.path.isdir('../Configure/' + protogen):
            os.rename('../Configure/' + protogen, '../Configure/' + protogen_number + '. ' + new_name)

        return True
    except:
        return False

def do_recolor(protogen, R, G, B):
    #delete jpg file
    #modify protogen_list
    try:
        protogen_number = protogen.split('.')[0]
        list = open('../output/Protogen_list.txt', 'r')
        list_new = open('../output/Protogen_list_bak.txt', 'w')
        for i in list:
            if i.split('.')[0] == protogen_number:
                i = i.replace(i.split('\t', 2)[1], '(' + R + ',' + G + ',' + B + ')')
            
            list_new.write(i)
        
        list_new.close()
        list.close()

        test_pause()
        os.remove('../output/Protogen_list.txt')
        os.rename('../output/Protogen_list_bak.txt', '../output/Protogen_list.txt')

        if os.path.isdir('../../image/Protogen/' + protogen):
            rmtree('../../image/Protogen/' + protogen)
            os.mkdir('../../image/Protogen/' + protogen)

        return True
    except:
        return False

def do_retheme(protogen, theme):
    #delete jpg file
    #modify protogen_list
    try:
        protogen_number = protogen.split('.')[0]
        list = open('../output/Protogen_list.txt', 'r')
        list_new = open('../output/Protogen_list_bak.txt', 'w')
        for i in list:
            if i.split('.')[0] == protogen_number:
                i = i.replace(i.split('\t', 2)[2], theme + '\n')
            
            list_new.write(i)
        
        list_new.close()
        list.close()

        test_pause()
        os.remove('../output/Protogen_list.txt')
        os.rename('../output/Protogen_list_bak.txt', '../output/Protogen_list.txt')

        if os.path.isdir('../../image/Protogen/' + protogen):
            rmtree('../../image/Protogen/' + protogen)
            os.mkdir('../../image/Protogen/' + protogen)
        
        return True
    except:
        return False

def do_delete(protogen):
    #delete jpg folder   (shift protogen number)
    #modify protogen_list   (shift protogen number)
    #modify UUID file   (shift protogen number)
    try:
        file_list = open('../output/Protogen_list.txt', 'r')
        protogen_num = len(file_list.readlines())
        file_list.close()
        protogen_serial_to_del = (int)(protogen.split('.')[0])

        remove_and_shift_folder('../../image/Protogen/', protogen_serial_to_del)
        remove_and_shift_folder('../Configure/', protogen_serial_to_del)

        list_f = open('../output/Protogen_list.txt', 'r')
        list = list_f.readlines()
        list_f.close()
        list_new = open('../output/Protogen_list_bak.txt', 'w')
        UUID_f = open('../output/UUID.txt', 'r')
        UUID = UUID_f.readlines()
        UUID_f.close()
        UUID_new = open('../output/UUID_bak.txt', 'w')
        for i in range(protogen_num - 1):
            if(i < protogen_serial_to_del):
                #keep
                list_new.write(list[i])
                for j in range(i * 4, i * 4 + 4):
                    UUID_new.write(UUID[j])
            else:
                #shift
                list[i + 1] =  '{:0>2d}{}'.format(i, list[i + 1][2:])
                list_new.write(list[i + 1])
                UUID[(i + 1) * 4] = '{}{:0>2d})\n'.format(UUID[(i + 1) * 4 ][:22], i)
                for j in range((i + 1) * 4, (i + 1) * 4 + 4):
                    UUID_new.write(UUID[j])

        list_new.close()
        UUID_new.close()
        
        test_pause()
        
        os.remove('../output/Protogen_list.txt')
        os.rename('../output/Protogen_list_bak.txt', '../output/Protogen_list.txt')
        os.remove('../output/UUID.txt')
        os.rename('../output/UUID_bak.txt', '../output/UUID.txt')
        
        return True
    except:
        return False

### Flow start

print('Hellowo!!')
print('>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>')
while(True):
    protogen_name = input('please enter New Protogen name or enter -E to edit Protogens：')

    space_num = 0
    for i in range(len(protogen_name)):
        if protogen_name[i].isspace():
            space_num += 1
    if space_num == len(protogen_name):
        print('No empty name allowed')
        os.system('pause')
        os._exit(0)

    if protogen_name != '-E' and protogen_name != '-e':
        color_R = input('please enter faceture color R：')
        color_G = input('please enter faceture color G：')
        color_B = input('please enter faceture color B：')

        print('Color Theme : ')
        remote_theme_list = os.listdir('../../image/color_theme')
        for i in remote_theme_list:
            print('\t' + i)
        remote_theme_path = input('Enter color Theme number of the remote :')

        color_R = color_threshold(color_R)
        color_G = color_threshold(color_G)
        color_B = color_threshold(color_B)

        if remote_theme_path.isdigit():
            if (int)(remote_theme_path) > ((int)(len(remote_theme_list)) - 1):
                remote_theme_path = (int)(len(remote_theme_list)) - 1
        else:
            remote_theme_path = 0
        remote_theme_path = remote_theme_list[(int)(remote_theme_path)]
        
        try:
            file_list = open('../output/Protogen_list.txt', 'r')
        except:
            file_list = open('../output/Protogen_list.txt', 'w')
            file_list.close()
            file_list = open('../output/Protogen_list.txt', 'r')
        protogen_ID = len(file_list.readlines())
        file_list.close()
        protogen = '{:0>2d}'.format(protogen_ID) + '. ' + protogen_name

        file_list = open('../output/Protogen_list.txt', 'a')
        file_list.write(protogen + '\t(' + color_R + ',' + color_G + ',' + color_B + ')\t' + remote_theme_path +'\n')
        file_list.close()

        if not os.path.isdir('../../image/Protogen/' + protogen):
            os.mkdir('../../image/Protogen/' + protogen)

        if not os.path.isdir('../Configure/' + protogen):
            os.mkdir('../Configure/' + protogen)
            os.mkdir('../Configure/' + protogen + '/Sig')
            copy('../Configure/Template.xlsx', '../Configure/{}/Template.xlsx'.format(protogen))
            os.rename('../Configure/{}/Template.xlsx'.format(protogen), '../Configure/{}/LED_Matrix.xlsx'.format(protogen))
            

        if not os.path.isdir('../output'):
            os.mkdir('../output')
        file_ID = open('../output/UUID.txt', 'a')
        file_ID.write('#elif (ProtoGen_ID == ' + '{:0>2d}'.format(protogen_ID) + ')\n\t')
        file_ID.write('#define SERVICE_UUID "' + (str)(uuid.uuid4()) + '"\n\t')
        file_ID.write('#define CHARACTERISTIC_UUID "' + (str)(uuid.uuid4()) + '"\n\t')
        file_ID.write('#define ProtoGun_Name "ProtoBlaster_' + '{:0>2d}'.format(protogen_ID) + '"\n')
        file_ID.close()

        print('\nProtogen  ' + protogen + '  has been created!!')
        continue
    else:
        while(True):
            if cmd == '-i' or cmd == '-I':
                file_list = open('../output/Protogen_list.txt', 'r')
                protogen_list = file_list.readlines()
                file_list.close()
                print('Choose Protogen：')
                for i in protogen_list:
                    print('\t' + i.split('\n')[0])

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
                print('****************************************************************************************************************')
                print('Name\t: ' + protogen_name)
                print('Color\t: ' + pixel_color_R + ',' + pixel_color_G + ',' + pixel_color_B)
                print('Themet\t: ' + remote_theme_path)
                print('****************************************************************************************************************')
                print('Command : ')
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
                        print("There's some error occurred during the process\n")
                    else:
                        print('Rename successfully')
                    cmd = '-i'
                else:
                    cmd = '-b'
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
                    if do_recolor(protogen_path, new_color_R, new_color_G, new_color_B) != True:
                        print("There's some error occurred during the process\n")
                    else:
                        print('Change faceture color successfully')
                    cmd = '-i'
                else:
                    cmd = '-b'
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
                    if do_retheme(protogen_path, new_theme_path) != True:
                        print("There's some error occurred during the process\n")
                    else:
                        print('Change color theme successfully')
                    cmd = '-i'
                else:
                    cmd = '-b'
                continue 

            elif cmd == '-d' or cmd == '-D':
                confirm = input('Do you really want to DELETE ' + protogen_name + '? (y/n)')
                if confirm == 'y' or confirm == 'Y':
                    confirm = input('Enter ' + protogen_name + ' to confirm : ')
                    if confirm == protogen_name:
                        if do_delete(protogen_path) != True:
                            print("There's some error occurred during the process\n")
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