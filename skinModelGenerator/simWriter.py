absorption = [[[.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01]],
              [[ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1]],
              [[  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1]]]

scattering = [[[.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01], [.01, .01, .01]],
              [[ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1], [ .1,  .1,  .1]],
              [[  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1], [  1,   1,   1]]]

dermatoscope = [1, 10, 100]
room = [1, 10, 100]

x = 1

SIMULATION_FOLDER = "simulations"
HEADERS = "Simulation Name,Eye Distance From Skin (CM),Field of View,Rays Per Pixel,Image Resolution (X),Image Resolution (Y),Arm X Dimension (mm),Arm Y Dimension (mm),Arm Z Dimension (mm),Volume File Location,Hair Color,Hair Density (0-10),Voxels per MM^3,Absorption (Red Layer 0),Absorption (Green Layer 0),Absorption (Blue Layer 0),Scattering (Red Layer 0),Scattering (Green Layer 0),Scattering (Blue Layer 0),Absorption (Red Layer 1),Absorption (Green Layer 1),Absorption (Blue Layer 1),Scattering (Red Layer 1),Scattering (Green Layer 1),Scattering (Blue Layer 1),Absorption (Red Layer 2),Absorption (Green Layer 2),Absorption (Blue Layer 2),Scattering (Red Layer 2),Scattering (Green Layer 2),Scattering (Blue Layer 2),Absorption (Red Layer 3),Absorption (Green Layer 3),Absorption (Blue Layer 3),Scattering (Red Layer 3),Scattering (Green Layer 3),Scattering (Blue Layer 3),Absorption (Red Layer 4),Absorption (Green Layer 4),Absorption (Blue Layer 4),Scattering (Red Layer 4),Scattering (Green Layer 4),Scattering (Blue Layer 4),Dermatascope Light Intensity (Red),Dermatascope Light Intensity (Green),Dermatascope Light Intensity (Blue),Dermatascope Material Color (Red),Dermatascope Material Color (Green),Dermatascope Material Color (Blue),Room Width,Room Length,Room Height,Room Light Intensity (Red),Room Light Intensity (Green),Room Light Intensity (Blue),Room Material Color (Red),Room Material Color (Green),Room Material Color (Blue)\n"

EYE_DISTANCE = 1
FIELD_OF_VIEW = 90
RAYS_PER_PIXEL = 2048
X_PIXELS = 400
Y_PIXELS = 400
X_DENSITY = 61
Y_DENSITY = 61
Z_DENSITY = 61
VOXELS_PER_MM = 8
DENSITY_FILE_NAME = "density.raw"
HAIR_COLOR = "brown"
HAIR_DENSITY = 1

EPIDERMIS = 0
DERMIS = 1
HYPERDERMIS = 2
SUBCUTANEOUS_FAT = 3
MUSCLE = 4
BONE = 5

COLOR_CHANNELS = 3
RED = 0
GREEN = 1
BLUE = 2

DERMATOSCOPE_LIGHT_BASE_INTENSITY = [0.95686, 1, 0.98039]
DERMATOSCOPE_MATERIAL_COLOR = [0, 0, 0]

X_ROOM = 5
Y_ROOM = 5
Z_ROOM = 5
ROOM_LIGHT_BASE_INTENSITY = [0.95686, 1, 0.98039]
ROOM_MATERIAL_COLOR = [0.95, 0.95, 0.95]

jobFile = open("jobs.txt", "w")

for a in absorption:
    for s in scattering:
        for d in dermatoscope:
            for r in room:
                f = open(SIMULATION_FOLDER + "/Simulation_" + str(x) + ".csv", "w")
                f.write(HEADERS)
                imageName = "[a:" + str(a[EPIDERMIS][RED]) + "][s:" + str(s[EPIDERMIS][RED]) + "][d:" + str(d) + "][r:" + str(r) + "]"

                values = [imageName,
                          str(EYE_DISTANCE),
                          str(FIELD_OF_VIEW),
                          str(RAYS_PER_PIXEL),
                          str(X_PIXELS),
                          str(Y_PIXELS),
                          str(X_DENSITY),
                          str(Y_DENSITY),
                          str(Z_DENSITY),
                          str(DENSITY_FILE_NAME),
                          HAIR_COLOR,
                          str(HAIR_DENSITY),
                          str(VOXELS_PER_MM)] 

                for layer in range(EPIDERMIS, BONE):
                    for color in range(COLOR_CHANNELS):
                        values.append(str(a[layer][color]))
                    for color in range(COLOR_CHANNELS):
                        values.append(str(s[layer][color]))

                for color in range(COLOR_CHANNELS):
                    values.append(str(DERMATOSCOPE_LIGHT_BASE_INTENSITY[color] * d))

                for color in range(COLOR_CHANNELS):
                    values.append(str(DERMATOSCOPE_MATERIAL_COLOR[color]))

                values.append(str(X_ROOM))
                values.append(str(Y_ROOM))
                values.append(str(Z_ROOM))

                for color in range(COLOR_CHANNELS):
                    values.append(str(ROOM_LIGHT_BASE_INTENSITY[color] * r))

                for color in range(COLOR_CHANNELS):
                    values.append(str(ROOM_MATERIAL_COLOR[color]))

                simulation = ",".join(values)
                print(simulation)

                f.write(simulation + '\n')

                jobFile.write("./bpbrt " + SIMULATION_FOLDER + "/Simulation_" + str(x) + ".csv\n")
                x = x + 1
