import os

directory = '.'

for file in os.listdir(directory):
    filename = os.fsdecode(file)
    if filename.endswith(".obj"):
        count = 0
        f = open(os.path.join(directory, filename), 'r')
        for line in f.readlines():
            if line.startswith("f "):
                count += 1
        print(filename, "=", str(count))
    else:
        continue
