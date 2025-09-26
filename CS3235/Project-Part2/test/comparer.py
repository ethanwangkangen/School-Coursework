with open('input.txt', 'r') as file:
    content = file.read().splitlines()

d = {}
i=1
for line in content:
    if line not in d:
        d[line] = 1
    else:
        d[line] +=1
    
for line in content:
    if d[line]!=2:
        print(line)
