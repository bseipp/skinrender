import subprocess
import os
import matplotlib.pyplot as plt

absorption = [[[.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01]], 
              [[ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1]], 
              [[  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1]]]

scattering = [[[.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01]], 
              [[ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1]], 
              [[  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1]]]

dermatoscope = [1, 10, 100]
room = [1, 10, 100]

IMAGES_FOLDER = "images/0218_Images"

CMD = "montage"

def aVs():
    absorptionVsScattering = ''
    for a in absorption:
        for s in scattering:
            print(" " + IMAGES_FOLDER + "/[a:" + str(a[0][0]) + "][s:" + str(s[0][0]) + "][d:1][r:1].png")
            absorptionVsScattering += " " + IMAGES_FOLDER + "/[a:" + str(a[0][0]) + "][s:" + str(s[0][0]) + "][d:1][r:1].png"
    absorptionVsScattering += " avs.png"
    os.system(CMD + absorptionVsScattering) 
    
    img = plt.imread("avs.png")
    fig, ax = plt.subplots()
    ax.imshow(img, extent=[0,1,0,1])
    ax.xaxis.tick_top()
    ax.xaxis.set_label_position('top')
    plt.xticks([.17,.5,.83],('.01','.1','1'))
    plt.yticks([.17,.5,.83],('1', '.1', '.01'))
    plt.xlabel('Scattering Coefficient')
    plt.ylabel('Absorption Coefficient')
    plt.title('Scattering vs Absorption', y=-.08)
    fig.savefig('ScatteringVsAbsorption.png')

def rVd():
    roomVsDerm = ""
    for r in room:
        for d in dermatoscope:
            print(" " + IMAGES_FOLDER + "/[a:.01][s:.01][d:" + str(d) + "][r:" + str(r) + "].png")
            roomVsDerm += " " + IMAGES_FOLDER + "/[a:0.01][s:0.01][d:" + str(d) + "][r:" + str(r) + "].png"
    roomVsDerm += " rvd.png"
    os.system(CMD + roomVsDerm)
    
    img = plt.imread("rvd.png")
    fig, ax = plt.subplots()
    ax.imshow(img, extent=[0,1,0,1])
    ax.xaxis.tick_top()
    ax.xaxis.set_label_position('top')
    plt.xticks([.17,.5,.83],('1','10','100'))
    plt.yticks([.17,.5,.83],('100', '10', '1'))
    plt.xlabel('Dermatoscope Light Intensity')
    plt.ylabel('Room Light Intensity')
    plt.title('Room I vs Dermatoscope I', y=-.08)
    fig.savefig('RoomIvDermI.png')

rVd()
